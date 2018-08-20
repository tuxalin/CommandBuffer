/* 
 * Copyright 2005-2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_vector_hpp__
#define __Cg_vector_hpp__

#include <Cg/assert.hpp>  // for __CGassert to catch out-of-range vector indexing

/*
 * Template-based implementation of Cg-style vector data types.
 *
 * Supports:
 * - All swizzle operations (.xyzw, .rgba, and .stpq)
 * - Write masking
 * - Complete overloaded operators (vector and scalar)
 *
 * Syntax supported by Cg but not implementable for C++:
 * - Aggregate initialization
 *   - Example: float4 foo = { 1, 2, 3, 4 };
 *   - Workaround: float4 foo = float4(1,2,3,4);
 * - Swizzling of base types
 *   - Example: float f = 3; f.xxxx;
 *   - Workaround: float1 f = 3; f.xxx;
 * - Type specifiers unsigned and signed not allowed for vectors
 *   - Example: unsigned int3 var;
 *   - Workaround: typedef __CGvector<unsigned int,3> uint3; uint3 var;
 * - Swizzled swizzles (including write mask usage)
 *   - Example: foo.xw.y;
 *   - Workaround: float2(foo.xw).y
 * - Type promotion for vector function parameters.
 *   - Example: half3 foo = half3(1,2,3); float3 bar = float3(4,5,6); dot(foo,bar);
 *   - Workaround: float(float3(foo), bar);
 *   - Alternative workaround for stdlib functions: Use stdlib routine's templated header such as #include <Cg/dot.hpp>
 * - ?: should work component-wise for vectors
 *   - Example: float2 c = float2(4,5); float2 foo2 = float2(-1,1); float2 var = (foo2 < 0) ? -c : c;
 *   - Workaround: float2 var = lerp(c, -c, float2(foo2 < 0));
 *
 * Not supported by Cg but (unavoidably) allowed here:
 * - Constructor style initialization
 *   - Example: float4 foo(1,2,3,4);
 * - Array indexing of 1-component swizzle
 *   - Example: float2 foo = float2(3.1,2.7); foo.y[0];
 *   - [0] is the only reasonable index
 *
 * The recommended way to initialize vectors is: float3 foo = float3(1,2,3)
 * This works for both Cg and this Cg-style vector data type implementation.
 * Avoid both aggregate initialization and construction-style initialization
 * for portability between Cg and C++ with cgvector.h.
 */

namespace Cg {

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(push)
#pragma warning(disable:4800)  // forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4512)  // assignment operator could not be generated
#pragma warning(disable:4510)  // default constructor could not be generated
#pragma warning(disable:4610)  // union can never be instantiated - user defined constructor required
#endif

// FUNDAMENTAL BASE CLASS

#ifdef __CG_BASE_CLASS
#define __CG_DERIVE_FROM_BASE_CLASS , public __CG_BASE_CLASS
#else
#define __CG_DERIVE_FROM_BASE_CLASS
#endif

// TYPE COMBINATION TRAITS

// Homogenous binary type traits default to the singular (specialized) type trait.
template <typename T1, typename T2 = T1>
struct __CGtype_trait { };

// Trinary type traits rely on composing binary traits.
template <typename T1, typename T2, typename T3, 
          typename result = typename __CGtype_trait<typename __CGtype_trait<T1,T2>::resultType,T3>::resultType>
struct __CGtype_trait3 : __CGtype_trait<typename __CGtype_trait<T1,T2>::resultType,T3> { };

// Specializations are needed for atomic types.
template <>
struct __CGtype_trait<float> {
    typedef float storageType;
    typedef float resultType;
#ifdef __CG_FLOAT_DOT_TYPE_IS_FLOAT
    typedef float dotType;
#else
    typedef double dotType;
#endif
    typedef bool boolType;
    typedef resultType numericType;
    typedef resultType realType;
};
template <>
struct __CGtype_trait<int> {
    typedef int storageType;
    typedef int resultType;
    typedef int dotType;
    typedef bool boolType;
    typedef resultType intType;
    typedef resultType numericType;
};
template <>
struct __CGtype_trait<bool> {
    typedef bool storageType;
    typedef bool resultType;
    typedef bool boolType;
};
// Heterogenous binary __CGtype_traits are defined by deriving the trait from the pair's higher priority type trait.
template <>
struct __CGtype_trait<float,int> : __CGtype_trait<float> { };
template <>
struct __CGtype_trait<int,float> : __CGtype_trait<float> { };
// DOUBLE TYPE
/* Because C/C++ treats floating-point literals such as 1.3 typed as
   the built-in double, type traits for mixing such literals with
   other values is desirable even when <Cg/double.hpp> is not included.
   The __CGdouble type is a double when <Cg/double.hpp> is included
   prior to <Cg/vector.hpp> but a float otherwise. */
#ifdef __Cg_double_hpp__
typedef double __CGdouble;
#else
typedef float __CGdouble;
#endif
template <>
struct __CGtype_trait<double> {
    typedef __CGdouble storageType;
    typedef __CGdouble resultType;
    typedef __CGdouble dotType;
    typedef bool boolType;
    typedef resultType numericType;
    typedef resultType realType;
};
template <>
struct __CGtype_trait<double,float> : __CGtype_trait<double> { };
template <>
struct __CGtype_trait<float,double> : __CGtype_trait<double> { };
template <>
struct __CGtype_trait<double,int> : __CGtype_trait<double> { };
template <>
struct __CGtype_trait<int,double> : __CGtype_trait<double> { };
// NON-DEFAULT HALF TYPE
#ifdef __Cg_half_hpp__
template <>
struct __CGtype_trait<half> {
    typedef __CGhalf_storage storageType;
    typedef half resultType;
    typedef float dotType;
    typedef bool boolType;
    typedef resultType numericType;
    typedef resultType realType;
};
template <>
struct __CGtype_trait<float,half> : __CGtype_trait<float> { };
template <>
struct __CGtype_trait<half,float> : __CGtype_trait<float> { };
template <>
struct __CGtype_trait<half,int> : __CGtype_trait<half> { };
template <>
struct __CGtype_trait<int,half> : __CGtype_trait<half> { };
#endif
// NON-DEFAULT FIXED TYPE
#ifdef __Cg_fixed_hpp__
template <>
struct __CGtype_trait<fixed> {
    typedef cgFixedStorage storageType;
    typedef fixed resultType;
    typedef float dotType;
    typedef bool boolType;
    typedef resultType numericType;
    typedef resultType realType;
};
template <>
struct __CGtype_trait<float,fixed> : __CGtype_trait<float> { };
template <>
struct __CGtype_trait<fixed,float> : __CGtype_trait<float> { };
template <>
struct __CGtype_trait<fixed,int> : __CGtype_trait<fixed> { };
template <>
struct __CGtype_trait<int,fixed> : __CGtype_trait<fixed> { };
#endif
// COMBINATIONS OF NON-DEFAULT TYPES
#if defined(__Cg_fixed_hpp__)
template <>
struct __CGtype_trait<double,fixed> : __CGtype_trait<double> { };
template <>
struct __CGtype_trait<fixed,double> : __CGtype_trait<double> { };
#endif
#if defined(__Cg_half_hpp__)
template <>
struct __CGtype_trait<double,half> : __CGtype_trait<double> { };
template <>
struct __CGtype_trait<half,double> : __CGtype_trait<double> { };
#endif
#if defined(__Cg_fixed_hpp__) && defined(__Cg_half_hpp__)
template <>
struct __CGtype_trait<half,fixed> : __CGtype_trait<half> { };
template <>
struct __CGtype_trait<fixed,half> : __CGtype_trait<half> { };
#endif

//// FORWARD-DECLARED TEMPLATED VECTOR TYPE
template <typename T, int N>
class __CGvector { };

//// TEMPLATED VECTOR USAGE CLASS
template <typename T, int N, typename Tstore>
class __CGvector_usage : public Tstore {
public:
    // Do not write any constructors!
    // NO assignment operator needed!
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator += (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] += tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator += (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] += s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator -= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] -= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator -= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] -= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator *= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] *= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator *= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] *= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator /= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] /= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator /= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] /= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator %= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] %= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator %= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] %= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator ^= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] ^= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator ^= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] ^= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator &= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] &= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator &= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] &= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator |= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] |= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator |= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] |= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator <<= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] <<= tmp[i];
        return *this;
    }
    inline __CGvector_usage & operator <<= (const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] <<= s;
        return *this;
    }
    template <typename T2, typename T2store>
    inline __CGvector_usage & operator >>= (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp;
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] >>= tmp[i];
        return *this;
    } 
    inline __CGvector_usage & operator >>= (const T & s) {
        for (int i=0; i<N; i++)
	    (*this)[i] >>= s;
        return *this;
    }
    inline __CGvector_usage & operator ++ () { /* prefix */
        for (int i=0; i<N; i++)
            ++ (*this)[i];
        return *this;
    }
    inline __CGvector_usage operator ++ (int) { /* postfix */
        __CGvector_usage oldVector = *this;
        for (int i=0; i<N; i++)
            ++ (*this)[i];
        return oldVector;
    }
    inline __CGvector_usage & operator -- () { /* prefix */
        for (int i=0; i<N; i++)
            -- (*this)[i];
        return *this;
    }
    inline __CGvector_usage operator -- (int) { /* postfix */
        __CGvector_usage oldVector = *this;
        for (int i=0; i<N; i++)
            -- (*this)[i];
        return oldVector;
    }
};

