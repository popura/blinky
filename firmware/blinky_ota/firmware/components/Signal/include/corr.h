//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//
//  These are functions implementing auto/cross correlation.
//

#ifndef __CORR_H__
#define __CORR_H__

#include <malloc.h>

void cross_corr(float *a, float *b, float *f);
void auto_corr(float *a, float *f);

#endif // __CORR_H__
