/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_matrix_hpp__
#define __Cg_matrix_hpp__

#include <Cg/vector.hpp>

namespace Cg {

#if defined(__GNUC__) && (__GNUC__>3 || (__GNUC__==3 && __GNUC_MINOR__>=3))
#define __CGmay_alias __attribute__((__may_alias__))
#else
#define __CGmay_alias 
#endif

// Utility macros for constructing matrix swizzles

#define __CG_MATRIX_X(row,col) ((row*cols+col)<<0)
#define __CG_MATRIX_Y(row,col) ((row*cols+col)<<8)
#define __CG_MATRIX_Z(row,col) ((row*cols+col)<<16)
#define __CG_MATRIX_W(row,col) ((row*cols+col)<<24)

#define __CG_MATRIX_SMASK1(x0,y0) \
    __CG_MATRIX_X(x0,y0)
#define __CG_MATRIX_SWIZZLE1(x0,y0) \
    __CGswizzle<T,ELEMS,1,__CG_MATRIX_SMASK1(x0,y0)> 

#define __CG_MATRIX_SMASK2(x0,y0,x1,y1) \
    __CG_MATRIX_X(x0,y0)| \
    __CG_MATRIX_Y(x1,y1)
#define __CG_MATRIX_SWIZZLE2(x0,y0,x1,y1) \
    __CGswizzle<T,ELEMS,2,__CG_MATRIX_SMASK2(x0,y0,x1,y1)> 

#define __CG_MATRIX_SMASK3(x0,y0,x1,y1,x2,y2) \
    __CG_MATRIX_X(x0,y0)| \
    __CG_MATRIX_Y(x1,y1)| \
    __CG_MATRIX_Z(x2,y2)
#define __CG_MATRIX_SWIZZLE3(x0,y0,x1,y1,x2,y2) \
    __CGswizzle<T,ELEMS,3,__CG_MATRIX_SMASK3(x0,y0,x1,y1,x2,y2)> 

#define __CG_MATRIX_SMASK4(x0,y0,x1,y1,x2,y2,x3,y3) \
    __CG_MATRIX_X(x0,y0)| \
    __CG_MATRIX_Y(x1,y1)| \
    __CG_MATRIX_Z(x2,y2)| \
    __CG_MATRIX_W(x3,y3)
#define __CG_MATRIX_SWIZZLE4(x0,y0,x1,y1,x2,y2,x3,y3) \
    __CGswizzle<T,ELEMS,4,__CG_MATRIX_SMASK4(x0,y0,x1,y1,x2,y2,x3,y3)> 

#if defined(__Cg_matrix_0based_hpp__) && defined(__Cg_matrix_1based_hpp__)
// include <Cg/matrix/0based.hpp> and <Cg/matrix/1based.hpp> for
// both _mIJ zero-based and _IJ one-based swizzling
#define __CG_USAGE(zeroBased, oneBased) zeroBased, oneBased
#elif defined(__Cg_matrix_0based_hpp__)
// include <Cg/matrix/0based.hpp> for _mIJ zero-based swizzling
#define __CG_USAGE(zeroBased, oneBased) zeroBased
#else
// include <Cg/matrix/1based.hpp> (or nothing) for _IJ one-based swizzling
#define __CG_USAGE(zeroBased, oneBased) oneBased
#endif

#define __CG_MATRIX_ELEMENT(row, x, ms,s) __CG_MATRIX_SWIZZLE1(row,x) __CG_USAGE(ms, s);

#define __CG_MATRIX_ROW_swizzle2(row, x,y, ms,s) __CG_MATRIX_SWIZZLE2(row,x,row,y) __CG_USAGE(ms, s);
#define __CG_MATRIX_ROW_swizzle3(row, x,y,z, ms,s) __CG_MATRIX_SWIZZLE3(row,x,row,y,row,z) __CG_USAGE(ms, s);
#define __CG_MATRIX_ROW_swizzle4(row, x,y,z,w, ms,s) __CG_MATRIX_SWIZZLE4(row,x,row,y,row,z,row,w) __CG_USAGE(ms, s);

#if defined(__Cg_matrix_elements_hpp__) || defined(__Cg_matrix_rows_hpp__) || defined(__Cg_matrix_columns_hpp__)
#define __CG_MATRIX_element(row, x, ms,s) __CG_MATRIX_SWIZZLE1(row,x) __CG_USAGE(ms, s);
#else
#define __CG_MATRIX_element(row, x, ms,s)
#endif

#ifdef __Cg_matrix_columns_hpp__ // include <Cg/matrix/columns.hpp> for column swizzling
#define __CG_MATRIX_col_swizzle2(col, x,y, ms,s) __CG_MATRIX_SWIZZLE2(x,col,y,col) __CG_USAGE(ms, s);
#define __CG_MATRIX_col_swizzle3(col, x,y,z, ms,s) __CG_MATRIX_SWIZZLE3(x,col,y,col,z,col) __CG_USAGE(ms, s);
#define __CG_MATRIX_col_swizzle4(col, x,y,z,w, ms,s) __CG_MATRIX_SWIZZLE4(x,col,y,col,z,col,w,col) __CG_USAGE(ms, s);
#else
#define __CG_MATRIX_col_swizzle2(col, x,y, ms,s)
#define __CG_MATRIX_col_swizzle3(col, x,y,z, ms,s)
#define __CG_MATRIX_col_swizzle4(col, x,y,z,w, ms,s)
#endif

#ifdef __Cg_matrix_rows_hpp__ // include <Cg/matrix/rows.hpp> for rows swizzling
#define __CG_MATRIX_row_swizzle2(row, x,y, ms,s) __CG_MATRIX_SWIZZLE2(row,x,row,y) __CG_USAGE(ms, s);
#define __CG_MATRIX_row_swizzle3(row, x,y,z, ms,s) __CG_MATRIX_SWIZZLE3(row,x,row,y,row,z) __CG_USAGE(ms, s);
#define __CG_MATRIX_row_swizzle4(row, x,y,z,w, ms,s) __CG_MATRIX_SWIZZLE4(row,x,row,y,row,z,row,w) __CG_USAGE(ms, s);
#else
#define __CG_MATRIX_row_swizzle2(row, x,y, ms,s)
#define __CG_MATRIX_row_swizzle3(row, x,y,z, ms,s)
#define __CG_MATRIX_row_swizzle4(row, x,y,z,w, ms,s)
#endif

// Incremental swizzles required for each matrix up to 4x4

#define __CG_MATRIX_SWIZZLE_1x1(mode) \
        __CG_MATRIX_##mode(0, 0, _m00, _11)

#define __CG_MATRIX_SWIZZLE_1x2(mode) \
        __CG_MATRIX_element(0, 1, _m01, _12) \
        __CG_MATRIX_##mode##_swizzle2(0, 0,1, _m00_m01, _11_12) \
        __CG_MATRIX_row_swizzle2(0, 1,0, _m01_m00, _12_11)

#define __CG_MATRIX_SWIZZLE_1x3(mode) \
        __CG_MATRIX_element(0, 2, _m02, _13) \
        __CG_MATRIX_row_swizzle2(0, 0,2, _m00_m02, _11_13) \
        __CG_MATRIX_row_swizzle2(0, 1,2, _m01_m02, _12_13) \
        __CG_MATRIX_row_swizzle2(0, 2,0, _m02_m00, _13_11) \
        __CG_MATRIX_row_swizzle2(0, 2,1, _m02_m01, _13_12) \
        __CG_MATRIX_##mode##_swizzle3(0, 0,1,2, _m00_m01_m02, _11_12_13) \
        __CG_MATRIX_row_swizzle3(0, 0,2,1, _m00_m02_m01, _11_13_12) \
        __CG_MATRIX_row_swizzle3(0, 1,0,2, _m01_m00_m02, _12_11_13) \
        __CG_MATRIX_row_swizzle3(0, 1,2,0, _m01_m02_m00, _12_13_11) \
        __CG_MATRIX_row_swizzle3(0, 2,0,1, _m02_m00_m01, _13_11_12) \
        __CG_MATRIX_row_swizzle3(0, 2,1,0, _m02_m01_m00, _13_12_11)

#define __CG_MATRIX_SWIZZLE_1x4(mode) \
        __CG_MATRIX_element(0, 3, _m03, _14) \
        __CG_MATRIX_row_swizzle2(0, 0,3, _m00_m03, _11_14) \
        __CG_MATRIX_row_swizzle2(0, 1,3, _m01_m03, _12_14) \
        __CG_MATRIX_row_swizzle2(0, 2,3, _m02_m03, _13_14) \
        __CG_MATRIX_row_swizzle2(0, 3,0, _m03_m00, _14_11) \
        __CG_MATRIX_row_swizzle2(0, 3,1, _m03_m01, _14_12) \
        __CG_MATRIX_row_swizzle2(0, 3,2, _m03_m02, _14_13) \
        __CG_MATRIX_row_swizzle3(0, 0,1,3, _m00_m01_m03, _11_12_14) \
        __CG_MATRIX_row_swizzle3(0, 0,2,3, _m00_m02_m03, _11_13_14) \
        __CG_MATRIX_row_swizzle3(0, 0,3,1, _m00_m03_m01, _11_14_12) \
        __CG_MATRIX_row_swizzle3(0, 0,3,2, _m00_m03_m02, _11_14_13) \
        __CG_MATRIX_row_swizzle3(0, 1,0,3, _m01_m00_m03, _12_11_14) \
        __CG_MATRIX_row_swizzle3(0, 1,2,3, _m01_m02_m03, _12_13_14) \
        __CG_MATRIX_row_swizzle3(0, 1,3,0, _m01_m03_m00, _12_14_11) \
        __CG_MATRIX_row_swizzle3(0, 1,3,2, _m01_m03_m02, _12_14_13) \
        __CG_MATRIX_row_swizzle3(0, 2,0,3, _m02_m00_m03, _13_11_14) \
        __CG_MATRIX_row_swizzle3(0, 2,1,3, _m02_m01_m03, _13_12_14) \
        __CG_MATRIX_row_swizzle3(0, 2,3,0, _m02_m03_m00, _13_14_11) \
        __CG_MATRIX_row_swizzle3(0, 2,3,1, _m02_m03_m01, _13_14_12) \
        __CG_MATRIX_row_swizzle3(0, 3,0,1, _m03_m00_m01, _14_11_12) \
        __CG_MATRIX_row_swizzle3(0, 3,0,2, _m03_m00_m02, _14_11_13) \
        __CG_MATRIX_row_swizzle3(0, 3,1,0, _m03_m01_m00, _14_12_11) \
        __CG_MATRIX_row_swizzle3(0, 3,1,2, _m03_m01_m02, _14_12_13) \
        __CG_MATRIX_row_swizzle3(0, 3,2,0, _m03_m02_m00, _14_13_11) \
        __CG_MATRIX_row_swizzle3(0, 3,2,1, _m03_m02_m01, _14_13_12) \
        __CG_MATRIX_##mode##_swizzle4(0, 0,1,2,3, _m00_m01_m02_m03, _11_12_13_14) \
        __CG_MATRIX_row_swizzle4(0, 0,1,3,2, _m00_m01_m03_m02, _11_12_14_13) \
        __CG_MATRIX_row_swizzle4(0, 0,2,1,3, _m00_m02_m01_m03, _11_13_12_14) \
        __CG_MATRIX_row_swizzle4(0, 0,2,3,1, _m00_m02_m03_m01, _11_13_14_12) \
        __CG_MATRIX_row_swizzle4(0, 0,3,1,2, _m00_m03_m01_m02, _11_14_12_13) \
        __CG_MATRIX_row_swizzle4(0, 0,3,2,1, _m00_m03_m02_m01, _11_14_13_12) \
        __CG_MATRIX_row_swizzle4(0, 1,0,2,3, _m01_m00_m02_m03, _12_11_13_14) \
        __CG_MATRIX_row_swizzle4(0, 1,0,3,2, _m01_m00_m03_m02, _12_11_14_13) \
        __CG_MATRIX_row_swizzle4(0, 1,2,0,3, _m01_m02_m00_m03, _12_13_11_14) \
        __CG_MATRIX_row_swizzle4(0, 1,2,3,0, _m01_m02_m03_m00, _12_13_14_11) \
        __CG_MATRIX_row_swizzle4(0, 1,3,0,2, _m01_m03_m00_m02, _12_14_11_13) \
        __CG_MATRIX_row_swizzle4(0, 1,3,2,0, _m01_m03_m02_m00, _12_14_13_11) \
        __CG_MATRIX_row_swizzle4(0, 2,0,1,3, _m02_m00_m01_m03, _13_11_12_14) \
        __CG_MATRIX_row_swizzle4(0, 2,0,3,1, _m02_m00_m03_m01, _13_11_14_12) \
        __CG_MATRIX_row_swizzle4(0, 2,1,0,3, _m02_m01_m00_m03, _13_12_11_14) \
        __CG_MATRIX_row_swizzle4(0, 2,1,3,0, _m02_m01_m03_m00, _13_12_14_11) \
        __CG_MATRIX_row_swizzle4(0, 2,3,0,1, _m02_m03_m00_m01, _13_14_11_12) \
        __CG_MATRIX_row_swizzle4(0, 2,3,1,0, _m02_m03_m01_m00, _13_14_12_11) \
        __CG_MATRIX_row_swizzle4(0, 3,0,1,2, _m03_m00_m01_m02, _14_11_12_13) \
        __CG_MATRIX_row_swizzle4(0, 3,0,2,1, _m03_m00_m02_m01, _14_11_13_12) \
        __CG_MATRIX_row_swizzle4(0, 3,1,0,2, _m03_m01_m00_m02, _14_12_11_13) \
        __CG_MATRIX_row_swizzle4(0, 3,1,2,0, _m03_m01_m02_m00, _14_12_13_11) \
        __CG_MATRIX_row_swizzle4(0, 3,2,0,1, _m03_m02_m00_m01, _14_13_11_12) \
        __CG_MATRIX_row_swizzle4(0, 3,2,1,0, _m03_m02_m01_m00, _14_13_12_11)

#define __CG_MATRIX_SWIZZLE_2x1(mode) \
        __CG_MATRIX_##mode(1, 0, _m10, _21) \
        __CG_MATRIX_col_swizzle2(0, 0,1, _m00_m10, _11_21) \
        __CG_MATRIX_col_swizzle2(0, 1,0, _m10_m00, _21_11)

#define __CG_MATRIX_SWIZZLE_2x2(mode) \
        __CG_MATRIX_element(1, 1, _m11, _22) \
        __CG_MATRIX_##mode##_swizzle2(1, 0,1, _m10_m11, _21_22) \
        __CG_MATRIX_row_swizzle2(1, 1,0, _m11_m10, _22_21) \
        __CG_MATRIX_col_swizzle2(1, 0,1, _m01_m11, _12_22) \
        __CG_MATRIX_col_swizzle2(1, 1,0, _m11_m01, _22_12)

#define __CG_MATRIX_SWIZZLE_2x3(mode) \
        __CG_MATRIX_element(1, 2, _m12, _23) \
        __CG_MATRIX_row_swizzle2(1, 0,2, _m10_m12, _21_23) \
        __CG_MATRIX_row_swizzle2(1, 1,2, _m11_m12, _22_23) \
        __CG_MATRIX_row_swizzle2(1, 2,0, _m12_m10, _23_21) \
        __CG_MATRIX_row_swizzle2(1, 2,1, _m12_m11, _23_22) \
        __CG_MATRIX_##mode##_swizzle3(1, 0,1,2, _m10_m11_m12, _21_22_23) \
        __CG_MATRIX_row_swizzle3(1, 0,2,1, _m10_m12_m11, _21_23_22) \
        __CG_MATRIX_row_swizzle3(1, 1,0,2, _m11_m10_m12, _22_21_23) \
        __CG_MATRIX_row_swizzle3(1, 1,2,0, _m11_m12_m10, _22_23_21) \
        __CG_MATRIX_row_swizzle3(1, 2,0,1, _m12_m10_m11, _23_21_22) \
        __CG_MATRIX_row_swizzle3(1, 2,1,0, _m12_m11_m10, _23_22_21) \
        __CG_MATRIX_col_swizzle2(2, 0,1, _m02_m12, _13_23) \
        __CG_MATRIX_col_swizzle2(2, 1,0, _m12_m02, _23_13)

#define __CG_MATRIX_SWIZZLE_2x4(mode) \
        __CG_MATRIX_element(1, 3, _m13, _24) \
        __CG_MATRIX_row_swizzle2(1, 0,3, _m10_m13, _21_24) \
        __CG_MATRIX_row_swizzle2(1, 1,3, _m11_m13, _22_24) \
        __CG_MATRIX_row_swizzle2(1, 2,3, _m12_m13, _23_24) \
        __CG_MATRIX_row_swizzle2(1, 3,0, _m13_m10, _24_21) \
        __CG_MATRIX_row_swizzle2(1, 3,1, _m13_m11, _24_22) \
        __CG_MATRIX_row_swizzle2(1, 3,2, _m13_m12, _24_23) \
        __CG_MATRIX_row_swizzle3(1, 0,1,3, _m10_m11_m13, _21_22_24) \
        __CG_MATRIX_row_swizzle3(1, 0,2,3, _m10_m12_m13, _21_23_24) \
        __CG_MATRIX_row_swizzle3(1, 0,3,1, _m10_m13_m11, _21_24_22) \
        __CG_MATRIX_row_swizzle3(1, 0,3,2, _m10_m13_m12, _21_24_23) \
        __CG_MATRIX_row_swizzle3(1, 1,0,3, _m11_m10_m13, _22_21_24) \
        __CG_MATRIX_row_swizzle3(1, 1,2,3, _m11_m12_m13, _22_23_24) \
        __CG_MATRIX_row_swizzle3(1, 1,3,0, _m11_m13_m10, _22_24_21) \
        __CG_MATRIX_row_swizzle3(1, 1,3,2, _m11_m13_m12, _22_24_23) \
        __CG_MATRIX_row_swizzle3(1, 2,0,3, _m12_m10_m13, _23_21_24) \
        __CG_MATRIX_row_swizzle3(1, 2,1,3, _m12_m11_m13, _23_22_24) \
        __CG_MATRIX_row_swizzle3(1, 2,3,0, _m12_m13_m10, _23_24_21) \
        __CG_MATRIX_row_swizzle3(1, 2,3,1, _m12_m13_m11, _23_24_22) \
        __CG_MATRIX_row_swizzle3(1, 3,0,1, _m13_m10_m11, _24_21_22) \
        __CG_MATRIX_row_swizzle3(1, 3,0,2, _m13_m10_m12, _24_21_23) \
        __CG_MATRIX_row_swizzle3(1, 3,1,0, _m13_m11_m10, _24_22_21) \
        __CG_MATRIX_row_swizzle3(1, 3,1,2, _m13_m11_m12, _24_22_23) \
        __CG_MATRIX_row_swizzle3(1, 3,2,0, _m13_m12_m10, _24_23_21) \
        __CG_MATRIX_row_swizzle3(1, 3,2,1, _m13_m12_m11, _24_23_22) \
        __CG_MATRIX_##mode##_swizzle4(1, 0,1,2,3, _m10_m11_m12_m13, _21_22_23_24) \
        __CG_MATRIX_row_swizzle4(1, 0,1,3,2, _m10_m11_m13_m12, _21_22_24_23) \
        __CG_MATRIX_row_swizzle4(1, 0,2,1,3, _m10_m12_m11_m13, _21_23_22_24) \
        __CG_MATRIX_row_swizzle4(1, 0,2,3,1, _m10_m12_m13_m11, _21_23_24_22) \
        __CG_MATRIX_row_swizzle4(1, 0,3,1,2, _m10_m13_m11_m12, _21_24_22_23) \
        __CG_MATRIX_row_swizzle4(1, 0,3,2,1, _m10_m13_m12_m11, _21_24_23_22) \
        __CG_MATRIX_row_swizzle4(1, 1,0,2,3, _m11_m10_m12_m13, _22_21_23_24) \
        __CG_MATRIX_row_swizzle4(1, 1,0,3,2, _m11_m10_m13_m12, _22_21_24_23) \
        __CG_MATRIX_row_swizzle4(1, 1,2,0,3, _m11_m12_m10_m13, _22_23_21_24) \
        __CG_MATRIX_row_swizzle4(1, 1,2,3,0, _m11_m12_m13_m10, _22_23_24_21) \
        __CG_MATRIX_row_swizzle4(1, 1,3,0,2, _m11_m13_m10_m12, _22_24_21_23) \
        __CG_MATRIX_row_swizzle4(1, 1,3,2,0, _m11_m13_m12_m10, _22_24_23_21) \
        __CG_MATRIX_row_swizzle4(1, 2,0,1,3, _m12_m10_m11_m13, _23_21_22_24) \
        __CG_MATRIX_row_swizzle4(1, 2,0,3,1, _m12_m10_m13_m11, _23_21_24_22) \
        __CG_MATRIX_row_swizzle4(1, 2,1,0,3, _m12_m11_m10_m13, _23_22_21_24) \
        __CG_MATRIX_row_swizzle4(1, 2,1,3,0, _m12_m11_m13_m10, _23_22_24_21) \
        __CG_MATRIX_row_swizzle4(1, 2,3,0,1, _m12_m13_m10_m11, _23_24_21_22) \
        __CG_MATRIX_row_swizzle4(1, 2,3,1,0, _m12_m13_m11_m10, _23_24_22_21) \
        __CG_MATRIX_row_swizzle4(1, 3,0,1,2, _m13_m10_m11_m12, _24_21_22_23) \
        __CG_MATRIX_row_swizzle4(1, 3,0,2,1, _m13_m10_m12_m11, _24_21_23_22) \
        __CG_MATRIX_row_swizzle4(1, 3,1,0,2, _m13_m11_m10_m12, _24_22_21_23) \
        __CG_MATRIX_row_swizzle4(1, 3,1,2,0, _m13_m11_m12_m10, _24_22_23_21) \
        __CG_MATRIX_row_swizzle4(1, 3,2,0,1, _m13_m12_m10_m11, _24_23_21_22) \
        __CG_MATRIX_row_swizzle4(1, 3,2,1,0, _m13_m12_m11_m10, _24_23_22_21) \
        __CG_MATRIX_col_swizzle2(3, 0,1, _m03_m13, _14_24) \
        __CG_MATRIX_col_swizzle2(3, 1,0, _m13_m03, _24_14)

#define __CG_MATRIX_SWIZZLE_3x1(mode) \
        __CG_MATRIX_##mode(2, 0, _m20, _31) \
        __CG_MATRIX_col_swizzle2(0, 0,2, _m00_m20, _11_31) \
        __CG_MATRIX_col_swizzle2(0, 1,2, _m10_m20, _21_31) \
        __CG_MATRIX_col_swizzle2(0, 2,0, _m20_m00, _31_11) \
        __CG_MATRIX_col_swizzle2(0, 2,1, _m20_m10, _31_21) \
        __CG_MATRIX_col_swizzle3(0, 0,1,2, _m00_m10_m20, _11_21_31) \
        __CG_MATRIX_col_swizzle3(0, 0,2,1, _m00_m20_m10, _11_31_21) \
        __CG_MATRIX_col_swizzle3(0, 1,0,2, _m10_m00_m20, _21_11_31) \
        __CG_MATRIX_col_swizzle3(0, 1,2,0, _m10_m20_m00, _21_31_11) \
        __CG_MATRIX_col_swizzle3(0, 2,0,1, _m20_m00_m10, _31_11_21) \
        __CG_MATRIX_col_swizzle3(0, 2,1,0, _m20_m10_m00, _31_21_11)

#define __CG_MATRIX_SWIZZLE_3x2(mode) \
        __CG_MATRIX_element(2, 1, _m21, _32) \
        __CG_MATRIX_##mode##_swizzle2(2, 0,1, _m20_m21, _31_32) \
        __CG_MATRIX_row_swizzle2(2, 1,0, _m21_m20, _32_31) \
        __CG_MATRIX_col_swizzle2(1, 0,2, _m01_m21, _12_32) \
        __CG_MATRIX_col_swizzle2(1, 1,2, _m11_m21, _22_32) \
        __CG_MATRIX_col_swizzle2(1, 2,0, _m21_m01, _32_12) \
        __CG_MATRIX_col_swizzle2(1, 2,1, _m21_m11, _32_22) \
        __CG_MATRIX_col_swizzle3(1, 0,1,2, _m01_m11_m21, _12_22_32) \
        __CG_MATRIX_col_swizzle3(1, 0,2,1, _m01_m21_m11, _12_32_22) \
        __CG_MATRIX_col_swizzle3(1, 1,0,2, _m11_m01_m21, _22_12_32) \
        __CG_MATRIX_col_swizzle3(1, 1,2,0, _m11_m21_m01, _22_32_12) \
        __CG_MATRIX_col_swizzle3(1, 2,0,1, _m21_m01_m11, _32_12_22) \
        __CG_MATRIX_col_swizzle3(1, 2,1,0, _m21_m11_m01, _32_22_12)

#define __CG_MATRIX_SWIZZLE_3x3(mode) \
        __CG_MATRIX_element(2, 2, _m22, _33) \
        __CG_MATRIX_row_swizzle2(2, 0,2, _m20_m22, _31_33) \
        __CG_MATRIX_row_swizzle2(2, 1,2, _m21_m22, _32_33) \
        __CG_MATRIX_row_swizzle2(2, 2,0, _m22_m20, _33_31) \
        __CG_MATRIX_row_swizzle2(2, 2,1, _m22_m21, _33_32) \
        __CG_MATRIX_##mode##_swizzle3(2, 0,1,2, _m20_m21_m22, _31_32_33) \
        __CG_MATRIX_row_swizzle3(2, 0,2,1, _m20_m22_m21, _31_33_32) \
        __CG_MATRIX_row_swizzle3(2, 1,0,2, _m21_m20_m22, _32_31_33) \
        __CG_MATRIX_row_swizzle3(2, 1,2,0, _m21_m22_m20, _32_33_31) \
        __CG_MATRIX_row_swizzle3(2, 2,0,1, _m22_m20_m21, _33_31_32) \
        __CG_MATRIX_row_swizzle3(2, 2,1,0, _m22_m21_m20, _33_32_31) \
        __CG_MATRIX_col_swizzle2(2, 0,2, _m02_m22, _13_33) \
        __CG_MATRIX_col_swizzle2(2, 1,2, _m12_m22, _23_33) \
        __CG_MATRIX_col_swizzle2(2, 2,0, _m22_m02, _33_13) \
        __CG_MATRIX_col_swizzle2(2, 2,1, _m22_m12, _33_23) \
        __CG_MATRIX_col_swizzle3(2, 0,1,2, _m02_m12_m22, _13_23_33) \
        __CG_MATRIX_col_swizzle3(2, 0,2,1, _m02_m22_m12, _13_33_23) \
        __CG_MATRIX_col_swizzle3(2, 1,0,2, _m12_m02_m22, _23_13_33) \
        __CG_MATRIX_col_swizzle3(2, 1,2,0, _m12_m22_m02, _23_33_13) \
        __CG_MATRIX_col_swizzle3(2, 2,0,1, _m22_m02_m12, _33_13_23) \
        __CG_MATRIX_col_swizzle3(2, 2,1,0, _m22_m12_m02, _33_23_13)

#define __CG_MATRIX_SWIZZLE_3x4(mode) \
        __CG_MATRIX_element(2, 3, _m23, _34) \
        __CG_MATRIX_row_swizzle2(2, 0,3, _m20_m23, _31_34) \
        __CG_MATRIX_row_swizzle2(2, 1,3, _m21_m23, _32_34) \
        __CG_MATRIX_row_swizzle2(2, 2,3, _m22_m23, _33_34) \
        __CG_MATRIX_row_swizzle2(2, 3,0, _m23_m20, _34_31) \
        __CG_MATRIX_row_swizzle2(2, 3,1, _m23_m21, _34_32) \
        __CG_MATRIX_row_swizzle2(2, 3,2, _m23_m22, _34_33) \
        __CG_MATRIX_row_swizzle3(2, 0,1,3, _m20_m21_m23, _31_32_34) \
        __CG_MATRIX_row_swizzle3(2, 0,2,3, _m20_m22_m23, _31_33_34) \
        __CG_MATRIX_row_swizzle3(2, 0,3,1, _m20_m23_m21, _31_34_32) \
        __CG_MATRIX_row_swizzle3(2, 0,3,2, _m20_m23_m22, _31_34_33) \
        __CG_MATRIX_row_swizzle3(2, 1,0,3, _m21_m20_m23, _32_31_34) \
        __CG_MATRIX_row_swizzle3(2, 1,2,3, _m21_m22_m23, _32_33_34) \
        __CG_MATRIX_row_swizzle3(2, 1,3,0, _m21_m23_m20, _32_34_31) \
        __CG_MATRIX_row_swizzle3(2, 1,3,2, _m21_m23_m22, _32_34_33) \
        __CG_MATRIX_row_swizzle3(2, 2,0,3, _m22_m20_m23, _33_31_34) \
        __CG_MATRIX_row_swizzle3(2, 2,1,3, _m22_m21_m23, _33_32_34) \
        __CG_MATRIX_row_swizzle3(2, 2,3,0, _m22_m23_m20, _33_34_31) \
        __CG_MATRIX_row_swizzle3(2, 2,3,1, _m22_m23_m21, _33_34_32) \
        __CG_MATRIX_row_swizzle3(2, 3,0,1, _m23_m20_m21, _34_31_32) \
        __CG_MATRIX_row_swizzle3(2, 3,0,2, _m23_m20_m22, _34_31_33) \
        __CG_MATRIX_row_swizzle3(2, 3,1,0, _m23_m21_m20, _34_32_31) \
        __CG_MATRIX_row_swizzle3(2, 3,1,2, _m23_m21_m22, _34_32_33) \
        __CG_MATRIX_row_swizzle3(2, 3,2,0, _m23_m22_m20, _34_33_31) \
        __CG_MATRIX_row_swizzle3(2, 3,2,1, _m23_m22_m21, _34_33_32) \
        __CG_MATRIX_##mode##_swizzle4(2, 0,1,2,3, _m20_m21_m22_m23, _31_32_33_34) \
        __CG_MATRIX_row_swizzle4(2, 0,1,3,2, _m20_m21_m23_m22, _31_32_34_33) \
        __CG_MATRIX_row_swizzle4(2, 0,2,1,3, _m20_m22_m21_m23, _31_33_32_34) \
        __CG_MATRIX_row_swizzle4(2, 0,2,3,1, _m20_m22_m23_m21, _31_33_34_32) \
        __CG_MATRIX_row_swizzle4(2, 0,3,1,2, _m20_m23_m21_m22, _31_34_32_33) \
        __CG_MATRIX_row_swizzle4(2, 0,3,2,1, _m20_m23_m22_m21, _31_34_33_32) \
        __CG_MATRIX_row_swizzle4(2, 1,0,2,3, _m21_m20_m22_m23, _32_31_33_34) \
        __CG_MATRIX_row_swizzle4(2, 1,0,3,2, _m21_m20_m23_m22, _32_31_34_33) \
        __CG_MATRIX_row_swizzle4(2, 1,2,0,3, _m21_m22_m20_m23, _32_33_31_34) \
        __CG_MATRIX_row_swizzle4(2, 1,2,3,0, _m21_m22_m23_m20, _32_33_34_31) \
        __CG_MATRIX_row_swizzle4(2, 1,3,0,2, _m21_m23_m20_m22, _32_34_31_33) \
        __CG_MATRIX_row_swizzle4(2, 1,3,2,0, _m21_m23_m22_m20, _32_34_33_31) \
        __CG_MATRIX_row_swizzle4(2, 2,0,1,3, _m22_m20_m21_m23, _33_31_32_34) \
        __CG_MATRIX_row_swizzle4(2, 2,0,3,1, _m22_m20_m23_m21, _33_31_34_32) \
        __CG_MATRIX_row_swizzle4(2, 2,1,0,3, _m22_m21_m20_m23, _33_32_31_34) \
        __CG_MATRIX_row_swizzle4(2, 2,1,3,0, _m22_m21_m23_m20, _33_32_34_31) \
        __CG_MATRIX_row_swizzle4(2, 2,3,0,1, _m22_m23_m20_m21, _33_34_31_32) \
        __CG_MATRIX_row_swizzle4(2, 2,3,1,0, _m22_m23_m21_m20, _33_34_32_31) \
        __CG_MATRIX_row_swizzle4(2, 3,0,1,2, _m23_m20_m21_m22, _34_31_32_33) \
        __CG_MATRIX_row_swizzle4(2, 3,0,2,1, _m23_m20_m22_m21, _34_31_33_32) \
        __CG_MATRIX_row_swizzle4(2, 3,1,0,2, _m23_m21_m20_m22, _34_32_31_33) \
        __CG_MATRIX_row_swizzle4(2, 3,1,2,0, _m23_m21_m22_m20, _34_32_33_31) \
        __CG_MATRIX_row_swizzle4(2, 3,2,0,1, _m23_m22_m20_m21, _34_33_31_32) \
        __CG_MATRIX_row_swizzle4(2, 3,2,1,0, _m23_m22_m21_m20, _34_33_32_31) \
        __CG_MATRIX_col_swizzle2(3, 0,2, _m03_m23, _14_34) \
        __CG_MATRIX_col_swizzle2(3, 1,2, _m13_m23, _24_34) \
        __CG_MATRIX_col_swizzle2(3, 2,0, _m23_m03, _34_14) \
        __CG_MATRIX_col_swizzle2(3, 2,1, _m23_m13, _34_24) \
        __CG_MATRIX_col_swizzle3(3, 0,1,2, _m03_m13_m23, _14_24_34) \
        __CG_MATRIX_col_swizzle3(3, 0,2,1, _m03_m23_m13, _14_34_24) \
        __CG_MATRIX_col_swizzle3(3, 1,0,2, _m13_m03_m23, _24_14_34) \
        __CG_MATRIX_col_swizzle3(3, 1,2,0, _m13_m23_m03, _24_34_14) \
        __CG_MATRIX_col_swizzle3(3, 2,0,1, _m23_m03_m13, _34_14_24) \
        __CG_MATRIX_col_swizzle3(3, 2,1,0, _m23_m13_m03, _34_24_14)

#define __CG_MATRIX_SWIZZLE_4x1(mode) \
        __CG_MATRIX_##mode(3, 0, _m30, _41) \
        __CG_MATRIX_col_swizzle2(0, 0,3, _m00_m30, _11_41) \
        __CG_MATRIX_col_swizzle2(0, 1,3, _m10_m30, _21_41) \
        __CG_MATRIX_col_swizzle2(0, 2,3, _m20_m30, _31_41) \
        __CG_MATRIX_col_swizzle2(0, 3,0, _m30_m00, _41_11) \
        __CG_MATRIX_col_swizzle2(0, 3,1, _m30_m10, _41_21) \
        __CG_MATRIX_col_swizzle2(0, 3,2, _m30_m20, _41_31) \
        __CG_MATRIX_col_swizzle3(0, 0,1,3, _m00_m10_m30, _11_21_41) \
        __CG_MATRIX_col_swizzle3(0, 0,2,3, _m00_m20_m30, _11_31_41) \
        __CG_MATRIX_col_swizzle3(0, 0,3,1, _m00_m30_m10, _11_41_21) \
        __CG_MATRIX_col_swizzle3(0, 0,3,2, _m00_m30_m20, _11_41_31) \
        __CG_MATRIX_col_swizzle3(0, 1,0,3, _m10_m00_m30, _21_11_41) \
        __CG_MATRIX_col_swizzle3(0, 1,2,3, _m10_m20_m30, _21_31_41) \
        __CG_MATRIX_col_swizzle3(0, 1,3,0, _m10_m30_m00, _21_41_11) \
        __CG_MATRIX_col_swizzle3(0, 1,3,2, _m10_m30_m20, _21_41_31) \
        __CG_MATRIX_col_swizzle3(0, 2,0,3, _m20_m00_m30, _31_11_41) \
        __CG_MATRIX_col_swizzle3(0, 2,1,3, _m20_m10_m30, _31_21_41) \
        __CG_MATRIX_col_swizzle3(0, 2,3,0, _m20_m30_m00, _31_41_11) \
        __CG_MATRIX_col_swizzle3(0, 2,3,1, _m20_m30_m10, _31_41_21) \
        __CG_MATRIX_col_swizzle3(0, 3,0,1, _m30_m00_m10, _41_11_21) \
        __CG_MATRIX_col_swizzle3(0, 3,0,2, _m30_m00_m20, _41_11_31) \
        __CG_MATRIX_col_swizzle3(0, 3,1,0, _m30_m10_m00, _41_21_11) \
        __CG_MATRIX_col_swizzle3(0, 3,1,2, _m30_m10_m20, _41_21_31) \
        __CG_MATRIX_col_swizzle3(0, 3,2,0, _m30_m20_m00, _41_31_11) \
        __CG_MATRIX_col_swizzle3(0, 3,2,1, _m30_m20_m10, _41_31_21) \
        __CG_MATRIX_col_swizzle4(0, 0,1,2,3, _m00_m10_m20_m30, _11_21_31_41) \
        __CG_MATRIX_col_swizzle4(0, 0,1,3,2, _m00_m10_m30_m20, _11_21_41_31) \
        __CG_MATRIX_col_swizzle4(0, 0,2,1,3, _m00_m20_m10_m30, _11_31_21_41) \
        __CG_MATRIX_col_swizzle4(0, 0,2,3,1, _m00_m20_m30_m10, _11_31_41_21) \
        __CG_MATRIX_col_swizzle4(0, 0,3,1,2, _m00_m30_m10_m20, _11_41_21_31) \
        __CG_MATRIX_col_swizzle4(0, 0,3,2,1, _m00_m30_m20_m10, _11_41_31_21) \
        __CG_MATRIX_col_swizzle4(0, 1,0,2,3, _m10_m00_m20_m30, _21_11_31_41) \
        __CG_MATRIX_col_swizzle4(0, 1,0,3,2, _m10_m00_m30_m20, _21_11_41_31) \
        __CG_MATRIX_col_swizzle4(0, 1,2,0,3, _m10_m20_m00_m30, _21_31_11_41) \
        __CG_MATRIX_col_swizzle4(0, 1,2,3,0, _m10_m20_m30_m00, _21_31_41_11) \
        __CG_MATRIX_col_swizzle4(0, 1,3,0,2, _m10_m30_m00_m20, _21_41_11_31) \
        __CG_MATRIX_col_swizzle4(0, 1,3,2,0, _m10_m30_m20_m00, _21_41_31_11) \
        __CG_MATRIX_col_swizzle4(0, 2,0,1,3, _m20_m00_m10_m30, _31_11_21_41) \
        __CG_MATRIX_col_swizzle4(0, 2,0,3,1, _m20_m00_m30_m10, _31_11_41_21) \
        __CG_MATRIX_col_swizzle4(0, 2,1,0,3, _m20_m10_m00_m30, _31_21_11_41) \
        __CG_MATRIX_col_swizzle4(0, 2,1,3,0, _m20_m10_m30_m00, _31_21_41_11) \
        __CG_MATRIX_col_swizzle4(0, 2,3,0,1, _m20_m30_m00_m10, _31_41_11_21) \
        __CG_MATRIX_col_swizzle4(0, 2,3,1,0, _m20_m30_m10_m00, _31_41_21_11) \
        __CG_MATRIX_col_swizzle4(0, 3,0,1,2, _m30_m00_m10_m20, _41_11_21_31) \
        __CG_MATRIX_col_swizzle4(0, 3,0,2,1, _m30_m00_m20_m10, _41_11_31_21) \
        __CG_MATRIX_col_swizzle4(0, 3,1,0,2, _m30_m10_m00_m20, _41_21_11_31) \
        __CG_MATRIX_col_swizzle4(0, 3,1,2,0, _m30_m10_m20_m00, _41_21_31_11) \
        __CG_MATRIX_col_swizzle4(0, 3,2,0,1, _m30_m20_m00_m10, _41_31_11_21) \
        __CG_MATRIX_col_swizzle4(0, 3,2,1,0, _m30_m20_m10_m00, _41_31_21_11)

#define __CG_MATRIX_SWIZZLE_4x2(mode) \
        __CG_MATRIX_element(3, 1, _m31, _42) \
        __CG_MATRIX_##mode##_swizzle2(3, 0,1, _m30_m31, _41_42) \
        __CG_MATRIX_row_swizzle2(3, 1,0, _m31_m30, _42_41) \
        __CG_MATRIX_col_swizzle2(1, 0,3, _m01_m31, _12_42) \
        __CG_MATRIX_col_swizzle2(1, 1,3, _m11_m31, _22_42) \
        __CG_MATRIX_col_swizzle2(1, 2,3, _m21_m31, _32_42) \
        __CG_MATRIX_col_swizzle2(1, 3,0, _m31_m01, _42_12) \
        __CG_MATRIX_col_swizzle2(1, 3,1, _m31_m11, _42_22) \
        __CG_MATRIX_col_swizzle2(1, 3,2, _m31_m21, _42_32) \
        __CG_MATRIX_col_swizzle3(1, 0,1,3, _m01_m11_m31, _12_22_42) \
        __CG_MATRIX_col_swizzle3(1, 0,2,3, _m01_m21_m31, _12_32_42) \
        __CG_MATRIX_col_swizzle3(1, 0,3,1, _m01_m31_m11, _12_42_22) \
        __CG_MATRIX_col_swizzle3(1, 0,3,2, _m01_m31_m21, _12_42_32) \
        __CG_MATRIX_col_swizzle3(1, 1,0,3, _m11_m01_m31, _22_12_42) \
        __CG_MATRIX_col_swizzle3(1, 1,2,3, _m11_m21_m31, _22_32_42) \
        __CG_MATRIX_col_swizzle3(1, 1,3,0, _m11_m31_m01, _22_42_12) \
        __CG_MATRIX_col_swizzle3(1, 1,3,2, _m11_m31_m21, _22_42_32) \
        __CG_MATRIX_col_swizzle3(1, 2,0,3, _m21_m01_m31, _32_12_42) \
        __CG_MATRIX_col_swizzle3(1, 2,1,3, _m21_m11_m31, _32_22_42) \
        __CG_MATRIX_col_swizzle3(1, 2,3,0, _m21_m31_m01, _32_42_12) \
        __CG_MATRIX_col_swizzle3(1, 2,3,1, _m21_m31_m11, _32_42_22) \
        __CG_MATRIX_col_swizzle3(1, 3,0,1, _m31_m01_m11, _42_12_22) \
        __CG_MATRIX_col_swizzle3(1, 3,0,2, _m31_m01_m21, _42_12_32) \
        __CG_MATRIX_col_swizzle3(1, 3,1,0, _m31_m11_m01, _42_22_12) \
        __CG_MATRIX_col_swizzle3(1, 3,1,2, _m31_m11_m21, _42_22_32) \
        __CG_MATRIX_col_swizzle3(1, 3,2,0, _m31_m21_m01, _42_32_12) \
        __CG_MATRIX_col_swizzle3(1, 3,2,1, _m31_m21_m11, _42_32_22) \
        __CG_MATRIX_col_swizzle4(1, 0,1,2,3, _m01_m11_m21_m31, _12_22_32_42) \
        __CG_MATRIX_col_swizzle4(1, 0,1,3,2, _m01_m11_m31_m21, _12_22_42_32) \
        __CG_MATRIX_col_swizzle4(1, 0,2,1,3, _m01_m21_m11_m31, _12_32_22_42) \
        __CG_MATRIX_col_swizzle4(1, 0,2,3,1, _m01_m21_m31_m11, _12_32_42_22) \
        __CG_MATRIX_col_swizzle4(1, 0,3,1,2, _m01_m31_m11_m21, _12_42_22_32) \
        __CG_MATRIX_col_swizzle4(1, 0,3,2,1, _m01_m31_m21_m11, _12_42_32_22) \
        __CG_MATRIX_col_swizzle4(1, 1,0,2,3, _m11_m01_m21_m31, _22_12_32_42) \
        __CG_MATRIX_col_swizzle4(1, 1,0,3,2, _m11_m01_m31_m21, _22_12_42_32) \
        __CG_MATRIX_col_swizzle4(1, 1,2,0,3, _m11_m21_m01_m31, _22_32_12_42) \
        __CG_MATRIX_col_swizzle4(1, 1,2,3,0, _m11_m21_m31_m01, _22_32_42_12) \
        __CG_MATRIX_col_swizzle4(1, 1,3,0,2, _m11_m31_m01_m21, _22_42_12_32) \
        __CG_MATRIX_col_swizzle4(1, 1,3,2,0, _m11_m31_m21_m01, _22_42_32_12) \
        __CG_MATRIX_col_swizzle4(1, 2,0,1,3, _m21_m01_m11_m31, _32_12_22_42) \
        __CG_MATRIX_col_swizzle4(1, 2,0,3,1, _m21_m01_m31_m11, _32_12_42_22) \
        __CG_MATRIX_col_swizzle4(1, 2,1,0,3, _m21_m11_m01_m31, _32_22_12_42) \
        __CG_MATRIX_col_swizzle4(1, 2,1,3,0, _m21_m11_m31_m01, _32_22_42_12) \
        __CG_MATRIX_col_swizzle4(1, 2,3,0,1, _m21_m31_m01_m11, _32_42_12_22) \
        __CG_MATRIX_col_swizzle4(1, 2,3,1,0, _m21_m31_m11_m01, _32_42_22_12) \
        __CG_MATRIX_col_swizzle4(1, 3,0,1,2, _m31_m01_m11_m21, _42_12_22_32) \
        __CG_MATRIX_col_swizzle4(1, 3,0,2,1, _m31_m01_m21_m11, _42_12_32_22) \
        __CG_MATRIX_col_swizzle4(1, 3,1,0,2, _m31_m11_m01_m21, _42_22_12_32) \
        __CG_MATRIX_col_swizzle4(1, 3,1,2,0, _m31_m11_m21_m01, _42_22_32_12) \
        __CG_MATRIX_col_swizzle4(1, 3,2,0,1, _m31_m21_m01_m11, _42_32_12_22) \
        __CG_MATRIX_col_swizzle4(1, 3,2,1,0, _m31_m21_m11_m01, _42_32_22_12)

#define __CG_MATRIX_SWIZZLE_4x3(mode) \
        __CG_MATRIX_element(3, 2, _m32, _43) \
        __CG_MATRIX_row_swizzle2(3, 0,2, _m30_m32, _41_43) \
        __CG_MATRIX_row_swizzle2(3, 1,2, _m31_m32, _42_43) \
        __CG_MATRIX_row_swizzle2(3, 2,0, _m32_m30, _43_41) \
        __CG_MATRIX_row_swizzle2(3, 2,1, _m32_m31, _43_42) \
        __CG_MATRIX_##mode##_swizzle3(3, 0,1,2, _m30_m31_m32, _41_42_43) \
        __CG_MATRIX_row_swizzle3(3, 0,2,1, _m30_m32_m31, _41_43_42) \
        __CG_MATRIX_row_swizzle3(3, 1,0,2, _m31_m30_m32, _42_41_43) \
        __CG_MATRIX_row_swizzle3(3, 1,2,0, _m31_m32_m30, _42_43_41) \
        __CG_MATRIX_row_swizzle3(3, 2,0,1, _m32_m30_m31, _43_41_42) \
        __CG_MATRIX_row_swizzle3(3, 2,1,0, _m32_m31_m30, _43_42_41) \
        __CG_MATRIX_col_swizzle2(2, 0,3, _m02_m32, _13_43) \
        __CG_MATRIX_col_swizzle2(2, 1,3, _m12_m32, _23_43) \
        __CG_MATRIX_col_swizzle2(2, 2,3, _m22_m32, _33_43) \
        __CG_MATRIX_col_swizzle2(2, 3,0, _m32_m02, _43_13) \
        __CG_MATRIX_col_swizzle2(2, 3,1, _m32_m12, _43_23) \
        __CG_MATRIX_col_swizzle2(2, 3,2, _m32_m22, _43_33) \
        __CG_MATRIX_col_swizzle3(2, 0,1,3, _m02_m12_m32, _13_23_43) \
        __CG_MATRIX_col_swizzle3(2, 0,2,3, _m02_m22_m32, _13_33_43) \
        __CG_MATRIX_col_swizzle3(2, 0,3,1, _m02_m32_m12, _13_43_23) \
        __CG_MATRIX_col_swizzle3(2, 0,3,2, _m02_m32_m22, _13_43_33) \
        __CG_MATRIX_col_swizzle3(2, 1,0,3, _m12_m02_m32, _23_13_43) \
        __CG_MATRIX_col_swizzle3(2, 1,2,3, _m12_m22_m32, _23_33_43) \
        __CG_MATRIX_col_swizzle3(2, 1,3,0, _m12_m32_m02, _23_43_13) \
        __CG_MATRIX_col_swizzle3(2, 1,3,2, _m12_m32_m22, _23_43_33) \
        __CG_MATRIX_col_swizzle3(2, 2,0,3, _m22_m02_m32, _33_13_43) \
        __CG_MATRIX_col_swizzle3(2, 2,1,3, _m22_m12_m32, _33_23_43) \
        __CG_MATRIX_col_swizzle3(2, 2,3,0, _m22_m32_m02, _33_43_13) \
        __CG_MATRIX_col_swizzle3(2, 2,3,1, _m22_m32_m12, _33_43_23) \
        __CG_MATRIX_col_swizzle3(2, 3,0,1, _m32_m02_m12, _43_13_23) \
        __CG_MATRIX_col_swizzle3(2, 3,0,2, _m32_m02_m22, _43_13_33) \
        __CG_MATRIX_col_swizzle3(2, 3,1,0, _m32_m12_m02, _43_23_13) \
        __CG_MATRIX_col_swizzle3(2, 3,1,2, _m32_m12_m22, _43_23_33) \
        __CG_MATRIX_col_swizzle3(2, 3,2,0, _m32_m22_m02, _43_33_13) \
        __CG_MATRIX_col_swizzle3(2, 3,2,1, _m32_m22_m12, _43_33_23) \
        __CG_MATRIX_col_swizzle4(2, 0,1,2,3, _m02_m12_m22_m32, _13_23_33_43) \
        __CG_MATRIX_col_swizzle4(2, 0,1,3,2, _m02_m12_m32_m22, _13_23_43_33) \
        __CG_MATRIX_col_swizzle4(2, 0,2,1,3, _m02_m22_m12_m32, _13_33_23_43) \
        __CG_MATRIX_col_swizzle4(2, 0,2,3,1, _m02_m22_m32_m12, _13_33_43_23) \
        __CG_MATRIX_col_swizzle4(2, 0,3,1,2, _m02_m32_m12_m22, _13_43_23_33) \
        __CG_MATRIX_col_swizzle4(2, 0,3,2,1, _m02_m32_m22_m12, _13_43_33_23) \
        __CG_MATRIX_col_swizzle4(2, 1,0,2,3, _m12_m02_m22_m32, _23_13_33_43) \
        __CG_MATRIX_col_swizzle4(2, 1,0,3,2, _m12_m02_m32_m22, _23_13_43_33) \
        __CG_MATRIX_col_swizzle4(2, 1,2,0,3, _m12_m22_m02_m32, _23_33_13_43) \
        __CG_MATRIX_col_swizzle4(2, 1,2,3,0, _m12_m22_m32_m02, _23_33_43_13) \
        __CG_MATRIX_col_swizzle4(2, 1,3,0,2, _m12_m32_m02_m22, _23_43_13_33) \
        __CG_MATRIX_col_swizzle4(2, 1,3,2,0, _m12_m32_m22_m02, _23_43_33_13) \
        __CG_MATRIX_col_swizzle4(2, 2,0,1,3, _m22_m02_m12_m32, _33_13_23_43) \
        __CG_MATRIX_col_swizzle4(2, 2,0,3,1, _m22_m02_m32_m12, _33_13_43_23) \
        __CG_MATRIX_col_swizzle4(2, 2,1,0,3, _m22_m12_m02_m32, _33_23_13_43) \
        __CG_MATRIX_col_swizzle4(2, 2,1,3,0, _m22_m12_m32_m02, _33_23_43_13) \
        __CG_MATRIX_col_swizzle4(2, 2,3,0,1, _m22_m32_m02_m12, _33_43_13_23) \
        __CG_MATRIX_col_swizzle4(2, 2,3,1,0, _m22_m32_m12_m02, _33_43_23_13) \
        __CG_MATRIX_col_swizzle4(2, 3,0,1,2, _m32_m02_m12_m22, _43_13_23_33) \
        __CG_MATRIX_col_swizzle4(2, 3,0,2,1, _m32_m02_m22_m12, _43_13_33_23) \
        __CG_MATRIX_col_swizzle4(2, 3,1,0,2, _m32_m12_m02_m22, _43_23_13_33) \
        __CG_MATRIX_col_swizzle4(2, 3,1,2,0, _m32_m12_m22_m02, _43_23_33_13) \
        __CG_MATRIX_col_swizzle4(2, 3,2,0,1, _m32_m22_m02_m12, _43_33_13_23) \
        __CG_MATRIX_col_swizzle4(2, 3,2,1,0, _m32_m22_m12_m02, _43_33_23_13)

#define __CG_MATRIX_SWIZZLE_4x4(mode) \
        __CG_MATRIX_element(3, 3, _m33, _44) \
        __CG_MATRIX_row_swizzle2(3, 0,3, _m30_m33, _41_44) \
        __CG_MATRIX_row_swizzle2(3, 1,3, _m31_m33, _42_44) \
        __CG_MATRIX_row_swizzle2(3, 2,3, _m32_m33, _43_44) \
        __CG_MATRIX_row_swizzle2(3, 3,0, _m33_m30, _44_41) \
        __CG_MATRIX_row_swizzle2(3, 3,1, _m33_m31, _44_42) \
        __CG_MATRIX_row_swizzle2(3, 3,2, _m33_m32, _44_43) \
        __CG_MATRIX_row_swizzle3(3, 0,1,3, _m30_m31_m33, _41_42_44) \
        __CG_MATRIX_row_swizzle3(3, 0,2,3, _m30_m32_m33, _41_43_44) \
        __CG_MATRIX_row_swizzle3(3, 0,3,1, _m30_m33_m31, _41_44_42) \
        __CG_MATRIX_row_swizzle3(3, 0,3,2, _m30_m33_m32, _41_44_43) \
        __CG_MATRIX_row_swizzle3(3, 1,0,3, _m31_m30_m33, _42_41_44) \
        __CG_MATRIX_row_swizzle3(3, 1,2,3, _m31_m32_m33, _42_43_44) \
        __CG_MATRIX_row_swizzle3(3, 1,3,0, _m31_m33_m30, _42_44_41) \
        __CG_MATRIX_row_swizzle3(3, 1,3,2, _m31_m33_m32, _42_44_43) \
        __CG_MATRIX_row_swizzle3(3, 2,0,3, _m32_m30_m33, _43_41_44) \
        __CG_MATRIX_row_swizzle3(3, 2,1,3, _m32_m31_m33, _43_42_44) \
        __CG_MATRIX_row_swizzle3(3, 2,3,0, _m32_m33_m30, _43_44_41) \
        __CG_MATRIX_row_swizzle3(3, 2,3,1, _m32_m33_m31, _43_44_42) \
        __CG_MATRIX_row_swizzle3(3, 3,0,1, _m33_m30_m31, _44_41_42) \
        __CG_MATRIX_row_swizzle3(3, 3,0,2, _m33_m30_m32, _44_41_43) \
        __CG_MATRIX_row_swizzle3(3, 3,1,0, _m33_m31_m30, _44_42_41) \
        __CG_MATRIX_row_swizzle3(3, 3,1,2, _m33_m31_m32, _44_42_43) \
        __CG_MATRIX_row_swizzle3(3, 3,2,0, _m33_m32_m30, _44_43_41) \
        __CG_MATRIX_row_swizzle3(3, 3,2,1, _m33_m32_m31, _44_43_42) \
        __CG_MATRIX_##mode##_swizzle4(3, 0,1,2,3, _m30_m31_m32_m33, _41_42_43_44) \
        __CG_MATRIX_row_swizzle4(3, 0,1,3,2, _m30_m31_m33_m32, _41_42_44_43) \
        __CG_MATRIX_row_swizzle4(3, 0,2,1,3, _m30_m32_m31_m33, _41_43_42_44) \
        __CG_MATRIX_row_swizzle4(3, 0,2,3,1, _m30_m32_m33_m31, _41_43_44_42) \
        __CG_MATRIX_row_swizzle4(3, 0,3,1,2, _m30_m33_m31_m32, _41_44_42_43) \
        __CG_MATRIX_row_swizzle4(3, 0,3,2,1, _m30_m33_m32_m31, _41_44_43_42) \
        __CG_MATRIX_row_swizzle4(3, 1,0,2,3, _m31_m30_m32_m33, _42_41_43_44) \
        __CG_MATRIX_row_swizzle4(3, 1,0,3,2, _m31_m30_m33_m32, _42_41_44_43) \
        __CG_MATRIX_row_swizzle4(3, 1,2,0,3, _m31_m32_m30_m33, _42_43_41_44) \
        __CG_MATRIX_row_swizzle4(3, 1,2,3,0, _m31_m32_m33_m30, _42_43_44_41) \
        __CG_MATRIX_row_swizzle4(3, 1,3,0,2, _m31_m33_m30_m32, _42_44_41_43) \
        __CG_MATRIX_row_swizzle4(3, 1,3,2,0, _m31_m33_m32_m30, _42_44_43_41) \
        __CG_MATRIX_row_swizzle4(3, 2,0,1,3, _m32_m30_m31_m33, _43_41_42_44) \
        __CG_MATRIX_row_swizzle4(3, 2,0,3,1, _m32_m30_m33_m31, _43_41_44_42) \
        __CG_MATRIX_row_swizzle4(3, 2,1,0,3, _m32_m31_m30_m33, _43_42_41_44) \
        __CG_MATRIX_row_swizzle4(3, 2,1,3,0, _m32_m31_m33_m30, _43_42_44_41) \
        __CG_MATRIX_row_swizzle4(3, 2,3,0,1, _m32_m33_m30_m31, _43_44_41_42) \
        __CG_MATRIX_row_swizzle4(3, 2,3,1,0, _m32_m33_m31_m30, _43_44_42_41) \
        __CG_MATRIX_row_swizzle4(3, 3,0,1,2, _m33_m30_m31_m32, _44_41_42_43) \
        __CG_MATRIX_row_swizzle4(3, 3,0,2,1, _m33_m30_m32_m31, _44_41_43_42) \
        __CG_MATRIX_row_swizzle4(3, 3,1,0,2, _m33_m31_m30_m32, _44_42_41_43) \
        __CG_MATRIX_row_swizzle4(3, 3,1,2,0, _m33_m31_m32_m30, _44_42_43_41) \
        __CG_MATRIX_row_swizzle4(3, 3,2,0,1, _m33_m32_m30_m31, _44_43_41_42) \
        __CG_MATRIX_row_swizzle4(3, 3,2,1,0, _m33_m32_m31_m30, _44_43_42_41) \
        __CG_MATRIX_col_swizzle2(3, 0,3, _m03_m33, _14_44) \
        __CG_MATRIX_col_swizzle2(3, 1,3, _m13_m33, _24_44) \
        __CG_MATRIX_col_swizzle2(3, 2,3, _m23_m33, _34_44) \
        __CG_MATRIX_col_swizzle2(3, 3,0, _m33_m03, _44_14) \
        __CG_MATRIX_col_swizzle2(3, 3,1, _m33_m13, _44_24) \
        __CG_MATRIX_col_swizzle2(3, 3,2, _m33_m23, _44_34) \
        __CG_MATRIX_col_swizzle3(3, 0,1,3, _m03_m13_m33, _14_24_44) \
        __CG_MATRIX_col_swizzle3(3, 0,2,3, _m03_m23_m33, _14_34_44) \
        __CG_MATRIX_col_swizzle3(3, 0,3,1, _m03_m33_m13, _14_44_24) \
        __CG_MATRIX_col_swizzle3(3, 0,3,2, _m03_m33_m23, _14_44_34) \
        __CG_MATRIX_col_swizzle3(3, 1,0,3, _m13_m03_m33, _24_14_44) \
        __CG_MATRIX_col_swizzle3(3, 1,2,3, _m13_m23_m33, _24_34_44) \
        __CG_MATRIX_col_swizzle3(3, 1,3,0, _m13_m33_m03, _24_44_14) \
        __CG_MATRIX_col_swizzle3(3, 1,3,2, _m13_m33_m23, _24_44_34) \
        __CG_MATRIX_col_swizzle3(3, 2,0,3, _m23_m03_m33, _34_14_44) \
        __CG_MATRIX_col_swizzle3(3, 2,1,3, _m23_m13_m33, _34_24_44) \
        __CG_MATRIX_col_swizzle3(3, 2,3,0, _m23_m33_m03, _34_44_14) \
        __CG_MATRIX_col_swizzle3(3, 2,3,1, _m23_m33_m13, _34_44_24) \
        __CG_MATRIX_col_swizzle3(3, 3,0,1, _m33_m03_m13, _44_14_24) \
        __CG_MATRIX_col_swizzle3(3, 3,0,2, _m33_m03_m23, _44_14_34) \
        __CG_MATRIX_col_swizzle3(3, 3,1,0, _m33_m13_m03, _44_24_14) \
        __CG_MATRIX_col_swizzle3(3, 3,1,2, _m33_m13_m23, _44_24_34) \
        __CG_MATRIX_col_swizzle3(3, 3,2,0, _m33_m23_m03, _44_34_14) \
        __CG_MATRIX_col_swizzle3(3, 3,2,1, _m33_m23_m13, _44_34_24) \
        __CG_MATRIX_col_swizzle4(3, 0,1,2,3, _m03_m13_m23_m33, _14_24_34_44) \
        __CG_MATRIX_col_swizzle4(3, 0,1,3,2, _m03_m13_m33_m23, _14_24_44_34) \
        __CG_MATRIX_col_swizzle4(3, 0,2,1,3, _m03_m23_m13_m33, _14_34_24_44) \
        __CG_MATRIX_col_swizzle4(3, 0,2,3,1, _m03_m23_m33_m13, _14_34_44_24) \
        __CG_MATRIX_col_swizzle4(3, 0,3,1,2, _m03_m33_m13_m23, _14_44_24_34) \
        __CG_MATRIX_col_swizzle4(3, 0,3,2,1, _m03_m33_m23_m13, _14_44_34_24) \
        __CG_MATRIX_col_swizzle4(3, 1,0,2,3, _m13_m03_m23_m33, _24_14_34_44) \
        __CG_MATRIX_col_swizzle4(3, 1,0,3,2, _m13_m03_m33_m23, _24_14_44_34) \
        __CG_MATRIX_col_swizzle4(3, 1,2,0,3, _m13_m23_m03_m33, _24_34_14_44) \
        __CG_MATRIX_col_swizzle4(3, 1,2,3,0, _m13_m23_m33_m03, _24_34_44_14) \
        __CG_MATRIX_col_swizzle4(3, 1,3,0,2, _m13_m33_m03_m23, _24_44_14_34) \
        __CG_MATRIX_col_swizzle4(3, 1,3,2,0, _m13_m33_m23_m03, _24_44_34_14) \
        __CG_MATRIX_col_swizzle4(3, 2,0,1,3, _m23_m03_m13_m33, _34_14_24_44) \
        __CG_MATRIX_col_swizzle4(3, 2,0,3,1, _m23_m03_m33_m13, _34_14_44_24) \
        __CG_MATRIX_col_swizzle4(3, 2,1,0,3, _m23_m13_m03_m33, _34_24_14_44) \
        __CG_MATRIX_col_swizzle4(3, 2,1,3,0, _m23_m13_m33_m03, _34_24_44_14) \
        __CG_MATRIX_col_swizzle4(3, 2,3,0,1, _m23_m33_m03_m13, _34_44_14_24) \
        __CG_MATRIX_col_swizzle4(3, 2,3,1,0, _m23_m33_m13_m03, _34_44_24_14) \
        __CG_MATRIX_col_swizzle4(3, 3,0,1,2, _m33_m03_m13_m23, _44_14_24_34) \
        __CG_MATRIX_col_swizzle4(3, 3,0,2,1, _m33_m03_m23_m13, _44_14_34_24) \
        __CG_MATRIX_col_swizzle4(3, 3,1,0,2, _m33_m13_m03_m23, _44_24_14_34) \
        __CG_MATRIX_col_swizzle4(3, 3,1,2,0, _m33_m13_m23_m03, _44_24_34_14) \
        __CG_MATRIX_col_swizzle4(3, 3,2,0,1, _m33_m23_m03_m13, _44_34_14_24) \
        __CG_MATRIX_col_swizzle4(3, 3,2,1,0, _m33_m23_m13_m03, _44_34_24_14)

template <typename T, int ROWS, int COLS, typename Ttrait = __CGtype_trait<T,T> >
class __CGmatrix_storage { } __CGmay_alias;

template <typename T, int ROWS, int COLS, typename Ttrait = __CGtype_trait<T,T> >
class __CGmatrix { };

template <typename T, int ROWS, int COLS>
class __CGmatrix_usage
    : public __CGmatrix_storage<T,ROWS,COLS>
    __CG_DERIVE_FROM_BASE_CLASS
{
private:
    static const int rows = ROWS;
    static const int cols = COLS;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    // CONSTRUCTORS
    inline __CGmatrix_usage() { }
    inline __CGmatrix_usage(const T & s) { 
        for (int i=0; i<ROWS; i++) {
            for (int j=0; j<COLS; j++) {
                (*this)[i][j] = s;
            }
        }
    }
    template <typename T2>
    inline __CGmatrix_usage(const __CGmatrix_usage<T2,ROWS,COLS> & v) {
        for (int i=0; i<ROWS; i++) {
            (*this)[i] = __CGvector<T,COLS>(v[i]);
        }
    }

    // INDEXING
    // Use reinterpret_cast magic to make indexing of rows work.
    inline __CGvector<T,COLS> & operator [] (int i) {
        storageType *base = reinterpret_cast<storageType*>(this);
        base += COLS*i;
        __CGvector<T,COLS> *row = reinterpret_cast<__CGvector<T,COLS>* >(base);
        return *row;
    }
    inline const __CGvector<T,COLS> & operator [] (int i) const { 
        const storageType *base = reinterpret_cast<const storageType*>(this);
        base += COLS*i;
        const __CGvector<T,COLS> *row = reinterpret_cast<const __CGvector<T,COLS>* >(base);
        return *row;
    }

    // BINARY OPERATIONS
    template <typename T2>
    inline __CGmatrix_usage & operator = (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] = __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator = (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] = s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator += (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] += __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator += (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] += s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator -= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] -= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator -= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] -= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator *= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] *= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator *= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] *= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator /= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] /= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator /= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] /= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator %= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] %= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator %= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] %= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator ^= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] ^= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator ^= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] ^= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator &= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] &= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator &= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] &= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator |= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] |= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator |= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] |= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator <<= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] <<= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator <<= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] <<= s;
        return *this;
    }
    template <typename T2>
    inline __CGmatrix_usage & operator >>= (const  __CGmatrix_usage <T2,ROWS,COLS> & ind) { /* for: foo = bar */
        for (int i=0; i<ROWS; i++)
            (*this)[i] >>= __CGvector<T,COLS>(ind[i]);
        return *this;
    }
    inline __CGmatrix_usage & operator >>= (const T & s) { /* for: foo = 2.7 */
        for (int i=0; i<ROWS; i++)
            (*this)[i] >>= s;
        return *this;
    }

    // UNARY OPERATIONS
    inline __CGmatrix_usage & operator ++ () { /* prefix */
        for (int i=0; i<ROWS; i++)
            ++ (*this)[i];
        return *this;
    }
    inline __CGmatrix_usage operator ++ (int) { /* postfix */
        __CGmatrix_usage oldVector = *this;
        for (int i=0; i<ROWS; i++)
            ++ (*this)[i];
        return oldVector;
    }
    inline __CGmatrix_usage & operator -- () { /* prefix */
        for (int i=0; i<ROWS; i++)
            -- (*this)[i];
        return *this;
    }
    inline __CGmatrix_usage operator -- (int) { /* postfix */
        __CGmatrix_usage oldVector = *this;
        for (int i=0; i<ROWS; i++)
            -- (*this)[i];
        return oldVector;
    }
} __CGmay_alias;

