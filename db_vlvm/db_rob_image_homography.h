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

/* $Id: db_rob_image_homography.h,v 1.3 2011/06/17 14:03:31 mbansal Exp $ */

#ifndef DB_ROB_IMAGE_HOMOGRAPHY
#define DB_ROB_IMAGE_HOMOGRAPHY

#include "db_utilities.h"
#include "db_robust.h"
#include "db_metrics.h"

#include <stdlib.h> // for NULL


/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/
/*!
 * \defgroup LMRobImageHomography (LM) Robust Image Homography
 */
/*\{*/

#define DB_HOMOGRAPHY_TYPE_DEFAULT           0
#define DB_HOMOGRAPHY_TYPE_PROJECTIVE        0
#define DB_HOMOGRAPHY_TYPE_AFFINE            1
#define DB_HOMOGRAPHY_TYPE_SIMILARITY        2
#define DB_HOMOGRAPHY_TYPE_SIMILARITY_U      3
#define DB_HOMOGRAPHY_TYPE_TRANSLATION       4
#define DB_HOMOGRAPHY_TYPE_ROTATION          5
#define DB_HOMOGRAPHY_TYPE_ROTATION_U        6
#define DB_HOMOGRAPHY_TYPE_SCALING           7
#define DB_HOMOGRAPHY_TYPE_S_T               8
#define DB_HOMOGRAPHY_TYPE_R_T               9
#define DB_HOMOGRAPHY_TYPE_R_S              10
#define DB_HOMOGRAPHY_TYPE_CAMROTATION      11
#define DB_HOMOGRAPHY_TYPE_CAMROTATION_F    12
#define DB_HOMOGRAPHY_TYPE_CAMROTATION_F_UD 13

/*!
Solve for homography H such that xp~Hx
\param H    best homography

2D point to 2D point constraints:

\param im           first image points
\param im_p         second image points
\param nr_points    number of points

Calibration matrices:

\param K    first camera
\param Kp   second camera

 Temporary space:

 \param temp_d      pre-allocated space of size 12*nr_samples+10*nr_points floats
 \param temp_i      pre-allocated space of size max(nr_samples,nr_points) ints

 Statistics for this estimation

 \param stat        NULL - do not compute

 \param homography_type see DB_HOMOGRAPHY_TYPE_* definitions above

 Estimation parameters:

 \param max_iterations  max number of polishing steps
 \param max_points      only use this many points
 \param scale           Cauchy scale coefficient (see db_ExpCauchyReprojectionError() )
 \param nr_samples      number of times to compute a hypothesis
 \param chunk_size      size of cost chunks
*/
void db_RobImageHomography(
                              /*Best homography*/
                              float H[9],
                              /*2DPoint to 2DPoint constraints
                              Points are assumed to be given in
                              homogenous coordinates*/
                              float *im,float *im_p,
                              /*Nr of points in total*/
                              int nr_points,
                              /*Calibration matrices
                              used to normalize the points*/
                              float K[9],
                              float Kp[9],
                              /*Pre-allocated space temp_d
                              should point to at least
                              12*nr_samples+10*nr_points
                              allocated positions*/
                              float *temp_d,
                              /*Pre-allocated space temp_i
                              should point to at least
                              max(nr_samples,nr_points)
                              allocated positions*/
                              int *temp_i,
                              int homography_type=DB_HOMOGRAPHY_TYPE_DEFAULT,
                              db_Statistics *stat=NULL,
                              int max_iterations=DB_DEFAULT_MAX_ITERATIONS,
                              int max_points=DB_DEFAULT_MAX_POINTS,
                              float scale=DB_POINT_STANDARDDEV,
                              int nr_samples=DB_DEFAULT_NR_SAMPLES,
                              int chunk_size=DB_DEFAULT_CHUNK_SIZE,
                              ///////////////////////////////////////////////////
                              // flag for the outlier removal
                              int outlierremoveflagE = 0,
                              // if flag is 1, then the following variables
                              // need to input
                              ///////////////////////////////////////////////////
                              // 3D coordinates
                              float *wp=NULL,
                              // its corresponding stereo pair's points
                              float *im_r=NULL,
                              // raw image coordinates
                              float *im_raw=NULL, float *im_raw_p=NULL,
                              // final matches
                              int *final_NumE=0);

float db_RobImageHomography_Cost(float H[9],int point_count,float *x_i,
                                                float *xp_i,float one_over_scale2);


void db_RobCamRotation_Polish(float H[9],int point_count,float *x_i,
                                     float *xp_i, float one_over_scale2,
                                     int max_iterations=DB_DEFAULT_MAX_ITERATIONS,
                                     float improvement_requirement=DB_DEFAULT_IMP_REQ);


void db_RobCamRotation_Polish_Generic(float H[9],int point_count,int homography_type,
                                             float *x_i,float *xp_i,float one_over_scale2,
                                             int max_iterations=DB_DEFAULT_MAX_ITERATIONS,
                                             float improvement_requirement=DB_DEFAULT_IMP_REQ);


#endif /* DB_ROB_IMAGE_HOMOGRAPHY */