//// PLURAL VERSION OF VECTOR USAGE BASE CLASS
template <typename T, int N, typename Tstore>
class __CGvector_plural_usage : public __CGvector_usage<T,N,Tstore> {
};

//// SWIZZLE STORAGE BASE CLASS
template <typename T, int STORAGE, int N, unsigned int SMASK>
class __CGswizzle_storage {
    typename __CGtype_trait<T>::storageType v[STORAGE];  // Don't allow access to storage except through indexing.
    static inline int swizzleIndex(int ndx)
    {
        int swizndx = (SMASK >> (8*ndx)) & 0xFF;
        __CGassert(swizndx < STORAGE);
        __CGassert(swizndx >= 0);
        return swizndx;
    }
public:
    // Swizzled indexing
    inline T & operator [](int ndx) { return *reinterpret_cast<T*>(&v[swizzleIndex(ndx)]); }
    inline const T & operator [](int ndx) const { return *reinterpret_cast<const T*>(&v[swizzleIndex(ndx)]); }

    template <typename T2, typename T2store>
    inline __CGswizzle_storage & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp; /* needed for proper vector self-assignments */
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] = tmp[i];
        return *this;
    }
    inline __CGswizzle_storage & operator = (const T v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v;
        return *this;
    }
};

#if defined(__GNUC__) && (__GNUC__>3 || (__GNUC__==3 && __GNUC_MINOR__>=3))
#define __CGmay_alias __attribute__((__may_alias__))
#else
#define __CGmay_alias 
#endif

//// TEMPLATED SWIZZLED TYPES
template <typename T, int STORAGE, int N, unsigned int SMASK>
class __CGswizzle : public __CGvector_plural_usage<T,N,__CGswizzle_storage<T,STORAGE,N,SMASK> > {
public:
    template <typename T2, typename T2store>
    inline __CGswizzle & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGswizzle_storage<T,STORAGE,N,SMASK>::operator=(ind);
        return *this;
    }
    inline __CGswizzle & operator = (const T v) {
        __CGswizzle_storage<T,STORAGE,N,SMASK>::operator=(v);
        return *this;
    }
} __CGmay_alias;

/// SPECIALIZED ONE-COMPONENT SWIZZLED TYPE (not derived from __CGvector_plural_usage)
template <typename T, int STORAGE, unsigned int SMASK>
class __CGswizzle<T,STORAGE,1,SMASK> : public __CGvector_usage<T,1,__CGswizzle_storage<T,STORAGE,1,SMASK> > {
public:
    template <typename T2, typename T2store>
    inline __CGswizzle & operator = (const __CGvector_usage<T2,1,T2store> & ind) {
        __CGswizzle_storage<T,STORAGE,1,SMASK>::operator=(ind);
        return *this;
    }
    inline __CGswizzle & operator = (const T v) {
        __CGswizzle_storage<T,STORAGE,1,SMASK>::operator=(v);
        return *this;
    }

    // CONVERSION TO SCALAR
    operator T() const {
        return (*this)[0];
    }
};

#ifdef __EDG__  // Edison Design Group C++ front-end
// suppress error ec_missing_initializer_on_fields
// ("class XXX provides no initializer for: <non-static member-list>")
// because vectors are a union of user-defined classes that are
// not allowed to have constructors
#pragma diag_suppress 366 
#endif

#if defined(__GNUC__) && (__GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=1))
// g++ 4.1 doesn't allow uninitialized const members -- need a better solution
#define __CGconst /*const*/
#else
#define __CGconst const
#endif

//// VECTOR STORAGE BASE CLASSES
template <typename T, int N, typename Ttrait = __CGtype_trait<T> >
class __CGvector_storage;

#define __CG_SWIZZLES_X(_x) \
    __CGconst __CGswizzle<T,N,2,0x0000> _x##_x; \
    __CGconst __CGswizzle<T,N,3,0x000000> _x##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x00000000> _x##_x##_x##_x // no trailing semi-colon

