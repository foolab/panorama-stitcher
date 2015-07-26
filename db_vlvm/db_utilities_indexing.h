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

/* $Id: db_utilities_indexing.h,v 1.3 2011/06/17 14:03:31 mbansal Exp $ */

#ifndef DB_UTILITIES_INDEXING
#define DB_UTILITIES_INDEXING



/*****************************************************************
*    Lean and mean begins here                                   *
*****************************************************************/

#include "db_utilities.h"

/*!
 * \defgroup LMIndexing (LM) Indexing Utilities (Order Statistics, Matrix Operations)
 */
/*\{*/

inline void db_SetupMatrixRefs(float **ar,long rows,long cols,float *a)
{
    long i;
    for(i=0;i<rows;i++) ar[i]=&a[i*cols];
}

inline void db_SymmetricExtendUpperToLower(float **A,int rows,int cols)
{
    int i,j;
    for(i=1;i<rows;i++) for(j=0;j<i;j++) A[i][j]=A[j][i];
}

void inline db_MultiplyMatrixVectorAtb(float *c,const float * const *At,const float *b,int arows,int acols)
{
    int i,j;
    float acc;

    for(i=0;i<arows;i++)
    {
        acc=0;
        for(j=0;j<acols;j++) acc+=At[j][i]*b[j];
        c[i]=acc;
    }
}

inline void db_MultiplyMatricesAB(float **C,const float * const *A,const float * const *B,int arows,int acols,int bcols)
{
    int i,j,k;
    float acc;

    for(i=0;i<arows;i++) for(j=0;j<bcols;j++)
    {
        acc=0;
        for(k=0;k<acols;k++) acc+=A[i][k]*B[k][j];
        C[i][j]=acc;
    }
}

inline void db_UpperMultiplyMatricesAtB(float **Cu,const float * const *At,const float * const *B,int arows,int acols,int bcols)
{
    int i,j,k;
    float acc;

    for(i=0;i<arows;i++) for(j=i;j<bcols;j++)
    {
        acc=0;
        for(k=0;k<acols;k++) acc+=At[k][i]*B[k][j];
        Cu[i][j]=acc;
    }
}

void db_Zero(float *d,long nr);

inline int db_MaxIndex2(float s[2])
{
    if(s[0]>=s[1]) return(0);
    return(1);
}

inline int db_MaxIndex3(const float s[3])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]>best){best=s[1];pos=1;}
    if(s[2]>best){best=s[2];pos=2;}
    return(pos);
}

inline int db_MaxIndex4(const float s[4])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]>best){best=s[1];pos=1;}
    if(s[2]>best){best=s[2];pos=2;}
    if(s[3]>best){best=s[3];pos=3;}
    return(pos);
}

inline int db_MaxIndex5(const float s[5])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]>best){best=s[1];pos=1;}
    if(s[2]>best){best=s[2];pos=2;}
    if(s[3]>best){best=s[3];pos=3;}
    if(s[4]>best){best=s[4];pos=4;}
    return(pos);
}

inline int db_MaxIndex6(const float s[6])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]>best){best=s[1];pos=1;}
    if(s[2]>best){best=s[2];pos=2;}
    if(s[3]>best){best=s[3];pos=3;}
    if(s[4]>best){best=s[4];pos=4;}
    if(s[5]>best){best=s[5];pos=5;}
    return(pos);
}

inline int db_MaxIndex7(const float s[7])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]>best){best=s[1];pos=1;}
    if(s[2]>best){best=s[2];pos=2;}
    if(s[3]>best){best=s[3];pos=3;}
    if(s[4]>best){best=s[4];pos=4;}
    if(s[5]>best){best=s[5];pos=5;}
    if(s[6]>best){best=s[6];pos=6;}
    return(pos);
}

inline int db_MinIndex7(const float s[7])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]<best){best=s[1];pos=1;}
    if(s[2]<best){best=s[2];pos=2;}
    if(s[3]<best){best=s[3];pos=3;}
    if(s[4]<best){best=s[4];pos=4;}
    if(s[5]<best){best=s[5];pos=5;}
    if(s[6]<best){best=s[6];pos=6;}
    return(pos);
}

inline int db_MinIndex9(const float s[9])
{
    float best;
    int pos;

    best=s[0];pos=0;
    if(s[1]<best){best=s[1];pos=1;}
    if(s[2]<best){best=s[2];pos=2;}
    if(s[3]<best){best=s[3];pos=3;}
    if(s[4]<best){best=s[4];pos=4;}
    if(s[5]<best){best=s[5];pos=5;}
    if(s[6]<best){best=s[6];pos=6;}
    if(s[7]<best){best=s[7];pos=7;}
    if(s[8]<best){best=s[8];pos=8;}
    return(pos);
}

inline int db_MaxAbsIndex3(const float *s)
{
    float t,best;
    int pos;

    best=fabsf(s[0]);pos=0;
    t=fabsf(s[1]);if(t>best){best=t;pos=1;}
    t=fabsf(s[2]);if(t>best){pos=2;}
    return(pos);
}

inline int db_MaxAbsIndex9(const float *s)
{
    float t,best;
    int pos;

    best=fabsf(s[0]);pos=0;
    t=fabsf(s[1]);if(t>best){best=t;pos=1;}
    t=fabsf(s[2]);if(t>best){best=t;pos=2;}
    t=fabsf(s[3]);if(t>best){best=t;pos=3;}
    t=fabsf(s[4]);if(t>best){best=t;pos=4;}
    t=fabsf(s[5]);if(t>best){best=t;pos=5;}
    t=fabsf(s[6]);if(t>best){best=t;pos=6;}
    t=fabsf(s[7]);if(t>best){best=t;pos=7;}
    t=fabsf(s[8]);if(t>best){best=t;pos=8;}
    return(pos);
}


/*!
Select ordinal pos (zero based) out of nr_elements in s.
temp should point to alloced memory of at least nr_elements*2
Optimized runtimes on 450MHz:
\code
  30 with   3 microsecs
 100 with  11 microsecs
 300 with  30 microsecs
 500 with  40 microsecs
1000 with 100 microsecs
5000 with 540 microsecs
\endcode
so the expected runtime is around
(nr_elements/10) microseconds
The total quickselect cost of splitting 500 hypotheses recursively
is thus around 100 microseconds

Does the same operation as std::nth_element().
*/
float db_LeanQuickSelect(const float *s,long nr_elements,long pos,float *temp);

/*!
 Median of 3 floats
 */
inline float db_TripleMedian(float a,float b,float c)
{
    if(a>b)
    {
        if(c>a) return(a);
        else if(c>b) return(c);
        else return(b);
    }
    else
    {
        if(c>b) return(b);
        else if(c>a) return(c);
        else return(a);
    }
}

/*!
Align float pointer to nr_bytes by moving forward
*/
float* db_AlignPointer_f(float *p,unsigned long nr_bytes);

/*!
Align short pointer to nr_bytes by moving forward
*/
short* db_AlignPointer_s(short *p,unsigned long nr_bytes);

#endif /* DB_UTILITIES_INDEXING */
