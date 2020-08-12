#include <vector>
#include <map>
#include <math.h>
#include <float.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "EVXTimer.h"
#include "EVXLEDController.h"
#include "EVXAudioRecorder.h"
#include "biquad.h"
#include "notch.h"
#include "ramp.h"
#include "blinky_functions.h"
#include "dip_switch.h"
extern "C" {
  #include "fft.h"
}
#include "doa.h"
#include "config.h"




// Here we define for convenience an array of LEDs GPIO
// and an enum to keep track of which is which
// The ordering should correspond
const vector<int> leds{LED_RED, LED_WHITE, LED_BLUE, LED_GREEN};

// States
enum class State : char
{
  WHITE_SIGNAL = 0,
  RED_SIGNAL = 1,
  CALIBRATION_RAMP = 2,
  CALIBRATION_RAMP_MAP = 3,
  RED_BLUE_DOUBLE_REF = 4,
  RED_CALIBRATION = 5,
  WHITE_CALIBRATION = 6,
  //STATE_7 = 7
  DOA = 7
};
State state_current = State::WHITE_SIGNAL;

// For the calibration
int counter = 0;
int current_led = LED_WHITE;

// DC removal filter
DCRemoval dc_rm(DC_REMOVAL_ALPHA);

// Distance between microphones [m]
float D_MICS = 0.029;

