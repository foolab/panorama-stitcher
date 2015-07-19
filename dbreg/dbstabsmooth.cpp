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

#include <stdlib.h>
#include "dbstabsmooth.h"

///// TODO TODO ////////// Replace this with the actual definition from Jayan's reply /////////////
#define vp_copy_motion_no_id vp_copy_motion
///////////////////////////////////////////////////////////////////////////////////////////////////

db_StabilizationSmoother::db_StabilizationSmoother()
{
    Init();
}

void db_StabilizationSmoother::Init()
{
    f_smoothOn = true;
    f_smoothReset = false;
    f_smoothFactor = 1.0f;
    f_minDampingFactor = 0.2f;
    f_zoom = 1.0f;
    VP_MOTION_ID(f_motLF);
    VP_MOTION_ID(f_imotLF);
    f_hsize = 0;
    f_vsize = 0;

    VP_MOTION_ID(f_disp_mot);
    VP_MOTION_ID(f_src_mot);
    VP_MOTION_ID(f_diff_avg);

    for( int i = 0; i < MOTION_ARRAY-1; i++) {
        VP_MOTION_ID(f_hist_mot_speed[i]);
        VP_MOTION_ID(f_hist_mot[i]);
        VP_MOTION_ID(f_hist_diff_mot[i]);
    }
    VP_MOTION_ID(f_hist_mot[MOTION_ARRAY-1]);

}

db_StabilizationSmoother::~db_StabilizationSmoother()
{}


bool db_StabilizationSmoother::smoothMotion(VP_MOTION *inmot, VP_MOTION *outmot)
{
    VP_MOTION_ID(f_motLF);
    VP_MOTION_ID(f_imotLF);
    f_motLF.insid = inmot->refid;
    f_motLF.refid = inmot->insid;

    if(f_smoothOn) {
        if(!f_smoothReset) {
            MXX(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MXX(f_motLF) + (1.0-f_smoothFactor)* (float) MXX(*inmot));
            MXY(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MXY(f_motLF) + (1.0-f_smoothFactor)* (float) MXY(*inmot));
            MXZ(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MXZ(f_motLF) + (1.0-f_smoothFactor)* (float) MXZ(*inmot));
            MXW(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MXW(f_motLF) + (1.0-f_smoothFactor)* (float) MXW(*inmot));

            MYX(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MYX(f_motLF) + (1.0-f_smoothFactor)* (float) MYX(*inmot));
            MYY(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MYY(f_motLF) + (1.0-f_smoothFactor)* (float) MYY(*inmot));
            MYZ(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MYZ(f_motLF) + (1.0-f_smoothFactor)* (float) MYZ(*inmot));
            MYW(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MYW(f_motLF) + (1.0-f_smoothFactor)* (float) MYW(*inmot));

            MZX(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MZX(f_motLF) + (1.0-f_smoothFactor)* (float) MZX(*inmot));
            MZY(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MZY(f_motLF) + (1.0-f_smoothFactor)* (float) MZY(*inmot));
            MZZ(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MZZ(f_motLF) + (1.0-f_smoothFactor)* (float) MZZ(*inmot));
            MZW(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MZW(f_motLF) + (1.0-f_smoothFactor)* (float) MZW(*inmot));

            MWX(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MWX(f_motLF) + (1.0-f_smoothFactor)* (float) MWX(*inmot));
            MWY(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MWY(f_motLF) + (1.0-f_smoothFactor)* (float) MWY(*inmot));
            MWZ(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MWZ(f_motLF) + (1.0-f_smoothFactor)* (float) MWZ(*inmot));
            MWW(f_motLF) = (VP_PAR) (f_smoothFactor*(float) MWW(f_motLF) + (1.0-f_smoothFactor)* (float) MWW(*inmot));
        }
        else
            vp_copy_motion_no_id(inmot, &f_motLF); // f_smoothFactor = 0.0

        // Only allow LF motion to be compensated. Remove HF motion from
        // the output transformation
        if(!vp_invert_motion(&f_motLF, &f_imotLF))
            return false;

        if(!vp_cascade_motion(&f_imotLF, inmot, outmot))
            return false;
    }
    else {
        vp_copy_motion_no_id(inmot, outmot);
    }

    return true;
}