/// ONE-COMPONENT VECTOR STORAGE BASE CLASS
template <typename T>
class __CGvector_storage<T,1> {
protected:
    static const int N = 1;
public:
    union {
        __CGswizzle<T,N,1,0x00> x;
        __CGswizzle<T,N,1,0x00> r;
        __CGswizzle<T,N,1,0x00> s;
#ifdef __Cg_vector_xyzw_hpp__ // include <Cg/vector/xyzw.hpp> for .xyzw swizzling
        __CG_SWIZZLES_X(x);
#endif
#ifdef __Cg_vector_rgba_hpp__ // include <Cg/vector/rgba.hpp> for .rgba swizzling
        __CG_SWIZZLES_X(r);
#endif
#ifdef __Cg_vector_stpq_hpp__ // include <Cg/vector/stpq.hpp> for .stpq swizzling
        __CG_SWIZZLES_X(s);
#endif
    };
#ifndef __GNUC__
    // Visual C++ and EDG require a defined default constructor, but g++
    // balks about failure to initialize const members in union if
    // a default constructor is provided.
    __CGvector_storage() { }
#endif
    inline T & operator [] (int i) { return reinterpret_cast<T*>(this)[i]; }
    inline const T & operator [] (int i) const { return reinterpret_cast<const T*>(this)[i]; }

    template <typename T2, typename T2store>
    inline __CGvector_storage & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp; /* needed for proper vector self-assignments */
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] = tmp[i];
        return *this;
    }
    inline __CGvector_storage & operator = (const T v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v;
        return *this;
    }
};

#define __CG_SWIZZLES_XY(_x,_y) \
    __CGswizzle<T,N,2,0x0100> _x##_y; \
    __CGswizzle<T,N,2,0x0001> _y##_x; \
    __CGconst __CGswizzle<T,N,2,0x0101> _y##_y; \
    __CGconst __CGswizzle<T,N,3,0x010000> _x##_x##_y; \
    __CGconst __CGswizzle<T,N,3,0x000100> _x##_y##_x; \
    __CGconst __CGswizzle<T,N,3,0x010100> _x##_y##_y; \
    __CGconst __CGswizzle<T,N,3,0x000001> _y##_x##_x; \
    __CGconst __CGswizzle<T,N,3,0x010001> _y##_x##_y; \
    __CGconst __CGswizzle<T,N,3,0x000101> _y##_y##_x; \
    __CGconst __CGswizzle<T,N,3,0x010101> _y##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x01000000> _x##_x##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x00010000> _x##_x##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010000> _x##_x##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x00000100> _x##_y##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000100> _x##_y##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x00010100> _x##_y##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010100> _x##_y##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x00000001> _y##_x##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000001> _y##_x##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x00010001> _y##_x##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010001> _y##_x##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x00000101> _y##_y##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000101> _y##_y##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x00010101> _y##_y##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010101> _y##_y##_y##_y // no trailing semi-colon

/// TWO-COMPONENT VECTOR STORAGE BASE CLASS
template <typename T>
class __CGvector_storage<T,2> {
protected:
    static const int N = 2;
public:
    union {
        __CGswizzle<T,N,1,0x00> x;
        __CGswizzle<T,N,1,0x00> r;
        __CGswizzle<T,N,1,0x00> s;
        __CGswizzle<T,N,1,0x01> y;
        __CGswizzle<T,N,1,0x01> g;
        __CGswizzle<T,N,1,0x01> t;
#ifdef __Cg_vector_xyzw_hpp__ // include <Cg/vector/xyzw.hpp> for .xyzw swizzling
        __CG_SWIZZLES_X(x);
        __CG_SWIZZLES_XY(x,y);
#endif
#ifdef __Cg_vector_rgba_hpp__ // include <Cg/vector/rgba.hpp> for .rgba swizzling
        __CG_SWIZZLES_X(r);
        __CG_SWIZZLES_XY(r,g);
#endif
#ifdef __Cg_vector_stpq_hpp__ // include <Cg/vector/stpq.hpp> for .stpq swizzling
        __CG_SWIZZLES_X(s);
        __CG_SWIZZLES_XY(s,t);
#endif
    };
#ifndef __GNUC__
    // Visual C++ and EDG require a defined default constructor, but g++
    // balks about failure to initialize const members in union if
    // a default constructor is provided.
    __CGvector_storage() { }
#endif
    inline T & operator [] (int i) { return reinterpret_cast<T*>(this)[i]; }
    inline const T & operator [] (int i) const { return reinterpret_cast<const T*>(this)[i]; }

    template <typename T2, typename T2store>
    inline __CGvector_storage & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp; /* needed for proper vector self-assignments */
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] = tmp[i];
        return *this;
    }
    inline __CGvector_storage & operator = (const T v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v;
        return *this;
    }
};

#define __CG_SWIZZLES_XYZ(_x,_y,_z) \
    __CGswizzle<T,N,2,0x0200> _x##_z; \
    __CGswizzle<T,N,2,0x0201> _y##_z; \
    __CGswizzle<T,N,2,0x0002> _z##_x; \
    __CGswizzle<T,N,2,0x0102> _z##_y; \
    __CGconst __CGswizzle<T,N,2,0x0202> _z##_z; \
    __CGconst __CGswizzle<T,N,3,0x020000> _x##_x##_z; \
    __CGswizzle<T,N,3,0x020100> _x##_y##_z; \
    __CGconst __CGswizzle<T,N,3,0x000200> _x##_z##_x; \
    __CGswizzle<T,N,3,0x010200> _x##_z##_y; \
    __CGconst __CGswizzle<T,N,3,0x020200> _x##_z##_z; \
    __CGswizzle<T,N,3,0x020001> _y##_x##_z; \
    __CGconst __CGswizzle<T,N,3,0x020101> _y##_y##_z; \
    __CGswizzle<T,N,3,0x000201> _y##_z##_x; \
    __CGconst __CGswizzle<T,N,3,0x010201> _y##_z##_y; \
    __CGconst __CGswizzle<T,N,3,0x020201> _y##_z##_z; \
    __CGconst __CGswizzle<T,N,3,0x000002> _z##_x##_x; \
    __CGswizzle<T,N,3,0x010002> _z##_x##_y; \
    __CGconst __CGswizzle<T,N,3,0x020002> _z##_x##_z; \
    __CGswizzle<T,N,3,0x000102> _z##_y##_x; \
    __CGconst __CGswizzle<T,N,3,0x010102> _z##_y##_y; \
    __CGconst __CGswizzle<T,N,3,0x020102> _z##_y##_z; \
    __CGconst __CGswizzle<T,N,3,0x000202> _z##_z##_x; \
    __CGconst __CGswizzle<T,N,3,0x010202> _z##_z##_y; \
    __CGconst __CGswizzle<T,N,3,0x020202> _z##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x02000000> _x##_x##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x02010000> _x##_x##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020000> _x##_x##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020000> _x##_x##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020000> _x##_x##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x02000100> _x##_y##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x02010100> _x##_y##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020100> _x##_y##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020100> _x##_y##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020100> _x##_y##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x00000200> _x##_z##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000200> _x##_z##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000200> _x##_z##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x00010200> _x##_z##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010200> _x##_z##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010200> _x##_z##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020200> _x##_z##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020200> _x##_z##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020200> _x##_z##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x02000001> _y##_x##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x02010001> _y##_x##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020001> _y##_x##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020001> _y##_x##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020001> _y##_x##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x02000101> _y##_y##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x02010101> _y##_y##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020101> _y##_y##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020101> _y##_y##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020101> _y##_y##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x00000201> _y##_z##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000201> _y##_z##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000201> _y##_z##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x00010201> _y##_z##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010201> _y##_z##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010201> _y##_z##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020201> _y##_z##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020201> _y##_z##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020201> _y##_z##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x00000002> _z##_x##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000002> _z##_x##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000002> _z##_x##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x00010002> _z##_x##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010002> _z##_x##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010002> _z##_x##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020002> _z##_x##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020002> _z##_x##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020002> _z##_x##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x00000102> _z##_y##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000102> _z##_y##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000102> _z##_y##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x00010102> _z##_y##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010102> _z##_y##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010102> _z##_y##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020102> _z##_y##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020102> _z##_y##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020102> _z##_y##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x00000202> _z##_z##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000202> _z##_z##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000202> _z##_z##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x00010202> _z##_z##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010202> _z##_z##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010202> _z##_z##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x00020202> _z##_z##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020202> _z##_z##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020202> _z##_z##_z##_z // no trailing semi-colon

