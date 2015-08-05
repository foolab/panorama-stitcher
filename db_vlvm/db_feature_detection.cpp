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

/*$Id: db_feature_detection.cpp,v 1.4 2011/06/17 14:03:30 mbansal Exp $*/

/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/

#include "db_utilities.h"
#include "db_feature_detection.h"
#ifdef _VERBOSE_
#include <iostream>
#endif
#include <float.h>
#include "log/log.h"
#include <malloc.h>

#define DB_SUB_PIXEL

#define BORDER 10 // 5

template <typename T> T** db_AllocImage(int w, int h)
{
  T **im = new T *[h];

  for(int i = 0; i < h; i++) {
    im[i] = (T *)memalign(32, w*sizeof(T));
  }

  return im;
}

template<typename T> void db_FreeImage(T **im, int h)
{
  for (int x = 0; x < h; x++) {
    free((void *)im[h]);
  }

  delete [] im;
}

/*Compute derivatives Ix,Iy for a subrow of img with upper left (i,j) and width 128
Memory references occur one pixel outside the subrow*/
inline void db_CornerDetector_u::db_IxIyRow_u(const unsigned char * const *img,int i,int j,int nc)
{
  int Ix,Iy;

  for(int c = 0; c < nc; c++) {
    Ix=(img[i][j+c-1]-img[i][j+c+1])>>1;
    Iy=(img[i-1][j+c]-img[i+1][j+c])>>1;

#ifdef DEBUG
    m_ix[i][c] = Ix;
    m_iy[i][c] = Iy;
#endif
    m_ix2[i][c] = Ix*Ix;
    m_iy2[i][c] = Iy*Iy;
    m_ixy[i][c] = Ix*Iy;
  }
}

/*Filter vertically five rows of derivatives of length 128 into gxx,gxy,gyy*/
inline void db_CornerDetector_u::db_gxx_gxy_gyy_row_s(int i, int nc)
{
  for(int c = 0; c < nc; c++) {
    /* Filter vertically */
    m_gx2[i][c] = m_ix2[i-2][c] + (m_ix2[i-1][c]<<2) + (m_ix2[i][c]<<2) + (m_ix2[i][c]<<1) + (m_ix2[i+1][c]<<2) + m_ix2[i+2][c];
    m_gxy[i][c] = m_ixy[i-2][c] + (m_ixy[i-1][c]<<2) + (m_ixy[i][c]<<2) + (m_ixy[i][c]<<1) + (m_ixy[i+1][c]<<2) + m_ixy[i+2][c];
    m_gy2[i][c] = m_iy2[i-2][c] + (m_iy2[i-1][c]<<2) + (m_iy2[i][c]<<2) + (m_iy2[i][c]<<1) + (m_iy2[i+1][c]<<2) + m_iy2[i+2][c];
  }
}

/*Filter g of length 128 in place with 14641. Output is shifted two steps
and of length 124*/
inline void db_CornerDetector_u::db_Filter14641_128_i(int i, int nc)
{
  for(int c = 0; c < nc - 4; c++) {
    m_gx2[i][c] = m_gx2[i][c] + (m_gx2[i][c+1]<<2) + (m_gx2[i][c+2]<<2) + (m_gx2[i][c+2]<<1) + (m_gx2[i][c+3]<<2) + m_gx2[i][c+4];
    m_gxy[i][c] = m_gxy[i][c] + (m_gxy[i][c+1]<<2) + (m_gxy[i][c+2]<<2) + (m_gxy[i][c+2]<<1) + (m_gxy[i][c+3]<<2) + m_gxy[i][c+4];
    m_gy2[i][c] = m_gy2[i][c] + (m_gy2[i][c+1]<<2) + (m_gy2[i][c+2]<<2) + (m_gy2[i][c+2]<<1) + (m_gy2[i][c+3]<<2) + m_gy2[i][c+4];
  }
}

