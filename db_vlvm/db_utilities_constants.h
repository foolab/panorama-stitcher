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

/* $Id: db_utilities_constants.h,v 1.2 2011/06/17 14:03:31 mbansal Exp $ */

#ifndef DB_UTILITIES_CONSTANTS
#define DB_UTILITIES_CONSTANTS

/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/

/****************Constants********************/
#define DB_SQRT3         1.7320508075688772935274463415059

/****************Default Parameters********************/
/*Preemptive ransac parameters*/
#define DB_DEFAULT_NR_SAMPLES 500
#define DB_DEFAULT_CHUNK_SIZE 100
#define DB_DEFAULT_GROUP_SIZE 10

/*Optimisation parameters*/
#define DB_DEFAULT_MAX_POINTS 1000
#define DB_DEFAULT_MAX_ITERATIONS 25
#define DB_DEFAULT_IMP_REQ 0.001

/*Feature standard deviation parameters*/
#define DB_POINT_STANDARDDEV (1.0f/(826.0f)) /*1 pixel for CIF (fraction of (image width+image height)/2)*/
#define DB_OUTLIER_THRESHOLD 3.0f /*In number of DB_POINT_STANDARDDEV's*/
#define DB_WORST_CASE 50.0f /*In number of DB_POINT_STANDARDDEV's*/

/*Front-end parameters*/
#define DB_DEFAULT_TARGET_NR_CORNERS 5000
#define DB_DEFAULT_NR_FEATURE_BLOCKS 10
#define DB_DEFAULT_ABS_CORNER_THRESHOLD 50000000.0f
#define DB_DEFAULT_REL_CORNER_THRESHOLD 0.00005f
#define DB_DEFAULT_MAX_DISPARITY 0.1f
#define DB_DEFAULT_NO_DISPARITY -1.0f
#define DB_DEFAULT_MAX_TRACK_LENGTH 300

#define DB_DEFAULT_MAX_NR_CAMERAS 1000

#define DB_DEFAULT_TRIPLE_STEP 2
#define DB_DEFAULT_DOUBLE_STEP 2
#define DB_DEFAULT_SINGLE_STEP 1
#define DB_DEFAULT_NR_SINGLES 10
#define DB_DEFAULT_NR_DOUBLES 1
#define DB_DEFAULT_NR_TRIPLES 1

#define DB_DEFAULT_TRIFOCAL_FOUR_STEPS 40

#define DB_DEFAULT_EPIPOLAR_ERROR 1 /*in pixels*/

////////////////////////// DOXYGEN /////////////////////

/*!
 * \def DB_DEFAULT_GROUP_SIZE
 * \ingroup LMRobust
 * \brief Default group size for db_PreemptiveRansac class.
 * Group size is the number of observation costs multiplied together
 * before a log of the product is added to the total cost.
*/

/*!
 * \def DB_DEFAULT_TARGET_NR_CORNERS
 * \ingroup FeatureDetection
 * \brief Default target number of corners
*/
/*!
 * \def DB_DEFAULT_NR_FEATURE_BLOCKS
 * \ingroup FeatureDetection
 * \brief Default number of regions (horizontal or vertical) that are considered separately
 * for feature detection. The greater the number, the more uniform the distribution of
 * detected features.
*/
/*!
 * \def DB_DEFAULT_ABS_CORNER_THRESHOLD
 * \ingroup FeatureDetection
 * \brief Absolute feature strength threshold.
*/
/*!
 * \def DB_DEFAULT_REL_CORNER_THRESHOLD
 * \ingroup FeatureDetection
 * \brief Relative feature strength threshold.
*/
/*!
 * \def DB_DEFAULT_MAX_DISPARITY
 * \ingroup FeatureMatching
 * \brief Maximum disparity (as fraction of image size) allowed in feature matching
*/
 /*!
 * \def DB_DEFAULT_NO_DISPARITY
 * \ingroup FeatureMatching
 * \brief Indicates that vertical disparity is the same as horizontal disparity.
*/
///////////////////////////////////////////////////////////////////////////////////
 /*!
 * \def DB_E
 * \ingroup LMBasicUtilities
 * \brief e
*/
 /*!
 * \def DB_LOG2E
 * \ingroup LMBasicUtilities
 * \brief log2(e)
*/
 /*!
 * \def DB_LOG10E
 * \ingroup LMBasicUtilities
 * \brief log10(e)
*/
 /*!
 * \def DB_LOG10E
 * \ingroup LMBasicUtilities
 * \brief log10(e)
*/
/*!
 * \def DB_LN2
 * \ingroup LMBasicUtilities
 * \brief ln(2)
*/
/*!
 * \def DB_LN10
 * \ingroup LMBasicUtilities
 * \brief ln(10)
*/
/*!
 * \def DB_PI
 * \ingroup LMBasicUtilities
 * \brief Pi
*/
/*!
 * \def DB_PI_2
 * \ingroup LMBasicUtilities
 * \brief Pi/2
*/
/*!
 * \def DB_PI_4
 * \ingroup LMBasicUtilities
 * \brief Pi/4
*/
/*!
 * \def DB_1_PI
 * \ingroup LMBasicUtilities
 * \brief 1/Pi
*/
/*!
 * \def DB_2_PI
 * \ingroup LMBasicUtilities
 * \brief 2/Pi
*/
/*!
 * \def DB_SQRTPI
 * \ingroup LMBasicUtilities
 * \brief sqrt(Pi)
*/
/*!
 * \def DB_SQRT_2PI
 * \ingroup LMBasicUtilities
 * \brief sqrt(2*Pi)
*/
/*!
 * \def DB_SQRT2
 * \ingroup LMBasicUtilities
 * \brief sqrt(2)
*/
/*!
 * \def DB_SQRT3
 * \ingroup LMBasicUtilities
 * \brief sqrt(3)
*/
/*!
 * \def DB_SQRT1_2
 * \ingroup LMBasicUtilities
 * \brief sqrt(1/2)
*/
#endif /* DB_UTILITIES_CONSTANTS */


