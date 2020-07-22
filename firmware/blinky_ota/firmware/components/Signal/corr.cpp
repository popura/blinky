//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//
#include <malloc.h>
#include "corr.h"

float *cross_corr(float *a, float *b)
{
  float *f;
  int len_a = sizeof(a) / sizeof(a[0]);
  int len_b = sizeof(b) / sizeof(b[0]);
  int len_f = len_a + len_b - 1;
  int shift = 0;
  int tmp_b = 0;

  f = (int *)malloc(len_f * sizeof(float));

  for(int i=0; i<len_f; i++){
    shift = i - len_b + 1
    f[i] = 0;
    for(int j=0; j<len_a; j++){
      // is index for b valid?
      if((j - shift) < len_b && (j - shift) >= 0){
        tmp_b = b[j - shift]
      }else{
        tmp_b = 0
      }
      f[i] += a[j] * tmp_b;
    }
  }
  return f;
}

float *auto_corr(float *a)
{
  return cross_corr(a, a)
}
