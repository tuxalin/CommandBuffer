/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/inout.hpp>
#include <Cg/stdlib.hpp>

namespace Cg {

template <typename T>
static inline __CGvector<typename __CGtype_trait<T>::realType,1> __CGfresnel(
    const __CGvector<T,3> & i, const __CGvector<T,3> & n,
    T eta,
    T& ktOut,
    __CGvector<T,3>& rOut,
    __CGvector<T,3>& tOut)
{
    typedef typename __CGtype_trait<T>::dotType dotType;
    // floats except doubles when T is double
    T result;
    dotType c1;
    dotType cs2;
    dotType tflag;
    __CGvector<dotType,3> t;

    // Refraction vector courtesy Paul Heckbert.
    c1 = __CGvector<dotType,1>(dot(-i,n));
    cs2 = 1-eta*eta*(1-c1*c1);
    tflag = cs2 >= 0;
    t = tflag * (((eta*c1-std::sqrt(cs2))*n) + eta*i);
    // t is already unit length or (0,0,0)

    // From Global Illumination Compendeum.
    // floats except doubles when T is double
    dotType ndott;
    dotType cosr_div_cosi;
    dotType cosi_div_cosr;
    dotType fs;
    dotType fp;
    dotType kr;

    ndott = dot(-__CGvector<dotType,3>(n),t);
    cosr_div_cosi = ndott / c1;
    cosi_div_cosr = c1 / ndott;
    fs = (cosr_div_cosi - eta) / (cosr_div_cosi + eta);
    fs = fs * fs;
    fp = (cosi_div_cosr - eta) / (cosi_div_cosr + eta);
    fp = fp * fp;
    kr = (fs+fp) / 2;
    result = T(tflag*kr + (1-tflag));

    // Extra outputs
    ktOut = T(tflag * ((1-kr)/(eta*eta)));
    tOut = __CGvector<T,3>(t);
    rOut = reflect(i, n);

    return __CGvector<T,1>(result);
}

float1 fresnel(float3 i, float3 n, float eta, Out<float> kt, Out<float3> r, Out<float3> t)
{
    return __CGfresnel<float>(i, n, eta, kt, r, t);
}
float1 fresnel(float3 i, float3 n, float eta, Out<float3> r, Out<float3> t)
{
    float kt;
    return __CGfresnel<float>(i, n, eta, kt, r, t);
}
float1 fresnel(float3 i, float3 n, float eta, Out<float> kt)
{
    float3 r;
    float3 t;
    return __CGfresnel<float>(i, n, eta, kt, r, t);
}
float1 fresnel(float3 i, float3 n, float eta)
{
    float kt;
    float3 r;
    float3 t;
    return __CGfresnel<float>(i, n, eta, kt, r, t);
}

half1 fresnel(half3 i, half3 n, half eta, Out<half> kt, Out<half3> r, Out<half3> t)
{
    return __CGfresnel<half>(i, n, eta, kt, r, t);
}
half1 fresnel(half3 i, half3 n, half eta, Out<half3> r, Out<half3> t)
{
    half kt;
    return __CGfresnel<half>(i, n, eta, kt, r, t);
}
half1 fresnel(half3 i, half3 n, half eta, Out<half> kt)
{
    half3 r;
    half3 t;
    return __CGfresnel<half>(i, n, eta, kt, r, t);
}
half1 fresnel(half3 i, half3 n, half eta)
{
    half kt;
    half3 r;
    half3 t;
    return __CGfresnel<half>(i, n, eta, kt, r, t);
}

fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed> kt, Out<fixed3> r, Out<fixed3> t)
{
    return __CGfresnel<fixed>(i, n, eta, kt, r, t);
}
fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed3> r, Out<fixed3> t)
{
    fixed kt;
    return __CGfresnel<fixed>(i, n, eta, kt, r, t);
}
fixed1 fresnel(fixed3 i, fixed3 n, fixed eta, Out<fixed> kt)
{
    fixed3 r;
    fixed3 t;
    return __CGfresnel<fixed>(i, n, eta, kt, r, t);
}
fixed1 fresnel(fixed3 i, fixed3 n, fixed eta)
{
    fixed kt;
    fixed3 r;
    fixed3 t;
    return __CGfresnel<fixed>(i, n, eta, kt, r, t);
}

double1 fresnel(double3 i, double3 n, double eta, Out<double> kt, Out<double3> r, Out<double3> t)
{
    return __CGfresnel<double>(i, n, eta, kt, r, t);
}
double1 fresnel(double3 i, double3 n, double eta, Out<double3> r, Out<double3> t)
{
    double kt;
    return __CGfresnel<double>(i, n, eta, kt, r, t);
}
double1 fresnel(double3 i, double3 n, double eta, Out<double> kt)
{
    double3 r;
    double3 t;
    return __CGfresnel<double>(i, n, eta, kt, r, t);
}
double1 fresnel(double3 i, double3 n, double eta)
{
    double kt;
    double3 r;
    double3 t;
    return __CGfresnel<double>(i, n, eta, kt, r, t);
}

} // namespace Cg

