/*
 * This file contains the definitions of some of the functions
 * used in the blinky
 */
#include "blinky_functions.h"

float clip(float x, float floor, float ceil)
{
  if (x < floor)
    return floor;
  else if (x > ceil)
    return ceil;
  else
    return x;
}

float mu_law(float d, float mu)
{
  static float c = 1.f / logf(1.f + mu);
  return logf(1 + mu * d) * c;
}

float sigmoid(float d, float min, float max, float margin)
{
  static float x_h = logf( (1 - margin) / (1 - (1 - margin)) );
  static float loc = (max + min) / 2.;
  static float scale = (max - min) / 2. / x_h;
  return 1. / (1. + expf(- (d - loc) / scale));
}

void angle_to_led(float theta, float led_duties[4])
{
  float led_theta[4] = {M_PI/3, 2*M_PI/3, 4*M_PI/3, 5*M_PI/3};

  if (std::isnan(theta)){
    return;
  }

  for (int i = 0; i < 4; i++){
    led_duties[i] = 0;
  }

  if (0 <= theta && theta < led_theta[0]){
      led_duties[0] = (theta - (led_theta[3]-2*M_PI)) / (led_theta[0] - (led_theta[3]-2*M_PI));
      led_duties[3] = 1 - led_duties[0];
  }
  if (led_theta[3] <= theta && theta < 2*M_PI){
      led_duties[0] = ((theta-2*M_PI) - (led_theta[3]-2*M_PI)) / (led_theta[0] - (led_theta[3]-2*M_PI));
      led_duties[3] = 1 - led_duties[0];
  }
  if (led_theta[0] <= theta && theta < led_theta[1]){
      led_duties[1] = (theta - led_theta[0]) / (led_theta[1] - led_theta[0]);
      led_duties[0] = 1 - led_duties[1];
  }
  if (led_theta[1] <= theta && theta < led_theta[2]){
      led_duties[2] = (theta - led_theta[1]) / (led_theta[2] - led_theta[1]);
      led_duties[1] = 1 - led_duties[2];
  }
  if(led_theta[2] <= theta && theta < led_theta[3]){
      led_duties[3] = (theta - led_theta[2]) / (led_theta[3] - led_theta[2]);
      led_duties[2] = 1 - led_duties[3];
  }
  return;
}