template <typename T>
class __CGmatrix_storage<T,1,1> {
    static const int rows = 1;
    static const int cols = 1;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(ELEMENT)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,1,1> : public __CGmatrix_usage<T,1,1> {
    static const int rows = 1;
    static const int cols = 1;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    template <typename TA,
              typename TAstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0) {
        (*this)[0] = __CGvector<T,cols>(v0);
    }
    // CONVERSION TO SCALAR (just for 1x1 case)
    operator T() const {
        return (*this)[0][0];
    }
};

template <typename T>
class __CGmatrix_storage<T,1,2> {
    static const int rows = 1;
    static const int cols = 2;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,1,2> : public __CGmatrix_usage<T,1,2> {
    static const int rows = 1;
    static const int cols = 2;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01) {
        (*this)[0][0] = m00; (*this)[0][1] = m01;
    }
    template <typename TA,
              typename TAstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0) {
        (*this)[0] = __CGvector<T,cols>(v0);
    }
};

template <typename T>
class __CGmatrix_storage<T,1,3> {
    static const int rows = 1;
    static const int cols = 3;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,1,3> : public __CGmatrix_usage<T,1,3> {
    static const int rows = 1;
    static const int cols = 3;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02;
    }
    template <typename TA,
              typename TAstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0) {
        (*this)[0] = __CGvector<T,cols>(v0);
    }
};

