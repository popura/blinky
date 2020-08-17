//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//
//  These are functions implementing auto/cross correlation.
//

#ifndef __CORR_H__
#define __CORR_H__

#include <malloc.h>
#include <stdint.h>

extern "C" {
    #include "fft.h"
}

void cross_corr(float *a, int len_a, float *b, int len_b, float *f, int len_f);
void cross_corr_fft(float *a, int len_a, float *b, int len_b, float *f, int len_f);
void auto_corr(float *a, int len_a, float *f, int len_f);
float parafit(int x, float f[3]);

#endif // __CORR_H__