/// THREE-COMPONENT VECTOR STORAGE BASE CLASS
template <typename T>
class __CGvector_storage<T,3> {
protected:
    static const int N = 3;
public:
    union {
        __CGswizzle<T,N,1,0x00> x;
        __CGswizzle<T,N,1,0x00> r;
        __CGswizzle<T,N,1,0x00> s;
        __CGswizzle<T,N,1,0x01> y;
        __CGswizzle<T,N,1,0x01> g;
        __CGswizzle<T,N,1,0x01> t;
        __CGswizzle<T,N,1,0x02> z;
        __CGswizzle<T,N,1,0x02> b;
        __CGswizzle<T,N,1,0x02> p;
#ifdef __Cg_vector_xyzw_hpp__ // include <Cg/vector/xyzw.hpp> for .xyzw swizzling
        __CG_SWIZZLES_X(x);
        __CG_SWIZZLES_XY(x,y);
        __CG_SWIZZLES_XYZ(x,y,z);
#endif
#ifdef __Cg_vector_rgba_hpp__ // include <Cg/vector/rgba.hpp> for .rgba swizzling
        __CG_SWIZZLES_X(r);
        __CG_SWIZZLES_XY(r,g);
        __CG_SWIZZLES_XYZ(r,g,b);
#endif
#ifdef __Cg_vector_stpq_hpp__ // include <Cg/vector/stpq.hpp> for .stpq swizzling
        __CG_SWIZZLES_X(s);
        __CG_SWIZZLES_XY(s,t);
        __CG_SWIZZLES_XYZ(s,t,p);
#endif
    };
#ifndef __GNUC__
    // Visual C++ and EDG require a defined default constructor, but g++
    // balks about failure to initialize const members in union if
    // a default constructor is provided.
    __CGvector_storage() { }
#endif
    inline T & operator [] (int i) { return reinterpret_cast<T*>(this)[i]; }
    inline const T & operator [] (int i) const { return reinterpret_cast<const T*>(this)[i]; }

    template <typename T2, typename T2store>
    inline __CGvector_storage & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp; /* needed for proper vector self-assignments */
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] = tmp[i];
        return *this;
    }
    inline __CGvector_storage & operator = (const T v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v;
        return *this;
    }
};