template <typename T>
class __CGmatrix_storage<T,1,4> {
    static const int rows = 1;
    static const int cols = 4;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(row)
        __CG_MATRIX_SWIZZLE_1x4(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,1,4> : public __CGmatrix_usage<T,1,4> {
    static const int rows = 1;
    static const int cols = 4;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02, const T & m03) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02; (*this)[0][3] = m03;
    }
    template <typename TA,
              typename TAstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0) {
        (*this)[0] = __CGvector<T,cols>(v0);
    }
};

template <typename T>
class __CGmatrix_storage<T,2,1> {
    static const int rows = 2;
    static const int cols = 1;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_2x1(ELEMENT)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,2,1> : public __CGmatrix_usage<T,2,1> {
    static const int rows = 2;
    static const int cols = 1;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00,
                      const T & m10) {
        (*this)[0][0] = m00;
        (*this)[1][0] = m10;
    }
    template <typename TA, typename TB,
              typename TAstore, typename TBstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
    }
};

template <typename T>
class __CGmatrix_storage<T,2,2> {
    static const int rows = 2;
    static const int cols = 2;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,2,2> : public __CGmatrix_usage<T,2,2> {
    static const int rows = 2;
    static const int cols = 2;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01,
                      const T & m10, const T & m11) {
        (*this)[0][0] = m00; (*this)[0][1] = m01;
        (*this)[1][0] = m10; (*this)[1][1] = m11;
    }
    template <typename TA, typename TB,
              typename TAstore, typename TBstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
    }
};