/*Filter horizontally the three rows gxx,gxy,gyy of length 128 into the strength subrow s
of length 124. gxx,gxy and gyy are assumed to be starting at (i,j-2) if s[i][j] is sought.
s should be 16 byte aligned*/
inline void db_CornerDetector_u::db_HarrisStrength_row_s(float *s, int i, int nc)
{
  float k;

  k=0.06f;

  float Gxx,Gxy,Gyy,det,trc;

  for(int c = 0; c < nc - 4; c++) {
    Gxx=(float)m_gx2[i][c];
    Gxy=(float)m_gxy[i][c];
    Gyy=(float)m_gy2[i][c];

    det=Gxx*Gyy-Gxy*Gxy;
    trc=Gxx+Gyy;
    s[c]=det-k*trc*trc;
  }
}

/*Compute the Harris corner strength of the chunk [left,top,left+123,bottom] of img and
store it into the corresponding region of s. left and top have to be at least 3 and
right and bottom have to be at most width-4,height-4. The left of the region in s should
be 16 byte aligned*/
inline void db_CornerDetector_u::db_HarrisStrengthChunk_u(float **s,const unsigned char * const *img,int left,int top,int bottom, int nc)
{
  /* fill our derivatives */
  for (int i = top - 2; i <= bottom + 2; i++) {
    db_IxIyRow_u(img, i, left-2, nc);
  }

  db_HarrisStrength_row_s(s[top]+left, top, nc);
  db_HarrisStrength_row_s(s[top+1]+left, top+1, nc);

  /* For each output row */
  for (int i = top + 2; i <= bottom; i++) {
    /* Filter Ix2,IxIy,Iy2 vertically into gxx,gxy,gyy */
    db_gxx_gxy_gyy_row_s(i, nc);

    /* Filter gxx,gxy,gyy horizontally */
    db_Filter14641_128_i(i, nc);

    /* compute corner response s */
    db_HarrisStrength_row_s(s[i]+left, i, nc);
  }
}

/*Compute Harris corner strength of img. Strength is returned for the region
with (3,3) as upper left and (w-4,h-4) as lower right, positioned in the
same place in s. In other words,image should be at least 7 pixels wide and 7 pixels high
for a meaningful result.Moreover, the image should be overallocated by 256 bytes.
s[i][3] should by 16 byte aligned for any i*/
void db_CornerDetector_u::db_HarrisStrength_u(float **s, const unsigned char * const *img,int w,int h)
{

  int x = 3;
  int last = w - 4 - x + 1;

  /* Compute the Harris strength of a chunk */
  db_HarrisStrengthChunk_u(s, img, x, 3, h-4, last);
}

inline float db_Max_128Aligned16_f(float *v)
{

  float val,max_val;
  float *p,*stop_p;

  max_val=v[0];

  for(p=v+1,stop_p=v+128;p!=stop_p;) {
    val= *p++;

    if(val>max_val)
      max_val=val;
  }

  return(max_val);
}

inline float db_Max_64Aligned16_f(float *v)
{
  float val,max_val;
  float *p,*stop_p;
  max_val=v[0];
  for(p=v+1,stop_p=v+64;p!=stop_p;) {
    val= *p++;

    if(val>max_val)
      max_val=val;
  }

  return(max_val);
}

inline float db_Max_32Aligned16_f(float *v)
{
    float val,max_val;
    float *p,*stop_p;
    max_val=v[0];
    for(p=v+1,stop_p=v+32;p!=stop_p;)
    {
        val= *p++;
        if(val>max_val) max_val=val;
    }
    return(max_val);
}

inline float db_Max_16Aligned16_f(float *v)
{
    float val,max_val;
    float *p,*stop_p;
    max_val=v[0];
    for(p=v+1,stop_p=v+16;p!=stop_p;)
    {
        val= *p++;
        if(val>max_val) max_val=val;
    }
    return(max_val);
}

inline float db_Max_8Aligned16_f(float *v)
{
    float val,max_val;
    float *p,*stop_p;
    max_val=v[0];
    for(p=v+1,stop_p=v+8;p!=stop_p;)
    {
        val= *p++;
        if(val>max_val) max_val=val;
    }
    return(max_val);
}

