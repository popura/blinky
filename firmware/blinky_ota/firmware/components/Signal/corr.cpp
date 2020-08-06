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

void auto_corr(float *a, int len_a, float *f, int len_f)
{
  cross_corr(a, len_a, a, len_a, f, len_f);
  return;
}
