//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//
//  This is a class implementing direction of arrival (DOA).
//

#ifndef __DOA_H_
#define __DOA_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "corr.h"

float doa_cross_corr(float *a, float *b, float interval, float fs);

#endif // __DOA_H__
