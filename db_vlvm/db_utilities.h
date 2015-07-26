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

/* $Id: db_utilities.h,v 1.3 2011/06/17 14:03:31 mbansal Exp $ */

#ifndef DB_UTILITIES_H
#define DB_UTILITIES_H


#ifdef _WIN32
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
#pragma warning(disable: 4786)
#pragma warning(disable: 4800)
#pragma warning(disable: 4018) /* signed-unsigned mismatch */
#endif /* _WIN32 */

#ifdef _WIN32
    #ifdef DBDYNAMIC_EXPORTS
        #define DB_API __declspec(dllexport)
    #else
        #ifdef DBDYNAMIC_IMPORTS
            #define DB_API __declspec(dllimport)
        #else
            #define DB_API
        #endif
    #endif
#else
    #define DB_API
#endif /* _WIN32 */

#ifdef _VERBOSE_
#include <iostream>
#endif

#include <math.h>

#include <assert.h>
#include "db_utilities_constants.h"
/*!
 * \defgroup LMBasicUtilities (LM) Utility Functions (basic math, linear algebra and array manipulations)
 */
/*\{*/

/*!
 * Round float into int using fld and fistp instructions.
 */
inline int db_roundi (float x) {
#ifdef WIN32_ASM
    int n;
    __asm
    {
        fld x;
        fistp n;
    }
    return n;
#else
    return static_cast<int>(floorf(x+0.5));
#endif
}

/*!
 * Square a float.
 */
inline float db_sqr(float a)
{
    return(a*a);
}

/*!
 * Square a long.
 */
inline long db_sqr(long a)
{
    return(a*a);
}

/*!
 * Square an int.
 */
inline long db_sqr(int a)
{
    return(a*a);
}

/*!
 * Maximum of two floats.
 */
inline float db_maxd(float a,float b)
{
    if(b>a) return(b);
    else return(a);
}
/*!
 * Minumum of two floats.
 */
inline float db_mind(float a,float b)
{
    if(b<a) return(b);
    else return(a);
}


/*!
 * Maximum of two ints.
 */
inline int db_maxi(int a,int b)
{
    if(b>a) return(b);
    else return(a);
}

/*!
 * Minimum of two numbers.
 */
inline int db_mini(int a,int b)
{
    if(b<a) return(b);
    else return(a);
}
/*!
 * Maximum of two numbers.
 */
inline long db_maxl(long a,long b)
{
    if(b>a) return(b);
    else return(a);
}

/*!
 * Minimum of two numbers.
 */
inline long db_minl(long a,long b)
{
    if(b<a) return(b);
    else return(a);
}

/*!
 * Sign of a number.
 * \return -1.0 if negative, 1.0 if positive.
 */
inline float db_sign(float x)
{
    if(x>=0.0) return(1.0);
    else return(-1.0);
}
/*!
 * Absolute value.
 */
inline int db_absi(int a)
{
    if(a<0) return(-a);
    else return(a);
}
/*!
 * Absolute value.
 */
inline float db_absf(float a)
{
    if(a<0) return(-a);
    else return(a);
}

/*!
 * Absolute value.
 */
inline float db_absd(float a)
{
    if(a<0) return(-a);
    else return(a);
}

/*!
 * Reciprocal (1/a). Prevents divide by 0.
 * \return 1/a if a != 0. 1.0 otherwise.
 */
inline float db_SafeReciprocal(float a)
{
    return((a!=0.0)?(1.0/a):1.0);
}

/*!
 * Division. Prevents divide by 0.
 * \return a/b if b!=0. a otherwise.
 */
inline float db_SafeDivision(float a,float b)
{
    return((b!=0.0)?(a/b):a);
}

/*!
 * Square root. Prevents imaginary output.
 * \return sqrt(a) if a > 0.0. 0.0 otherewise.
 */
inline float db_SafeSqrt(float a)
{
    return((a>=0.0)?(sqrtf(a)):0.0);
}

/*!
 * Square root of a reciprocal. Prevents divide by 0 and imaginary output.
 * \return sqrt(1/a) if a > 0.0. 1.0 otherewise.
 */
inline float db_SafeSqrtReciprocal(float a)
{
    return((a>0.0)?(sqrtf(1.0/a)):1.0);
}
/*!
 * Cube root.
 */