template <typename T>
class __CGmatrix_storage<T,2,3> {
    static const int rows = 2;
    static const int cols = 3;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,2,3> : public __CGmatrix_usage<T,2,3> {
    static const int rows = 2;
    static const int cols = 3;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02,
                    const T & m10, const T & m11, const T & m12) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12;
    }
    template <typename TA, typename TB,
              typename TAstore, typename TBstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
    }
};

template <typename T>
class __CGmatrix_storage<T,2,4> {
    static const int rows = 2;
    static const int cols = 4;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(row)
        __CG_MATRIX_SWIZZLE_1x4(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(row)
        __CG_MATRIX_SWIZZLE_2x4(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,2,4> : public __CGmatrix_usage<T,2,4> {
    static const int rows = 2;
    static const int cols = 4;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02, const T & m03,
                      const T & m10, const T & m11, const T & m12, const T & m13) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02; (*this)[0][3] = m03;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12; (*this)[1][3] = m13;
    }
    template <typename TAstore, typename TBstore>
    inline __CGmatrix(const __CGvector_usage<T,cols,TAstore> & v0,
                      const __CGvector_usage<T,cols,TBstore> & v1) {
        (*this)[0] = v0;
        (*this)[1] = v1;
    }
    template <typename TA, typename TB,
              typename TAstore, typename TBstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
    }
};