#define __CG_SWIZZLES_XYZW(_x,_y,_z,_w) \
    __CGswizzle<T,N,2,0x0300> _x##_w; \
    __CGswizzle<T,N,2,0x0301> _y##_w; \
    __CGswizzle<T,N,2,0x0302> _z##_w; \
    __CGswizzle<T,N,2,0x0003> _w##_x; \
    __CGswizzle<T,N,2,0x0103> _w##_y; \
    __CGswizzle<T,N,2,0x0203> _w##_z; \
    __CGconst __CGswizzle<T,N,2,0x0303> _w##_w; \
    __CGconst __CGswizzle<T,N,3,0x030000> _x##_x##_w; \
    __CGswizzle<T,N,3,0x030100> _x##_y##_w; \
    __CGswizzle<T,N,3,0x030200> _x##_z##_w; \
    __CGconst __CGswizzle<T,N,3,0x000300> _x##_w##_x; \
    __CGswizzle<T,N,3,0x010300> _x##_w##_y; \
    __CGswizzle<T,N,3,0x020300> _x##_w##_z; \
    __CGconst __CGswizzle<T,N,3,0x030300> _x##_w##_w; \
    __CGswizzle<T,N,3,0x030001> _y##_x##_w; \
    __CGconst __CGswizzle<T,N,3,0x030101> _y##_y##_w; \
    __CGswizzle<T,N,3,0x030201> _y##_z##_w; \
    __CGswizzle<T,N,3,0x000301> _y##_w##_x; \
    __CGconst __CGswizzle<T,N,3,0x010301> _y##_w##_y; \
    __CGswizzle<T,N,3,0x020301> _y##_w##_z; \
    __CGconst __CGswizzle<T,N,3,0x030301> _y##_w##_w; \
    __CGswizzle<T,N,3,0x030002> _z##_x##_w; \
    __CGswizzle<T,N,3,0x030102> _z##_y##_w; \
    __CGconst __CGswizzle<T,N,3,0x030202> _z##_z##_w; \
    __CGswizzle<T,N,3,0x000302> _z##_w##_x; \
    __CGswizzle<T,N,3,0x010302> _z##_w##_y; \
    __CGconst __CGswizzle<T,N,3,0x020302> _z##_w##_z; \
    __CGconst __CGswizzle<T,N,3,0x030302> _z##_w##_w; \
    __CGconst __CGswizzle<T,N,3,0x000003> _w##_x##_x; \
    __CGswizzle<T,N,3,0x010003> _w##_x##_y; \
    __CGswizzle<T,N,3,0x020003> _w##_x##_z; \
    __CGconst __CGswizzle<T,N,3,0x030003> _w##_x##_w; \
    __CGswizzle<T,N,3,0x000103> _w##_y##_x; \
    __CGconst __CGswizzle<T,N,3,0x010103> _w##_y##_y; \
    __CGswizzle<T,N,3,0x020103> _w##_y##_z; \
    __CGconst __CGswizzle<T,N,3,0x030103> _w##_y##_w; \
    __CGswizzle<T,N,3,0x000203> _w##_z##_x; \
    __CGswizzle<T,N,3,0x010203> _w##_z##_y; \
    __CGconst __CGswizzle<T,N,3,0x020203> _w##_z##_z; \
    __CGconst __CGswizzle<T,N,3,0x030203> _w##_z##_w; \
    __CGconst __CGswizzle<T,N,3,0x000303> _w##_w##_x; \
    __CGconst __CGswizzle<T,N,3,0x010303> _w##_w##_y; \
    __CGconst __CGswizzle<T,N,3,0x020303> _w##_w##_z; \
    __CGconst __CGswizzle<T,N,3,0x030303> _w##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000000> _x##_x##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010000> _x##_x##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020000> _x##_x##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030000> _x##_x##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030000> _x##_x##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030000> _x##_x##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030000> _x##_x##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000100> _x##_y##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010100> _x##_y##_y##_w; \
    __CGswizzle<T,N,4,0x03020100> _x##_y##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030100> _x##_y##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030100> _x##_y##_w##_y; \
    __CGswizzle<T,N,4,0x02030100> _x##_y##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030100> _x##_y##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000200> _x##_z##_x##_w; \
    __CGswizzle<T,N,4,0x03010200> _x##_z##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020200> _x##_z##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030200> _x##_z##_w##_x; \
    __CGswizzle<T,N,4,0x01030200> _x##_z##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030200> _x##_z##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030200> _x##_z##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000300> _x##_w##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000300> _x##_w##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000300> _x##_w##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000300> _x##_w##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x00010300> _x##_w##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010300> _x##_w##_y##_y; \
    __CGswizzle<T,N,4,0x02010300> _x##_w##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010300> _x##_w##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x00020300> _x##_w##_z##_x; \
    __CGswizzle<T,N,4,0x01020300> _x##_w##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020300> _x##_w##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020300> _x##_w##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030300> _x##_w##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030300> _x##_w##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030300> _x##_w##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030300> _x##_w##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000001> _y##_x##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010001> _y##_x##_y##_w; \
    __CGswizzle<T,N,4,0x03020001> _y##_x##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030001> _y##_x##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030001> _y##_x##_w##_y; \
    __CGswizzle<T,N,4,0x02030001> _y##_x##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030001> _y##_x##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000101> _y##_y##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010101> _y##_y##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020101> _y##_y##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030101> _y##_y##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030101> _y##_y##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030101> _y##_y##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030101> _y##_y##_w##_w; \
    __CGswizzle<T,N,4,0x03000201> _y##_z##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010201> _y##_z##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020201> _y##_z##_z##_w; \
    __CGswizzle<T,N,4,0x00030201> _y##_z##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030201> _y##_z##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030201> _y##_z##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030201> _y##_z##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000301> _y##_w##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000301> _y##_w##_x##_y; \
    __CGswizzle<T,N,4,0x02000301> _y##_w##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000301> _y##_w##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x00010301> _y##_w##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010301> _y##_w##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010301> _y##_w##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010301> _y##_w##_y##_w; \
    __CGswizzle<T,N,4,0x00020301> _y##_w##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020301> _y##_w##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020301> _y##_w##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020301> _y##_w##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030301> _y##_w##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030301> _y##_w##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030301> _y##_w##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030301> _y##_w##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000002> _z##_x##_x##_w; \
    __CGswizzle<T,N,4,0x03010002> _z##_x##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020002> _z##_x##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030002> _z##_x##_w##_x; \
    __CGswizzle<T,N,4,0x01030002> _z##_x##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030002> _z##_x##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030002> _z##_x##_w##_w; \
    __CGswizzle<T,N,4,0x03000102> _z##_y##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010102> _z##_y##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020102> _z##_y##_z##_w; \
    __CGswizzle<T,N,4,0x00030102> _z##_y##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030102> _z##_y##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030102> _z##_y##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030102> _z##_y##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x03000202> _z##_z##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x03010202> _z##_z##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x03020202> _z##_z##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030202> _z##_z##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030202> _z##_z##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030202> _z##_z##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030202> _z##_z##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000302> _z##_w##_x##_x; \
    __CGswizzle<T,N,4,0x01000302> _z##_w##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000302> _z##_w##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000302> _z##_w##_x##_w; \
    __CGswizzle<T,N,4,0x00010302> _z##_w##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010302> _z##_w##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010302> _z##_w##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010302> _z##_w##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x00020302> _z##_w##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020302> _z##_w##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020302> _z##_w##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020302> _z##_w##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030302> _z##_w##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030302> _z##_w##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030302> _z##_w##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030302> _z##_w##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000003> _w##_x##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000003> _w##_x##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000003> _w##_x##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000003> _w##_x##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x00010003> _w##_x##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010003> _w##_x##_y##_y; \
    __CGswizzle<T,N,4,0x02010003> _w##_x##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010003> _w##_x##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x00020003> _w##_x##_z##_x; \
    __CGswizzle<T,N,4,0x01020003> _w##_x##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020003> _w##_x##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020003> _w##_x##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030003> _w##_x##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030003> _w##_x##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030003> _w##_x##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030003> _w##_x##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000103> _w##_y##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000103> _w##_y##_x##_y; \
    __CGswizzle<T,N,4,0x02000103> _w##_y##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000103> _w##_y##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x00010103> _w##_y##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010103> _w##_y##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010103> _w##_y##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010103> _w##_y##_y##_w; \
    __CGswizzle<T,N,4,0x00020103> _w##_y##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020103> _w##_y##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020103> _w##_y##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020103> _w##_y##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030103> _w##_y##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030103> _w##_y##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030103> _w##_y##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030103> _w##_y##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000203> _w##_z##_x##_x; \
    __CGswizzle<T,N,4,0x01000203> _w##_z##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000203> _w##_z##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000203> _w##_z##_x##_w; \
    __CGswizzle<T,N,4,0x00010203> _w##_z##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010203> _w##_z##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010203> _w##_z##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010203> _w##_z##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x00020203> _w##_z##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020203> _w##_z##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020203> _w##_z##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020203> _w##_z##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030203> _w##_z##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030203> _w##_z##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030203> _w##_z##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030203> _w##_z##_w##_w; \
    __CGconst __CGswizzle<T,N,4,0x00000303> _w##_w##_x##_x; \
    __CGconst __CGswizzle<T,N,4,0x01000303> _w##_w##_x##_y; \
    __CGconst __CGswizzle<T,N,4,0x02000303> _w##_w##_x##_z; \
    __CGconst __CGswizzle<T,N,4,0x03000303> _w##_w##_x##_w; \
    __CGconst __CGswizzle<T,N,4,0x00010303> _w##_w##_y##_x; \
    __CGconst __CGswizzle<T,N,4,0x01010303> _w##_w##_y##_y; \
    __CGconst __CGswizzle<T,N,4,0x02010303> _w##_w##_y##_z; \
    __CGconst __CGswizzle<T,N,4,0x03010303> _w##_w##_y##_w; \
    __CGconst __CGswizzle<T,N,4,0x00020303> _w##_w##_z##_x; \
    __CGconst __CGswizzle<T,N,4,0x01020303> _w##_w##_z##_y; \
    __CGconst __CGswizzle<T,N,4,0x02020303> _w##_w##_z##_z; \
    __CGconst __CGswizzle<T,N,4,0x03020303> _w##_w##_z##_w; \
    __CGconst __CGswizzle<T,N,4,0x00030303> _w##_w##_w##_x; \
    __CGconst __CGswizzle<T,N,4,0x01030303> _w##_w##_w##_y; \
    __CGconst __CGswizzle<T,N,4,0x02030303> _w##_w##_w##_z; \
    __CGconst __CGswizzle<T,N,4,0x03030303> _w##_w##_w##_w // no trailing semi-colon