inline float db_Max_Aligned16_f(float *v,int size)
{
    float val,max_val;
    float *stop_v;

    max_val=v[0];
    for(;size>=128;size-=128)
    {
        val=db_Max_128Aligned16_f(v);
        v+=128;
        if(val>max_val) max_val=val;
    }
    if(size&64)
    {
        val=db_Max_64Aligned16_f(v);
        v+=64;
        if(val>max_val) max_val=val;
    }
    if(size&32)
    {
        val=db_Max_32Aligned16_f(v);
        v+=32;
        if(val>max_val) max_val=val;
    }
    if(size&16)
    {
        val=db_Max_16Aligned16_f(v);
        v+=16;
        if(val>max_val) max_val=val;
    }
    if(size&8)
    {
        val=db_Max_8Aligned16_f(v);
        v+=8;
        if(val>max_val) max_val=val;
    }
    if(size&7)
    {
        for(stop_v=v+(size&7);v!=stop_v;)
        {
            val= *v++;
            if(val>max_val) max_val=val;
        }
    }

    return(max_val);
}

/*Find maximum value of img in the region starting at (left,top)
and with width w and height h. img[left] should be 16 byte aligned*/
float db_MaxImage_Aligned16_f(float **img,int left,int top,int w,int h)
{
    float val,max_val;
    int i,stop_i;

    if(w && h)
    {
        stop_i=top+h;
        max_val=img[top][left];

        for(i=top;i<stop_i;i++)
        {
            val=db_Max_Aligned16_f(img[i]+left,w);
            if(val>max_val) max_val=val;
        }
        return(max_val);
    }
    return(0.0);
}

inline void db_MaxVector_128_Aligned16_f(float *m,float *v1,float *v2)
{
    int i;
    float a,b;
    for(i=0;i<128;i++)
    {
        a=v1[i];
        b=v2[i];
        if(a>=b) m[i]=a;
        else m[i]=b;
    }
}

inline void db_MaxVector_128_SecondSourceDestAligned16_f(float *m,float *v1,float *v2)
{
    int i;
    float a,b;
    for(i=0;i<128;i++)
    {
        a=v1[i];
        b=v2[i];
        if(a>=b) m[i]=a;
        else m[i]=b;
    }
}

/*Extract corners from the chunk (left,top) to (right,bottom). Store in x_temp,y_temp and s_temp
which should point to space of at least as many positions as there are pixels in the chunk*/
inline int db_CornersFromChunk(float **strength,int left,int top,int right,int bottom,float threshold,float *x_temp,float *y_temp,float *s_temp)
{
    int i,j,nr;
    float s;

    nr=0;
    for(i=top;i<=bottom;i++) for(j=left;j<=right;j++)
    {
        s=strength[i][j];

        if(s>=threshold &&
            s>strength[i-2][j-2] && s>strength[i-2][j-1] && s>strength[i-2][j] && s>strength[i-2][j+1] && s>strength[i-2][j+2] &&
            s>strength[i-1][j-2] && s>strength[i-1][j-1] && s>strength[i-1][j] && s>strength[i-1][j+1] && s>strength[i-1][j+2] &&
            s>strength[  i][j-2] && s>strength[  i][j-1] &&                       s>strength[  i][j+1] && s>strength[  i][j+2] &&
            s>strength[i+1][j-2] && s>strength[i+1][j-1] && s>strength[i+1][j] && s>strength[i+1][j+1] && s>strength[i+1][j+2] &&
            s>strength[i+2][j-2] && s>strength[i+2][j-1] && s>strength[i+2][j] && s>strength[i+2][j+1] && s>strength[i+2][j+2])
        {
            x_temp[nr]=(float) j;
            y_temp[nr]=(float) i;
            s_temp[nr]=(float) s;
            nr++;
        }
    }
    return(nr);
}


//Sub-pixel accuracy using 2D quadratic interpolation.(YCJ)
inline void db_SubPixel(float **strength, const float xd, const float yd, float &xs, float &ys)
{
    int x = (int) xd;
    int y = (int) yd;

    float fxx = strength[y][x-1] - strength[y][x] - strength[y][x] + strength[y][x+1];
    float fyy = strength[y-1][x] - strength[y][x] - strength[y][x] + strength[y+1][x];
    float fxy = (strength[y-1][x-1] - strength[y-1][x+1] - strength[y+1][x-1] + strength[y+1][x+1])/(float)4.0;

    float denom = (fxx * fyy - fxy * fxy) * (float) 2.0;

    xs = xd;
    ys = yd;

    if ( db_absf(denom) <= FLT_EPSILON )
    {
        return;
    }
    else
    {
        float fx = strength[y][x+1] - strength[y][x-1];
        float fy = strength[y+1][x] - strength[y-1][x];

        float dx = (fyy * fx - fxy * fy) / denom;
        float dy = (fxx * fy - fxy * fx) / denom;

        if ( db_absf(dx) > 1.0 || db_absf(dy) > 1.0 )
        {
            return;
        }
        else
        {
            xs -= dx;
            ys -= dy;
        }
    }

    return;
}

