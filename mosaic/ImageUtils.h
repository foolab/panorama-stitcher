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
// ImageUtils.h
// $Id: ImageUtils.h,v 1.9 2011/05/16 15:33:06 mbansal Exp $

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <stdlib.h>

/**
 *  Definition of basic image types
 */
typedef unsigned char ImageTypeBase;
typedef ImageTypeBase *ImageType;

typedef short ImageTypeShortBase;
typedef ImageTypeShortBase *ImageTypeShort;

class ImageUtils {
public:

  /**
   *  Default number of channels in image.
   */
  static const int IMAGE_TYPE_NUM_CHANNELS = 3;

  /**
   *  Definition of an empty image.
   */
  static const int IMAGE_TYPE_NOIMAGE = 0;

  /**
   *  Convert image from YVU (non-interlaced) to BGR (interlaced)
   *
   *  Arguments:
   *    out: Resulting image (note must be preallocated before
   *    call)
   *    in: Input image
   *    width: Width of input image
   *    height: Height of input image
   */
  static void yvu2rgb(ImageType out, ImageType in, int width, int height);

  /**
   *  Allocate space for a standard image.
   */
  static ImageType allocateImage(int width, int height, int numChannels);

  /**
   *  Free memory of image
   */
  static void freeImage(ImageType image);

  static ImageType *imageTypeToRowPointers(ImageType out, int width, int height);
  /**
   *  Get time.
   */
  static float getTime();

protected:

  /**
  *  Constants for YVU/RGB conversion
  */
  static const int REDY = 257;
  static const int REDV = 439;
  static const int REDU = 148;
  static const int GREENY = 504;
  static const int GREENV = 368;
  static const int GREENU = 291;
  static const int BLUEY = 98;
  static const int BLUEV = 71;
  static const int BLUEU = 439;

};

/**
 * Structure containing an image and other bookkeeping items.
 * Used in YUVinfo to store separate YVU image planes.
 */
typedef struct {
  unsigned char **ptr;
  unsigned short width;
  unsigned short height;
} BimageInfo;

/**
 * A YUV image container,
 */
class YUVinfo {
public:
  YUVinfo(int width, int height);
  ~YUVinfo();

  /**
  * Y Plane
  */
  BimageInfo Y;

  /**
  *  V (1st color) plane
  */
  BimageInfo V;

  /**
  *  U (1st color) plane
  */
  BimageInfo U;

  /**
   * Actual data
   */
  unsigned char *data;
};

#endif
