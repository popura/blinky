//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//

#include <math.h>
#include "doa.h"
#include "corr.h"

float *doa_cross_corr(float *a, float *b, float interval, float fs)
{
  float SOUND_SPEED = 340.65; // [m/s]
  float *f;
  float max_corr = 0;
  float max_idx = 0;
  float tdoa = 0;
  float theta = 0;
  int len_f;
  int len_b;

  f = cross_corr(a, b);
  len_f = sizeof(f) / sizeof(f[0]);
  len_b = sizeof(b) / sizeof(b[0]);

  for(int i=0;i<len_f;i++)
  {
    if(max_corr < f[i]){
      max_corr = f[i];
      max_idx = i;
    }
  }
  free(f)
  
  tdoa = (float)(max_idx - len_b) / fs;
  theta = arccos((tdoa * SOUND_SPEED) / interval);
  return theta
}