/*Extract corners from the image part from (left,top) to (right,bottom).
Store in x and y, extracting at most satnr corners in each block of size (bw,bh).
The pointer temp_d should point to at least 5*bw*bh positions.
area_factor holds how many corners max to extract per 10000 pixels*/
void db_ExtractCornersSaturated(float **strength,int left,int top,int right,int bottom,
                                int bw,int bh,unsigned long area_factor,
                                float threshold,float *temp_d,
                                float *x_coord,float *y_coord,int *nr_corners)
{
    float *x_temp,*y_temp,*s_temp,*select_temp;
    float loc_thresh;
    unsigned long bwbh,area,saturation;
    int x,next_x,last_x;
    int y,next_y,last_y;
    int nr,nr_points,i,stop;

    bwbh=bw*bh;
    x_temp=temp_d;
    y_temp=x_temp+bwbh;
    s_temp=y_temp+bwbh;
    select_temp=s_temp+bwbh;

#ifdef DB_SUB_PIXEL
    // subpixel processing may sometimes push the corner ourside the real border
    // increasing border size:
    left++;
    top++;
    bottom--;
    right--;
#endif /*DB_SUB_PIXEL*/

    nr_points=0;
    for(y=top;y<=bottom;y=next_y)
    {
        next_y=y+bh;
        last_y=next_y-1;
        if(last_y>bottom) last_y=bottom;
        for(x=left;x<=right;x=next_x)
        {
            next_x=x+bw;
            last_x=next_x-1;
            if(last_x>right) last_x=right;

            area=(last_x-x+1)*(last_y-y+1);
            saturation=(area*area_factor)/10000;
            nr=db_CornersFromChunk(strength,x,y,last_x,last_y,threshold,x_temp,y_temp,s_temp);
            if(nr)
            {
                if(((unsigned long)nr)>saturation) loc_thresh=db_LeanQuickSelect(s_temp,nr,nr-saturation,select_temp);
                else loc_thresh=threshold;

                stop=nr_points+saturation;
                for(i=0;(i<nr)&&(nr_points<stop);i++)
                {
                    if(s_temp[i]>=loc_thresh)
                    {
                        #ifdef DB_SUB_PIXEL
                               db_SubPixel(strength, x_temp[i], y_temp[i], x_coord[nr_points], y_coord[nr_points]);
                        #else
                               x_coord[nr_points]=x_temp[i];
                               y_coord[nr_points]=y_temp[i];
                        #endif

                        nr_points++;
                    }
                }
            }
        }
    }
    *nr_corners=nr_points;
}

db_CornerDetector_u::db_CornerDetector_u()
{
    m_w=0; m_h=0;
}

db_CornerDetector_u::~db_CornerDetector_u()
{
    Clean();
}

db_CornerDetector_u::db_CornerDetector_u(const db_CornerDetector_u& cd)
{
    Start(cd.m_w, cd.m_h, cd.m_bw, cd.m_bh, cd.m_area_factor,
        cd.m_a_thresh, cd.m_r_thresh);
}

db_CornerDetector_u& db_CornerDetector_u::operator=(const db_CornerDetector_u& cd)
{
    if ( this == &cd ) return *this;

    Clean();

    Start(cd.m_w, cd.m_h, cd.m_bw, cd.m_bh, cd.m_area_factor,
        cd.m_a_thresh, cd.m_r_thresh);

    return *this;
}

void db_CornerDetector_u::Clean()
{
    if(m_w!=0)
    {
        delete [] m_temp_d;
        db_FreeImage(m_strength, m_h);
#ifdef DEBUG
	db_FreeImage(m_ix, m_h);
	db_FreeImage(m_iy, m_h);
#endif
	db_FreeImage(m_ix2, m_h);
	db_FreeImage(m_iy2, m_h);
	db_FreeImage(m_ixy, m_h);
	db_FreeImage(m_gx2, m_h);
	db_FreeImage(m_gy2, m_h);
	db_FreeImage(m_gxy, m_h);
    }
    m_w=0; m_h=0;
}

