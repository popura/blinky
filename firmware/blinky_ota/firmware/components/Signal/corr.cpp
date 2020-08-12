//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//

#include "corr.h"

void cross_corr(float *a, int len_a, float *b, int len_b, float *f, int len_f)
{
  int shift = 0;
  float tmp_b = 0;


  for(int i=0; i<len_f; i++){
    shift = len_b - i - 1;
    f[i] = 0;
    for(int j=0; j<len_a; j++){
      // is index for b valid?
      if(((j + shift) < len_b) && ((j + shift) >= 0)){
        tmp_b = b[j + shift];
      }else{
        tmp_b = 0;
      }
      f[i] += a[j] * tmp_b;
    }
  }
  return;
}

void cross_corr_fft(float *a, int len_a, float *b, int len_b, float *f, int len_f)
{
  uint32_t n_fft = 1;
  uint32_t tmp_n = (uint32_t)len_f;
  if (tmp_n && !(tmp_n & (tmp_n - 1))){
    n_fft = tmp_n;
  }else{
    while (n_fft < tmp_n)
      n_fft <<= 1;
  }
  
  fft_config_t *a_fft_plan = fft_init(n_fft, FFT_REAL, FFT_FORWARD, NULL, NULL);
  fft_config_t *b_fft_plan = fft_init(n_fft, FFT_REAL, FFT_FORWARD, NULL, NULL);
  for (int i = 0; i < n_fft; i++){
    a_fft_plan->input[i] = (float)0;
    b_fft_plan->input[i] = (float)0;
  }

  for (int i = 0; i < len_a; i++){
    a_fft_plan->input[len_b - 1 + i] = a[i];
  }
  fft_execute(a_fft_plan);

  for (int i = 0; i < len_b; i++){
    b_fft_plan->input[i] = b[i];
  }
  fft_execute(b_fft_plan);

  fft_config_t *f_fft_plan = fft_init(n_fft, FFT_REAL, FFT_BACKWARD, NULL, NULL);
  f_fft_plan->input[0] = a_fft_plan->output[0] * b_fft_plan->output[0];
  f_fft_plan->input[1] = a_fft_plan->output[1] * b_fft_plan->output[1];
  for (int i = 1; i < f_fft_plan->size / 2; i++){
    f_fft_plan->input[2*i] = (a_fft_plan->output[2*i] * b_fft_plan->output[2*i])
                           + (a_fft_plan->output[2*i+1] + b_fft_plan->output[2*i+1]);
    f_fft_plan->input[2*i+1] = -1 * (a_fft_plan->output[2*i] * b_fft_plan->output[2*i+1])
                             + (a_fft_plan->output[2*i+1] + b_fft_plan->output[2*i]);
  }

  fft_execute(f_fft_plan);

  for (int i = 0; i < len_f; i++){
    f[i] = f_fft_plan->output[i];
  }

  fft_destroy(a_fft_plan);
  fft_destroy(b_fft_plan);
  fft_destroy(f_fft_plan);

  return;
}

void auto_corr(float *a, int len_a, float *f, int len_f)
{
  cross_corr(a, len_a, a, len_a, f, len_f);
  return;
}
