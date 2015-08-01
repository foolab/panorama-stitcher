/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

///////////////////////////////////////////////////////////
// Interp.h
// $Id: Interp.h,v 1.2 2011/06/17 13:35:48 mbansal Exp $

#ifndef INTERP_H
#define INTERP_H

#include "Pyramid.h"

#define CTAPS 40
static float ciTable[81] = {
        1.0f, 0.998461f, 0.993938f, 0.98657f, 0.9765f,
        0.963867f, 0.948813f, 0.931477f, 0.912f, 0.890523f,
        0.867188f, 0.842133f, 0.8155f, 0.78743f, 0.758062f,
        0.727539f, 0.696f, 0.663586f, 0.630437f, 0.596695f,
        0.5625f, 0.527992f, 0.493312f, 0.458602f, 0.424f,
        0.389648f, 0.355687f, 0.322258f, 0.2895f, 0.257555f,
        0.226562f, 0.196664f, 0.168f, 0.140711f, 0.114937f,
        0.0908203f, 0.0685f, 0.0481172f, 0.0298125f, 0.0137266f,
        0.0f, -0.0118828f, -0.0225625f, -0.0320859f, -0.0405f,
        -0.0478516f, -0.0541875f, -0.0595547f, -0.064f, -0.0675703f,
        -0.0703125f, -0.0722734f, -0.0735f, -0.0740391f, -0.0739375f,
        -0.0732422f, -0.072f, -0.0702578f, -0.0680625f, -0.0654609f,
        -0.0625f, -0.0592266f, -0.0556875f, -0.0519297f, -0.048f,
        -0.0439453f, -0.0398125f, -0.0356484f, -0.0315f, -0.0274141f,
        -0.0234375f, -0.0196172f, -0.016f, -0.0126328f, -0.0095625f,
        -0.00683594f, -0.0045f, -0.00260156f, -0.0011875f, -0.000304687f, 0.0f
};

inline float ciCalc(PyramidShort *img, int xi, int yi, float xfrac, float yfrac)
{
  float tmpf[4];

  // Interpolate using 16 points
  ImageTypeShortBase *in = img->ptr[yi-1] + xi - 1;
  int off = (int)(xfrac * CTAPS);

  tmpf[0] = in[0] * ciTable[off + 40];
  tmpf[0] += in[1] * ciTable[off];
  tmpf[0] += in[2] * ciTable[40 - off];
  tmpf[0] += in[3] * ciTable[80 - off];
  in += img->pitch;
  tmpf[1] = in[0] * ciTable[off + 40];
  tmpf[1] += in[1] * ciTable[off];
  tmpf[1] += in[2] * ciTable[40 - off];
  tmpf[1] += in[3] * ciTable[80 - off];
  in += img->pitch;
  tmpf[2] = in[0] * ciTable[off + 40];
  tmpf[2] += in[1] * ciTable[off];
  tmpf[2] += in[2] * ciTable[40 - off];
  tmpf[2] += in[3] * ciTable[80 - off];
  in += img->pitch;
  tmpf[3] = in[0] * ciTable[off + 40];
  tmpf[3] += in[1] * ciTable[off];
  tmpf[3] += in[2] * ciTable[40 - off];
  tmpf[3] += in[3] * ciTable[80 - off];

  // this is the final interpolation
  off = (int)(yfrac * CTAPS);
  return (ciTable[off + 40] * tmpf[0] + ciTable[off] * tmpf[1] +
          ciTable[40 - off] * tmpf[2] + ciTable[80 - off] * tmpf[3]);
}

#endif