inline float db_CubRoot(float x)
{
    if(x>=0.0) return(powf(x,1.0/3.0));
    else return(-powf(-x,1.0/3.0));
}
/*!
 * Sum of squares of elements of x.
 */
inline float db_SquareSum3(const float x[3])
{
    return(db_sqr(x[0])+db_sqr(x[1])+db_sqr(x[2]));
}
/*!
 * Sum of squares of elements of x.
 */
inline float db_SquareSum7(float x[7])
{
    return(db_sqr(x[0])+db_sqr(x[1])+db_sqr(x[2])+
        db_sqr(x[3])+db_sqr(x[4])+db_sqr(x[5])+
        db_sqr(x[6]));
}
/*!
 * Sum of squares of elements of x.
 */
inline float db_SquareSum9(float x[9])
{
    return(db_sqr(x[0])+db_sqr(x[1])+db_sqr(x[2])+
        db_sqr(x[3])+db_sqr(x[4])+db_sqr(x[5])+
        db_sqr(x[6])+db_sqr(x[7])+db_sqr(x[8]));
}
/*!
 * Copy a vector.
 * \param xd destination
 * \param xs source
 */
void inline db_Copy3(float xd[3],const float xs[3])
{
    xd[0]=xs[0];xd[1]=xs[1];xd[2]=xs[2];
}
/*!
 * Copy a vector.
 * \param xd destination
 * \param xs source
 */
void inline db_Copy6(float xd[6],const float xs[6])
{
    xd[0]=xs[0];xd[1]=xs[1];xd[2]=xs[2];
    xd[3]=xs[3];xd[4]=xs[4];xd[5]=xs[5];
}
/*!
 * Copy a vector.
 * \param xd destination
 * \param xs source
 */
void inline db_Copy9(float xd[9],const float xs[9])
{
    xd[0]=xs[0];xd[1]=xs[1];xd[2]=xs[2];
    xd[3]=xs[3];xd[4]=xs[4];xd[5]=xs[5];
    xd[6]=xs[6];xd[7]=xs[7];xd[8]=xs[8];
}

/*!
 * Scalar product: Transpose(A)*B.
 */
inline float db_ScalarProduct4(const float A[4],const float B[4])
{
    return(A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+A[3]*B[3]);
}
/*!
 * Scalar product: Transpose(A)*B.
 */
inline float db_ScalarProduct7(const float A[7],const float B[7])
{
    return(A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+
        A[3]*B[3]+A[4]*B[4]+A[5]*B[5]+
        A[6]*B[6]);
}
/*!
 * Scalar product: Transpose(A)*B.
 */
inline float db_ScalarProduct9(const float A[9],const float B[9])
{
    return(A[0]*B[0]+A[1]*B[1]+A[2]*B[2]+
        A[3]*B[3]+A[4]*B[4]+A[5]*B[5]+
        A[6]*B[6]+A[7]*B[7]+A[8]*B[8]);
}
/*!
 * Vector addition: S=A+B.
 */
inline void db_AddVectors6(float S[6],const float A[6],const float B[6])
{
    S[0]=A[0]+B[0]; S[1]=A[1]+B[1]; S[2]=A[2]+B[2]; S[3]=A[3]+B[3]; S[4]=A[4]+B[4];
    S[5]=A[5]+B[5];
}
/*!
 * Multiplication: C(3x1)=A(3x3)*B(3x1).
 */
inline void db_Multiply3x3_3x1(float y[3],const float A[9],const float x[3])
{
    y[0]=A[0]*x[0]+A[1]*x[1]+A[2]*x[2];
    y[1]=A[3]*x[0]+A[4]*x[1]+A[5]*x[2];
    y[2]=A[6]*x[0]+A[7]*x[1]+A[8]*x[2];
}
inline void db_Multiply3x3_3x3(float C[9], const float A[9],const float B[9])
{
    C[0]=A[0]*B[0]+A[1]*B[3]+A[2]*B[6];
    C[1]=A[0]*B[1]+A[1]*B[4]+A[2]*B[7];
    C[2]=A[0]*B[2]+A[1]*B[5]+A[2]*B[8];

    C[3]=A[3]*B[0]+A[4]*B[3]+A[5]*B[6];
    C[4]=A[3]*B[1]+A[4]*B[4]+A[5]*B[7];
    C[5]=A[3]*B[2]+A[4]*B[5]+A[5]*B[8];

    C[6]=A[6]*B[0]+A[7]*B[3]+A[8]*B[6];
    C[7]=A[6]*B[1]+A[7]*B[4]+A[8]*B[7];
    C[8]=A[6]*B[2]+A[7]*B[5]+A[8]*B[8];
}
/*!
 * Multiplication: C(4x1)=A(4x4)*B(4x1).
 */
