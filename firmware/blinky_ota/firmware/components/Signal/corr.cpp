//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//

#include "corr.h"

void cross_corr(float *a, float *b, float *f)
{
  int len_a = sizeof(a) / sizeof(a[0]);
  int len_b = sizeof(b) / sizeof(b[0]);
  int len_f = sizeof(f) / sizeof(f[0]);
  int shift = 0;
  int tmp_b = 0;


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

void auto_corr(float *a, float *f)
{
  cross_corr(a, a, f);
  return;
}