/// FOUR-COMPONENT VECTOR STORAGE BASE CLASS
template <typename T>
class __CGvector_storage<T,4> {
protected:
    static const int N = 4;
public:
    union {
        __CGswizzle<T,N,1,0x00> x;
        __CGswizzle<T,N,1,0x00> r;
        __CGswizzle<T,N,1,0x00> s;
        __CGswizzle<T,N,1,0x01> y;
        __CGswizzle<T,N,1,0x01> g;
        __CGswizzle<T,N,1,0x01> t;
        __CGswizzle<T,N,1,0x02> z;
        __CGswizzle<T,N,1,0x02> b;
        __CGswizzle<T,N,1,0x02> p;
        __CGswizzle<T,N,1,0x03> w;
        __CGswizzle<T,N,1,0x03> a;
        __CGswizzle<T,N,1,0x03> q;
#ifdef __Cg_vector_xyzw_hpp__ // include <Cg/vector/xyzw.hpp> for .xyzw swizzling
        __CG_SWIZZLES_X(x);
        __CG_SWIZZLES_XY(x,y);
        __CG_SWIZZLES_XYZ(x,y,z);
        __CG_SWIZZLES_XYZW(x,y,z,w);
#endif
#ifdef __Cg_vector_rgba_hpp__ // include <Cg/vector/rgba.hpp> for .rgba swizzling
        __CG_SWIZZLES_X(r);
        __CG_SWIZZLES_XY(r,g);
        __CG_SWIZZLES_XYZ(r,g,b);
        __CG_SWIZZLES_XYZW(r,g,b,a);
#endif
#ifdef __Cg_vector_stpq_hpp__ // include <Cg/vector/stpq.hpp> for .stpq swizzling
        __CG_SWIZZLES_X(s);
        __CG_SWIZZLES_XY(s,t);
        __CG_SWIZZLES_XYZ(s,t,p);
        __CG_SWIZZLES_XYZW(s,t,p,q);
#endif
    };
#ifndef __GNUC__
    // Visual C++ and EDG require a defined default constructor, but g++
    // balks about failure to initialize const members in union if
    // a default constructor is provided.
    __CGvector_storage() { }
#endif
    inline T & operator [] (int i) { return reinterpret_cast<T*>(this)[i]; }
    inline const T & operator [] (int i) const { return reinterpret_cast<const T*>(this)[i]; }

    template <typename T2, typename T2store>
    inline __CGvector_storage & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector<T,N> tmp; /* needed for proper vector self-assignments */
        for (int i=0; i<N; i++)
            tmp[i] = ind[i];
        for (int i=0; i<N; i++)
            (*this)[i] = tmp[i];
        return *this;
    }
    inline __CGvector_storage & operator = (const T v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v;
        return *this;
    }
};

//// PLURAL VERSION OF VECTOR BASE CLASS
template <typename T, int N>
class __CGvector_plural
    : public __CGvector_plural_usage<T,N,__CGvector_storage<T,N> >
    __CG_DERIVE_FROM_BASE_CLASS
{
public:
    inline __CGvector_plural() { }
    inline __CGvector_plural(const T & s) {
        for (int i=0; i<N; i++)
            (*this)[i] = s;
    }
    template <typename T2store>
    inline __CGvector_plural(__CGvector_plural_usage<T,N,T2store> v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v[i];
    }
    // EXPLICIT CONSTRUCTOR FOR TYPE CHANGES FOR VECTORS
    template <typename T2,typename T2store>
    explicit inline __CGvector_plural(__CGvector_usage<T2,N,T2store> v) {
        for (int i=0; i<N; i++)
            (*this)[i] = T(v[i]); /* precision mismatch warning possible */
    }
    // scalar-to-vector smear constructor
    template <typename T2store>
    inline __CGvector_plural(__CGvector_usage<T,1,T2store> v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v[0];
    }
    template <typename T2,typename T2store>
    explicit inline __CGvector_plural(__CGvector_usage<T2,1,T2store> v) {
        for (int i=0; i<N; i++)
            (*this)[i] = v[0];
    }
};

/// ONE-COMPONENT TEMPLATED VECTOR TYPE
template <typename T>
class __CGvector<T,1> :
    public __CGvector_usage<T,1,__CGvector_storage<T,1> >
    __CG_DERIVE_FROM_BASE_CLASS
{
private:
    static const int N = 1;
public:
    // CONSTRUCTORS
    inline __CGvector() { }  // Needed!
    inline __CGvector(const T & s) {
        (*this)[0] = s;
    }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,1,T2store> v) {
        (*this)[0] = v[0]; /* precision mismatch warning possible */
    }
    // EXPLICIT CONSTRUCTOR FOR TYPE CHANGES FOR VECTORS
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,1,T2store> v) {
        (*this)[0] = T(v[0]); /* precision mismatch warning possible */
    }

    // ASSIGNMENT OPERATORS
    template <typename T2, typename T2store>
    inline __CGvector & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector_storage<T,N>::operator=(ind);
        return *this;
    }
    inline __CGvector & operator = (const T v) {
        __CGvector_storage<T,N>::operator=(v);
        return *this;
    }

    // CONVERSION TO SCALAR
    operator T() const {
        return (*this)[0];
    }
} __CGmay_alias;

/// TWO-COMPONENT TEMPLATED VECTOR TYPE
template <typename T>
class __CGvector<T,2> : public __CGvector_plural<T,2> {
private:
    static const int N = 2;
public:
    // CONSTRUCTORS
    inline __CGvector() { }  // Needed!
    template <typename TA, typename TB>
    inline __CGvector(const TA & s0, const TB & s1) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s1);
    }
    inline __CGvector(const T & v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // EXPLICIT CONSTRUCTORS FOR TYPE CONVERSIONS BETWEEN VECTORS
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // ASSIGNMENT OPERATORS
    template <typename T2, typename T2store>
    inline __CGvector & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector_storage<T,N>::operator=(ind);
        return *this;
    }
	inline __CGvector & operator = (const __CGvector& v) {
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		return *this;
	}
	inline __CGvector & operator = (const T& v) {
		__CGvector_storage<T, N>::operator=(v);
		return *this;
	}
} __CGmay_alias;