template <typename T>
class __CGmatrix_storage<T,3,1> {
    static const int rows = 3;
    static const int cols = 1;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_2x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_3x1(ELEMENT)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,3,1> : public __CGmatrix_usage<T,3,1> {
    static const int rows = 3;
    static const int cols = 1;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00,
                      const T & m10,
                      const T & m20) {
        (*this)[0][0] = m00;
        (*this)[1][0] = m10;
        (*this)[2][0] = m20;
    }
    template <typename TA, typename TB, typename TC,
              typename TAstore, typename TBstore, typename TCstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
    }
};

template <typename T>
class __CGmatrix_storage<T,3,2> {
    static const int rows = 3;
    static const int cols = 2;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,3,2> : public __CGmatrix_usage<T,3,2> {
    static const int rows = 3;
    static const int cols = 2;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01,
                      const T & m10, const T & m11,
                      const T & m20, const T & m21) {
        (*this)[0][0] = m00; (*this)[0][1] = m01;
        (*this)[1][0] = m10; (*this)[1][1] = m11;
        (*this)[2][0] = m20; (*this)[2][1] = m21;
    }
    template <typename TA, typename TB, typename TC,
              typename TAstore, typename TBstore, typename TCstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
    }
};

template <typename T>
class __CGmatrix_storage<T,3,3> {
    static const int rows = 3;
    static const int cols = 3;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(row)
        __CG_MATRIX_SWIZZLE_3x3(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,3,3> : public __CGmatrix_usage<T,3,3> {
    static const int rows = 3;
    static const int cols = 3;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02,
                      const T & m10, const T & m11, const T & m12,
                      const T & m20, const T & m21, const T & m22) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12;
        (*this)[2][0] = m20; (*this)[2][1] = m21; (*this)[2][2] = m22;
    }
    template <typename TA, typename TB, typename TC,
              typename TAstore, typename TBstore, typename TCstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
    }
};