bool db_StabilizationSmoother::smoothMotionAdaptive(/*VP_BIMG *bimg,*/int hsize, int vsize, VP_MOTION *inmot, VP_MOTION *outmot)
{
    VP_MOTION tmpMotion, testMotion;
    VP_PAR p1x, p2x, p3x, p4x;
    VP_PAR p1y, p2y, p3y, p4y;
    float smoothFactor;
    float minSmoothFactor = f_minDampingFactor;

//  int hsize = bimg->w;
//  int vsize = bimg->h;
    float border_factor = 0.01;//0.2;
    float border_x = border_factor * hsize;
    float border_y = border_factor * vsize;

    VP_MOTION_ID(f_motLF);
    VP_MOTION_ID(f_imotLF);
    VP_MOTION_ID(testMotion);
    VP_MOTION_ID(tmpMotion);

    if (f_smoothOn) {
        VP_MOTION identityMotion;
        VP_MOTION_ID(identityMotion); // initialize the motion
        vp_copy_motion(inmot/*in*/, &testMotion/*out*/);
        VP_PAR delta = vp_motion_cornerdiff(&testMotion, &identityMotion, 0, 0,(int)hsize, (int)vsize);

        smoothFactor = 0.99 - 0.0015 * delta;

        if(smoothFactor < minSmoothFactor)
            smoothFactor = minSmoothFactor;

        // Find the amount of motion that must be compensated so that no "border" pixels are seen in the stable video
        for (smoothFactor = smoothFactor; smoothFactor >= minSmoothFactor; smoothFactor -= 0.01) {
            // Compute the smoothed motion
            if(!smoothMotion(inmot, &tmpMotion, smoothFactor))
                break;

            // TmpMotion, or Qsi where s is the smoothed display reference and i is the
            // current image, tells us how points in the S co-ordinate system map to
            // points in the I CS.  We would like to check whether the four corners of the
            // warped and smoothed display reference lies entirely within the I co-ordinate
            // system.  If yes, then the amount of smoothing is sufficient so that NO
            // border pixels are seen at the output.  We test for f_smoothFactor terms
            // between 0.9 and 1.0, in steps of 0.01, and between 0.5 ands 0.9 in steps of 0.1

            (void) vp_zoom_motion2d(&tmpMotion, &testMotion, 1, hsize, vsize, (float)f_zoom); // needs to return bool

            VP_WARP_POINT_2D(0, 0, testMotion, p1x, p1y);
            VP_WARP_POINT_2D(hsize - 1, 0, testMotion, p2x, p2y);
            VP_WARP_POINT_2D(hsize - 1, vsize - 1, testMotion, p3x, p3y);
            VP_WARP_POINT_2D(0, vsize - 1, testMotion, p4x, p4y);

            if (!is_point_in_rect((float)p1x,(float)p1y,-border_x,-border_y,(float)(hsize+2.0*border_x),(float)(vsize+2.0*border_y))) {
                continue;
            }
            if (!is_point_in_rect((float)p2x, (float)p2y,-border_x,-border_y,(float)(hsize+2.0*border_x),(float)(vsize+2.0*border_y))) {
                continue;
            }
            if (!is_point_in_rect((float)p3x,(float)p3y,-border_x,-border_y,(float)(hsize+2.0*border_x),(float)(vsize+2.0*border_y))) {
                continue;
            }
            if (!is_point_in_rect((float)p4x, (float)p4y,-border_x,-border_y,(float)(hsize+2.0*border_x),(float)(vsize+2.0*border_y))) {
                continue;
            }

            // If we get here, then all the points are in the rectangle.
            // Therefore, break out of this loop
            break;
        }

        // if we get here and f_smoothFactor <= fMinDampingFactor, reset the stab reference
        if (smoothFactor < f_minDampingFactor)
            smoothFactor = f_minDampingFactor;

        // use the smoothed motion for stabilization
        vp_copy_motion_no_id(&tmpMotion/*in*/, outmot/*out*/);
    }
    else
    {
        vp_copy_motion_no_id(inmot, outmot);
    }

    return true;
}

bool db_StabilizationSmoother::smoothMotion(VP_MOTION *inmot, VP_MOTION *outmot, float smooth_factor)
{
    f_motLF.insid = inmot->refid;
    f_motLF.refid = inmot->insid;

    if(f_smoothOn) {
        if(!f_smoothReset) {
            MXX(f_motLF) = (VP_PAR) (smooth_factor*(float) MXX(f_motLF) + (1.0-smooth_factor)* (float) MXX(*inmot));
            MXY(f_motLF) = (VP_PAR) (smooth_factor*(float) MXY(f_motLF) + (1.0-smooth_factor)* (float) MXY(*inmot));
            MXZ(f_motLF) = (VP_PAR) (smooth_factor*(float) MXZ(f_motLF) + (1.0-smooth_factor)* (float) MXZ(*inmot));
            MXW(f_motLF) = (VP_PAR) (smooth_factor*(float) MXW(f_motLF) + (1.0-smooth_factor)* (float) MXW(*inmot));

            MYX(f_motLF) = (VP_PAR) (smooth_factor*(float) MYX(f_motLF) + (1.0-smooth_factor)* (float) MYX(*inmot));
            MYY(f_motLF) = (VP_PAR) (smooth_factor*(float) MYY(f_motLF) + (1.0-smooth_factor)* (float) MYY(*inmot));
            MYZ(f_motLF) = (VP_PAR) (smooth_factor*(float) MYZ(f_motLF) + (1.0-smooth_factor)* (float) MYZ(*inmot));
            MYW(f_motLF) = (VP_PAR) (smooth_factor*(float) MYW(f_motLF) + (1.0-smooth_factor)* (float) MYW(*inmot));

            MZX(f_motLF) = (VP_PAR) (smooth_factor*(float) MZX(f_motLF) + (1.0-smooth_factor)* (float) MZX(*inmot));
            MZY(f_motLF) = (VP_PAR) (smooth_factor*(float) MZY(f_motLF) + (1.0-smooth_factor)* (float) MZY(*inmot));
            MZZ(f_motLF) = (VP_PAR) (smooth_factor*(float) MZZ(f_motLF) + (1.0-smooth_factor)* (float) MZZ(*inmot));
            MZW(f_motLF) = (VP_PAR) (smooth_factor*(float) MZW(f_motLF) + (1.0-smooth_factor)* (float) MZW(*inmot));

            MWX(f_motLF) = (VP_PAR) (smooth_factor*(float) MWX(f_motLF) + (1.0-smooth_factor)* (float) MWX(*inmot));
            MWY(f_motLF) = (VP_PAR) (smooth_factor*(float) MWY(f_motLF) + (1.0-smooth_factor)* (float) MWY(*inmot));
            MWZ(f_motLF) = (VP_PAR) (smooth_factor*(float) MWZ(f_motLF) + (1.0-smooth_factor)* (float) MWZ(*inmot));
            MWW(f_motLF) = (VP_PAR) (smooth_factor*(float) MWW(f_motLF) + (1.0-smooth_factor)* (float) MWW(*inmot));
        }
        else
            vp_copy_motion_no_id(inmot, &f_motLF); // smooth_factor = 0.0

        // Only allow LF motion to be compensated. Remove HF motion from
        // the output transformation
        if(!vp_invert_motion(&f_motLF, &f_imotLF))
            return false;

        if(!vp_cascade_motion(&f_imotLF, inmot, outmot))
            return false;
    }
    else {
        vp_copy_motion_no_id(inmot, outmot);
    }

    return true;
}

