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

/*$Id: db_feature_detection.h,v 1.3 2011/06/17 14:03:30 mbansal Exp $*/

#ifndef DB_FEATURE_DETECTION_H
#define DB_FEATURE_DETECTION_H

/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/
/*!
 * \defgroup FeatureDetection Feature Detection
 */
#include "db_utilities.h"
#include "db_utilities_constants.h"
#include <stdlib.h> //for NULL

/*!
 * \class db_CornerDetector_u
 * \ingroup FeatureDetection
 * \brief Harris corner detector for byte images.
 *
 *  This class performs Harris corner extraction on *byte* images managed
 * with functions in \ref LMImageBasicUtilities.
 */
class db_CornerDetector_u
{
public:
    db_CornerDetector_u();
    virtual ~db_CornerDetector_u();

    /*!
     Copy ctor duplicates settings.
     Memory is not copied.
     */
    db_CornerDetector_u(const db_CornerDetector_u& cd);
    /*!
     Assignment optor duplicates settings.
     Memory not copied.
     */
    db_CornerDetector_u& operator=(const db_CornerDetector_u& cd);

    /*!
     * Set parameters and pre-allocate memory. Return an upper bound
     * on the number of corners detected in one frame
     */
    virtual unsigned long Init(int im_width,int im_height,
                            int target_nr_corners=DB_DEFAULT_TARGET_NR_CORNERS,
                            int nr_horizontal_blocks=DB_DEFAULT_NR_FEATURE_BLOCKS,
                            int nr_vertical_blocks=DB_DEFAULT_NR_FEATURE_BLOCKS,
                            float absolute_threshold=DB_DEFAULT_ABS_CORNER_THRESHOLD,
                            float relative_threshold=DB_DEFAULT_REL_CORNER_THRESHOLD);

    /*!
     * Detect the corners.
     * Observe that the image should be overallocated by at least 256 bytes
     * at the end.
     * x_coord and y_coord should be pre-allocated arrays of length returned by Init().
     * Specifying image mask will restrict corner output to foreground regions.
     * Foreground value can be specified using fgnd. By default any >0 mask value
     * is considered to be foreground
     * \param img   row array pointer
     * \param x_coord   corner locations
     * \param y_coord   corner locations
     * \param nr_corners    actual number of corners computed
     * \param msk       row array pointer to mask image
     * \param fgnd      foreground value in the mask
     */
    virtual void DetectCorners(const unsigned char * const *img,float *x_coord,float *y_coord,int *nr_corners,
        const unsigned char * const * msk=NULL, unsigned char fgnd=255);

    /*!
     Set absolute feature threshold
     */
    virtual void SetAbsoluteThreshold(float a_thresh) { m_a_thresh = a_thresh; };
    /*!
     Set relative feature threshold
     */
    virtual void SetRelativeThreshold(float r_thresh) { m_r_thresh = r_thresh; };

    /*!
     Extract corners from a pre-computed strength image.
     \param strength    Harris strength image
     \param x_coord corner locations
     \param y_coord corner locations
     \param nr_corners  actual number of corners computed
     */
    virtual void ExtractCorners(float ** strength, float *x_coord, float *y_coord, int *nr_corners);
protected:
    virtual void Clean();

    /*The absolute threshold to this function should be 16.0 times
    normal*/
    unsigned long Start(int im_width,int im_height,
			int block_width, int block_height,
			unsigned long area_factor,
			float absolute_threshold,
			float relative_threshold);

    void db_HarrisStrength_u(float **s, const unsigned char * const *img,
			     int w, int h);
    inline void db_HarrisStrengthChunk_u(float **s, const unsigned char * const *img,
					 int left, int top, int bottom, int nc);
    inline void db_IxIyRow_u(const unsigned char * const *img,int i,int j,int nc);
    inline void db_gxx_gxy_gyy_row_s(int i, int nc);
    inline void db_HarrisStrength_row_s(float *s, int i, int nc);
    inline void db_Filter14641_128_i(int i, int nc);

    int m_w, m_h, m_bw, m_bh;
    /*Area factor holds the maximum number of corners to detect
    per 10000 pixels*/
    unsigned long m_area_factor,m_max_nr;
    float m_a_thresh,m_r_thresh;
    float *m_temp_d;

#ifdef DEBUG
    int **m_ix;
    int **m_iy;
#endif
    int **m_ix2;
    int **m_iy2;
    int **m_ixy;
    int **m_gx2;
    int **m_gy2;
    int **m_gxy;

    float **m_strength;
};

#endif /*DB_FEATURE_DETECTION_H*/