template <typename T>
class __CGmatrix_storage<T,3,4> {
    static const int rows = 3;
    static const int cols = 4;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(row)
        __CG_MATRIX_SWIZZLE_1x4(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(row)
        __CG_MATRIX_SWIZZLE_2x4(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(row)
        __CG_MATRIX_SWIZZLE_3x3(row)
        __CG_MATRIX_SWIZZLE_3x4(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,3,4> : public __CGmatrix_usage<T,3,4> {
    static const int rows = 3;
    static const int cols = 4;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02, const T & m03,
                      const T & m10, const T & m11, const T & m12, const T & m13,
                      const T & m20, const T & m21, const T & m22, const T & m23) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02; (*this)[0][3] = m03;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12; (*this)[1][3] = m13;
        (*this)[2][0] = m20; (*this)[2][1] = m21; (*this)[2][2] = m22; (*this)[2][3] = m23;
    }
    template <typename TA, typename TB, typename TC,
              typename TAstore, typename TBstore, typename TCstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
    }
};

template <typename T>
class __CGmatrix_storage<T,4,1> {
    static const int rows = 4;
    static const int cols = 1;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_2x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_3x1(ELEMENT)

        __CG_MATRIX_SWIZZLE_4x1(ELEMENT)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,4,1> : public __CGmatrix_usage<T,4,1> {
    static const int rows = 4;
    static const int cols = 1;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00,
                      const T & m10,
                      const T & m20,
                      const T & m30) {
        (*this)[0][0] = m00;
        (*this)[1][0] = m10;
        (*this)[2][0] = m20;
        (*this)[3][0] = m30;
    }
    template <typename TA, typename TB, typename TC, typename TD,
              typename TAstore, typename TBstore, typename TCstore, typename TDstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2,
                      const __CGvector_usage<TD,cols,TDstore> & v3) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
        (*this)[3] = __CGvector<T,cols>(v3);
    }
};