/// THREE-COMPONENT TEMPLATED VECTOR TYPE
template <typename T>
class __CGvector<T,3> : public __CGvector_plural<T,3> {
private:
    static const int N = 3;
public:
    // CONSTRUCTORS
    inline __CGvector() { }
    template <typename TA, typename TB, typename TC>
    inline __CGvector(const TA & s0, const TB & s1, const TC & s2) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s1);
        (*this)[2] = T(s2);
    }
    template <typename TA, typename TAstore,
              typename TB>
    inline __CGvector(const __CGvector_usage<TA,2,TAstore> &s01, const TB &s2) {
        (*this)[0] = T(s01[0]);
        (*this)[1] = T(s01[1]);
        (*this)[2] = T(s2);
    }
    template <typename TA,
              typename TB, typename TBstore>
    inline __CGvector(const TA &s0, const __CGvector_usage<TB,2,TBstore> &s12) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s12[0]);
        (*this)[2] = T(s12[1]);
    }
    inline __CGvector(const T & v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // EXPLICIT CONSTRUCTORS FOR TYPE CONVERSIONS BETWEEN VECTORS
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // ASSIGNMENT OPERATORS
    template <typename T2, typename T2store>
    inline __CGvector & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector_storage<T,N>::operator=(ind);
        return *this;
    }
	inline __CGvector & operator = (const __CGvector& v) {
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		(*this)[2] = v[2];
		return *this;
	}
	inline __CGvector & operator = (const T v) {
        __CGvector_storage<T,N>::operator=(v);
        return *this;
    }
} __CGmay_alias;

/// FOUR-COMPONENT TEMPLATED VECTOR TYPE
template <typename T>
class __CGvector<T,4> : public __CGvector_plural<T,4> {
private:
    static const int N = 4;
public:
    // CONSTRUCTORS
    inline __CGvector() { }
    template <typename TA, typename TB, typename TC, typename TD>
    inline __CGvector(const TA & s0, const TB & s1, const TC & s2, const TD & s3) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s1);
        (*this)[2] = T(s2);
        (*this)[3] = T(s3);
    }
    template <typename TA, typename TAstore,
              typename TB,
              typename TC>
    inline __CGvector(const __CGvector_usage<TA,2,TAstore> &s01, const TB &s2, const TC &s3) {
        (*this)[0] = T(s01[0]);
        (*this)[1] = T(s01[1]);
        (*this)[2] = T(s2);
        (*this)[3] = T(s3);
    }
    template <typename TA,
              typename TB, typename TBstore,
              typename TC>
    inline __CGvector(const TA &s0, const __CGvector_usage<TB,2,TBstore> &s12, const TC &s3) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s12[0]);
        (*this)[2] = T(s12[1]);
        (*this)[3] = T(s3);
    }
    template <typename TA,
              typename TB,
              typename TC, typename TCstore>
    inline __CGvector(const TA &s0, const TB &s1, const __CGvector_usage<TC,2,TCstore> &s23) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s1);
        (*this)[2] = T(s23[0]);
        (*this)[3] = T(s23[1]);
    }
    template <typename TA, typename TAstore,
              typename TB, typename TBstore>
    inline __CGvector(const __CGvector_usage<TA,2,TAstore> &s01, const __CGvector_usage<TB,2,TBstore> &s23) {
        (*this)[0] = T(s01[0]);
        (*this)[1] = T(s01[1]);
        (*this)[2] = T(s23[0]);
        (*this)[3] = T(s23[1]);
    }
    template <typename TA, typename TAstore,
              typename TB>
    inline __CGvector(const __CGvector_usage<TA,3,TAstore> &s012, const TB &s3) {
        (*this)[0] = T(s012[0]);
        (*this)[1] = T(s012[1]);
        (*this)[2] = T(s012[2]);
        (*this)[3] = T(s3);
    }
    template <typename TA,
              typename TB, typename TBstore>
    inline __CGvector(const TA &s0, const __CGvector_usage<TB,3,TBstore> &s123) {
        (*this)[0] = T(s0);
        (*this)[1] = T(s123[0]);
        (*this)[2] = T(s123[1]);
        (*this)[3] = T(s123[2]);
    }
    inline __CGvector(const T & v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2store>
    inline __CGvector(__CGvector_usage<T,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // EXPLICIT CONSTRUCTORS FOR TYPE CONVERSIONS BETWEEN VECTORS
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,N,T2store> v) : __CGvector_plural<T,N>(v) { }
    template <typename T2, typename T2store>
    explicit inline __CGvector(__CGvector_usage<T2,1,T2store> v) : __CGvector_plural<T,N>(v) { }

    // ASSIGNMENT OPERATORS
    template <typename T2, typename T2store>
    inline __CGvector & operator = (const __CGvector_usage<T2,N,T2store> & ind) {
        __CGvector_storage<T,N>::operator=(ind);
        return *this;
    }
	inline __CGvector & operator = (const __CGvector& v) {
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		(*this)[2] = v[2];
		(*this)[3] = v[3];
		return *this;
	}
	inline __CGvector & operator = (const T v) {
        __CGvector_storage<T,N>::operator=(v);
        return *this;
    }
} __CGmay_alias;

//// UNARY VECTOR OPERATORS

// UNARY LOGICAL NEGATE
template <typename T, int N, typename Tstore>
inline __CGvector<bool,N> operator ! (const __CGvector_usage<T,N,Tstore> & a)
{
    __CGvector<bool,N> r;
    for (int i=0; i<N; i++)
        r[i] = !a[i];
    return r;
}
// UNARY BITWISE NEGATE
template <typename T, int N, typename Tstore>
inline __CGvector<T,N> operator ~ (const __CGvector_usage<T,N,Tstore> & a)
{
    __CGvector<T,N> r;
    for (int i=0; i<N; i++)
        r[i] = ~a[i];
    return r;
}
// UNARY SIGN NEGATE
template <typename T, int N, typename Tstore>
inline __CGvector<T,N> operator - (const __CGvector_usage<T,N,Tstore> & a)
{
    __CGvector<T,N> r;
    for (int i=0; i<N; i++)
        r[i] = -a[i];
    return r;
}

//// BINARY VECTOR OPERATORS
// Five possible combinations:
// 1) vector_usage OP vector_usage
// 2) vector_usage OP scalar type
// 3) scalar type OP vector_usage
// 4) plural_vector_usage OP vector_usage<1>
// 5) vector_usage<1> OP plural_vector_usage

// BINARY ADDITION (+)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator + (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) + ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator + (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) + ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator + (const T1 & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++) {
        r[i] = ResultType(static_cast<T1>(a)) + ResultType(b[i]);
    }
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator + (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) + ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator + (const __CGvector_usage<T1,1,T1store> & a,
                                                                             const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) + ResultType(b[i]);
    return r;
}

// BINARY SUBTRACTION (-)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator - (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) - ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator - (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) - ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator - (const T1 & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) - ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator - (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) - ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator - (const __CGvector_usage<T1,1,T1store> & a,
                                                                             const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) - ResultType(b[i]);
    return r;
}

// BINARY MULTIPLICATION (*)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator * (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) * ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator * (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) * ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator * (const T1 & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) * ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator * (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) * ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator * (const __CGvector_usage<T1,1,T1store> & a,
                                                                             const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) * ResultType(b[i]);
    return r;
}