inline void db_Multiply4x4_4x1(float y[4],const float A[16],const float x[4])
{
    y[0]=A[0]*x[0]+A[1]*x[1]+A[2]*x[2]+A[3]*x[3];
    y[1]=A[4]*x[0]+A[5]*x[1]+A[6]*x[2]+A[7]*x[3];
    y[2]=A[8]*x[0]+A[9]*x[1]+A[10]*x[2]+A[11]*x[3];
    y[3]=A[12]*x[0]+A[13]*x[1]+A[14]*x[2]+A[15]*x[3];
}
/*!
 * Scalar multiplication in place: A(3)=mult*A(3).
 */
inline void db_MultiplyScalar3(float *A,float mult)
{
    (*A++) *= mult; (*A++) *= mult; (*A++) *= mult;
}

/*!
 * Scalar multiplication: A(3)=mult*B(3).
 */
inline void db_MultiplyScalarCopy3(float *A,const float *B,float mult)
{
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
}

/*!
 * Scalar multiplication: A(4)=mult*B(4).
 */
inline void db_MultiplyScalarCopy4(float *A,const float *B,float mult)
{
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
}
/*!
 * Scalar multiplication: A(7)=mult*B(7).
 */
inline void db_MultiplyScalarCopy7(float *A,const float *B,float mult)
{
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
}
/*!
 * Scalar multiplication: A(9)=mult*B(9).
 */