//! Overloaded smoother function that takes in user-specidied smoothing factor
bool
db_StabilizationSmoother::smoothMotion1(VP_MOTION *inmot, VP_MOTION *outmot, VP_MOTION *motLF, VP_MOTION *imotLF, float factor)
{

    if(!f_smoothOn) {
        vp_copy_motion(inmot, outmot);
        return true;
    }
    else {
        if(!f_smoothReset) {
            MXX(*motLF) = (VP_PAR) (factor*(float) MXX(*motLF) + (1.0-factor)* (float) MXX(*inmot));
            MXY(*motLF) = (VP_PAR) (factor*(float) MXY(*motLF) + (1.0-factor)* (float) MXY(*inmot));
            MXZ(*motLF) = (VP_PAR) (factor*(float) MXZ(*motLF) + (1.0-factor)* (float) MXZ(*inmot));
            MXW(*motLF) = (VP_PAR) (factor*(float) MXW(*motLF) + (1.0-factor)* (float) MXW(*inmot));

            MYX(*motLF) = (VP_PAR) (factor*(float) MYX(*motLF) + (1.0-factor)* (float) MYX(*inmot));
            MYY(*motLF) = (VP_PAR) (factor*(float) MYY(*motLF) + (1.0-factor)* (float) MYY(*inmot));
            MYZ(*motLF) = (VP_PAR) (factor*(float) MYZ(*motLF) + (1.0-factor)* (float) MYZ(*inmot));
            MYW(*motLF) = (VP_PAR) (factor*(float) MYW(*motLF) + (1.0-factor)* (float) MYW(*inmot));

            MZX(*motLF) = (VP_PAR) (factor*(float) MZX(*motLF) + (1.0-factor)* (float) MZX(*inmot));
            MZY(*motLF) = (VP_PAR) (factor*(float) MZY(*motLF) + (1.0-factor)* (float) MZY(*inmot));
            MZZ(*motLF) = (VP_PAR) (factor*(float) MZZ(*motLF) + (1.0-factor)* (float) MZZ(*inmot));
            MZW(*motLF) = (VP_PAR) (factor*(float) MZW(*motLF) + (1.0-factor)* (float) MZW(*inmot));

            MWX(*motLF) = (VP_PAR) (factor*(float) MWX(*motLF) + (1.0-factor)* (float) MWX(*inmot));
            MWY(*motLF) = (VP_PAR) (factor*(float) MWY(*motLF) + (1.0-factor)* (float) MWY(*inmot));
            MWZ(*motLF) = (VP_PAR) (factor*(float) MWZ(*motLF) + (1.0-factor)* (float) MWZ(*inmot));
            MWW(*motLF) = (VP_PAR) (factor*(float) MWW(*motLF) + (1.0-factor)* (float) MWW(*inmot));
        }
        else {
            vp_copy_motion(inmot, motLF);
        }
        // Only allow LF motion to be compensated. Remove HF motion from the output transformation
        if(!vp_invert_motion(motLF, imotLF)) {
#if DEBUG_PRINT
            printfOS("Invert failed \n");
#endif
            return false;
        }
        if(!vp_cascade_motion(imotLF, inmot, outmot)) {
#if DEBUG_PRINT
            printfOS("cascade failed \n");
#endif
            return false;
        }
    }
    return true;
}




bool db_StabilizationSmoother::is_point_in_rect(float px, float py, float rx, float ry, float w, float h)
{
    if (px < rx)
        return(false);
    if (px >= rx + w)
        return(false);
    if (py < ry)
        return(false);
    if (py >= ry + h)
        return(false);

    return(true);
}