// BINARY DIVISION (/)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator / (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) / ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator / (const __CGvector_usage<T1,N,T1store> & a,
                                                                             const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) / ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator / (const T1 & a,
                                                                             const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) / ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator / (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                             const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) / ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::numericType,N> operator / (const __CGvector_usage<T1,1,T1store> & a,
                                                                             const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::numericType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) / ResultType(b[i]);
    return r;
}

// BINARY MODULO (%)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator % (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) % ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator % (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) % ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator % (const T1 & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) % ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator % (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) % ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator % (const __CGvector_usage<T1,1,T1store> & a,
                                                                         const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) % ResultType(b[i]);
    return r;
}

// BINARY XOR (^)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator ^ (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) ^ ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator ^ (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) ^ ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator ^ (const T1 & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) ^ ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator ^ (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) ^ ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator ^ (const __CGvector_usage<T1,1,T1store> & a,
                                                                         const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) ^ ResultType(b[i]);
    return r;
}

// BINARY BITWISE AND (&)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator & (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) & ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator & (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) & ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator & (const T1 & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) & ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator & (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) & ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator & (const __CGvector_usage<T1,1,T1store> & a,
                                                                         const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) & ResultType(b[i]);
    return r;
}

// BINARY BITWISE OR (|)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator | (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) | ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator | (const __CGvector_usage<T1,N,T1store> & a,
                                                                         const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) | ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator | (const T1 & a,
                                                                         const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) | ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator | (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                         const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) | ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator | (const __CGvector_usage<T1,1,T1store> & a,
                                                                         const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) | ResultType(b[i]);
    return r;
}


// BINARY SHIFT LEFT (<<)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator << (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) << ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator << (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) << ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator << (const T1 & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) << ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator << (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) << ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator << (const __CGvector_usage<T1,1,T1store> & a,
                                                                          const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) << ResultType(b[i]);
    return r;
}

// BINARY SHIFT RIGHT (>>)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator >> (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >> ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator >> (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >> ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator >> (const T1 & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) >> ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator >> (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >> ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::intType,N> operator >> (const __CGvector_usage<T1,1,T1store> & a,
                                                                          const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::intType ResultType;
    __CGvector<ResultType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) >> ResultType(b[i]);
    return r;
}

// BINARY LESS THAN (<)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator < (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) < ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator < (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) < ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator < (const T1 & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) < ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator < (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) < ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator < (const __CGvector_usage<T1,1,T1store> & a,
                                                                          const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) < ResultType(b[i]);
    return r;
}

// BINARY GREATER THAN (>)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator > (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) > ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator > (const __CGvector_usage<T1,N,T1store> & a,
                                                                          const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) > ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator > (const T1 & a,
                                                                          const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) > ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator > (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                          const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) > ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator > (const __CGvector_usage<T1,1,T1store> & a,
                                                                          const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) > ResultType(b[i]);
    return r;
}

// BINARY LESS THAN OR EQUAL (<=)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator <= (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) <= ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator <= (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) <= ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator <= (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) <= ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator <= (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) <= ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator <= (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) <= ResultType(b[i]);
    return r;
}

// BINARY GREATER THAN OR EQUAL (>=)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator >= (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >= ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator >= (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >= ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator >= (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) >= ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator >= (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) >= ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator >= (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) >= ResultType(b[i]);
    return r;
}

// BINARY EQUALITY (==)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator == (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) == ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator == (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) == ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator == (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) == ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator == (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) == ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator == (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) == ResultType(b[i]);
    return r;
}

// BINARY INEQUALITY (!=)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator != (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) != ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator != (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) != ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator != (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) != ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator != (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) != ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator != (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) != ResultType(b[i]);
    return r;
}

// BINARY LOGICAL AND (&&)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator && (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) && ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator && (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) && ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator && (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) && ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator && (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) && ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator && (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) && ResultType(b[i]);
    return r;
}

// BINARY LOGICAL OR (||)
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator || (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) || ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator || (const __CGvector_usage<T1,N,T1store> & a,
                                                                           const T2 & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) || ResultType(static_cast<T2>(b));
    return r;
}
template <typename T1, typename T2, int N, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator || (const T1 & a,
                                                                           const __CGvector_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(static_cast<T1>(a)) || ResultType(b[i]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator || (const __CGvector_plural_usage<T1,N,T1store> & a,
                                                                           const __CGvector_usage<T2,1,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[i]) || ResultType(b[0]);
    return r;
}
template <typename T1, typename T2, int N, typename T1store, typename T2store> 
inline __CGvector<typename __CGtype_trait<T1,T2>::boolType,N> operator || (const __CGvector_usage<T1,1,T1store> & a,
                                                                           const __CGvector_plural_usage<T2,N,T2store> & b)
{
    typedef typename __CGtype_trait<T1,T2>::resultType ResultType;
    typedef typename __CGtype_trait<T1,T2>::boolType BoolType;
    __CGvector<BoolType,N> r;
    for (int i=0; i<N; i++)
        r[i] = ResultType(a[0]) || ResultType(b[i]);
    return r;
}

// Undefine helper #defines
#undef __CG_SWIZZLES_X
#undef __CG_SWIZZLES_XY
#undef __CG_SWIZZLES_XYZ
#undef __CG_SWIZZLES_XYZW
#undef __CGconst
#undef __CGmay_alias

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(pop)
#endif

//// TYPE DEFINTIONS FOR CG-NAMED VECTOR TYPES
typedef __CGvector<float,1> float1;
typedef __CGvector<float,2> float2;
typedef __CGvector<float,3> float3;
typedef __CGvector<float,4> float4;
typedef __CGvector<int,1> int1;
typedef __CGvector<int,2> int2;
typedef __CGvector<int,3> int3;
typedef __CGvector<int,4> int4;
typedef __CGvector<bool,1> bool1;
typedef __CGvector<bool,2> bool2;
typedef __CGvector<bool,3> bool3;
typedef __CGvector<bool,4> bool4;
#ifdef __Cg_double_hpp__
typedef __CGvector<double,1> double1;
typedef __CGvector<double,2> double2;
typedef __CGvector<double,3> double3;
typedef __CGvector<double,4> double4;
#endif
#ifdef __Cg_half_hpp__
typedef __CGvector<half,1> half1;
typedef __CGvector<half,2> half2;
typedef __CGvector<half,3> half3;
typedef __CGvector<half,4> half4;
#endif
#ifdef __Cg_fixed_hpp__
typedef __CGvector<fixed,1> fixed1;
typedef __CGvector<fixed,2> fixed2;
typedef __CGvector<fixed,3> fixed3;
typedef __CGvector<fixed,4> fixed4;
#endif

} // namespace Cg

#endif // __Cg_vector_hpp__