template <typename T>
class __CGmatrix_storage<T,4,2> {
    static const int rows = 4;
    static const int cols = 2;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(ROW)

        __CG_MATRIX_SWIZZLE_4x1(element)
        __CG_MATRIX_SWIZZLE_4x2(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,4,2> : public __CGmatrix_usage<T,4,2> {
    static const int rows = 4;
    static const int cols = 2;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01,
                      const T & m10, const T & m11,
                      const T & m20, const T & m21,
                      const T & m30, const T & m31) {
        (*this)[0][0] = m00; (*this)[0][1] = m01;
        (*this)[1][0] = m10; (*this)[1][1] = m11;
        (*this)[2][0] = m20; (*this)[2][1] = m21;
        (*this)[3][0] = m30; (*this)[3][1] = m31;
    }
    template <typename TA, typename TB, typename TC, typename TD,
              typename TAstore, typename TBstore, typename TCstore, typename TDstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2,
                      const __CGvector_usage<TD,cols,TDstore> & v3) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
        (*this)[3] = __CGvector<T,cols>(v3);
    }
};

template <typename T>
class __CGmatrix_storage<T,4,3> {
    static const int rows = 4;
    static const int cols = 3;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(row)
        __CG_MATRIX_SWIZZLE_3x3(ROW)

        __CG_MATRIX_SWIZZLE_4x1(element)
        __CG_MATRIX_SWIZZLE_4x2(row)
        __CG_MATRIX_SWIZZLE_4x3(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,4,3> : public __CGmatrix_usage<T,4,3> {
    static const int rows = 4;
    static const int cols = 3;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02,
                      const T & m10, const T & m11, const T & m12,
                      const T & m20, const T & m21, const T & m22,
                      const T & m30, const T & m31, const T & m32) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12;
        (*this)[2][0] = m20; (*this)[2][1] = m21; (*this)[2][2] = m22;
        (*this)[3][0] = m30; (*this)[3][1] = m31; (*this)[3][2] = m32;
    }
    template <typename TA, typename TB, typename TC, typename TD,
              typename TAstore, typename TBstore, typename TCstore, typename TDstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2,
                      const __CGvector_usage<TD,cols,TDstore> & v3) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
        (*this)[3] = __CGvector<T,cols>(v3);
    }
};

template <typename T>
class __CGmatrix_storage<T,4,4> {
    static const int rows = 4;
    static const int cols = 4;
    static const int ELEMS = rows * cols;
    typedef typename __CGtype_trait<T>::storageType storageType;
public:
    union {
        __CG_MATRIX_SWIZZLE_1x1(element)
        __CG_MATRIX_SWIZZLE_1x2(row)
        __CG_MATRIX_SWIZZLE_1x3(row)
        __CG_MATRIX_SWIZZLE_1x4(ROW)

        __CG_MATRIX_SWIZZLE_2x1(element)
        __CG_MATRIX_SWIZZLE_2x2(row)
        __CG_MATRIX_SWIZZLE_2x3(row)
        __CG_MATRIX_SWIZZLE_2x4(ROW)

        __CG_MATRIX_SWIZZLE_3x1(element)
        __CG_MATRIX_SWIZZLE_3x2(row)
        __CG_MATRIX_SWIZZLE_3x3(row)
        __CG_MATRIX_SWIZZLE_3x4(ROW)

        __CG_MATRIX_SWIZZLE_4x1(element)
        __CG_MATRIX_SWIZZLE_4x2(row)
        __CG_MATRIX_SWIZZLE_4x3(row)
        __CG_MATRIX_SWIZZLE_4x4(ROW)
    };
#ifndef __GNUC__
    // Visual C++ needs defined default constructor, but g++ balks about
    // failure to initialize const members in union if a default
    // constructor is provided.
    __CGmatrix_storage() { }
#endif
};

template <typename T>
class __CGmatrix<T,4,4> : public __CGmatrix_usage<T,4,4> {
    static const int rows = 4;
    static const int cols = 4;
public:
    // CONSTRUCTORS
    inline __CGmatrix() {}
    inline __CGmatrix(const T & s) : __CGmatrix_usage<T,rows,cols>(s) { }
    template <typename T2>
    inline __CGmatrix(const __CGmatrix<T2,rows,cols> & v) : __CGmatrix_usage<T,rows,cols>(v) { }
    inline __CGmatrix(const T & m00, const T & m01, const T & m02, const T & m03,
                      const T & m10, const T & m11, const T & m12, const T & m13,
                      const T & m20, const T & m21, const T & m22, const T & m23,
                      const T & m30, const T & m31, const T & m32, const T & m33) {
        (*this)[0][0] = m00; (*this)[0][1] = m01; (*this)[0][2] = m02; (*this)[0][3] = m03;
        (*this)[1][0] = m10; (*this)[1][1] = m11; (*this)[1][2] = m12; (*this)[1][3] = m13;
        (*this)[2][0] = m20; (*this)[2][1] = m21; (*this)[2][2] = m22; (*this)[2][3] = m23;
        (*this)[3][0] = m30; (*this)[3][1] = m31; (*this)[3][2] = m32; (*this)[3][3] = m33;
    }
    template <typename TA, typename TB, typename TC, typename TD,
              typename TAstore, typename TBstore, typename TCstore, typename TDstore>
    inline __CGmatrix(const __CGvector_usage<TA,cols,TAstore> & v0,
                      const __CGvector_usage<TB,cols,TBstore> & v1,
                      const __CGvector_usage<TC,cols,TCstore> & v2,
                      const __CGvector_usage<TD,cols,TDstore> & v3) {
        (*this)[0] = __CGvector<T,cols>(v0);
        (*this)[1] = __CGvector<T,cols>(v1);
        (*this)[2] = __CGvector<T,cols>(v2);
        (*this)[3] = __CGvector<T,cols>(v3);
    }
} __CGmay_alias;

//// UNARY VECTOR OPERATORS

// UNARY LOGICAL NEGATE
template <typename T, int N, int M>
inline __CGmatrix<T,N,M> operator!(const __CGmatrix<T,N,M> & a)
{
    __CGmatrix<T,N,M> rv;
    for (int i=0; i<N; i++)
        rv[i] = !a[i];
    return rv;
}
// UNARY BITWISE NEGATE
template <typename T, int N, int M>
inline __CGmatrix<T,N,M> operator~(const __CGmatrix<T,N,M> & a)
{
    __CGmatrix<T,N,M> rv;
    for (int i=0; i<N; i++)
        rv[i] = ~a[i];
    return rv;
}
// UNARY SIGN NEGATE
template <typename T, int N, int M>
inline __CGmatrix<T,N,M> operator-(const __CGmatrix<T,N,M> & a)
{
    __CGmatrix<T,N,M> rv;
    for (int i=0; i<N; i++)
        rv[i] = -a[i];
    return rv;
}

//// BINARY VECTOR OPERATORS
// Three possible combinations:
// 1) matrix OP matrix
// 2) matrix OP scalar type
// 3) scalar type OP matrix