void main_process()
{

    // The maximum duty cycle of blue and red was set so that
    // there is no saturation in the video recording
    // The reference is set to half of the maximum
    map<int, uint32_t> duty_ref;
    for (auto p: duty_max)
      duty_ref[p.first] = p.second / 2;

    // We use a ramp signal for calibration
    Ramp the_ramp(1. / CALIB_PERIOD_SEC, 0.);

    // Configure dip switch and read current state
    DipSwitch3 dip_switch(DIP_SWITCH_1, DIP_SWITCH_2, DIP_SWITCH_3);


    // for measuring elapsed time.
    EVXTimer* timer = new EVXTimer();
    // for controlling LEDs.
    EVXLEDController* ledC = new EVXLEDController(LED_RESOLUTION, LED_FREQUENCY, leds);
    // for recording.
    EVXAudioRecorder* recorder = new EVXAudioRecorder(SAMPLE_RATE, AUDIO_BUFFER_SIZE, I2S_BCK, I2S_WS, I2S_DATA_IN, CPU_NUMBER);
    
    // audio record start
    recorder->start();

    float elapsed_time;

    while (1) {
        
        // Get the audio data.
        // This function waits until buffer is accumulated
        // audio_data is interleaved data. (length:AUDIO_BUFFER_SIZE x AUDIO_CHANNELS)
        float* audio_data = recorder->wait_for_buffer_to_accumulate();

        State state_new = (State)dip_switch.read();

        switch (state_new)
        {


          case State::RED_BLUE_DOUBLE_REF:
            if (state_new != state_current)
            {
              // Turn off all LEDs
              for (int i=0 ; i < 4 ; i++)
                ledC->updateDuty(leds[i], 0);
              // Use the Blue LED as reference at half PWM resolution
              ledC->updateDuty(LED_RED, duty_ref[LED_RED]);
              ledC->updateDuty(LED_BLUE, duty_ref[LED_BLUE]);
              state_current = state_new;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

          case State::CALIBRATION_RAMP:
          case State::CALIBRATION_RAMP_MAP:
            {
              if (state_new != state_current)
              {
                // do some initialization
                current_led = LED_WHITE;
                state_current = state_new;

                // Turn off all LEDs
                for (int i=0 ; i < 4 ; i++)
                  ledC->updateDuty(leds[i], 0);

                // Reset the ramp function
                the_ramp.reset(0.);
                timer->start();
              }
              else
              {
                float now = timer->measure();
                the_ramp.update(now * 1e-3);
              }

              float duty_f = the_ramp.get_value();

              if (duty_f >= 1.)
              {
                ledC->updateDuty(current_led, 0);

                if (current_led == LED_WHITE)
                  current_led = LED_RED;  // red -> blue
                else
                  current_led = LED_WHITE;  // blue -> red

                // Reset the function
                the_ramp.reset(0.);
                timer->start();
                duty_f = the_ramp.get_value();
              }

              // Optionally apply mapping depending on state
              if (state_current == State::CALIBRATION_RAMP_MAP)
                duty_f = blinky_non_linearity(duty_f);

              uint32_t duty = duty_f * duty_max[current_led];
              ledC->updateDuty(current_led, duty);

              vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            break;

          case State::WHITE_SIGNAL:
          case State::RED_SIGNAL:
            {
              if (state_new != state_current)
              {
                state_current = state_new;

                // Turn off all LEDs
                for (int i = 0 ; i < leds.size() ; i++)
                  ledC->updateDuty(leds[i], 0);

                counter = 0;
              }

              timer->start();
              // Only single channel processing
              for (int n=0; n<1; n++) {

                float filter_output = 0.;
                for (int i = 0 ; i < AUDIO_BUFFER_SIZE ; i++) {
                  // remove the mean using a notch filter
                  float sample = dc_rm.process(audio_data[AUDIO_CHANNELS * i + n]);
                  // square to compute the power and accumulate
                  filter_output += sample * sample;
                }
                // divide to obtain mean power in the current buffer
                filter_output /= AUDIO_BUFFER_SIZE;

                // Apply the non-linear transformation
                float val_db = decibels(filter_output);
                float duty_f = map_to_unit_interval(val_db, MIN_DB, MAX_DB);
                duty_f = blinky_non_linearity(duty_f);  // this will also clip in [0, 1]

                // Detect if the signal is too large
                if (duty_f >= 1.)
                  ledC->updateDuty(leds[LED_GREEN], 400);
                else
                  ledC->updateDuty(leds[LED_GREEN], 0);

                // Set the LED duty cycle
                uint32_t duty = 0;
                if (state_current == State::RED_SIGNAL)
                {
                  duty = (uint32_t)(duty_f * duty_max[LED_RED]);
                  ledC->updateDuty(LED_RED, duty);
                }
                else if (state_current == State::WHITE_SIGNAL)
                {
                  duty = (uint32_t)(duty_f * duty_max[LED_WHITE]);
                  ledC->updateDuty(LED_WHITE, duty);
                }

                if (ENABLE_MONITOR and counter % 50 == 0)
                {
                  printf("power_val=%e db=%d duty_f=%e duty=%d dip_val=%d\n",
                      (double)filter_output, (int)val_db, (double)duty_f, (int)duty, (int)dip_switch.read());
                }
                counter += 1;

              }

              elapsed_time = timer->measure();
              if (elapsed_time > (float)AUDIO_BUFFER_SIZE/(float)SAMPLE_RATE*1000.0f)
              {
                // elapsed_time must be less than AUDIO_BUFFER_SIZE/SAMPLE_RATE*1000(msec)
                printf(
                    "elapsed_time must be less than %f(msec)\n",
                    (float)AUDIO_BUFFER_SIZE/(float)SAMPLE_RATE*1000.0f
                );
              }

            }
            break;

          case State::RED_CALIBRATION:
          case State::WHITE_CALIBRATION:
            {
              if (state_new != state_current)
              {
                state_current = state_new;

                // Turn off all LEDs
                for (int i = 0 ; i < leds.size() ; i++)
                  ledC->updateDuty(leds[i], 0);

                counter = 0;
                timer->start();

                if (state_current == State::RED_CALIBRATION)
                  ledC->updateDuty(LED_RED, duty_max[LED_RED]);
                else if (state_current == State::WHITE_CALIBRATION)
                  ledC->updateDuty(LED_WHITE, duty_max[LED_WHITE]);
              }

              float elapsed_time = timer->measure();

              if (elapsed_time >= CALIB_PERIOD_SEC * 1000)
              {
                if (state_current == State::RED_CALIBRATION)
                  ledC->updateDuty(LED_RED, duty_max[LED_RED]);
                else if (state_current == State::WHITE_CALIBRATION)
                  ledC->updateDuty(LED_WHITE, duty_max[LED_WHITE]);

                timer->start();
              }
              else if (elapsed_time >= CALIB_PERIOD_SEC * 500)
              {
                if (state_current == State::RED_CALIBRATION)
                  ledC->updateDuty(LED_RED, 0);
                else if (state_current == State::WHITE_CALIBRATION)
                  ledC->updateDuty(LED_WHITE, 0);
              }

              vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;

        //case State::STATE_7:
        //    state_current = state_new;
        //    vTaskDelay(10 / portTICK_PERIOD_MS);
        //    break;
        case State::DOA:
            {
              if (state_new != state_current)
              {
                state_current = state_new;

                // Turn off all LEDs
                for (int i = 0 ; i < leds.size() ; i++)
                  ledC->updateDuty(leds[i], 0);

                counter = 0;
              }

              timer->start();

              float a[AUDIO_BUFFER_SIZE];
              float b[AUDIO_BUFFER_SIZE];

              // Two channels processing
              for (int i = 0 ; i < AUDIO_BUFFER_SIZE ; i++) {
                // remove the mean using a notch filter
                a[i] = dc_rm.process(audio_data[AUDIO_CHANNELS * i + 0]) * 10;
                b[i] = dc_rm.process(audio_data[AUDIO_CHANNELS * i + 1]) * 10;
              }
              float read_time = timer->measure();
              if (ENABLE_MONITOR and counter % 10 == 0)
              {
                printf("a=%e b=%e\n",
                    (double)a[0],
                    (double)b[0]
                );
              }
              
              // calculate cross correlation
              float *f;
              int len_f = 2*AUDIO_BUFFER_SIZE - 1;
              f = (float *)malloc(len_f * sizeof(float));

              cross_corr_fft(a, AUDIO_BUFFER_SIZE, b, AUDIO_BUFFER_SIZE, f, len_f);
              //cross_corr(a, AUDIO_BUFFER_SIZE, b, AUDIO_BUFFER_SIZE, f, len_f);
              float cross_corr_time = timer->measure();

              // estimate TDOA
              int max_idx = 0;
              float max_corr = -FLT_MAX;
              float max_tdoa = D_MICS / 340.65;
              int max_sample_diff = ceil(max_tdoa*SAMPLE_RATE);
              for(int i=((len_f-1)/2)-max_sample_diff; i<=((len_f-1)/2)+max_sample_diff; i++)
              {
                if(max_corr < f[i]){
                  max_corr = f[i];
                  max_idx = i;
                }
              }
              free(f);

              float tdoa;
              tdoa = ((float)max_idx - (AUDIO_BUFFER_SIZE-1)) / (float)SAMPLE_RATE;
              float tdoa_time = timer->measure();


              // DOA
              float theta;
              theta = doa_cross_corr(tdoa, D_MICS);
              float doa_time = timer->measure();

              float duty_f_array[4] = {0, 0, 0, 0};
              //angle_to_led((float)theta*(2*M_PI)/360, duty_f_array);
              angle_to_led(theta, duty_f_array);
              
              // Set the duty cycle of each LED
              int led_indices[4] = {LED_RED, LED_WHITE, LED_BLUE, LED_GREEN};
              for (int i = 0; i < 4; i++){
                uint32_t duty = 0;
                duty = (uint32_t)(duty_f_array[i] * duty_max[led_indices[i]]);
                ledC->updateDuty(led_indices[i], duty);
              }
              float led_time = timer->measure();

              if (ENABLE_MONITOR and counter % 10 == 0)
              {
                printf("theta=%e isnan=%d a=%e b=%e max_idx=%d tdoa=%e duty_f_red=%e duty_f_white=%e duty_f_blue=%e duty_f_green=%e dip_val=%d\n",
                    (double)theta,
                    (int)std::isnan(theta),
                    (double)a[0],
                    (double)b[0],
                    (int)max_idx,
                    (double)tdoa,
                    (double)duty_f_array[0],
                    (double)duty_f_array[1],
                    (double)duty_f_array[2],
                    (double)duty_f_array[3],
                    (int)dip_switch.read());
                printf(
                    "read_time=%f cross_corr_time=%f tdoa_time=%f doa_time=%f led_time=%f\n",
                    read_time,
                    cross_corr_time,
                    tdoa_time,
                    doa_time,
                    led_time
                );
              }
              counter += 1;

              elapsed_time = timer->measure();
              if (elapsed_time > (float)AUDIO_BUFFER_SIZE/(float)SAMPLE_RATE*1000.0f)
              {
                // elapsed_time must be less than AUDIO_BUFFER_SIZE/SAMPLE_RATE*1000(msec)
                printf(
                    "elapsed_time must be less than %f(msec)\n",
                    (float)AUDIO_BUFFER_SIZE/(float)SAMPLE_RATE*1000.0f
                );
              }

            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
            break;
        }
    }
}
