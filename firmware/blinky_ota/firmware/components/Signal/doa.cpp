//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//

#include "doa.h"

float doa_cross_corr(float *a, int len_a, float *b, int len_b, float interval, float fs)
{
  float SOUND_SPEED = 340.65; // [m/s]
  float max_corr = 0;
  int max_idx = 0;
  float tdoa = 0;
  float theta = 0;

  float *f;
  int len_f = len_a + len_b - 1;
  f = (float *)malloc(len_f * sizeof(float));

  cross_corr(a, len_a, b, len_b, f, len_f);

  for(int i=0;i<len_f;i++)
  {
    if(max_corr < f[i]){
      max_corr = f[i];
      max_idx = i;
    }
  }
  
  tdoa = (float)(max_idx - len_b) / fs;
  theta = acos((tdoa * SOUND_SPEED) / interval);
  free(f);
  return theta;
}
