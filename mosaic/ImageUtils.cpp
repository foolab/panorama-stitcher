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

///////////////////////////////////////////////////
// ImageUtils.cpp
// $Id: ImageUtils.cpp,v 1.12 2011/06/17 13:35:48 mbansal Exp $


#include <string.h>
#include "ImageUtils.h"

ImageType *ImageUtils::imageTypeToRowPointers(ImageType in, int width, int height)
{
  int i;
  int m_h = height;
  int m_w = width;

  ImageType *m_rows = new ImageType[m_h];

  for (i=0;i<m_h;i++) {
    m_rows[i] = &in[(m_w)*i];
  }
  return m_rows;
}

void ImageUtils::yvu2rgb(ImageType out, ImageType in, int width, int height)
{
  int y,v,u, r, g, b;
  unsigned char *yimg = in;
  unsigned char *vimg = yimg + width*height;
  unsigned char *uimg = vimg + width*height;
  unsigned char *image = out;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {

      y = (*yimg);
      v = (*vimg);
      u = (*uimg);

      if (y < 0) y = 0;
      if (y > 255) y = 255;
      if (u < 0) u = 0;
      if (u > 255) u = 255;
      if (v < 0) v = 0;
      if (v > 255) v = 255;

      b = (int) ( 1.164*(y - 16) + 2.018*(u-128));
      g = (int) ( 1.164*(y - 16) - 0.813*(v-128) - 0.391*(u-128));
      r = (int) ( 1.164*(y - 16) + 1.596*(v-128));

      if (r < 0) r = 0;
      if (r > 255) r = 255;
      if (g < 0) g = 0;
      if (g > 255) g = 255;
      if (b < 0) b = 0;
      if (b > 255) b = 255;

      *(image++) = r;
      *(image++) = g;
      *(image++) = b;

      yimg++;
      uimg++;
      vimg++;

    }
  }
}

ImageType ImageUtils::allocateImage(int width, int height, int numChannels)
{
 return (ImageType) calloc(width*height*numChannels, sizeof(ImageTypeBase));
}


void ImageUtils::freeImage(ImageType image)
{
  free(image);
}

YUVinfo::YUVinfo(int width, int height) {
  Y.width = V.width = U.width = width;
  Y.height = V.height = U.height = height;

  data = new unsigned char[width * height * 3];

  // Set the Y image to 255 so we can distinguish when frame idx are written to it
  memset(data, 255, width * height * sizeof(unsigned char));

  // Set the v and u images to black
  memset(&data[width * height], 128, width * height * 2 * sizeof(unsigned char));

  Y.ptr = new unsigned char *[height];
  V.ptr = new unsigned char *[height];
  U.ptr = new unsigned char *[height];

  for (int x = 0; x < height; x++) {
    Y.ptr[x] = &data[x * width];
    V.ptr[x] = &data[width*height + x*width];
    U.ptr[x] = &data[width*height*2 + x*width];
  }
}

YUVinfo::~YUVinfo() {
  delete[] data;
  delete[] Y.ptr;
  delete[] V.ptr;
  delete[] U.ptr;
}