unsigned long db_CornerDetector_u::Init(int im_width,int im_height,int target_nr_corners,
                            int nr_horizontal_blocks,int nr_vertical_blocks,
                            float absolute_threshold,float relative_threshold)
{
    int block_width,block_height;
    unsigned long area_factor;
    int active_width,active_height;

    active_width=db_maxi(1,im_width-10);
    active_height=db_maxi(1,im_height-10);
    block_width=db_maxi(1,active_width/nr_horizontal_blocks);
    block_height=db_maxi(1,active_height/nr_vertical_blocks);

    area_factor=db_minl(1000,db_maxl(1,(long)(10000.0*((float)target_nr_corners)/
        (((float)active_width)*((float)active_height)))));

    return(Start(im_width,im_height,block_width,block_height,area_factor,
        16.0f*absolute_threshold,relative_threshold));
}

unsigned long db_CornerDetector_u::Start(int im_width,int im_height,
                             int block_width,int block_height,unsigned long area_factor,
                             float absolute_threshold,float relative_threshold)
{
    Clean();

    m_w=im_width;
    m_h=im_height;
    m_bw=block_width;
    m_bh=block_height;
    m_area_factor=area_factor;
    m_r_thresh=relative_threshold;
    m_a_thresh=absolute_threshold;
    m_max_nr=db_maxl(1,1+(m_w*m_h*m_area_factor)/10000);

    m_temp_d=new float[5*m_bw*m_bh];
    m_strength = db_AllocImage<float>(m_w, m_h);
#ifdef DEBUG
    m_ix = db_AllocImage<int>(m_w, m_h);
    m_iy = db_AllocImage<int>(m_w, m_h);
#endif
    m_ix2 = db_AllocImage<int>(m_w, m_h);
    m_iy2 = db_AllocImage<int>(m_w, m_h);
    m_ixy = db_AllocImage<int>(m_w, m_h);
    m_gx2 = db_AllocImage<int>(m_w, m_h);
    m_gy2 = db_AllocImage<int>(m_w, m_h);
    m_gxy = db_AllocImage<int>(m_w, m_h);

    return(m_max_nr);
}

void db_CornerDetector_u::DetectCorners(const unsigned char * const *img,float *x_coord,float *y_coord,int *nr_corners,
                                        const unsigned char * const *msk, unsigned char fgnd)
{
    float max_val,threshold;

    db_HarrisStrength_u(m_strength,img,m_w,m_h);


    if(m_r_thresh)
    {
        max_val=db_MaxImage_Aligned16_f(m_strength,3,3,m_w-6,m_h-6);
        threshold= (float) db_maxd(m_a_thresh,max_val*m_r_thresh);
    }
    else threshold= (float) m_a_thresh;

    db_ExtractCornersSaturated(m_strength,BORDER,BORDER,m_w-BORDER-1,m_h-BORDER-1,m_bw,m_bh,m_area_factor,threshold,
        m_temp_d,x_coord,y_coord,nr_corners);

    LOGV("Detected corners: %d", *nr_corners);

    if ( msk )
    {
        int nr_corners_mask=0;

        for ( int i = 0; i < *nr_corners; ++i)
        {
            int cor_x = db_roundi(*(x_coord+i));
            int cor_y = db_roundi(*(y_coord+i));
            if ( msk[cor_y][cor_x] == fgnd )
            {
                x_coord[nr_corners_mask] = x_coord[i];
                y_coord[nr_corners_mask] = y_coord[i];
                nr_corners_mask++;
            }
        }
        *nr_corners = nr_corners_mask;
    }
}

void db_CornerDetector_u::ExtractCorners(float ** strength, float *x_coord, float *y_coord, int *nr_corners) {
    if ( m_w!=0 )
        db_ExtractCornersSaturated(strength,BORDER,BORDER,m_w-BORDER-1,m_h-BORDER-1,m_bw,m_bh,m_area_factor,float(m_a_thresh),
            m_temp_d,x_coord,y_coord,nr_corners);
}

