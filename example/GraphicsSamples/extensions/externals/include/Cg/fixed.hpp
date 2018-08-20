/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_fixed_hpp__
#define __Cg_fixed_hpp__

#include <Cg/assert.hpp> // for __CGassert

#include <cmath> // for std::floor

namespace Cg {

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(push)
#pragma warning(disable:4127)  // conditional expression is constant
#endif

#if defined(__GNUC__) && (__GNUC__>3 || (__GNUC__==3 && __GNUC_MINOR__>=3))
#define __CGmay_alias __attribute__((__may_alias__))
#else
#define __CGmay_alias 
#endif

/* __CGcustom_fixed_storage is the pure storage required for a cgCustomFloat.
   Importantly, it has no constructor, destructor, etc. so it can be
   kept within a union, unlike __CGcustom_fixed that has a constructor, etc. */

// intBits is number of integer bits
// fracBits is number of fractional bits
// sgnBit is whether or not a sign bit is provided
template <unsigned int intBits, unsigned int fracBits, bool sgnBit>
struct __CGcustom_fixed_storage {
    static const unsigned int bits          = (intBits+fracBits+sgnBit);  // 16 bits for s1.10
    static const unsigned int bytesPerFixed = (bits + 7) / 8;             // 2 bytes for s1.10

    unsigned char v[bytesPerFixed];
} __CGmay_alias;

template <unsigned int intBits, unsigned int fracBits, bool sgnBit>
struct __CGcustom_fixed : private __CGcustom_fixed_storage<intBits,fracBits,sgnBit> {

private:
    static const unsigned int fp32ManBits = 23u;                    // 23 mantissa bits for s23e8
    static const unsigned int fp32ManMask = ((1u<<fp32ManBits)-1);  // 0x007FFFFF for s23e8

    static const unsigned int intBitsPow2 = 1u<<intBits;                         // 2 for s1.10
    static const unsigned int fracBitsPow2 = 1u<<fracBits;                       // 1024 for s1.10
    static const unsigned int fracInt = fracBitsPow2-1;                          // 1023 for s1.10
    static const unsigned int fixedScale = fracInt+sgnBit;                       // 1024 for s1.10
    static const unsigned int bits = __CGcustom_fixed_storage<intBits,fracBits,sgnBit>::bits;
    static const unsigned int fixedShift = 8*sizeof(int) - bits;                 // 20 for s1.10
    static const          int minFixed = sgnBit ? -(1<<(intBits+fracBits)) : 0; // -2048 for s1.10
    static const          int maxFixed = (1<<(intBits+fracBits)) - 1;           // 2047 for s1.10

