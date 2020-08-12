//
//  Created by Yuma Kinoshita
//  Copyright (c) 2020 Yuma Kinoshita. All rights reserved.
//

#include "doa.h"

float doa_cross_corr(float tdoa, float interval)
{
  float SOUND_SPEED = 340.65; // [m/s]
  float theta = 0;

  theta = (float)acos((double)(tdoa * SOUND_SPEED) / interval);
  return theta;
}