inline void db_MultiplyScalarCopy9(float *A,const float *B,float mult)
{
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
    (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult; (*A++)=(*B++)*mult;
}

/*!
 * \defgroup LMImageBasicUtilities (LM) Basic Image Utility Functions

 Images in db are simply 2D arrays of unsigned char or float types.
 Only the very basic operations are supported: allocation/deallocation,
copying, simple pyramid construction and LUT warping. These images are used
by db_CornerDetector_u and db_Matcher_u. The db_Image class is an attempt
to wrap these images. It has not been tested well.

 */
/*\{*/
/*!
 * Given a float image array, allocates and returns the set of row poiners.
 * \param im    image pointer
 * \param w     image width
 * \param h     image height
 */
DB_API float** db_SetupImageReferences_f(float *im,int w,int h);
/*!
 * Allocate a float image.
 * Note: for feature detection images must be overallocated by 256 bytes.
 * \param w                 width
 * \param h                 height
 * \param over_allocation   allocate this many extra bytes at the end
 * \return row array pointer
 */
DB_API float** db_AllocImage_f(int w,int h,int over_allocation=256);
/*!
 * Free a float image
 * \param img   row array pointer
 * \param h     image height (number of rows)
 */
DB_API void db_FreeImage_f(float **img,int h);
/*!
 * Given an unsigned char image array, allocates and returns the set of row poiners.
 * \param im    image pointer
 * \param w     image width
 * \param h     image height
 */
DB_API unsigned char** db_SetupImageReferences_u(unsigned char *im,int w,int h);
/*!
 * Allocate an unsigned char image.
 * Note: for feature detection images must be overallocated by 256 bytes.
 * \param w                 width
 * \param h                 height
 * \param over_allocation   allocate this many extra bytes at the end
 * \return row array pointer
 */
DB_API unsigned char** db_AllocImage_u(int w,int h,int over_allocation=256);
/*!
 * Free an unsigned char image
 * \param img   row array pointer
 * \param h     image height (number of rows)
 */
DB_API void db_FreeImage_u(unsigned char **img,int h);

/*!
 Copy an image from s to d. Both s and d must be pre-allocated at of the same size.
 Copy is done row by row.
 \param s   source
 \param d   destination
 \param w   width
 \param h   height
 \param over_allocation copy this many bytes after the end of the last line
 */
DB_API void db_CopyImage_u(unsigned char **d,const unsigned char * const *s,int w,int h,int over_allocation=0);

DB_API inline unsigned char db_BilinearInterpolation(float y, float x, const unsigned char * const * v)
{
    int floor_x=(int) x;
    int floor_y=(int) y;

    int ceil_x=floor_x+1;
    int ceil_y=floor_y+1;

    unsigned char f00 = v[floor_y][floor_x];
    unsigned char f01 = v[floor_y][ceil_x];
    unsigned char f10 = v[ceil_y][floor_x];
    unsigned char f11 = v[ceil_y][ceil_x];

    float xl = x-floor_x;
    float yl = y-floor_y;

    return (unsigned char)(f00*(1-yl)*(1-xl) + f10*yl*(1-xl) + f01*(1-yl)*xl + f11*yl*xl);
}
/*\}*/
/*!
 * \ingroup LMRotation
 * Compute an incremental rotation matrix using the update dx=[sin(phi) sin(ohm) sin(kap)]
 */
inline void db_IncrementalRotationMatrix(float R[9],const float dx[3])
{
    float sp,so,sk,om_sp2,om_so2,om_sk2,cp,co,ck,sp_so,cp_so;

    /*Store sines*/
    sp=dx[0]; so=dx[1]; sk=dx[2];
    om_sp2=1.0-sp*sp;
    om_so2=1.0-so*so;
    om_sk2=1.0-sk*sk;
    /*Compute cosines*/
    cp=(om_sp2>=0.0)?sqrtf(om_sp2):1.0;
    co=(om_so2>=0.0)?sqrtf(om_so2):1.0;
    ck=(om_sk2>=0.0)?sqrtf(om_sk2):1.0;
    /*Compute matrix*/
    sp_so=sp*so;
    cp_so=cp*so;
    R[0]=sp_so*sk+cp*ck; R[1]=co*sk; R[2]=cp_so*sk-sp*ck;
    R[3]=sp_so*ck-cp*sk; R[4]=co*ck; R[5]=cp_so*ck+sp*sk;
    R[6]=sp*co;          R[7]= -so;  R[8]=cp*co;
}
/*!
 * Zero out 2 vector in place.
 */
void inline db_Zero2(float x[2])
{
    x[0]=x[1]=0;
}
/*!
 * Zero out 3 vector in place.
 */
void inline db_Zero3(float x[3])
{
    x[0]=x[1]=x[2]=0;
}
/*!
 * Zero out 4 vector in place.
 */
void inline db_Zero4(float x[4])
{
    x[0]=x[1]=x[2]=x[3]=0;
}
/*!
 * Zero out 9 vector in place.
 */
void inline db_Zero9(float x[9])
{
    x[0]=x[1]=x[2]=x[3]=x[4]=x[5]=x[6]=x[7]=x[8]=0;
}

#define DB_WARP_FAST        0
#define DB_WARP_BILINEAR    1

/*!
 * Perform a look-up table warp.
 * The LUTs must be float images of the same size as source image.
 * The source value x_s is determined from destination (x_d,y_d) through lut_x
 * and y_s is determined from lut_y:
   \code
   x_s = lut_x[y_d][x_d];
   y_s = lut_y[y_d][x_d];
   \endcode

 * \param src   source image
 * \param dst   destination image
 * \param w     width
 * \param h     height
 * \param lut_x LUT for x
 * \param lut_y LUT for y
 * \param type  warp type (DB_WARP_FAST or DB_WARP_BILINEAR)
 */
DB_API void db_WarpImageLut_u(const unsigned char * const * src,unsigned char ** dst, int w, int h,
                               const float * const * lut_x, const float * const * lut_y, int type=DB_WARP_BILINEAR);

DB_API void db_PrintDoubleVector(float *a,long size);
DB_API void db_PrintDoubleMatrix(float *a,long rows,long cols);

#include "db_utilities_constants.h"
#include "db_utilities_algebra.h"
#include "db_utilities_indexing.h"
#include "db_utilities_linalg.h"
#include "db_utilities_poly.h"
#include "db_utilities_geometry.h"
#include "db_utilities_random.h"
#include "db_utilities_rotation.h"
#include "db_utilities_camera.h"

#define DB_INVALID (-1)


#endif /* DB_UTILITIES_H */