// BINARY ADDITION (+)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator + (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) + VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator + (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) + VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator + (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) + VectorType(b[i]);
    }
    return rv;
}

// BINARY SUBTRACTION (-)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator - (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) - VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator - (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) - VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator - (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) - VectorType(b[i]);
    }
    return rv;
}

// BINARY MULTIPLICATION (*)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator * (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) * VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator * (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) * VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator * (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) * VectorType(b[i]);
    }
    return rv;
}

// BINARY DIVISION (/)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator / (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) / VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator / (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) / VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator / (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) / VectorType(b[i]);
    }
    return rv;
}

// BINARY MODULO (%)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator % (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) % VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator % (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) % VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator % (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) % VectorType(b[i]);
    }
    return rv;
}

// BINARY XOR (^)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator ^ (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) ^ VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator ^ (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) ^ VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator ^ (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) ^ VectorType(b[i]);
    }
    return rv;
}

// BINARY BITWISE AND (&)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator & (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) & VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator & (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) & VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator & (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) & VectorType(b[i]);
    }
    return rv;
}

// BINARY BITWISE OR (|)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator | (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) | VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator | (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) | VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator | (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) | VectorType(b[i]);
    }
    return rv;
}

// BINARY LESS THAN (<)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator < (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) < VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator < (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) < VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator < (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) < VectorType(b[i]);
    }
    return rv;
}

// BINARY GREATER THAN (>)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator > (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) > VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator > (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) > VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator > (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) > VectorType(b[i]);
    }
    return rv;
}

// BINARY LESS THAN OR EQUAL (<=)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator <= (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) <= VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator <= (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) <= VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator <= (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) <= VectorType(b[i]);
    }
    return rv;
}

// BINARY GREATER THAN OR EQUAL (>=)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator >= (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) >= VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator >= (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) >= VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator >= (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) >= VectorType(b[i]);
    }
    return rv;
}

// BINARY EQUALITY (==)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator == (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) == VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator == (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) == VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator == (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) == VectorType(b[i]);
    }
    return rv;
}

// BINARY INEQUALITY (!=)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator != (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) != VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator != (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) != VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator != (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) != VectorType(b[i]);
    }
    return rv;
}

// BINARY LEFT SHIFT (<<)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator << (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) << VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator << (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) << VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator << (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) << VectorType(b[i]);
    }
    return rv;
}

// BINARY SHIFT RIGHT (>>)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator >> (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) >> VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator >> (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) >> VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::resultType,N,M> operator >> (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::resultType ResultType;
    typedef const __CGvector<ResultType,M> VectorType;
    __CGmatrix<ResultType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) >> VectorType(b[i]);
    }
    return rv;
}

// BINARY LOGICAL AND (&&)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator && (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) && VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator && (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) && VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator && (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) && VectorType(b[i]);
    }
    return rv;
}

// BINARY LOGICAL OR (||)
template <typename T, typename T2, int N, int M>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator || (const __CGmatrix<T,N,M> & a, const __CGmatrix<T2,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) || VectorType(b[i]);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator || (const __CGmatrix<T,N,M> & a, const T2 & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a[i]) || VectorType(b);
    }
    return rv;
}
template <typename T, int N, int M, typename T2>
inline __CGmatrix<typename __CGtype_trait<T,T2>::boolType,N,M> operator || (const T2 & a, const __CGmatrix<T,N,M> & b)
{
    typedef typename __CGtype_trait<T,T2>::boolType boolType;
    typedef const __CGvector<boolType,M> VectorType;
    __CGmatrix<boolType,N,M> rv;
    for (int i=0; i<N; i++) {
        rv[i] = VectorType(a) || VectorType(b[i]);
    }
    return rv;
}

// typedefs for Cg-named matrix types

typedef __CGmatrix<float,1,1> float1x1;
typedef __CGmatrix<float,1,2> float1x2;
typedef __CGmatrix<float,1,3> float1x3;
typedef __CGmatrix<float,1,4> float1x4;
typedef __CGmatrix<float,2,1> float2x1;
typedef __CGmatrix<float,2,2> float2x2;
typedef __CGmatrix<float,2,3> float2x3;
typedef __CGmatrix<float,2,4> float2x4;
typedef __CGmatrix<float,3,1> float3x1;
typedef __CGmatrix<float,3,2> float3x2;
typedef __CGmatrix<float,3,3> float3x3;
typedef __CGmatrix<float,3,4> float3x4;
typedef __CGmatrix<float,4,1> float4x1;
typedef __CGmatrix<float,4,2> float4x2;
typedef __CGmatrix<float,4,3> float4x3;
typedef __CGmatrix<float,4,4> float4x4;

typedef __CGmatrix<int,1,1> int1x1;
typedef __CGmatrix<int,1,2> int1x2;
typedef __CGmatrix<int,1,3> int1x3;
typedef __CGmatrix<int,1,4> int1x4;
typedef __CGmatrix<int,2,1> int2x1;
typedef __CGmatrix<int,2,2> int2x2;
typedef __CGmatrix<int,2,3> int2x3;
typedef __CGmatrix<int,2,4> int2x4;
typedef __CGmatrix<int,3,1> int3x1;
typedef __CGmatrix<int,3,2> int3x2;
typedef __CGmatrix<int,3,3> int3x3;
typedef __CGmatrix<int,3,4> int3x4;
typedef __CGmatrix<int,4,1> int4x1;
typedef __CGmatrix<int,4,2> int4x2;
typedef __CGmatrix<int,4,3> int4x3;
typedef __CGmatrix<int,4,4> int4x4;

typedef __CGmatrix<bool,1,1> bool1x1;
typedef __CGmatrix<bool,1,2> bool1x2;
typedef __CGmatrix<bool,1,3> bool1x3;
typedef __CGmatrix<bool,1,4> bool1x4;
typedef __CGmatrix<bool,2,1> bool2x1;
typedef __CGmatrix<bool,2,2> bool2x2;
typedef __CGmatrix<bool,2,3> bool2x3;
typedef __CGmatrix<bool,2,4> bool2x4;
typedef __CGmatrix<bool,3,1> bool3x1;
typedef __CGmatrix<bool,3,2> bool3x2;
typedef __CGmatrix<bool,3,3> bool3x3;
typedef __CGmatrix<bool,3,4> bool3x4;
typedef __CGmatrix<bool,4,1> bool4x1;
typedef __CGmatrix<bool,4,2> bool4x2;
typedef __CGmatrix<bool,4,3> bool4x3;
typedef __CGmatrix<bool,4,4> bool4x4;

#if defined(__Cg_double_hpp__)
typedef __CGmatrix<double,1,1> double1x1;
typedef __CGmatrix<double,1,2> double1x2;
typedef __CGmatrix<double,1,3> double1x3;
typedef __CGmatrix<double,1,4> double1x4;
typedef __CGmatrix<double,2,1> double2x1;
typedef __CGmatrix<double,2,2> double2x2;
typedef __CGmatrix<double,2,3> double2x3;
typedef __CGmatrix<double,2,4> double2x4;
typedef __CGmatrix<double,3,1> double3x1;
typedef __CGmatrix<double,3,2> double3x2;
typedef __CGmatrix<double,3,3> double3x3;
typedef __CGmatrix<double,3,4> double3x4;
typedef __CGmatrix<double,4,1> double4x1;
typedef __CGmatrix<double,4,2> double4x2;
typedef __CGmatrix<double,4,3> double4x3;
typedef __CGmatrix<double,4,4> double4x4;
#endif
#if defined(__Cg_half_hpp__)
typedef __CGmatrix<half,1,1> half1x1;
typedef __CGmatrix<half,1,2> half1x2;
typedef __CGmatrix<half,1,3> half1x3;
typedef __CGmatrix<half,1,4> half1x4;
typedef __CGmatrix<half,2,1> half2x1;
typedef __CGmatrix<half,2,2> half2x2;
typedef __CGmatrix<half,2,3> half2x3;
typedef __CGmatrix<half,2,4> half2x4;
typedef __CGmatrix<half,3,1> half3x1;
typedef __CGmatrix<half,3,2> half3x2;
typedef __CGmatrix<half,3,3> half3x3;
typedef __CGmatrix<half,3,4> half3x4;
typedef __CGmatrix<half,4,1> half4x1;
typedef __CGmatrix<half,4,2> half4x2;
typedef __CGmatrix<half,4,3> half4x3;
typedef __CGmatrix<half,4,4> half4x4;
#endif
#if defined(__Cg_fixed_hpp__)
typedef __CGmatrix<fixed,1,1> fixed1x1;
typedef __CGmatrix<fixed,1,2> fixed1x2;
typedef __CGmatrix<fixed,1,3> fixed1x3;
typedef __CGmatrix<fixed,1,4> fixed1x4;
typedef __CGmatrix<fixed,2,1> fixed2x1;
typedef __CGmatrix<fixed,2,2> fixed2x2;
typedef __CGmatrix<fixed,2,3> fixed2x3;
typedef __CGmatrix<fixed,2,4> fixed2x4;
typedef __CGmatrix<fixed,3,1> fixed3x1;
typedef __CGmatrix<fixed,3,2> fixed3x2;
typedef __CGmatrix<fixed,3,3> fixed3x3;
typedef __CGmatrix<fixed,3,4> fixed3x4;
typedef __CGmatrix<fixed,4,1> fixed4x1;
typedef __CGmatrix<fixed,4,2> fixed4x2;
typedef __CGmatrix<fixed,4,3> fixed4x3;
typedef __CGmatrix<fixed,4,4> fixed4x4;
#endif

#undef __CG_MATRIX_X
#undef __CG_MATRIX_Y
#undef __CG_MATRIX_Z
#undef __CG_MATRIX_W
#undef __CG_MATRIX_SMASK1
#undef __CG_MATRIX_SWIZZLE1
#undef __CG_MATRIX_SMASK2
#undef __CG_MATRIX_SWIZZLE2
#undef __CG_MATRIX_SMASK3
#undef __CG_MATRIX_SWIZZLE3
#undef __CG_MATRIX_SMASK4
#undef __CG_MATRIX_SWIZZLE4
#undef __CG_MATRIX_ELEMENT
#undef __CG_MATRIX_ROW_swizzle2
#undef __CG_MATRIX_ROW_swizzle3
#undef __CG_MATRIX_ROW_swizzle4
#undef __CG_MATRIX_element
#undef __CG_MATRIX_col_swizzle2
#undef __CG_MATRIX_row_swizzle2
#undef __CG_MATRIX_col_swizzle3
#undef __CG_MATRIX_row_swizzle3
#undef __CG_MATRIX_col_swizzle4
#undef __CG_MATRIX_row_swizzle4
#undef __CG_MATRIX_SWIZZLE_1x1
#undef __CG_MATRIX_SWIZZLE_1x2
#undef __CG_MATRIX_SWIZZLE_1x3
#undef __CG_MATRIX_SWIZZLE_1x4
#undef __CG_MATRIX_SWIZZLE_2x1
#undef __CG_MATRIX_SWIZZLE_2x2
#undef __CG_MATRIX_SWIZZLE_2x3
#undef __CG_MATRIX_SWIZZLE_2x4
#undef __CG_MATRIX_SWIZZLE_3x1
#undef __CG_MATRIX_SWIZZLE_3x2
#undef __CG_MATRIX_SWIZZLE_3x3
#undef __CG_MATRIX_SWIZZLE_3x4
#undef __CG_MATRIX_SWIZZLE_4x1
#undef __CG_MATRIX_SWIZZLE_4x2
#undef __CG_MATRIX_SWIZZLE_4x3
#undef __CG_MATRIX_SWIZZLE_4x4
#undef __CG_ASSIGN_OP
#undef __CG_MODIFY_OP
#undef __CG_MATRIX_MISC
#undef __CGmay_alias

} // namespace Cg

#endif // __Cg_matrix_hpp__