    static int floatToCustomFixed(float f);
    float customFixedToFloat() const;

public:
    __CGcustom_fixed() {}
    // FLOAT-TO-FIXED CONSTRUCTOR
    __CGcustom_fixed(float val) {
        int fx = floatToCustomFixed(val);

        for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
            __CGcustom_fixed::v[i] = fx & 0xFF;
            fx >>= 8;
        }
    }
    // DOUBLE-TO-FIXED CONSTRUCTOR
    __CGcustom_fixed(double val) {
        int fx = floatToCustomFixed(float(val));

        for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
            __CGcustom_fixed::v[i] = fx & 0xFF;
            fx >>= 8;
        }
    }
    // INT-TO-FIXED CONSTRUCTOR
    // (should this be marked explicit?) requires fixed3(fixed(0),fixed(4),fixed(5))
    __CGcustom_fixed(int val) {
        int fx = floatToCustomFixed(float(val));

        for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
            __CGcustom_fixed::v[i] = fx & 0xFF;
            fx >>= 8;
        }
    }

    inline __CGcustom_fixed(const __CGcustom_fixed &rhs)
    {
        for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
            __CGcustom_fixed::v[i] = rhs.v[i];
        }
    }

    inline operator float()
    {
        float f = customFixedToFloat();
        return f;
    }

    inline operator float() const
    {
        float f = customFixedToFloat();
        return f;
    }

    __CGcustom_fixed & operator = (const __CGcustom_fixed & val) {
        for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
            __CGcustom_fixed::v[i] = val.v[i];
        }
        return *this;
    }

    template <typename T>
    inline __CGcustom_fixed & operator += (const T & val) {
        float floatVersion = *this;
        floatVersion += val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_fixed & operator -= (const T & val) {
        float floatVersion = *this;
        floatVersion -= val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_fixed & operator *= (const T & val) {
        float floatVersion = *this;
        floatVersion *= val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_fixed & operator /= (const T & val) {
        float floatVersion = *this;
        floatVersion /= val;
        *this = floatVersion;
        return *this;
    }

    inline __CGcustom_fixed & operator ++ () {
        float floatVersion = *this;
        ++floatVersion;
        *this = floatVersion;
        return *this;
    }
    inline __CGcustom_fixed operator ++ (int) {
        __CGcustom_fixed oldValue = *this;
        float floatVersion = *this;
        ++floatVersion;
        *this = floatVersion;
        return oldValue;
    }
    inline __CGcustom_fixed & operator -- () {
        float floatVersion = *this;
        --floatVersion;
        *this = floatVersion;
        return *this;
    }
    inline __CGcustom_fixed operator -- (int) {
        __CGcustom_fixed oldValue = *this;
        float floatVersion = *this;
        --floatVersion;
        *this = floatVersion;
        return oldValue;
    }
};

template <unsigned int intBits, unsigned int fracBits, bool sgnBit>
float __CGcustom_fixed<intBits,fracBits,sgnBit>::customFixedToFloat() const
{
    static const float oneOverFixedScale = 1.0f / fixedScale;

    int fx = 0;
    for (int i=0; i<__CGcustom_fixed::bytesPerFixed; i++) {
        fx |= __CGcustom_fixed::v[i] << i*8;
    }

    if (sgnBit) {
        // Make sure fixed MSB is replicated for all MSBs of int above the fixed MSB
        fx <<= fixedShift;
        fx >>= fixedShift;
    }

    float f = fx * oneOverFixedScale;
    return f;
}

template <unsigned int intBits, unsigned int fracBits, bool sgnBit>
int __CGcustom_fixed<intBits,fracBits,sgnBit>::floatToCustomFixed(float f)
{
    float fs = std::floor(f * fixedScale);

    if (fs >= maxFixed) {
        return maxFixed;
    } else if (fs > minFixed) {
        // Clever code that performs proper rounding but assumes IEEE 754 floating-point.
        const float bigBias = float(3<<(fp32ManBits-1));
        float fsbias = fs + bigBias;
        int *ip = reinterpret_cast<int*>(&fsbias);
        int i = *ip;

        const int lessBigBias = 1<<(fp32ManBits-1);
        int fx = (i & fp32ManMask) - lessBigBias;
        __CGassert(fx >= minFixed);
        __CGassert(fx <= maxFixed);
        return fx;
    } else if (fs <= minFixed) {
        return minFixed;
    } else {
        // NaN maps to zero.
        return 0;
    }
}

// Undefine helper #defines
#undef __CGmay_alias

typedef __CGcustom_fixed<1,10,1> __CGcustom_fixedS1_10;
typedef __CGcustom_fixed_storage<1,10,1> __CGcustom_fixed_storageS1_10;

typedef __CGcustom_fixed<0,8,0> __CGcustom_fixedU0_8;
typedef __CGcustom_fixed_storage<0,8,0> __CGcustom_fixed_storageU0_8;

// Cg data type for fp30/fp40 profile
typedef __CGcustom_fixedS1_10 fixed;
typedef __CGcustom_fixed_storageS1_10 cgFixedStorage;

// Cg data type for OpenGL texenv profile
//typedef __CGcustom_fixedU0_8 fixed;
//typedef __CGcustom_fixed_storageU0_8 cgFixedStorage;

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(pop)
#endif

} // namespace Cg

#endif // __Cg_fixed_hpp__
