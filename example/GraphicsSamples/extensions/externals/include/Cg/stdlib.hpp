/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_stdlib_hpp__
#define __Cg_stdlib_hpp__

/*
 * To be implemented:
 * - noise
 *
 * Sampler standard library support:
 * - Use <Cg/sampler.hpp>
 *
 * Limitations:
 * - Calling functions with mixed underlying data types is not instantiated
 *   - Example: fv = float2(1,4); hv = half2(-2,3); float f = dot(fv, hv);
 *   - Workaround by casting to same types: float f = dot(fv, float2(hv));
 * - Ambiguous when multi-parameter function is called with all 1-component swizzles
 *   - Example: float3 foo = float(1,2,3); float m = max(foo.x, foo.z);
 *   - Workound by casting at least one swizzle to float1
 *
 * Not possible to implement:
 * - clip
 * - ddx
 * - ddy
 * - fwidth
 * - debug
 */

#include <Cg/vector.hpp>

namespace Cg {

#ifdef __Cg_double_hpp__
#define CG_DOUBLE_SUPPORT(_x) _x
#else
#define CG_DOUBLE_SUPPORT(_x)
#endif
#ifdef __Cg_half_hpp__
#define CG_HALF_SUPPORT(_x) _x
#else
#define CG_HALF_SUPPORT(_x)
#endif
#ifdef __Cg_fixed_hpp__
#define CG_FIXED_SUPPORT(_x) _x
#else
#define CG_FIXED_SUPPORT(_x)
#endif

/// ABS - absolute value
#ifndef __Cg_abs_hpp__
extern int abs(int v);
extern int1 abs(int1 v);
extern int2 abs(int2 v);
extern int3 abs(int3 v);
extern int4 abs(int4 v);
extern float abs(float v);
extern float1 abs(float1 v);
extern float2 abs(float2 v);
extern float3 abs(float3 v);
extern float4 abs(float4 v);
CG_DOUBLE_SUPPORT(extern double abs(double v);)
CG_DOUBLE_SUPPORT(extern double1 abs(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 abs(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 abs(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 abs(double4 v);)
CG_HALF_SUPPORT(extern half abs(half v);)
CG_HALF_SUPPORT(extern half1 abs(half1 v);)
CG_HALF_SUPPORT(extern half2 abs(half2 v);)
CG_HALF_SUPPORT(extern half3 abs(half3 v);)
CG_HALF_SUPPORT(extern half4 abs(half4 v);)
CG_FIXED_SUPPORT(extern fixed abs(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 abs(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 abs(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 abs(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 abs(fixed4 v);)
#endif

/// ACOS - arc cosine
#ifndef __Cg_acos_hpp__
extern float acos(float v);
extern float1 acos(float1 v);
extern float2 acos(float2 v);
extern float3 acos(float3 v);
extern float4 acos(float4 v);
CG_DOUBLE_SUPPORT(extern double acos(double v);)
CG_DOUBLE_SUPPORT(extern double1 acos(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 acos(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 acos(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 acos(double4 v);)
CG_HALF_SUPPORT(extern half acos(half v);)
CG_HALF_SUPPORT(extern half1 acos(half1 v);)
CG_HALF_SUPPORT(extern half2 acos(half2 v);)
CG_HALF_SUPPORT(extern half3 acos(half3 v);)
CG_HALF_SUPPORT(extern half4 acos(half4 v);)
CG_FIXED_SUPPORT(extern fixed acos(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 acos(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 acos(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 acos(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 acos(fixed4 v);)
#endif

/// ALL - all true booleans
#ifndef __Cg_all_hpp__
bool1 all(bool1 v);
bool1 all(bool2 v);
bool1 all(bool3 v);
bool1 all(bool4 v);
#endif

/// ANY - any true booleans
#ifndef __Cg_any_hpp__
bool1 any(bool1 v);
bool1 any(bool2 v);
bool1 any(bool3 v);
bool1 any(bool4 v);
#endif

/// ASIN - arc sine
#ifndef __Cg_asin_hpp__
extern float asin(float v);
extern float1 asin(float1 v);
extern float2 asin(float2 v);
extern float3 asin(float3 v);
extern float4 asin(float4 v);
CG_DOUBLE_SUPPORT(extern double asin(double v);)
CG_DOUBLE_SUPPORT(extern double1 asin(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 asin(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 asin(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 asin(double4 v);)
CG_HALF_SUPPORT(extern half asin(half v);)
CG_HALF_SUPPORT(extern half1 asin(half1 v);)
CG_HALF_SUPPORT(extern half2 asin(half2 v);)
CG_HALF_SUPPORT(extern half3 asin(half3 v);)
CG_HALF_SUPPORT(extern half4 asin(half4 v);)
CG_FIXED_SUPPORT(extern fixed asin(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 asin(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 asin(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 asin(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 asin(fixed4 v);)
#endif

/// ATAN - arc tangent
#ifndef __Cg_atan_hpp__
extern float atan(float v);
extern float1 atan(float1 v);
extern float2 atan(float2 v);
extern float3 atan(float3 v);
extern float4 atan(float4 v);
CG_DOUBLE_SUPPORT(extern double atan(double v);)
CG_DOUBLE_SUPPORT(extern double1 atan(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 atan(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 atan(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 atan(double4 v);)
CG_HALF_SUPPORT(extern half atan(half v);)
CG_HALF_SUPPORT(extern half1 atan(half1 v);)
CG_HALF_SUPPORT(extern half2 atan(half2 v);)
CG_HALF_SUPPORT(extern half3 atan(half3 v);)
CG_HALF_SUPPORT(extern half4 atan(half4 v);)
CG_FIXED_SUPPORT(extern fixed atan(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 atan(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 atan(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 atan(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 atan(fixed4 v);)
#endif

/// ATAN2 - arc tangent of y divided by x
#ifndef __Cg_atan2_hpp__
extern float1 atan2(float1 y, float1 x);
extern float2 atan2(float2 y, float2 x);
extern float3 atan2(float3 y, float3 x);
extern float4 atan2(float4 y, float4 x);
CG_HALF_SUPPORT(extern half1 atan2(half1 y, half1 x);)
CG_HALF_SUPPORT(extern half2 atan2(half2 y, half2 x);)
CG_HALF_SUPPORT(extern half3 atan2(half3 y, half3 x);)
CG_HALF_SUPPORT(extern half4 atan2(half4 y, half4 x);)
CG_FIXED_SUPPORT(extern fixed1 atan2(fixed1 y, fixed1 x);)
CG_FIXED_SUPPORT(extern fixed2 atan2(fixed2 y, fixed2 x);)
CG_FIXED_SUPPORT(extern fixed3 atan2(fixed3 y, fixed3 x);)
CG_FIXED_SUPPORT(extern fixed4 atan2(fixed4 y, fixed4 x);)
CG_DOUBLE_SUPPORT(extern double1 atan2(double1 y, double1 x);)
CG_DOUBLE_SUPPORT(extern double2 atan2(double2 y, double2 x);)
CG_DOUBLE_SUPPORT(extern double3 atan2(double3 y, double3 x);)
CG_DOUBLE_SUPPORT(extern double4 atan2(double4 y, double4 x);)
#endif

/// CEIL - nearest integer towards infinity
#ifndef __Cg_ceil_hpp__
extern float ceil(float v);
extern float1 ceil(float1 v);
extern float2 ceil(float2 v);
extern float3 ceil(float3 v);
extern float4 ceil(float4 v);
CG_DOUBLE_SUPPORT(extern double ceil(double v);)
CG_DOUBLE_SUPPORT(extern double1 ceil(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 ceil(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 ceil(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 ceil(double4 v);)
CG_HALF_SUPPORT(extern half ceil(half v);)
CG_HALF_SUPPORT(extern half1 ceil(half1 v);)
CG_HALF_SUPPORT(extern half2 ceil(half2 v);)
CG_HALF_SUPPORT(extern half3 ceil(half3 v);)
CG_HALF_SUPPORT(extern half4 ceil(half4 v);)
CG_FIXED_SUPPORT(extern fixed ceil(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 ceil(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 ceil(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 ceil(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 ceil(fixed4 v);)
#endif

/// CLAMP - clamp to range
#ifndef __Cg_clamp_hpp__
extern int1 clamp(int1 x, int1 a, int1 b);
extern int2 clamp(int2 x, int2 a, int2 b);
extern int3 clamp(int3 x, int3 a, int3 b);
extern int4 clamp(int4 x, int4 a, int4 b);
extern float1 clamp(float1 x, float1 a, float1 b);
extern float2 clamp(float2 x, float2 a, float2 b);
extern float3 clamp(float3 x, float3 a, float3 b);
extern float4 clamp(float4 x, float4 a, float4 b);
CG_HALF_SUPPORT(extern half1 clamp(half1 x, half1 a, half1 b);)
CG_HALF_SUPPORT(extern half2 clamp(half2 x, half2 a, half2 b);)
CG_HALF_SUPPORT(extern half3 clamp(half3 x, half3 a, half3 b);)
CG_HALF_SUPPORT(extern half4 clamp(half4 x, half4 a, half4 b);)
CG_FIXED_SUPPORT(extern fixed1 clamp(fixed1 x, fixed1 a, fixed1 b);)
CG_FIXED_SUPPORT(extern fixed2 clamp(fixed2 x, fixed2 a, fixed2 b);)
CG_FIXED_SUPPORT(extern fixed3 clamp(fixed3 x, fixed3 a, fixed3 b);)
CG_FIXED_SUPPORT(extern fixed4 clamp(fixed4 x, fixed4 a, fixed4 b);)
CG_DOUBLE_SUPPORT(extern double1 clamp(double1 x, double1 a, double1 b);)
CG_DOUBLE_SUPPORT(extern double2 clamp(double2 x, double2 a, double2 b);)
CG_DOUBLE_SUPPORT(extern double3 clamp(double3 x, double3 a, double3 b);)
CG_DOUBLE_SUPPORT(extern double4 clamp(double4 x, double4 a, double4 b);)
#endif

/// COS - cosine
#ifndef __Cg_cos_hpp__
extern float cos(float v);
extern float1 cos(float1 v);
extern float2 cos(float2 v);
extern float3 cos(float3 v);
extern float4 cos(float4 v);
CG_DOUBLE_SUPPORT(extern double cos(double v);)
CG_DOUBLE_SUPPORT(extern double1 cos(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 cos(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 cos(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 cos(double4 v);)
CG_HALF_SUPPORT(extern half cos(half v);)
CG_HALF_SUPPORT(extern half1 cos(half1 v);)
CG_HALF_SUPPORT(extern half2 cos(half2 v);)
CG_HALF_SUPPORT(extern half3 cos(half3 v);)
CG_HALF_SUPPORT(extern half4 cos(half4 v);)
CG_FIXED_SUPPORT(extern fixed cos(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 cos(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 cos(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 cos(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 cos(fixed4 v);)
#endif

/// COSH - hyperbolic cosine
#ifndef __Cg_cosh_hpp__
extern float1 cosh(float v);
extern float1 cosh(float1 v);
extern float2 cosh(float2 v);
extern float3 cosh(float3 v);
extern float4 cosh(float4 v);
CG_DOUBLE_SUPPORT(extern double cosh(double v);)
CG_DOUBLE_SUPPORT(extern double1 cosh(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 cosh(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 cosh(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 cosh(double4 v);)
CG_HALF_SUPPORT(extern half cosh(half v);)
CG_HALF_SUPPORT(extern half1 cosh(half1 v);)
CG_HALF_SUPPORT(extern half2 cosh(half2 v);)
CG_HALF_SUPPORT(extern half3 cosh(half3 v);)
CG_HALF_SUPPORT(extern half4 cosh(half4 v);)
CG_FIXED_SUPPORT(extern fixed cosh(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 cosh(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 cosh(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 cosh(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 cosh(fixed4 v);)
#endif

/// CROSS - cross product
#ifndef __Cg_cross_hpp__
extern float3 cross(float3 a, float3 b);
extern int3 cross(int3 a, int3 b);
CG_DOUBLE_SUPPORT(extern double3 cross(double3 a, double3 b);)
CG_HALF_SUPPORT(extern half3 cross(half3 a, half3 b);)
CG_FIXED_SUPPORT(extern fixed3 cross(fixed3 a, fixed3 b);)
#endif

/// DEGREES - convert radians to degrees
#ifndef __Cg_degrees_hpp__
extern float degrees(float v);
extern float1 degrees(float1 v);
extern float2 degrees(float2 v);
extern float3 degrees(float3 v);
extern float4 degrees(float4 v);
CG_DOUBLE_SUPPORT(extern double degrees(double v);)
CG_DOUBLE_SUPPORT(extern double1 degrees(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 degrees(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 degrees(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 degrees(double4 v);)
CG_HALF_SUPPORT(extern half degrees(half v);)
CG_HALF_SUPPORT(extern half1 degrees(half1 v);)
CG_HALF_SUPPORT(extern half2 degrees(half2 v);)
CG_HALF_SUPPORT(extern half3 degrees(half3 v);)
CG_HALF_SUPPORT(extern half4 degrees(half4 v);)
CG_FIXED_SUPPORT(extern fixed degrees(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 degrees(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 degrees(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 degrees(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 degrees(fixed4 v);)
#endif

/// DETERMINANT
#ifdef __Cg_matrix_hpp__
#ifndef __Cg_determinant_hpp__
extern float1 determinant(float1x1 A);
extern float1 determinant(float2x2 A);
extern float1 determinant(float3x3 A);
extern float1 determinant(float4x4 A);
CG_DOUBLE_SUPPORT(extern double1 determinant(double1x1 A);)
CG_DOUBLE_SUPPORT(extern double1 determinant(double2x2 A);)
CG_DOUBLE_SUPPORT(extern double1 determinant(double3x3 A);)
CG_DOUBLE_SUPPORT(extern double1 determinant(double4x4 A);)
#endif
#endif // __Cg_matrix_hpp__

/// DISTANCE - distance from point a to point b
#ifndef __Cg_distance_hpp__
extern float1 distance(float1 a, float1 b);
extern float1 distance(float2 a, float2 b);
extern float1 distance(float3 a, float3 b);
extern float1 distance(float4 a, float4 b);
CG_HALF_SUPPORT(extern half1 distance(half1 a, half1 b);)
CG_HALF_SUPPORT(extern half1 distance(half2 a, half2 b);)
CG_HALF_SUPPORT(extern half1 distance(half3 a, half3 b);)
CG_HALF_SUPPORT(extern half1 distance(half4 a, half4 b);)
CG_FIXED_SUPPORT(extern fixed1 distance(fixed1 a, fixed1 b);)
CG_FIXED_SUPPORT(extern fixed1 distance(fixed2 a, fixed2 b);)
CG_FIXED_SUPPORT(extern fixed1 distance(fixed3 a, fixed3 b);)
CG_FIXED_SUPPORT(extern fixed1 distance(fixed4 a, fixed4 b);)
CG_DOUBLE_SUPPORT(extern double1 distance(double1 a, double1 b);)
CG_DOUBLE_SUPPORT(extern double1 distance(double2 a, double2 b);)
CG_DOUBLE_SUPPORT(extern double1 distance(double3 a, double3 b);)
CG_DOUBLE_SUPPORT(extern double1 distance(double4 a, double4 b);)
#endif

/// DOT - dot product
#ifndef __Cg_dot_hpp__
extern float dot(float a, float b);
extern float1 dot(float1 a, float1 b);
extern float1 dot(float2 a, float2 b);
extern float1 dot(float3 a, float3 b);
extern float1 dot(float4 a, float4 b);
CG_HALF_SUPPORT(extern half dot(half a, half b);)
CG_HALF_SUPPORT(extern half1 dot(half1 a, half1 b);)
CG_HALF_SUPPORT(extern half1 dot(half2 a, half2 b);)
CG_HALF_SUPPORT(extern half1 dot(half3 a, half3 b);)
CG_HALF_SUPPORT(extern half1 dot(half4 a, half4 b);)
CG_FIXED_SUPPORT(extern fixed dot(fixed a, fixed b);)
CG_FIXED_SUPPORT(extern fixed1 dot(fixed1 a, fixed1 b);)
CG_FIXED_SUPPORT(extern fixed1 dot(fixed2 a, fixed2 b);)
CG_FIXED_SUPPORT(extern fixed1 dot(fixed3 a, fixed3 b);)
CG_FIXED_SUPPORT(extern fixed1 dot(fixed4 a, fixed4 b);)
CG_DOUBLE_SUPPORT(extern double dot(double a, double b);)
CG_DOUBLE_SUPPORT(extern double1 dot(double1 a, double1 b);)
CG_DOUBLE_SUPPORT(extern double1 dot(double2 a, double2 b);)
CG_DOUBLE_SUPPORT(extern double1 dot(double3 a, double3 b);)
CG_DOUBLE_SUPPORT(extern double1 dot(double4 a, double4 b);)
#endif

/// EXP - natural exponential (base e)
#ifndef __Cg_exp_hpp__
extern float exp(float v);
extern float1 exp(float1 v);
extern float2 exp(float2 v);
extern float3 exp(float3 v);
extern float4 exp(float4 v);
CG_DOUBLE_SUPPORT(extern double exp(double v);)
CG_DOUBLE_SUPPORT(extern double1 exp(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 exp(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 exp(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 exp(double4 v);)
CG_HALF_SUPPORT(extern half exp(half v);)
CG_HALF_SUPPORT(extern half1 exp(half1 v);)
CG_HALF_SUPPORT(extern half2 exp(half2 v);)
CG_HALF_SUPPORT(extern half3 exp(half3 v);)
CG_HALF_SUPPORT(extern half4 exp(half4 v);)
CG_FIXED_SUPPORT(extern fixed exp(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 exp(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 exp(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 exp(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 exp(fixed4 v);)
#endif

/// EXP2 - base 2 exponential
#ifndef __Cg_exp2_hpp__
extern float exp2(float v);
extern float1 exp2(float1 v);
extern float2 exp2(float2 v);
extern float3 exp2(float3 v);
extern float4 exp2(float4 v);
CG_DOUBLE_SUPPORT(extern double exp2(double v);)
CG_DOUBLE_SUPPORT(extern double1 exp2(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 exp2(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 exp2(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 exp2(double4 v);)
CG_HALF_SUPPORT(extern half exp2(half v);)
CG_HALF_SUPPORT(extern half1 exp2(half1 v);)
CG_HALF_SUPPORT(extern half2 exp2(half2 v);)
CG_HALF_SUPPORT(extern half3 exp2(half3 v);)
CG_HALF_SUPPORT(extern half4 exp2(half4 v);)
CG_FIXED_SUPPORT(extern fixed exp2(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 exp2(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 exp2(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 exp2(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 exp2(fixed4 v);)
#endif

/// FACEFORWARD - faceforward
#ifndef __Cg_faceforward_hpp__
extern float1 faceforward(float n, float i, float ng);
extern float2 faceforward(float2 n, float2 i, float2 ng);
extern float3 faceforward(float3 n, float3 i, float3 ng);
extern float4 faceforward(float4 n, float4 i, float4 ng);
CG_HALF_SUPPORT(extern half1 faceforward(half n, half i, half ng);)
CG_HALF_SUPPORT(extern half2 faceforward(half2 n, half2 i, half2 ng);)
CG_HALF_SUPPORT(extern half3 faceforward(half3 n, half3 i, half3 ng);)
CG_HALF_SUPPORT(extern half4 faceforward(half4 n, half4 i, half4 ng);)
CG_FIXED_SUPPORT(extern fixed1 faceforward(fixed n, fixed i, fixed ng);)
CG_FIXED_SUPPORT(extern fixed2 faceforward(fixed2 n, fixed2 i, fixed2 ng);)
CG_FIXED_SUPPORT(extern fixed3 faceforward(fixed3 n, fixed3 i, fixed3 ng);)
CG_FIXED_SUPPORT(extern fixed4 faceforward(fixed4 n, fixed4 i, fixed4 ng);)
CG_DOUBLE_SUPPORT(extern double1 faceforward(double n, double i, double ng);)
CG_DOUBLE_SUPPORT(extern double2 faceforward(double2 n, double2 i, double2 ng);)
CG_DOUBLE_SUPPORT(extern double3 faceforward(double3 n, double3 i, double3 ng);)
CG_DOUBLE_SUPPORT(extern double4 faceforward(double4 n, double4 i, double4 ng);)
#endif

/// FLOATTOINTBITS - bit cast of IEEE float to 2's complement integer
#ifndef __Cg_floatToIntBits_hpp__
extern int floatToIntBits(float v);
extern int1 floatToIntBits(float1 v);
extern int2 floatToIntBits(float2 v);
extern int3 floatToIntBits(float3 v);
extern int4 floatToIntBits(float4 v);
#endif

/// FLOATTORAWINTBITS - bit cast of IEEE float to 2's complement integer
#ifndef __Cg_floatToRawIntBits_hpp__
extern int floatToRawIntBits(float v);
extern int1 floatToRawIntBits(float1 v);
extern int2 floatToRawIntBits(float2 v);
extern int3 floatToRawIntBits(float3 v);
extern int4 floatToRawIntBits(float4 v);
#endif

/// FLOOR - nearest integer towards negative infinity
#ifndef __Cg_floor_hpp__
extern float floor(float v);
extern float1 floor(float1 v);
extern float2 floor(float2 v);
extern float3 floor(float3 v);
extern float4 floor(float4 v);
CG_DOUBLE_SUPPORT(extern double floor(double v);)
CG_DOUBLE_SUPPORT(extern double1 floor(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 floor(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 floor(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 floor(double4 v);)
CG_HALF_SUPPORT(extern half floor(half v);)
CG_HALF_SUPPORT(extern half1 floor(half1 v);)
CG_HALF_SUPPORT(extern half2 floor(half2 v);)
CG_HALF_SUPPORT(extern half3 floor(half3 v);)
CG_HALF_SUPPORT(extern half4 floor(half4 v);)
CG_FIXED_SUPPORT(extern fixed floor(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 floor(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 floor(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 floor(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 floor(fixed4 v);)
#endif

/// FMOD - remainder of x/y with the same sign as x
#ifndef __Cg_fmod_hpp__
extern float1 fmod(float1 x, float1 y);
extern float2 fmod(float2 x, float2 y);
extern float3 fmod(float3 x, float3 y);
extern float4 fmod(float4 x, float4 y);
CG_HALF_SUPPORT(extern half1 fmod(half1 x, half1 y);)
CG_HALF_SUPPORT(extern half2 fmod(half2 x, half2 y);)
CG_HALF_SUPPORT(extern half3 fmod(half3 x, half3 y);)
CG_HALF_SUPPORT(extern half4 fmod(half4 x, half4 y);)
CG_FIXED_SUPPORT(extern fixed1 fmod(fixed1 x, fixed1 y);)
CG_FIXED_SUPPORT(extern fixed2 fmod(fixed2 x, fixed2 y);)
CG_FIXED_SUPPORT(extern fixed3 fmod(fixed3 x, fixed3 y);)
CG_FIXED_SUPPORT(extern fixed4 fmod(fixed4 x, fixed4 y);)
CG_DOUBLE_SUPPORT(extern double1 fmod(double1 x, double1 y);)
CG_DOUBLE_SUPPORT(extern double2 fmod(double2 x, double2 y);)
CG_DOUBLE_SUPPORT(extern double3 fmod(double3 x, double3 y);)
CG_DOUBLE_SUPPORT(extern double4 fmod(double4 x, double4 y);)
#endif

/// FRAC - fractional portion
#ifndef __Cg_frac_hpp__
extern float frac(float v);
extern float1 frac(float1 v);
extern float2 frac(float2 v);
extern float3 frac(float3 v);
extern float4 frac(float4 v);
CG_DOUBLE_SUPPORT(extern double frac(double v);)
CG_DOUBLE_SUPPORT(extern double1 frac(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 frac(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 frac(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 frac(double4 v);)
CG_HALF_SUPPORT(extern half frac(half v);)
CG_HALF_SUPPORT(extern half1 frac(half1 v);)
CG_HALF_SUPPORT(extern half2 frac(half2 v);)
CG_HALF_SUPPORT(extern half3 frac(half3 v);)
CG_HALF_SUPPORT(extern half4 frac(half4 v);)
CG_FIXED_SUPPORT(extern fixed frac(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 frac(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 frac(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 frac(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 frac(fixed4 v);)
#endif

/// FRESNEL - evaluate Fresnel term
#ifdef __Cg_inout_hpp__
#ifndef __Cg_fresnel_hpp__
extern float1 fresnel(float3 i, float3 n, float eta, Out<float> kt, Out<float3> r, Out<float3> t);
extern float1 fresnel(float3 i, float3 n, float eta, Out<float3> r, Out<float3> t);
extern float1 fresnel(float3 i, float3 n, float eta, Out<float> kt);
extern float1 fresnel(float3 i, float3 n, float eta);
CG_DOUBLE_SUPPORT(extern double1 fresnel(double3 i, double3 n, double eta, Out<double> kt, Out<double3> r, Out<double3> t);)
CG_DOUBLE_SUPPORT(extern double1 fresnel(double3 i, double3 n, double eta, Out<double3> r, Out<double3> t);)
CG_DOUBLE_SUPPORT(extern double1 fresnel(double3 i, double3 n, double eta, Out<double> kt);)
CG_DOUBLE_SUPPORT(extern double1 fresnel(double3 i, double3 n, double eta);)
CG_HALF_SUPPORT(extern half1 fresnel(half3 i, half3 n, half eta, Out<half> kt, Out<half3> r, Out<half3> t);)
CG_HALF_SUPPORT(extern half1 fresnel(half3 i, half3 n, half eta, Out<half3> r, Out<half3> t);)
CG_HALF_SUPPORT(extern half1 fresnel(half3 i, half3 n, half eta, Out<half> kt);)
CG_HALF_SUPPORT(extern half1 fresnel(half3 i, half3 n, half eta);)
CG_FIXED_SUPPORT(extern fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed> kt, Out<fixed3> r, Out<fixed3> t);)
CG_FIXED_SUPPORT(extern fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed3> r, Out<fixed3> t);)
CG_FIXED_SUPPORT(extern fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed> kt);)
CG_FIXED_SUPPORT(extern fixed1 fresnel(fixed3 i, fixed3 n, fixed eta);)
#endif
#endif // __Cg_inout_hpp__

/// FREXP - split x into normalized fraction in [0.5, 1] interval and power-of-2 exponent, return fraction and output exponent
#ifdef __Cg_inout_hpp__
#ifndef __Cg_frexp_hpp__
extern float1 frexp(float1 x, Out<float1> expOut);
extern float2 frexp(float2 x, Out<float2> expOut);
extern float3 frexp(float3 x, Out<float3> expOut);
extern float4 frexp(float4 x, Out<float4> expOut);
CG_DOUBLE_SUPPORT(extern double1 frexp(double1 x, Out<double1> expOut);)
CG_DOUBLE_SUPPORT(extern double2 frexp(double2 x, Out<double2> expOut);)
CG_DOUBLE_SUPPORT(extern double3 frexp(double3 x, Out<double3> expOut);)
CG_DOUBLE_SUPPORT(extern double4 frexp(double4 x, Out<double4> expOut);)
CG_HALF_SUPPORT(extern half1 frexp(half1 x, Out<half1> expOut);)
CG_HALF_SUPPORT(extern half2 frexp(half2 x, Out<half2> expOut);)
CG_HALF_SUPPORT(extern half3 frexp(half3 x, Out<half3> expOut);)
CG_HALF_SUPPORT(extern half4 frexp(half4 x, Out<half4> expOut);)
CG_FIXED_SUPPORT(extern fixed1 frexp(fixed1 x, Out<fixed1> expOut);)
CG_FIXED_SUPPORT(extern fixed2 frexp(fixed2 x, Out<fixed2> expOut);)
CG_FIXED_SUPPORT(extern fixed3 frexp(fixed3 x, Out<fixed3> expOut);)
CG_FIXED_SUPPORT(extern fixed4 frexp(fixed4 x, Out<fixed4> expOut);)
#endif
#endif // __Cg_inout_hpp__

/// INTBITSTOFLOAT - bit cast 2's complement integer to IEEE float
#ifndef __Cg_intBitsToFloat_hpp__
extern float intBitsToFloat(int v);
extern float1 intBitsToFloat(int1 v);
extern float2 intBitsToFloat(int2 v);
extern float3 intBitsToFloat(int3 v);
extern float4 intBitsToFloat(int4 v);
#endif

/// INVERSE - matrix inverse
#ifdef __Cg_matrix_hpp__
# ifndef __Cg_inverse_hpp__
extern float4x4 inverse(float4x4 a);
extern float3x3 inverse(float3x3 a);
extern float2x2 inverse(float2x2 a);
extern float1x1 inverse(float1x1 a);
CG_DOUBLE_SUPPORT(extern double1x1 inverse(double1x1 a);)
CG_DOUBLE_SUPPORT(extern double2x2 inverse(double2x2 a);)
CG_DOUBLE_SUPPORT(extern double3x3 inverse(double3x3 a);)
CG_DOUBLE_SUPPORT(extern double4x4 inverse(double4x4 a);)
# endif
#endif // __Cg_matrix_hpp__

/// ISFINITE - has finite representation
#ifndef __Cg_isinfinite_hpp__
#pragma push_macro("isfinite")
#undef isfinite
extern bool isfinite(float v);
extern bool1 isfinite(float1 v);
extern bool2 isfinite(float2 v);
extern bool3 isfinite(float3 v);
extern bool4 isfinite(float4 v);
CG_DOUBLE_SUPPORT(extern bool isfinite(double v);)
CG_DOUBLE_SUPPORT(extern bool1 isfinite(double1 v);)
CG_DOUBLE_SUPPORT(extern bool2 isfinite(double2 v);)
CG_DOUBLE_SUPPORT(extern bool3 isfinite(double3 v);)
CG_DOUBLE_SUPPORT(extern bool4 isfinite(double4 v);)
CG_HALF_SUPPORT(extern bool isfinite(half v);)
CG_HALF_SUPPORT(extern bool1 isfinite(half1 v);)
CG_HALF_SUPPORT(extern bool2 isfinite(half2 v);)
CG_HALF_SUPPORT(extern bool3 isfinite(half3 v);)
CG_HALF_SUPPORT(extern bool4 isfinite(half4 v);)
CG_FIXED_SUPPORT(extern bool isfinite(fixed v);)
CG_FIXED_SUPPORT(extern bool1 isfinite(fixed1 v);)
CG_FIXED_SUPPORT(extern bool2 isfinite(fixed2 v);)
CG_FIXED_SUPPORT(extern bool3 isfinite(fixed3 v);)
CG_FIXED_SUPPORT(extern bool4 isfinite(fixed4 v);)
#pragma pop_macro("isfinite")
#endif

/// ISINF - has infinite representation
#ifndef __Cg_isinf_hpp__
#pragma push_macro("isinf")
#undef isinf
extern bool isinf(float v);
extern bool1 isinf(float1 v);
extern bool2 isinf(float2 v);
extern bool3 isinf(float3 v);
extern bool4 isinf(float4 v);
CG_DOUBLE_SUPPORT(extern bool isinf(double v);)
CG_DOUBLE_SUPPORT(extern bool1 isinf(double1 v);)
CG_DOUBLE_SUPPORT(extern bool2 isinf(double2 v);)
CG_DOUBLE_SUPPORT(extern bool3 isinf(double3 v);)
CG_DOUBLE_SUPPORT(extern bool4 isinf(double4 v);)
CG_HALF_SUPPORT(extern bool isinf(half v);)
CG_HALF_SUPPORT(extern bool1 isinf(half1 v);)
CG_HALF_SUPPORT(extern bool2 isinf(half2 v);)
CG_HALF_SUPPORT(extern bool3 isinf(half3 v);)
CG_HALF_SUPPORT(extern bool4 isinf(half4 v);)
CG_FIXED_SUPPORT(extern bool isinf(fixed v);)
CG_FIXED_SUPPORT(extern bool1 isinf(fixed1 v);)
CG_FIXED_SUPPORT(extern bool2 isinf(fixed2 v);)
CG_FIXED_SUPPORT(extern bool3 isinf(fixed3 v);)
CG_FIXED_SUPPORT(extern bool4 isinf(fixed4 v);)
#pragma pop_macro("isinf")
#endif

/// ISNAN - has Not-A-Number representation
#ifndef __Cg_isnan_hpp__
#pragma push_macro("isnan")
#undef isnan
extern bool isnan(float v);
extern bool1 isnan(float1 v);
extern bool2 isnan(float2 v);
extern bool3 isnan(float3 v);
extern bool4 isnan(float4 v);
CG_DOUBLE_SUPPORT(extern bool isnan(double v);)
CG_DOUBLE_SUPPORT(extern bool1 isnan(double1 v);)
CG_DOUBLE_SUPPORT(extern bool2 isnan(double2 v);)
CG_DOUBLE_SUPPORT(extern bool3 isnan(double3 v);)
CG_DOUBLE_SUPPORT(extern bool4 isnan(double4 v);)
CG_HALF_SUPPORT(extern bool isnan(half v);)
CG_HALF_SUPPORT(extern bool1 isnan(half1 v);)
CG_HALF_SUPPORT(extern bool2 isnan(half2 v);)
CG_HALF_SUPPORT(extern bool3 isnan(half3 v);)
CG_HALF_SUPPORT(extern bool4 isnan(half4 v);)
CG_FIXED_SUPPORT(extern bool isnan(fixed v);)
CG_FIXED_SUPPORT(extern bool1 isnan(fixed1 v);)
CG_FIXED_SUPPORT(extern bool2 isnan(fixed2 v);)
CG_FIXED_SUPPORT(extern bool3 isnan(fixed3 v);)
CG_FIXED_SUPPORT(extern bool4 isnan(fixed4 v);)
#pragma pop_macro("isnan")
#endif

/// LENGTH - Euclidean norm
#ifndef __Cg_length_hpp__
extern float1 length(float1 v);
extern float1 length(float2 v);
extern float1 length(float3 v);
extern float1 length(float4 v);
CG_HALF_SUPPORT(extern half1 length(half1 v);)
CG_HALF_SUPPORT(extern half1 length(half2 v);)
CG_HALF_SUPPORT(extern half1 length(half3 v);)
CG_HALF_SUPPORT(extern half1 length(half4 v);)
CG_FIXED_SUPPORT(extern fixed1 length(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed1 length(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed1 length(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed1 length(fixed4 v);)
CG_DOUBLE_SUPPORT(extern double1 length(double1 v);)
CG_DOUBLE_SUPPORT(extern double1 length(double2 v);)
CG_DOUBLE_SUPPORT(extern double1 length(double3 v);)
CG_DOUBLE_SUPPORT(extern double1 length(double4 v);)
#endif

/// LDEXP - x times 2 raised to n
#ifndef __Cg_ldexp_hpp__
extern float1 ldexp(float1 x, float1 n);
extern float2 ldexp(float2 x, float2 n);
extern float3 ldexp(float3 x, float3 n);
extern float4 ldexp(float4 x, float4 n);
CG_HALF_SUPPORT(extern half1 ldexp(half1 x, half1 n);)
CG_HALF_SUPPORT(extern half2 ldexp(half2 x, half2 n);)
CG_HALF_SUPPORT(extern half3 ldexp(half3 x, half3 n);)
CG_HALF_SUPPORT(extern half4 ldexp(half4 x, half4 n);)
CG_FIXED_SUPPORT(extern fixed1 ldexp(fixed1 x, fixed1 n);)
CG_FIXED_SUPPORT(extern fixed2 ldexp(fixed2 x, fixed2 n);)
CG_FIXED_SUPPORT(extern fixed3 ldexp(fixed3 x, fixed3 n);)
CG_FIXED_SUPPORT(extern fixed4 ldexp(fixed4 x, fixed4 n);)
CG_DOUBLE_SUPPORT(extern double1 ldexp(double1 x, double1 n);)
CG_DOUBLE_SUPPORT(extern double2 ldexp(double2 x, double2 n);)
CG_DOUBLE_SUPPORT(extern double3 ldexp(double3 x, double3 n);)
CG_DOUBLE_SUPPORT(extern double4 ldexp(double4 x, double4 n);)
#endif

/// LERP - linear interpolation
#ifndef __Cg_lerp_hpp__
extern float1 lerp(float1 a, float1 b, float1 x);
extern float2 lerp(float2 a, float2 b, float2 x);
extern float3 lerp(float3 a, float3 b, float3 x);
extern float4 lerp(float4 a, float4 b, float4 x);
extern int1 lerp(int1 a, int1 b, int1 x);
extern int2 lerp(int2 a, int2 b, int2 x);
extern int3 lerp(int3 a, int3 b, int3 x);
extern int4 lerp(int4 a, int4 b, int4 x);
CG_HALF_SUPPORT(extern half1 lerp(half1 a, half1 b, half1 x);)
CG_HALF_SUPPORT(extern half2 lerp(half2 a, half2 b, half2 x);)
CG_HALF_SUPPORT(extern half3 lerp(half3 a, half3 b, half3 x);)
CG_HALF_SUPPORT(extern half4 lerp(half4 a, half4 b, half4 x);)
CG_FIXED_SUPPORT(extern fixed1 lerp(fixed1 a, fixed1 b, fixed1 x);)
CG_FIXED_SUPPORT(extern fixed2 lerp(fixed2 a, fixed2 b, fixed2 x);)
CG_FIXED_SUPPORT(extern fixed3 lerp(fixed3 a, fixed3 b, fixed3 x);)
CG_FIXED_SUPPORT(extern fixed4 lerp(fixed4 a, fixed4 b, fixed4 x);)
CG_DOUBLE_SUPPORT(extern double1 lerp(double1 a, double1 b, double1 x);)
CG_DOUBLE_SUPPORT(extern double2 lerp(double2 a, double2 b, double2 x);)
CG_DOUBLE_SUPPORT(extern double3 lerp(double3 a, double3 b, double3 x);)
CG_DOUBLE_SUPPORT(extern double4 lerp(double4 a, double4 b, double4 x);)
#endif

/// LIT - specular lighting vector
#ifndef __Cg_lit_hpp__
extern float4 lit(float1 NdotL, float1 NdotH, float1 m);
CG_HALF_SUPPORT(extern half4 lit(half1 NdotL, half1 NdotH, half1 m);)
CG_FIXED_SUPPORT(extern fixed4 lit(fixed1 NdotL, fixed1 NdotH, fixed1 m);)
#endif

/// LOG - natural logarithm
#ifndef __Cg_log_hpp__
extern float log(float v);
extern float1 log(float1 v);
extern float2 log(float2 v);
extern float3 log(float3 v);
extern float4 log(float4 v);
CG_DOUBLE_SUPPORT(extern double log(double v);)
CG_DOUBLE_SUPPORT(extern double1 log(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 log(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 log(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 log(double4 v);)
CG_HALF_SUPPORT(extern half log(half v);)
CG_HALF_SUPPORT(extern half1 log(half1 v);)
CG_HALF_SUPPORT(extern half2 log(half2 v);)
CG_HALF_SUPPORT(extern half3 log(half3 v);)
CG_HALF_SUPPORT(extern half4 log(half4 v);)
CG_FIXED_SUPPORT(extern fixed log(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 log(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 log(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 log(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 log(fixed4 v);)
#endif

/// LOG2 - base 2 logarithm
#ifndef __Cg_log2_hpp__
extern float log2(float v);
extern float1 log2(float1 v);
extern float2 log2(float2 v);
extern float3 log2(float3 v);
extern float4 log2(float4 v);
CG_DOUBLE_SUPPORT(extern double log2(double v);)
CG_DOUBLE_SUPPORT(extern double1 log2(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 log2(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 log2(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 log2(double4 v);)
CG_HALF_SUPPORT(extern half log2(half v);)
CG_HALF_SUPPORT(extern half1 log2(half1 v);)
CG_HALF_SUPPORT(extern half2 log2(half2 v);)
CG_HALF_SUPPORT(extern half3 log2(half3 v);)
CG_HALF_SUPPORT(extern half4 log2(half4 v);)
CG_FIXED_SUPPORT(extern fixed log2(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 log2(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 log2(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 log2(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 log2(fixed4 v);)
#endif

/// LOG10 - base 10 logarithm
#ifndef __Cg_log10_hpp__
extern float log10(float v);
extern float1 log10(float1 v);
extern float2 log10(float2 v);
extern float3 log10(float3 v);
extern float4 log10(float4 v);
CG_DOUBLE_SUPPORT(extern double log10(double v);)
CG_DOUBLE_SUPPORT(extern double1 log10(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 log10(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 log10(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 log10(double4 v);)
CG_HALF_SUPPORT(extern half log10(half v);)
CG_HALF_SUPPORT(extern half1 log10(half1 v);)
CG_HALF_SUPPORT(extern half2 log10(half2 v);)
CG_HALF_SUPPORT(extern half3 log10(half3 v);)
CG_HALF_SUPPORT(extern half4 log10(half4 v);)
CG_FIXED_SUPPORT(extern fixed log10(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 log10(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 log10(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 log10(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 log10(fixed4 v);)
#endif

/// MAX - maximum
#ifndef __Cg_max_hpp__
#pragma push_macro("max")
#undef max
extern float1 max(float1 a, float1 b);
extern float2 max(float2 a, float2 b);
extern float3 max(float3 a, float3 b);
extern float4 max(float4 a, float4 b);
extern int1 max(int1 a, int1 b);
extern int2 max(int2 a, int2 b);
extern int3 max(int3 a, int3 b);
extern int4 max(int4 a, int4 b);
CG_HALF_SUPPORT(extern half1 max(half1 a, half1 b);)
CG_HALF_SUPPORT(extern half2 max(half2 a, half2 b);)
CG_HALF_SUPPORT(extern half3 max(half3 a, half3 b);)
CG_HALF_SUPPORT(extern half4 max(half4 a, half4 b);)
CG_FIXED_SUPPORT(extern fixed1 max(fixed1 a, fixed1 b);)
CG_FIXED_SUPPORT(extern fixed2 max(fixed2 a, fixed2 b);)
CG_FIXED_SUPPORT(extern fixed3 max(fixed3 a, fixed3 b);)
CG_FIXED_SUPPORT(extern fixed4 max(fixed4 a, fixed4 b);)
CG_DOUBLE_SUPPORT(extern double1 max(double1 a, double1 b);)
CG_DOUBLE_SUPPORT(extern double2 max(double2 a, double2 b);)
CG_DOUBLE_SUPPORT(extern double3 max(double3 a, double3 b);)
CG_DOUBLE_SUPPORT(extern double4 max(double4 a, double4 b);)
#pragma pop_macro("max")
#endif

/// MIN - minimum
#ifndef __Cg_min_hpp__
#pragma push_macro("min")
#undef min
extern float1 min(float1 a, float1 b);
extern float2 min(float2 a, float2 b);
extern float3 min(float3 a, float3 b);
extern float4 min(float4 a, float4 b);
extern int1 min(int1 a, int1 b);
extern int2 min(int2 a, int2 b);
extern int3 min(int3 a, int3 b);
extern int4 min(int4 a, int4 b);
CG_HALF_SUPPORT(extern half1 min(half1 a, half1 b);)
CG_HALF_SUPPORT(extern half2 min(half2 a, half2 b);)
CG_HALF_SUPPORT(extern half3 min(half3 a, half3 b);)
CG_HALF_SUPPORT(extern half4 min(half4 a, half4 b);)
CG_FIXED_SUPPORT(extern fixed1 min(fixed1 a, fixed1 b);)
CG_FIXED_SUPPORT(extern fixed2 min(fixed2 a, fixed2 b);)
CG_FIXED_SUPPORT(extern fixed3 min(fixed3 a, fixed3 b);)
CG_FIXED_SUPPORT(extern fixed4 min(fixed4 a, fixed4 b);)
CG_DOUBLE_SUPPORT(extern double1 min(double1 a, double1 b);)
CG_DOUBLE_SUPPORT(extern double2 min(double2 a, double2 b);)
CG_DOUBLE_SUPPORT(extern double3 min(double3 a, double3 b);)
CG_DOUBLE_SUPPORT(extern double4 min(double4 a, double4 b);)
#pragma pop_macro("min")
#endif

/// MODF - return fractional part of x and output integer part
#ifdef __Cg_inout_hpp__
#ifndef __Cg_modf_hpp__
extern float1 modf(float1 x, Out<float1> ipOut);
extern float2 modf(float2 x, Out<float2> ipOut);
extern float3 modf(float3 x, Out<float3> ipOut);
extern float4 modf(float4 x, Out<float4> ipOut);
CG_DOUBLE_SUPPORT(extern double1 modf(double1 x, Out<double1> ipOut);)
CG_DOUBLE_SUPPORT(extern double2 modf(double2 x, Out<double2> ipOut);)
CG_DOUBLE_SUPPORT(extern double3 modf(double3 x, Out<double3> ipOut);)
CG_DOUBLE_SUPPORT(extern double4 modf(double4 x, Out<double4> ipOut);)
CG_HALF_SUPPORT(extern half1 modf(half1 x, Out<half1> ipOut);)
CG_HALF_SUPPORT(extern half2 modf(half2 x, Out<half2> ipOut);)
CG_HALF_SUPPORT(extern half3 modf(half3 x, Out<half3> ipOut);)
CG_HALF_SUPPORT(extern half4 modf(half4 x, Out<half4> ipOut);)
CG_FIXED_SUPPORT(extern fixed1 modf(fixed1 x, Out<fixed1> ipOut);)
CG_FIXED_SUPPORT(extern fixed2 modf(fixed2 x, Out<fixed2> ipOut);)
CG_FIXED_SUPPORT(extern fixed3 modf(fixed3 x, Out<fixed3> ipOut);)
CG_FIXED_SUPPORT(extern fixed4 modf(fixed4 x, Out<fixed4> ipOut);)
#endif
#endif // __Cg_inout_hpp__

/// MUL - multiply matrix by vector
#ifdef __Cg_matrix_hpp__
#ifndef __Cg_mul_hpp__
extern float1 mul(float1x1 m, float1 v);
extern int1 mul(int1x1 m, int1 v);
CG_DOUBLE_SUPPORT(extern double1 mul(double1x1 m, double1 v);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed1x1 m, fixed1 v);)
CG_HALF_SUPPORT(extern half1 mul(half1x1 m, half1 v);)
extern float2 mul(float2x1 m, float1 v);
extern int2 mul(int2x1 m, int1 v);
CG_DOUBLE_SUPPORT(extern double2 mul(double2x1 m, double1 v);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed2x1 m, fixed1 v);)
CG_HALF_SUPPORT(extern half2 mul(half2x1 m, half1 v);)
extern float3 mul(float3x1 m, float1 v);
extern int3 mul(int3x1 m, int1 v);
CG_DOUBLE_SUPPORT(extern double3 mul(double3x1 m, double1 v);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed3x1 m, fixed1 v);)
CG_HALF_SUPPORT(extern half3 mul(half3x1 m, half1 v);)
extern float4 mul(float4x1 m, float1 v);
extern int4 mul(int4x1 m, int1 v);
CG_DOUBLE_SUPPORT(extern double4 mul(double4x1 m, double1 v);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed4x1 m, fixed1 v);)
CG_HALF_SUPPORT(extern half4 mul(half4x1 m, half1 v);)
extern float1 mul(float1x2 m, float2 v);
extern int1 mul(int1x2 m, int2 v);
CG_DOUBLE_SUPPORT(extern double1 mul(double1x2 m, double2 v);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed1x2 m, fixed2 v);)
CG_HALF_SUPPORT(extern half1 mul(half1x2 m, half2 v);)
extern float2 mul(float2x2 m, float2 v);
extern int2 mul(int2x2 m, int2 v);
CG_DOUBLE_SUPPORT(extern double2 mul(double2x2 m, double2 v);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed2x2 m, fixed2 v);)
CG_HALF_SUPPORT(extern half2 mul(half2x2 m, half2 v);)
extern float3 mul(float3x2 m, float2 v);
extern int3 mul(int3x2 m, int2 v);
CG_DOUBLE_SUPPORT(extern double3 mul(double3x2 m, double2 v);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed3x2 m, fixed2 v);)
CG_HALF_SUPPORT(extern half3 mul(half3x2 m, half2 v);)
extern float4 mul(float4x2 m, float2 v);
extern int4 mul(int4x2 m, int2 v);
CG_DOUBLE_SUPPORT(extern double4 mul(double4x2 m, double2 v);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed4x2 m, fixed2 v);)
CG_HALF_SUPPORT(extern half4 mul(half4x2 m, half2 v);)
extern float1 mul(float1x3 m, float3 v);
extern int1 mul(int1x3 m, int3 v);
CG_DOUBLE_SUPPORT(extern double1 mul(double1x3 m, double3 v);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed1x3 m, fixed3 v);)
CG_HALF_SUPPORT(extern half1 mul(half1x3 m, half3 v);)
extern float2 mul(float2x3 m, float3 v);
extern int2 mul(int2x3 m, int3 v);
CG_DOUBLE_SUPPORT(extern double2 mul(double2x3 m, double3 v);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed2x3 m, fixed3 v);)
CG_HALF_SUPPORT(extern half2 mul(half2x3 m, half3 v);)
extern float3 mul(float3x3 m, float3 v);
extern int3 mul(int3x3 m, int3 v);
CG_DOUBLE_SUPPORT(extern double3 mul(double3x3 m, double3 v);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed3x3 m, fixed3 v);)
CG_HALF_SUPPORT(extern half3 mul(half3x3 m, half3 v);)
extern float4 mul(float4x3 m, float3 v);
extern int4 mul(int4x3 m, int3 v);
CG_DOUBLE_SUPPORT(extern double4 mul(double4x3 m, double3 v);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed4x3 m, fixed3 v);)
CG_HALF_SUPPORT(extern half4 mul(half4x3 m, half3 v);)
extern float1 mul(float1x4 m, float4 v);
extern int1 mul(int1x4 m, int4 v);
CG_DOUBLE_SUPPORT(extern double1 mul(double1x4 m, double4 v);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed1x4 m, fixed4 v);)
CG_HALF_SUPPORT(extern half1 mul(half1x4 m, half4 v);)
extern float2 mul(float2x4 m, float4 v);
extern int2 mul(int2x4 m, int4 v);
CG_DOUBLE_SUPPORT(extern double2 mul(double2x4 m, double4 v);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed2x4 m, fixed4 v);)
CG_HALF_SUPPORT(extern half2 mul(half2x4 m, half4 v);)
extern float3 mul(float3x4 m, float4 v);
extern int3 mul(int3x4 m, int4 v);
CG_DOUBLE_SUPPORT(extern double3 mul(double3x4 m, double4 v);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed3x4 m, fixed4 v);)
CG_HALF_SUPPORT(extern half3 mul(half3x4 m, half4 v);)
extern float4 mul(float4x4 m, float4 v);
extern int4 mul(int4x4 m, int4 v);
CG_DOUBLE_SUPPORT(extern double4 mul(double4x4 m, double4 v);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed4x4 m, fixed4 v);)
CG_HALF_SUPPORT(extern half4 mul(half4x4 m, half4 v);)

/// MUL - multiply vector by matrix
extern float1 mul(float1 v, float1x1 m);
extern int1 mul(int1 v, int1x1 m);
CG_DOUBLE_SUPPORT(extern double1 mul(double1 v, double1x1 m);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed1 v, fixed1x1 m);)
CG_HALF_SUPPORT(extern half1 mul(half1 v, half1x1 m);)
extern float2 mul(float1 v, float1x2 m);
extern int2 mul(int1 v, int1x2 m);
CG_DOUBLE_SUPPORT(extern double2 mul(double1 v, double1x2 m);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed1 v, fixed1x2 m);)
CG_HALF_SUPPORT(extern half2 mul(half1 v, half1x2 m);)
extern float3 mul(float1 v, float1x3 m);
extern int3 mul(int1 v, int1x3 m);
CG_DOUBLE_SUPPORT(extern double3 mul(double1 v, double1x3 m);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed1 v, fixed1x3 m);)
CG_HALF_SUPPORT(extern half3 mul(half1 v, half1x3 m);)
extern float4 mul(float1 v, float1x4 m);
extern int4 mul(int1 v, int1x4 m);
CG_DOUBLE_SUPPORT(extern double4 mul(double1 v, double1x4 m);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed1 v, fixed1x4 m);)
CG_HALF_SUPPORT(extern half4 mul(half1 v, half1x4 m);)
extern float1 mul(float2 v, float2x1 m);
extern int1 mul(int2 v, int2x1 m);
CG_DOUBLE_SUPPORT(extern double1 mul(double2 v, double2x1 m);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed2 v, fixed2x1 m);)
CG_HALF_SUPPORT(extern half1 mul(half2 v, half2x1 m);)
extern float2 mul(float2 v, float2x2 m);
extern int2 mul(int2 v, int2x2 m);
CG_DOUBLE_SUPPORT(extern double2 mul(double2 v, double2x2 m);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed2 v, fixed2x2 m);)
CG_HALF_SUPPORT(extern half2 mul(half2 v, half2x2 m);)
extern float3 mul(float2 v, float2x3 m);
extern int3 mul(int2 v, int2x3 m);
CG_DOUBLE_SUPPORT(extern double3 mul(double2 v, double2x3 m);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed2 v, fixed2x3 m);)
CG_HALF_SUPPORT(extern half3 mul(half2 v, half2x3 m);)
extern float4 mul(float2 v, float2x4 m);
extern int4 mul(int2 v, int2x4 m);
CG_DOUBLE_SUPPORT(extern double4 mul(double2 v, double2x4 m);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed2 v, fixed2x4 m);)
CG_HALF_SUPPORT(extern half4 mul(half2 v, half2x4 m);)
extern float1 mul(float3 v, float3x1 m);
extern int1 mul(int3 v, int3x1 m);
CG_DOUBLE_SUPPORT(extern double1 mul(double3 v, double3x1 m);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed3 v, fixed3x1 m);)
CG_HALF_SUPPORT(extern half1 mul(half3 v, half3x1 m);)
extern float2 mul(float3 v, float3x2 m);
extern int2 mul(int3 v, int3x2 m);
CG_DOUBLE_SUPPORT(extern double2 mul(double3 v, double3x2 m);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed3 v, fixed3x2 m);)
CG_HALF_SUPPORT(extern half2 mul(half3 v, half3x2 m);)
extern float3 mul(float3 v, float3x3 m);
extern int3 mul(int3 v, int3x3 m);
CG_DOUBLE_SUPPORT(extern double3 mul(double3 v, double3x3 m);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed3 v, fixed3x3 m);)
CG_HALF_SUPPORT(extern half3 mul(half3 v, half3x3 m);)
extern float4 mul(float3 v, float3x4 m);
extern int4 mul(int3 v, int3x4 m);
CG_DOUBLE_SUPPORT(extern double4 mul(double3 v, double3x4 m);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed3 v, fixed3x4 m);)
CG_HALF_SUPPORT(extern half4 mul(half3 v, half3x4 m);)
extern float1 mul(float4 v, float4x1 m);
extern int1 mul(int4 v, int4x1 m);
CG_DOUBLE_SUPPORT(extern double1 mul(double4 v, double4x1 m);)
CG_FIXED_SUPPORT(extern fixed1 mul(fixed4 v, fixed4x1 m);)
CG_HALF_SUPPORT(extern half1 mul(half4 v, half4x1 m);)
extern float2 mul(float4 v, float4x2 m);
extern int2 mul(int4 v, int4x2 m);
CG_DOUBLE_SUPPORT(extern double2 mul(double4 v, double4x2 m);)
CG_FIXED_SUPPORT(extern fixed2 mul(fixed4 v, fixed4x2 m);)
CG_HALF_SUPPORT(extern half2 mul(half4 v, half4x2 m);)
extern float3 mul(float4 v, float4x3 m);
extern int3 mul(int4 v, int4x3 m);
CG_DOUBLE_SUPPORT(extern double3 mul(double4 v, double4x3 m);)
CG_FIXED_SUPPORT(extern fixed3 mul(fixed4 v, fixed4x3 m);)
CG_HALF_SUPPORT(extern half3 mul(half4 v, half4x3 m);)
extern float4 mul(float4 v, float4x4 m);
extern int4 mul(int4 v, int4x4 m);
CG_DOUBLE_SUPPORT(extern double4 mul(double4 v, double4x4 m);)
CG_FIXED_SUPPORT(extern fixed4 mul(fixed4 v, fixed4x4 m);)
CG_HALF_SUPPORT(extern half4 mul(half4 v, half4x4 m);)

/// MUL - multiply matrix by matrix
extern float1x1 mul(float1x1 m1, float1x1 m2);
extern int1x1 mul(int1x1 m1, int1x1 m2);
CG_DOUBLE_SUPPORT(extern double1x1 mul(double1x1 m1, double1x1 m2);)
CG_FIXED_SUPPORT(extern fixed1x1 mul(fixed1x1 m1, fixed1x1 m2);)
CG_HALF_SUPPORT(extern half1x1 mul(half1x1 m1, half1x1 m2);)
extern float1x2 mul(float1x1 m1, float1x2 m2);
extern int1x2 mul(int1x1 m1, int1x2 m2);
CG_DOUBLE_SUPPORT(extern double1x2 mul(double1x1 m1, double1x2 m2);)
CG_FIXED_SUPPORT(extern fixed1x2 mul(fixed1x1 m1, fixed1x2 m2);)
CG_HALF_SUPPORT(extern half1x2 mul(half1x1 m1, half1x2 m2);)
extern float1x3 mul(float1x1 m1, float1x3 m2);
extern int1x3 mul(int1x1 m1, int1x3 m2);
CG_DOUBLE_SUPPORT(extern double1x3 mul(double1x1 m1, double1x3 m2);)
CG_FIXED_SUPPORT(extern fixed1x3 mul(fixed1x1 m1, fixed1x3 m2);)
CG_HALF_SUPPORT(extern half1x3 mul(half1x1 m1, half1x3 m2);)
extern float1x4 mul(float1x1 m1, float1x4 m2);
extern int1x4 mul(int1x1 m1, int1x4 m2);
CG_DOUBLE_SUPPORT(extern double1x4 mul(double1x1 m1, double1x4 m2);)
CG_FIXED_SUPPORT(extern fixed1x4 mul(fixed1x1 m1, fixed1x4 m2);)
CG_HALF_SUPPORT(extern half1x4 mul(half1x1 m1, half1x4 m2);)
extern float1x1 mul(float1x2 m1, float2x1 m2);
extern int1x1 mul(int1x2 m1, int2x1 m2);
CG_DOUBLE_SUPPORT(extern double1x1 mul(double1x2 m1, double2x1 m2);)
CG_FIXED_SUPPORT(extern fixed1x1 mul(fixed1x2 m1, fixed2x1 m2);)
CG_HALF_SUPPORT(extern half1x1 mul(half1x2 m1, half2x1 m2);)
extern float1x2 mul(float1x2 m1, float2x2 m2);
extern int1x2 mul(int1x2 m1, int2x2 m2);
CG_DOUBLE_SUPPORT(extern double1x2 mul(double1x2 m1, double2x2 m2);)
CG_FIXED_SUPPORT(extern fixed1x2 mul(fixed1x2 m1, fixed2x2 m2);)
CG_HALF_SUPPORT(extern half1x2 mul(half1x2 m1, half2x2 m2);)
extern float1x3 mul(float1x2 m1, float2x3 m2);
extern int1x3 mul(int1x2 m1, int2x3 m2);
CG_DOUBLE_SUPPORT(extern double1x3 mul(double1x2 m1, double2x3 m2);)
CG_FIXED_SUPPORT(extern fixed1x3 mul(fixed1x2 m1, fixed2x3 m2);)
CG_HALF_SUPPORT(extern half1x3 mul(half1x2 m1, half2x3 m2);)
extern float1x4 mul(float1x2 m1, float2x4 m2);
extern int1x4 mul(int1x2 m1, int2x4 m2);
CG_DOUBLE_SUPPORT(extern double1x4 mul(double1x2 m1, double2x4 m2);)
CG_FIXED_SUPPORT(extern fixed1x4 mul(fixed1x2 m1, fixed2x4 m2);)
CG_HALF_SUPPORT(extern half1x4 mul(half1x2 m1, half2x4 m2);)
extern float1x1 mul(float1x3 m1, float3x1 m2);
extern int1x1 mul(int1x3 m1, int3x1 m2);
CG_DOUBLE_SUPPORT(extern double1x1 mul(double1x3 m1, double3x1 m2);)
CG_FIXED_SUPPORT(extern fixed1x1 mul(fixed1x3 m1, fixed3x1 m2);)
CG_HALF_SUPPORT(extern half1x1 mul(half1x3 m1, half3x1 m2);)
extern float1x2 mul(float1x3 m1, float3x2 m2);
extern int1x2 mul(int1x3 m1, int3x2 m2);
CG_DOUBLE_SUPPORT(extern double1x2 mul(double1x3 m1, double3x2 m2);)
CG_FIXED_SUPPORT(extern fixed1x2 mul(fixed1x3 m1, fixed3x2 m2);)
CG_HALF_SUPPORT(extern half1x2 mul(half1x3 m1, half3x2 m2);)
extern float1x3 mul(float1x3 m1, float3x3 m2);
extern int1x3 mul(int1x3 m1, int3x3 m2);
CG_DOUBLE_SUPPORT(extern double1x3 mul(double1x3 m1, double3x3 m2);)
CG_FIXED_SUPPORT(extern fixed1x3 mul(fixed1x3 m1, fixed3x3 m2);)
CG_HALF_SUPPORT(extern half1x3 mul(half1x3 m1, half3x3 m2);)
extern float1x4 mul(float1x3 m1, float3x4 m2);
extern int1x4 mul(int1x3 m1, int3x4 m2);
CG_DOUBLE_SUPPORT(extern double1x4 mul(double1x3 m1, double3x4 m2);)
CG_FIXED_SUPPORT(extern fixed1x4 mul(fixed1x3 m1, fixed3x4 m2);)
CG_HALF_SUPPORT(extern half1x4 mul(half1x3 m1, half3x4 m2);)
extern float1x1 mul(float1x4 m1, float4x1 m2);
extern int1x1 mul(int1x4 m1, int4x1 m2);
CG_DOUBLE_SUPPORT(extern double1x1 mul(double1x4 m1, double4x1 m2);)
CG_FIXED_SUPPORT(extern fixed1x1 mul(fixed1x4 m1, fixed4x1 m2);)
CG_HALF_SUPPORT(extern half1x1 mul(half1x4 m1, half4x1 m2);)
extern float1x2 mul(float1x4 m1, float4x2 m2);
extern int1x2 mul(int1x4 m1, int4x2 m2);
CG_DOUBLE_SUPPORT(extern double1x2 mul(double1x4 m1, double4x2 m2);)
CG_FIXED_SUPPORT(extern fixed1x2 mul(fixed1x4 m1, fixed4x2 m2);)
CG_HALF_SUPPORT(extern half1x2 mul(half1x4 m1, half4x2 m2);)
extern float1x3 mul(float1x4 m1, float4x3 m2);
extern int1x3 mul(int1x4 m1, int4x3 m2);
CG_DOUBLE_SUPPORT(extern double1x3 mul(double1x4 m1, double4x3 m2);)
CG_FIXED_SUPPORT(extern fixed1x3 mul(fixed1x4 m1, fixed4x3 m2);)
CG_HALF_SUPPORT(extern half1x3 mul(half1x4 m1, half4x3 m2);)
extern float1x4 mul(float1x4 m1, float4x4 m2);
extern int1x4 mul(int1x4 m1, int4x4 m2);
CG_DOUBLE_SUPPORT(extern double1x4 mul(double1x4 m1, double4x4 m2);)
CG_FIXED_SUPPORT(extern fixed1x4 mul(fixed1x4 m1, fixed4x4 m2);)
CG_HALF_SUPPORT(extern half1x4 mul(half1x4 m1, half4x4 m2);)
extern float2x1 mul(float2x1 m1, float1x1 m2);
extern int2x1 mul(int2x1 m1, int1x1 m2);
CG_DOUBLE_SUPPORT(extern double2x1 mul(double2x1 m1, double1x1 m2);)
CG_FIXED_SUPPORT(extern fixed2x1 mul(fixed2x1 m1, fixed1x1 m2);)
CG_HALF_SUPPORT(extern half2x1 mul(half2x1 m1, half1x1 m2);)
extern float2x2 mul(float2x1 m1, float1x2 m2);
extern int2x2 mul(int2x1 m1, int1x2 m2);
CG_DOUBLE_SUPPORT(extern double2x2 mul(double2x1 m1, double1x2 m2);)
CG_FIXED_SUPPORT(extern fixed2x2 mul(fixed2x1 m1, fixed1x2 m2);)
CG_HALF_SUPPORT(extern half2x2 mul(half2x1 m1, half1x2 m2);)
extern float2x3 mul(float2x1 m1, float1x3 m2);
extern int2x3 mul(int2x1 m1, int1x3 m2);
CG_DOUBLE_SUPPORT(extern double2x3 mul(double2x1 m1, double1x3 m2);)
CG_FIXED_SUPPORT(extern fixed2x3 mul(fixed2x1 m1, fixed1x3 m2);)
CG_HALF_SUPPORT(extern half2x3 mul(half2x1 m1, half1x3 m2);)
extern float2x4 mul(float2x1 m1, float1x4 m2);
extern int2x4 mul(int2x1 m1, int1x4 m2);
CG_DOUBLE_SUPPORT(extern double2x4 mul(double2x1 m1, double1x4 m2);)
CG_FIXED_SUPPORT(extern fixed2x4 mul(fixed2x1 m1, fixed1x4 m2);)
CG_HALF_SUPPORT(extern half2x4 mul(half2x1 m1, half1x4 m2);)
extern float2x1 mul(float2x2 m1, float2x1 m2);
extern int2x1 mul(int2x2 m1, int2x1 m2);
CG_DOUBLE_SUPPORT(extern double2x1 mul(double2x2 m1, double2x1 m2);)
CG_FIXED_SUPPORT(extern fixed2x1 mul(fixed2x2 m1, fixed2x1 m2);)
CG_HALF_SUPPORT(extern half2x1 mul(half2x2 m1, half2x1 m2);)
extern float2x2 mul(float2x2 m1, float2x2 m2);
extern int2x2 mul(int2x2 m1, int2x2 m2);
CG_DOUBLE_SUPPORT(extern double2x2 mul(double2x2 m1, double2x2 m2);)
CG_FIXED_SUPPORT(extern fixed2x2 mul(fixed2x2 m1, fixed2x2 m2);)
CG_HALF_SUPPORT(extern half2x2 mul(half2x2 m1, half2x2 m2);)
extern float2x3 mul(float2x2 m1, float2x3 m2);
extern int2x3 mul(int2x2 m1, int2x3 m2);
CG_DOUBLE_SUPPORT(extern double2x3 mul(double2x2 m1, double2x3 m2);)
CG_FIXED_SUPPORT(extern fixed2x3 mul(fixed2x2 m1, fixed2x3 m2);)
CG_HALF_SUPPORT(extern half2x3 mul(half2x2 m1, half2x3 m2);)
extern float2x4 mul(float2x2 m1, float2x4 m2);
extern int2x4 mul(int2x2 m1, int2x4 m2);
CG_DOUBLE_SUPPORT(extern double2x4 mul(double2x2 m1, double2x4 m2);)
CG_FIXED_SUPPORT(extern fixed2x4 mul(fixed2x2 m1, fixed2x4 m2);)
CG_HALF_SUPPORT(extern half2x4 mul(half2x2 m1, half2x4 m2);)
extern float2x1 mul(float2x3 m1, float3x1 m2);
extern int2x1 mul(int2x3 m1, int3x1 m2);
CG_DOUBLE_SUPPORT(extern double2x1 mul(double2x3 m1, double3x1 m2);)
CG_FIXED_SUPPORT(extern fixed2x1 mul(fixed2x3 m1, fixed3x1 m2);)
CG_HALF_SUPPORT(extern half2x1 mul(half2x3 m1, half3x1 m2);)
extern float2x2 mul(float2x3 m1, float3x2 m2);
extern int2x2 mul(int2x3 m1, int3x2 m2);
CG_DOUBLE_SUPPORT(extern double2x2 mul(double2x3 m1, double3x2 m2);)
CG_FIXED_SUPPORT(extern fixed2x2 mul(fixed2x3 m1, fixed3x2 m2);)
CG_HALF_SUPPORT(extern half2x2 mul(half2x3 m1, half3x2 m2);)
extern float2x3 mul(float2x3 m1, float3x3 m2);
extern int2x3 mul(int2x3 m1, int3x3 m2);
CG_DOUBLE_SUPPORT(extern double2x3 mul(double2x3 m1, double3x3 m2);)
CG_FIXED_SUPPORT(extern fixed2x3 mul(fixed2x3 m1, fixed3x3 m2);)
CG_HALF_SUPPORT(extern half2x3 mul(half2x3 m1, half3x3 m2);)
extern float2x4 mul(float2x3 m1, float3x4 m2);
extern int2x4 mul(int2x3 m1, int3x4 m2);
CG_DOUBLE_SUPPORT(extern double2x4 mul(double2x3 m1, double3x4 m2);)
CG_FIXED_SUPPORT(extern fixed2x4 mul(fixed2x3 m1, fixed3x4 m2);)
CG_HALF_SUPPORT(extern half2x4 mul(half2x3 m1, half3x4 m2);)
extern float2x1 mul(float2x4 m1, float4x1 m2);
extern int2x1 mul(int2x4 m1, int4x1 m2);
CG_DOUBLE_SUPPORT(extern double2x1 mul(double2x4 m1, double4x1 m2);)
CG_FIXED_SUPPORT(extern fixed2x1 mul(fixed2x4 m1, fixed4x1 m2);)
CG_HALF_SUPPORT(extern half2x1 mul(half2x4 m1, half4x1 m2);)
extern float2x2 mul(float2x4 m1, float4x2 m2);
extern int2x2 mul(int2x4 m1, int4x2 m2);
CG_DOUBLE_SUPPORT(extern double2x2 mul(double2x4 m1, double4x2 m2);)
CG_FIXED_SUPPORT(extern fixed2x2 mul(fixed2x4 m1, fixed4x2 m2);)
CG_HALF_SUPPORT(extern half2x2 mul(half2x4 m1, half4x2 m2);)
extern float2x3 mul(float2x4 m1, float4x3 m2);
extern int2x3 mul(int2x4 m1, int4x3 m2);
CG_DOUBLE_SUPPORT(extern double2x3 mul(double2x4 m1, double4x3 m2);)
CG_FIXED_SUPPORT(extern fixed2x3 mul(fixed2x4 m1, fixed4x3 m2);)
CG_HALF_SUPPORT(extern half2x3 mul(half2x4 m1, half4x3 m2);)
extern float2x4 mul(float2x4 m1, float4x4 m2);
extern int2x4 mul(int2x4 m1, int4x4 m2);
CG_DOUBLE_SUPPORT(extern double2x4 mul(double2x4 m1, double4x4 m2);)
CG_FIXED_SUPPORT(extern fixed2x4 mul(fixed2x4 m1, fixed4x4 m2);)
CG_HALF_SUPPORT(extern half2x4 mul(half2x4 m1, half4x4 m2);)
extern float3x1 mul(float3x1 m1, float1x1 m2);
extern int3x1 mul(int3x1 m1, int1x1 m2);
CG_DOUBLE_SUPPORT(extern double3x1 mul(double3x1 m1, double1x1 m2);)
CG_FIXED_SUPPORT(extern fixed3x1 mul(fixed3x1 m1, fixed1x1 m2);)
CG_HALF_SUPPORT(extern half3x1 mul(half3x1 m1, half1x1 m2);)
extern float3x2 mul(float3x1 m1, float1x2 m2);
extern int3x2 mul(int3x1 m1, int1x2 m2);
CG_DOUBLE_SUPPORT(extern double3x2 mul(double3x1 m1, double1x2 m2);)
CG_FIXED_SUPPORT(extern fixed3x2 mul(fixed3x1 m1, fixed1x2 m2);)
CG_HALF_SUPPORT(extern half3x2 mul(half3x1 m1, half1x2 m2);)
extern float3x3 mul(float3x1 m1, float1x3 m2);
extern int3x3 mul(int3x1 m1, int1x3 m2);
CG_DOUBLE_SUPPORT(extern double3x3 mul(double3x1 m1, double1x3 m2);)
CG_FIXED_SUPPORT(extern fixed3x3 mul(fixed3x1 m1, fixed1x3 m2);)
CG_HALF_SUPPORT(extern half3x3 mul(half3x1 m1, half1x3 m2);)
extern float3x4 mul(float3x1 m1, float1x4 m2);
extern int3x4 mul(int3x1 m1, int1x4 m2);
CG_DOUBLE_SUPPORT(extern double3x4 mul(double3x1 m1, double1x4 m2);)
CG_FIXED_SUPPORT(extern fixed3x4 mul(fixed3x1 m1, fixed1x4 m2);)
CG_HALF_SUPPORT(extern half3x4 mul(half3x1 m1, half1x4 m2);)
extern float3x1 mul(float3x2 m1, float2x1 m2);
extern int3x1 mul(int3x2 m1, int2x1 m2);
CG_DOUBLE_SUPPORT(extern double3x1 mul(double3x2 m1, double2x1 m2);)
CG_FIXED_SUPPORT(extern fixed3x1 mul(fixed3x2 m1, fixed2x1 m2);)
CG_HALF_SUPPORT(extern half3x1 mul(half3x2 m1, half2x1 m2);)
extern float3x2 mul(float3x2 m1, float2x2 m2);
extern int3x2 mul(int3x2 m1, int2x2 m2);
CG_DOUBLE_SUPPORT(extern double3x2 mul(double3x2 m1, double2x2 m2);)
CG_FIXED_SUPPORT(extern fixed3x2 mul(fixed3x2 m1, fixed2x2 m2);)
CG_HALF_SUPPORT(extern half3x2 mul(half3x2 m1, half2x2 m2);)
extern float3x3 mul(float3x2 m1, float2x3 m2);
extern int3x3 mul(int3x2 m1, int2x3 m2);
CG_DOUBLE_SUPPORT(extern double3x3 mul(double3x2 m1, double2x3 m2);)
CG_FIXED_SUPPORT(extern fixed3x3 mul(fixed3x2 m1, fixed2x3 m2);)
CG_HALF_SUPPORT(extern half3x3 mul(half3x2 m1, half2x3 m2);)
extern float3x4 mul(float3x2 m1, float2x4 m2);
extern int3x4 mul(int3x2 m1, int2x4 m2);
CG_DOUBLE_SUPPORT(extern double3x4 mul(double3x2 m1, double2x4 m2);)
CG_FIXED_SUPPORT(extern fixed3x4 mul(fixed3x2 m1, fixed2x4 m2);)
CG_HALF_SUPPORT(extern half3x4 mul(half3x2 m1, half2x4 m2);)
extern float3x1 mul(float3x3 m1, float3x1 m2);
extern int3x1 mul(int3x3 m1, int3x1 m2);
CG_DOUBLE_SUPPORT(extern double3x1 mul(double3x3 m1, double3x1 m2);)
CG_FIXED_SUPPORT(extern fixed3x1 mul(fixed3x3 m1, fixed3x1 m2);)
CG_HALF_SUPPORT(extern half3x1 mul(half3x3 m1, half3x1 m2);)
extern float3x2 mul(float3x3 m1, float3x2 m2);
extern int3x2 mul(int3x3 m1, int3x2 m2);
CG_DOUBLE_SUPPORT(extern double3x2 mul(double3x3 m1, double3x2 m2);)
CG_FIXED_SUPPORT(extern fixed3x2 mul(fixed3x3 m1, fixed3x2 m2);)
CG_HALF_SUPPORT(extern half3x2 mul(half3x3 m1, half3x2 m2);)
extern float3x3 mul(float3x3 m1, float3x3 m2);
extern int3x3 mul(int3x3 m1, int3x3 m2);
CG_DOUBLE_SUPPORT(extern double3x3 mul(double3x3 m1, double3x3 m2);)
CG_FIXED_SUPPORT(extern fixed3x3 mul(fixed3x3 m1, fixed3x3 m2);)
CG_HALF_SUPPORT(extern half3x3 mul(half3x3 m1, half3x3 m2);)
extern float3x4 mul(float3x3 m1, float3x4 m2);
extern int3x4 mul(int3x3 m1, int3x4 m2);
CG_DOUBLE_SUPPORT(extern double3x4 mul(double3x3 m1, double3x4 m2);)
CG_FIXED_SUPPORT(extern fixed3x4 mul(fixed3x3 m1, fixed3x4 m2);)
CG_HALF_SUPPORT(extern half3x4 mul(half3x3 m1, half3x4 m2);)
extern float3x1 mul(float3x4 m1, float4x1 m2);
extern int3x1 mul(int3x4 m1, int4x1 m2);
CG_DOUBLE_SUPPORT(extern double3x1 mul(double3x4 m1, double4x1 m2);)
CG_FIXED_SUPPORT(extern fixed3x1 mul(fixed3x4 m1, fixed4x1 m2);)
CG_HALF_SUPPORT(extern half3x1 mul(half3x4 m1, half4x1 m2);)
extern float3x2 mul(float3x4 m1, float4x2 m2);
extern int3x2 mul(int3x4 m1, int4x2 m2);
CG_DOUBLE_SUPPORT(extern double3x2 mul(double3x4 m1, double4x2 m2);)
CG_FIXED_SUPPORT(extern fixed3x2 mul(fixed3x4 m1, fixed4x2 m2);)
CG_HALF_SUPPORT(extern half3x2 mul(half3x4 m1, half4x2 m2);)
extern float3x3 mul(float3x4 m1, float4x3 m2);
extern int3x3 mul(int3x4 m1, int4x3 m2);
CG_DOUBLE_SUPPORT(extern double3x3 mul(double3x4 m1, double4x3 m2);)
CG_FIXED_SUPPORT(extern fixed3x3 mul(fixed3x4 m1, fixed4x3 m2);)
CG_HALF_SUPPORT(extern half3x3 mul(half3x4 m1, half4x3 m2);)
extern float3x4 mul(float3x4 m1, float4x4 m2);
extern int3x4 mul(int3x4 m1, int4x4 m2);
CG_DOUBLE_SUPPORT(extern double3x4 mul(double3x4 m1, double4x4 m2);)
CG_FIXED_SUPPORT(extern fixed3x4 mul(fixed3x4 m1, fixed4x4 m2);)
CG_HALF_SUPPORT(extern half3x4 mul(half3x4 m1, half4x4 m2);)
extern float4x1 mul(float4x1 m1, float1x1 m2);
extern int4x1 mul(int4x1 m1, int1x1 m2);
CG_DOUBLE_SUPPORT(extern double4x1 mul(double4x1 m1, double1x1 m2);)
CG_FIXED_SUPPORT(extern fixed4x1 mul(fixed4x1 m1, fixed1x1 m2);)
CG_HALF_SUPPORT(extern half4x1 mul(half4x1 m1, half1x1 m2);)
extern float4x2 mul(float4x1 m1, float1x2 m2);
extern int4x2 mul(int4x1 m1, int1x2 m2);
CG_DOUBLE_SUPPORT(extern double4x2 mul(double4x1 m1, double1x2 m2);)
CG_FIXED_SUPPORT(extern fixed4x2 mul(fixed4x1 m1, fixed1x2 m2);)
CG_HALF_SUPPORT(extern half4x2 mul(half4x1 m1, half1x2 m2);)
extern float4x3 mul(float4x1 m1, float1x3 m2);
extern int4x3 mul(int4x1 m1, int1x3 m2);
CG_DOUBLE_SUPPORT(extern double4x3 mul(double4x1 m1, double1x3 m2);)
CG_FIXED_SUPPORT(extern fixed4x3 mul(fixed4x1 m1, fixed1x3 m2);)
CG_HALF_SUPPORT(extern half4x3 mul(half4x1 m1, half1x3 m2);)
extern float4x4 mul(float4x1 m1, float1x4 m2);
extern int4x4 mul(int4x1 m1, int1x4 m2);
CG_DOUBLE_SUPPORT(extern double4x4 mul(double4x1 m1, double1x4 m2);)
CG_FIXED_SUPPORT(extern fixed4x4 mul(fixed4x1 m1, fixed1x4 m2);)
CG_HALF_SUPPORT(extern half4x4 mul(half4x1 m1, half1x4 m2);)
extern float4x1 mul(float4x2 m1, float2x1 m2);
extern int4x1 mul(int4x2 m1, int2x1 m2);
CG_DOUBLE_SUPPORT(extern double4x1 mul(double4x2 m1, double2x1 m2);)
CG_FIXED_SUPPORT(extern fixed4x1 mul(fixed4x2 m1, fixed2x1 m2);)
CG_HALF_SUPPORT(extern half4x1 mul(half4x2 m1, half2x1 m2);)
extern float4x2 mul(float4x2 m1, float2x2 m2);
extern int4x2 mul(int4x2 m1, int2x2 m2);
CG_DOUBLE_SUPPORT(extern double4x2 mul(double4x2 m1, double2x2 m2);)
CG_FIXED_SUPPORT(extern fixed4x2 mul(fixed4x2 m1, fixed2x2 m2);)
CG_HALF_SUPPORT(extern half4x2 mul(half4x2 m1, half2x2 m2);)
extern float4x3 mul(float4x2 m1, float2x3 m2);
extern int4x3 mul(int4x2 m1, int2x3 m2);
CG_DOUBLE_SUPPORT(extern double4x3 mul(double4x2 m1, double2x3 m2);)
CG_FIXED_SUPPORT(extern fixed4x3 mul(fixed4x2 m1, fixed2x3 m2);)
CG_HALF_SUPPORT(extern half4x3 mul(half4x2 m1, half2x3 m2);)
extern float4x4 mul(float4x2 m1, float2x4 m2);
extern int4x4 mul(int4x2 m1, int2x4 m2);
CG_DOUBLE_SUPPORT(extern double4x4 mul(double4x2 m1, double2x4 m2);)
CG_FIXED_SUPPORT(extern fixed4x4 mul(fixed4x2 m1, fixed2x4 m2);)
CG_HALF_SUPPORT(extern half4x4 mul(half4x2 m1, half2x4 m2);)
extern float4x1 mul(float4x3 m1, float3x1 m2);
extern int4x1 mul(int4x3 m1, int3x1 m2);
CG_DOUBLE_SUPPORT(extern double4x1 mul(double4x3 m1, double3x1 m2);)
CG_FIXED_SUPPORT(extern fixed4x1 mul(fixed4x3 m1, fixed3x1 m2);)
CG_HALF_SUPPORT(extern half4x1 mul(half4x3 m1, half3x1 m2);)
extern float4x2 mul(float4x3 m1, float3x2 m2);
extern int4x2 mul(int4x3 m1, int3x2 m2);
CG_DOUBLE_SUPPORT(extern double4x2 mul(double4x3 m1, double3x2 m2);)
CG_FIXED_SUPPORT(extern fixed4x2 mul(fixed4x3 m1, fixed3x2 m2);)
CG_HALF_SUPPORT(extern half4x2 mul(half4x3 m1, half3x2 m2);)
extern float4x3 mul(float4x3 m1, float3x3 m2);
extern int4x3 mul(int4x3 m1, int3x3 m2);
CG_DOUBLE_SUPPORT(extern double4x3 mul(double4x3 m1, double3x3 m2);)
CG_FIXED_SUPPORT(extern fixed4x3 mul(fixed4x3 m1, fixed3x3 m2);)
CG_HALF_SUPPORT(extern half4x3 mul(half4x3 m1, half3x3 m2);)
extern float4x4 mul(float4x3 m1, float3x4 m2);
extern int4x4 mul(int4x3 m1, int3x4 m2);
CG_DOUBLE_SUPPORT(extern double4x4 mul(double4x3 m1, double3x4 m2);)
CG_FIXED_SUPPORT(extern fixed4x4 mul(fixed4x3 m1, fixed3x4 m2);)
CG_HALF_SUPPORT(extern half4x4 mul(half4x3 m1, half3x4 m2);)
extern float4x1 mul(float4x4 m1, float4x1 m2);
extern int4x1 mul(int4x4 m1, int4x1 m2);
CG_DOUBLE_SUPPORT(extern double4x1 mul(double4x4 m1, double4x1 m2);)
CG_FIXED_SUPPORT(extern fixed4x1 mul(fixed4x4 m1, fixed4x1 m2);)
CG_HALF_SUPPORT(extern half4x1 mul(half4x4 m1, half4x1 m2);)
extern float4x2 mul(float4x4 m1, float4x2 m2);
extern int4x2 mul(int4x4 m1, int4x2 m2);
CG_DOUBLE_SUPPORT(extern double4x2 mul(double4x4 m1, double4x2 m2);)
CG_FIXED_SUPPORT(extern fixed4x2 mul(fixed4x4 m1, fixed4x2 m2);)
CG_HALF_SUPPORT(extern half4x2 mul(half4x4 m1, half4x2 m2);)
extern float4x3 mul(float4x4 m1, float4x3 m2);
extern int4x3 mul(int4x4 m1, int4x3 m2);
CG_DOUBLE_SUPPORT(extern double4x3 mul(double4x4 m1, double4x3 m2);)
CG_FIXED_SUPPORT(extern fixed4x3 mul(fixed4x4 m1, fixed4x3 m2);)
CG_HALF_SUPPORT(extern half4x3 mul(half4x4 m1, half4x3 m2);)
extern float4x4 mul(float4x4 m1, float4x4 m2);
extern int4x4 mul(int4x4 m1, int4x4 m2);
CG_DOUBLE_SUPPORT(extern double4x4 mul(double4x4 m1, double4x4 m2);)
CG_FIXED_SUPPORT(extern fixed4x4 mul(fixed4x4 m1, fixed4x4 m2);)
CG_HALF_SUPPORT(extern half4x4 mul(half4x4 m1, half4x4 m2);)
#endif
#endif // __Cg_matrix_hpp__

/// NORMALIZE - normalize
#ifndef __Cg_normalize_hpp__
extern float1 normalize(float1 v);
extern float2 normalize(float2 v);
extern float3 normalize(float3 v);
extern float4 normalize(float4 v);
CG_DOUBLE_SUPPORT(extern double1 normalize(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 normalize(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 normalize(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 normalize(double4 v);)
CG_HALF_SUPPORT(extern half1 normalize(half1 v);)
CG_HALF_SUPPORT(extern half2 normalize(half2 v);)
CG_HALF_SUPPORT(extern half3 normalize(half3 v);)
CG_HALF_SUPPORT(extern half4 normalize(half4 v);)
CG_FIXED_SUPPORT(extern fixed1 normalize(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 normalize(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 normalize(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 normalize(fixed4 v);)
#endif

/// POW - x raised to y
#ifndef __Cg_pow_hpp__
extern float1 pow(float1 x, float1 y);
extern float2 pow(float2 x, float2 y);
extern float3 pow(float3 x, float3 y);
extern float4 pow(float4 x, float4 y);
CG_HALF_SUPPORT(extern half1 pow(half1 x, half1 y);)
CG_HALF_SUPPORT(extern half2 pow(half2 x, half2 y);)
CG_HALF_SUPPORT(extern half3 pow(half3 x, half3 y);)
CG_HALF_SUPPORT(extern half4 pow(half4 x, half4 y);)
CG_FIXED_SUPPORT(extern fixed1 pow(fixed1 x, fixed1 y);)
CG_FIXED_SUPPORT(extern fixed2 pow(fixed2 x, fixed2 y);)
CG_FIXED_SUPPORT(extern fixed3 pow(fixed3 x, fixed3 y);)
CG_FIXED_SUPPORT(extern fixed4 pow(fixed4 x, fixed4 y);)
CG_DOUBLE_SUPPORT(extern double1 pow(double1 x, double1 y);)
CG_DOUBLE_SUPPORT(extern double2 pow(double2 x, double2 y);)
CG_DOUBLE_SUPPORT(extern double3 pow(double3 x, double3 y);)
CG_DOUBLE_SUPPORT(extern double4 pow(double4 x, double4 y);)
#endif

/// RADIANS - convert degrees to radians
#ifndef __Cg_radians_hpp__
extern float radians(float v);
extern float1 radians(float1 v);
extern float2 radians(float2 v);
extern float3 radians(float3 v);
extern float4 radians(float4 v);
CG_DOUBLE_SUPPORT(extern double radians(double v);)
CG_DOUBLE_SUPPORT(extern double1 radians(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 radians(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 radians(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 radians(double4 v);)
CG_HALF_SUPPORT(extern half radians(half v);)
CG_HALF_SUPPORT(extern half1 radians(half1 v);)
CG_HALF_SUPPORT(extern half2 radians(half2 v);)
CG_HALF_SUPPORT(extern half3 radians(half3 v);)
CG_HALF_SUPPORT(extern half4 radians(half4 v);)
CG_FIXED_SUPPORT(extern fixed1 radians(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 radians(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 radians(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 radians(fixed4 v);)
#endif

/// REFLECT - reflection of incident ray i and surface normal n
#ifndef __Cg_reflect_hpp__
extern float1 reflect(float1 i, float1 n);
extern float2 reflect(float2 i, float2 n);
extern float3 reflect(float3 i, float3 n);
extern float4 reflect(float4 i, float4 n);
CG_HALF_SUPPORT(extern half1 reflect(half1 i, half1 n);)
CG_HALF_SUPPORT(extern half2 reflect(half2 i, half2 n);)
CG_HALF_SUPPORT(extern half3 reflect(half3 i, half3 n);)
CG_HALF_SUPPORT(extern half4 reflect(half4 i, half4 n);)
CG_FIXED_SUPPORT(extern fixed1 reflect(fixed1 i, fixed1 n);)
CG_FIXED_SUPPORT(extern fixed2 reflect(fixed2 i, fixed2 n);)
CG_FIXED_SUPPORT(extern fixed3 reflect(fixed3 i, fixed3 n);)
CG_FIXED_SUPPORT(extern fixed4 reflect(fixed4 i, fixed4 n);)
CG_DOUBLE_SUPPORT(extern double1 reflect(double1 i, double1 n);)
CG_DOUBLE_SUPPORT(extern double2 reflect(double2 i, double2 n);)
CG_DOUBLE_SUPPORT(extern double3 reflect(double3 i, double3 n);)
CG_DOUBLE_SUPPORT(extern double4 reflect(double4 i, double4 n);)
#endif

/// ROUND - round to nearest integer
#ifndef __Cg_round_hpp__
extern float round(float v);
extern float1 round(float1 v);
extern float2 round(float2 v);
extern float3 round(float3 v);
extern float4 round(float4 v);
CG_DOUBLE_SUPPORT(extern double round(double v);)
CG_DOUBLE_SUPPORT(extern double1 round(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 round(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 round(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 round(double4 v);)
CG_HALF_SUPPORT(extern half round(half v);)
CG_HALF_SUPPORT(extern half1 round(half1 v);)
CG_HALF_SUPPORT(extern half2 round(half2 v);)
CG_HALF_SUPPORT(extern half3 round(half3 v);)
CG_HALF_SUPPORT(extern half4 round(half4 v);)
CG_FIXED_SUPPORT(extern fixed round(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 round(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 round(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 round(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 round(fixed4 v);)
#endif

/// RSQRT - reciprocal square root
#ifndef __Cg_rsqrt_hpp__
extern float rsqrt(float v);
extern float1 rsqrt(float1 v);
extern float2 rsqrt(float2 v);
extern float3 rsqrt(float3 v);
extern float4 rsqrt(float4 v);
CG_DOUBLE_SUPPORT(extern double rsqrt(double v);)
CG_DOUBLE_SUPPORT(extern double1 rsqrt(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 rsqrt(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 rsqrt(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 rsqrt(double4 v);)
CG_HALF_SUPPORT(extern half rsqrt(half v);)
CG_HALF_SUPPORT(extern half1 rsqrt(half1 v);)
CG_HALF_SUPPORT(extern half2 rsqrt(half2 v);)
CG_HALF_SUPPORT(extern half3 rsqrt(half3 v);)
CG_HALF_SUPPORT(extern half4 rsqrt(half4 v);)
CG_FIXED_SUPPORT(extern fixed rsqrt(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 rsqrt(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 rsqrt(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 rsqrt(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 rsqrt(fixed4 v);)
#endif

/// SATURATE - saturate to [0,1] range
#ifndef __Cg_saturate_hpp__
extern int saturate(int v);
extern int1 saturate(int1 v);
extern int2 saturate(int2 v);
extern int3 saturate(int3 v);
extern int4 saturate(int4 v);
extern float saturate(float v);
extern float1 saturate(float1 v);
extern float2 saturate(float2 v);
extern float3 saturate(float3 v);
extern float4 saturate(float4 v);
CG_DOUBLE_SUPPORT(extern double saturate(double v);)
CG_DOUBLE_SUPPORT(extern double1 saturate(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 saturate(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 saturate(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 saturate(double4 v);)
CG_HALF_SUPPORT(extern half saturate(half v);)
CG_HALF_SUPPORT(extern half1 saturate(half1 v);)
CG_HALF_SUPPORT(extern half2 saturate(half2 v);)
CG_HALF_SUPPORT(extern half3 saturate(half3 v);)
CG_HALF_SUPPORT(extern half4 saturate(half4 v);)
CG_FIXED_SUPPORT(extern fixed saturate(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 saturate(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 saturate(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 saturate(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 saturate(fixed4 v);)
#endif

/// SIGN - determine sign
#ifndef __Cg_sign_hpp__
extern int sign(int v);
extern int1 sign(int1 v);
extern int2 sign(int2 v);
extern int3 sign(int3 v);
extern int4 sign(int4 v);
extern float sign(float v);
extern float1 sign(float1 v);
extern float2 sign(float2 v);
extern float3 sign(float3 v);
extern float4 sign(float4 v);
CG_DOUBLE_SUPPORT(extern double sign(double v);)
CG_DOUBLE_SUPPORT(extern double1 sign(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 sign(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 sign(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 sign(double4 v);)
CG_HALF_SUPPORT(extern half sign(half v);)
CG_HALF_SUPPORT(extern half1 sign(half1 v);)
CG_HALF_SUPPORT(extern half2 sign(half2 v);)
CG_HALF_SUPPORT(extern half3 sign(half3 v);)
CG_HALF_SUPPORT(extern half4 sign(half4 v);)
CG_FIXED_SUPPORT(extern fixed sign(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 sign(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 sign(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 sign(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 sign(fixed4 v);)
#endif

/// SIN - sine
#ifndef __Cg_sin_hpp__
extern float sin(float v);
extern float1 sin(float1 v);
extern float2 sin(float2 v);
extern float3 sin(float3 v);
extern float4 sin(float4 v);
CG_DOUBLE_SUPPORT(extern double sin(double v);)
CG_DOUBLE_SUPPORT(extern double1 sin(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 sin(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 sin(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 sin(double4 v);)
CG_HALF_SUPPORT(extern half sin(half v);)
CG_HALF_SUPPORT(extern half1 sin(half1 v);)
CG_HALF_SUPPORT(extern half2 sin(half2 v);)
CG_HALF_SUPPORT(extern half3 sin(half3 v);)
CG_HALF_SUPPORT(extern half4 sin(half4 v);)
CG_FIXED_SUPPORT(extern fixed sin(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 sin(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 sin(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 sin(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 sin(fixed4 v);)
#endif

/// SINCOS - output both sine and cosine of v
#ifdef __Cg_inout_hpp__
#ifndef __Cg_sincos_hpp__
extern void sincos(float v, Out<float> s, Out<float> c);
extern void sincos(float1 v, Out<float1> s, Out<float1> c);
extern void sincos(float2 v, Out<float2> s, Out<float2> c);
extern void sincos(float3 v, Out<float3> s, Out<float3> c);
extern void sincos(float4 v, Out<float4> s, Out<float4> c);
CG_DOUBLE_SUPPORT(extern void sincos(double v, Out<double> s, Out<double> c);)
CG_DOUBLE_SUPPORT(extern void sincos(double1 v, Out<double1> s, Out<double1> c);)
CG_DOUBLE_SUPPORT(extern void sincos(double2 v, Out<double2> s, Out<double2> c);)
CG_DOUBLE_SUPPORT(extern void sincos(double3 v, Out<double3> s, Out<double3> c);)
CG_DOUBLE_SUPPORT(extern void sincos(double4 v, Out<double4> s, Out<double4> c);)
CG_HALF_SUPPORT(extern void sincos(half v, Out<half> s, Out<half> c);)
CG_HALF_SUPPORT(extern void sincos(half1 v, Out<half1> s, Out<half1> c);)
CG_HALF_SUPPORT(extern void sincos(half2 v, Out<half2> s, Out<half2> c);)
CG_HALF_SUPPORT(extern void sincos(half3 v, Out<half3> s, Out<half3> c);)
CG_HALF_SUPPORT(extern void sincos(half4 v, Out<half4> s, Out<half4> c);)
CG_FIXED_SUPPORT(extern void sincos(fixed v, Out<fixed> s, Out<fixed> c);)
CG_FIXED_SUPPORT(extern void sincos(fixed1 v, Out<fixed1> s, Out<fixed1> c);)
CG_FIXED_SUPPORT(extern void sincos(fixed2 v, Out<fixed2> s, Out<fixed2> c);)
CG_FIXED_SUPPORT(extern void sincos(fixed3 v, Out<fixed3> s, Out<fixed3> c);)
CG_FIXED_SUPPORT(extern void sincos(fixed4 v, Out<fixed4> s, Out<fixed4> c);)
#endif
#endif // __Cg_inout_hpp__

/// SINH - hyperbolic sine
#ifndef __Cg_sinh_hpp__
extern float sinh(float v);
extern float1 sinh(float1 v);
extern float2 sinh(float2 v);
extern float3 sinh(float3 v);
extern float4 sinh(float4 v);
CG_DOUBLE_SUPPORT(extern double sinh(double v);)
CG_DOUBLE_SUPPORT(extern double1 sinh(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 sinh(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 sinh(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 sinh(double4 v);)
CG_HALF_SUPPORT(extern half sinh(half v);)
CG_HALF_SUPPORT(extern half1 sinh(half1 v);)
CG_HALF_SUPPORT(extern half2 sinh(half2 v);)
CG_HALF_SUPPORT(extern half3 sinh(half3 v);)
CG_HALF_SUPPORT(extern half4 sinh(half4 v);)
CG_FIXED_SUPPORT(extern fixed sinh(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 sinh(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 sinh(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 sinh(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 sinh(fixed4 v);)
#endif

/// SMOOTHSTEP - smooth step function
#ifndef __Cg_smoothstep_hpp__
extern float1 smoothstep(float1 a, float1 b, float1 x);
extern float2 smoothstep(float2 a, float2 b, float2 x);
extern float3 smoothstep(float3 a, float3 b, float3 x);
extern float4 smoothstep(float4 a, float4 b, float4 x);
CG_HALF_SUPPORT(extern half1 smoothstep(half1 a, half1 b, half1 x);)
CG_HALF_SUPPORT(extern half2 smoothstep(half2 a, half2 b, half2 x);)
CG_HALF_SUPPORT(extern half3 smoothstep(half3 a, half3 b, half3 x);)
CG_HALF_SUPPORT(extern half4 smoothstep(half4 a, half4 b, half4 x);)
CG_FIXED_SUPPORT(extern fixed1 smoothstep(fixed1 a, fixed1 b, fixed1 x);)
CG_FIXED_SUPPORT(extern fixed2 smoothstep(fixed2 a, fixed2 b, fixed2 x);)
CG_FIXED_SUPPORT(extern fixed3 smoothstep(fixed3 a, fixed3 b, fixed3 x);)
CG_FIXED_SUPPORT(extern fixed4 smoothstep(fixed4 a, fixed4 b, fixed4 x);)
CG_DOUBLE_SUPPORT(extern double1 smoothstep(double1 a, double1 b, double1 x);)
CG_DOUBLE_SUPPORT(extern double2 smoothstep(double2 a, double2 b, double2 x);)
CG_DOUBLE_SUPPORT(extern double3 smoothstep(double3 a, double3 b, double3 x);)
CG_DOUBLE_SUPPORT(extern double4 smoothstep(double4 a, double4 b, double4 x);)
#endif

/// SQRT - square root
#ifndef __Cg_sqrt_hpp__
extern float sqrt(float v);
extern float1 sqrt(float1 v);
extern float2 sqrt(float2 v);
extern float3 sqrt(float3 v);
extern float4 sqrt(float4 v);
CG_DOUBLE_SUPPORT(extern double sqrt(double v);)
CG_DOUBLE_SUPPORT(extern double1 sqrt(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 sqrt(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 sqrt(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 sqrt(double4 v);)
CG_HALF_SUPPORT(extern half sqrt(half v);)
CG_HALF_SUPPORT(extern half1 sqrt(half1 v);)
CG_HALF_SUPPORT(extern half2 sqrt(half2 v);)
CG_HALF_SUPPORT(extern half3 sqrt(half3 v);)
CG_HALF_SUPPORT(extern half4 sqrt(half4 v);)
CG_FIXED_SUPPORT(extern fixed sqrt(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 sqrt(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 sqrt(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 sqrt(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 sqrt(fixed4 v);)
#endif

/// STEP - zero if x is less than a; otherwise one
#ifndef __Cg_step_hpp__
extern float1 step(float1 a, float1 x);
extern float2 step(float2 a, float2 x);
extern float3 step(float3 a, float3 x);
extern float4 step(float4 a, float4 x);
CG_HALF_SUPPORT(extern half1 step(half1 a, half1 x);)
CG_HALF_SUPPORT(extern half2 step(half2 a, half2 x);)
CG_HALF_SUPPORT(extern half3 step(half3 a, half3 x);)
CG_HALF_SUPPORT(extern half4 step(half4 a, half4 x);)
CG_FIXED_SUPPORT(extern fixed1 step(fixed1 a, fixed1 x);)
CG_FIXED_SUPPORT(extern fixed2 step(fixed2 a, fixed2 x);)
CG_FIXED_SUPPORT(extern fixed3 step(fixed3 a, fixed3 x);)
CG_FIXED_SUPPORT(extern fixed4 step(fixed4 a, fixed4 x);)
CG_DOUBLE_SUPPORT(extern double1 step(double1 a, double1 x);)
CG_DOUBLE_SUPPORT(extern double2 step(double2 a, double2 x);)
CG_DOUBLE_SUPPORT(extern double3 step(double3 a, double3 x);)
CG_DOUBLE_SUPPORT(extern double4 step(double4 a, double4 x);)
#endif

/// TAN - tangent
#ifndef __Cg_tan_hpp__
extern float tan(float v);
extern float1 tan(float1 v);
extern float2 tan(float2 v);
extern float3 tan(float3 v);
extern float4 tan(float4 v);
CG_DOUBLE_SUPPORT(extern double tan(double v);)
CG_DOUBLE_SUPPORT(extern double1 tan(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 tan(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 tan(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 tan(double4 v);)
CG_HALF_SUPPORT(extern half tan(half v);)
CG_HALF_SUPPORT(extern half1 tan(half1 v);)
CG_HALF_SUPPORT(extern half2 tan(half2 v);)
CG_HALF_SUPPORT(extern half3 tan(half3 v);)
CG_HALF_SUPPORT(extern half4 tan(half4 v);)
CG_FIXED_SUPPORT(extern fixed tan(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 tan(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 tan(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 tan(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 tan(fixed4 v);)
#endif

/// TANH - hyperbolic tangent
#ifndef __Cg_tanh_hpp__
extern float tanh(float v);
extern float1 tanh(float1 v);
extern float2 tanh(float2 v);
extern float3 tanh(float3 v);
extern float4 tanh(float4 v);
CG_DOUBLE_SUPPORT(extern double tanh(double v);)
CG_DOUBLE_SUPPORT(extern double1 tanh(double1 v);)
CG_DOUBLE_SUPPORT(extern double2 tanh(double2 v);)
CG_DOUBLE_SUPPORT(extern double3 tanh(double3 v);)
CG_DOUBLE_SUPPORT(extern double4 tanh(double4 v);)
CG_HALF_SUPPORT(extern half tanh(half v);)
CG_HALF_SUPPORT(extern half1 tanh(half1 v);)
CG_HALF_SUPPORT(extern half2 tanh(half2 v);)
CG_HALF_SUPPORT(extern half3 tanh(half3 v);)
CG_HALF_SUPPORT(extern half4 tanh(half4 v);)
CG_FIXED_SUPPORT(extern fixed tanh(fixed v);)
CG_FIXED_SUPPORT(extern fixed1 tanh(fixed1 v);)
CG_FIXED_SUPPORT(extern fixed2 tanh(fixed2 v);)
CG_FIXED_SUPPORT(extern fixed3 tanh(fixed3 v);)
CG_FIXED_SUPPORT(extern fixed4 tanh(fixed4 v);)
#endif

/// TRANSPOSE - matrix tranpose
#ifdef __Cg_matrix_hpp__
# ifndef __Cg_transpose_hpp__
extern float4x4 transpose(float4x4 a);
extern float3x4 transpose(float4x3 a);
extern float2x4 transpose(float4x2 a);
extern float1x4 transpose(float4x1 a);
extern float4x3 transpose(float3x4 a);
extern float3x3 transpose(float3x3 a);
extern float2x3 transpose(float3x2 a);
extern float1x3 transpose(float3x1 a);
extern float4x2 transpose(float2x4 a);
extern float3x2 transpose(float2x3 a);
extern float2x2 transpose(float2x2 a);
extern float1x2 transpose(float2x1 a);
extern float4x1 transpose(float1x4 a);
extern float3x1 transpose(float1x3 a);
extern float2x1 transpose(float1x2 a);
extern float1x1 transpose(float1x1 a);
CG_DOUBLE_SUPPORT(extern double4x4 transpose(double4x4 a);)
CG_DOUBLE_SUPPORT(extern double3x4 transpose(double4x3 a);)
CG_DOUBLE_SUPPORT(extern double2x4 transpose(double4x2 a);)
CG_DOUBLE_SUPPORT(extern double1x4 transpose(double4x1 a);)
CG_DOUBLE_SUPPORT(extern double4x3 transpose(double3x4 a);)
CG_DOUBLE_SUPPORT(extern double3x3 transpose(double3x3 a);)
CG_DOUBLE_SUPPORT(extern double2x3 transpose(double3x2 a);)
CG_DOUBLE_SUPPORT(extern double1x3 transpose(double3x1 a);)
CG_DOUBLE_SUPPORT(extern double4x2 transpose(double2x4 a);)
CG_DOUBLE_SUPPORT(extern double3x2 transpose(double2x3 a);)
CG_DOUBLE_SUPPORT(extern double2x2 transpose(double2x2 a);)
CG_DOUBLE_SUPPORT(extern double1x2 transpose(double2x1 a);)
CG_DOUBLE_SUPPORT(extern double4x1 transpose(double1x4 a);)
CG_DOUBLE_SUPPORT(extern double3x1 transpose(double1x3 a);)
CG_DOUBLE_SUPPORT(extern double2x1 transpose(double1x2 a);)
CG_DOUBLE_SUPPORT(extern double1x1 transpose(double1x1 a);)
# endif
#endif // __Cg_matrix_hpp__

/// UPPER4x4 - matrix upper 4x4
#ifdef __Cg_matrix_hpp__
# ifndef __Cg_upper4x4_hpp__
extern float4x4 upper4x4(float4x4 a);
extern float4x4 upper4x4(float4x3 a);
extern float4x4 upper4x4(float4x2 a);
extern float4x4 upper4x4(float4x1 a);
extern float4x4 upper4x4(float3x4 a);
extern float4x4 upper4x4(float3x3 a);
extern float4x4 upper4x4(float3x2 a);
extern float4x4 upper4x4(float3x1 a);
extern float4x4 upper4x4(float2x4 a);
extern float4x4 upper4x4(float2x3 a);
extern float4x4 upper4x4(float2x2 a);
extern float4x4 upper4x4(float2x1 a);
extern float4x4 upper4x4(float1x4 a);
extern float4x4 upper4x4(float1x3 a);
extern float4x4 upper4x4(float1x2 a);
extern float4x4 upper4x4(float1x1 a);
extern int4x4 upper4x4(int4x4 a);
extern int4x4 upper4x4(int4x3 a);
extern int4x4 upper4x4(int4x2 a);
extern int4x4 upper4x4(int4x1 a);
extern int4x4 upper4x4(int3x4 a);
extern int4x4 upper4x4(int3x3 a);
extern int4x4 upper4x4(int3x2 a);
extern int4x4 upper4x4(int3x1 a);
extern int4x4 upper4x4(int2x4 a);
extern int4x4 upper4x4(int2x3 a);
extern int4x4 upper4x4(int2x2 a);
extern int4x4 upper4x4(int2x1 a);
extern int4x4 upper4x4(int1x4 a);
extern int4x4 upper4x4(int1x3 a);
extern int4x4 upper4x4(int1x2 a);
extern int4x4 upper4x4(int1x1 a);
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double4x4 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double4x3 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double4x2 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double4x1 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double3x4 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double3x3 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double3x2 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double3x1 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double2x4 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double2x3 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double2x2 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double2x1 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double1x4 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double1x3 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double1x2 a);)
CG_DOUBLE_SUPPORT(extern double4x4 upper4x4(double1x1 a);)
# endif
#endif // __Cg_matrix_hpp__

} // namespace Cg

#endif // __Cg_stdlib_hpp__
