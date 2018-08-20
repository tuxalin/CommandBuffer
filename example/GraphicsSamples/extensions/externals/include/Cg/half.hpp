/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_half_hpp__
#define __Cg_half_hpp__

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

/* __CGcustom_float_storage is the pure storage required for a __CGcustom_float.
   Importantly, it has no constructor, destructor, etc. so it can be
   kept within a union, unlike __CGcustom_float that has a constructor, etc. */

// manBits is number of mantissa bits
// expBits is number of exponent bias
// sgnBit is whether or not a sign bit is provided
template <unsigned int manBits, unsigned int expBits, bool sgnBit>
struct __CGcustom_float_storage {
    static const int bits = manBits + expBits + sgnBit;
    static const int bytesPerFloat = (bits + 7) / 8;

    unsigned char v[bytesPerFloat];
} __CGmay_alias;

// satToMaxFloat is whether or not values greater than MAXFLOAT map to Infinity
template <unsigned int manBits, unsigned int expBits, bool sgnBit, bool satToMaxFloat>
struct __CGcustom_float : private __CGcustom_float_storage<manBits,expBits,sgnBit> {

private:
    static const unsigned int fp32ManBits      = 23u;                                   // 23 mantissa bits for s23e8
    static const unsigned int fp32ExpBits      = 8u;                                    // 8 exponent bits for s23e8
    static const unsigned int fp32Bits         = (fp32ManBits+fp32ExpBits+1);           // 32 for s23e8 = (includes sign bit)
    static const unsigned int fp32ExpBias      = ((1u<<(fp32ExpBits-1))-1);             // 127 for s23e8
    static const unsigned int fp32ManexpExpLsb = (1u<<fp32ManBits);                     // 0x00800000 for s23e8
    static const unsigned int fp32ManexpInf    = (((1u<<fp32ExpBits)-1)<<fp32ManBits);  // 0x7F800000 for s23e8
    static const unsigned int fp32ManexpNaN    = ((1u<<(fp32ExpBits+fp32ManBits))-1);   // 0x7FFFFFFF for s23e8
    static const unsigned int fp32ExpMax       = ((1u<<fp32ExpBits)-1);                 // 255
    static const unsigned int fp32ExpMask      = ((1u<<fp32ExpBits)-1);                 // 255
    static const unsigned int fp32SgnMask      = (1u<<(fp32ManBits+fp32ExpBits));       // MSB set for s23e8

    static const unsigned int expBias         = ((1u<<(expBits-1))-1);                        // 15 for s10e5
    static const unsigned int expMax          = ((1u<<expBits)-1);                            // 31 for s10e5
    static const unsigned int bits            = (manBits+expBits+sgnBit);                     // 16 for s10e5 = (includes sign Bit)
    static const unsigned int manexpMask      = ((1u<<(manBits+expBits))-1);                  // 0x7FFF for s10e5
    static const unsigned int manexpExpLsb    = (1u<<manBits);                                // 0x400 for s10e5
    static const unsigned int manexpManMask   = ((1u<<manBits)-1);                            // 0x3FF for s10e5
    static const unsigned int manexpExpMask   = (((1u<<expBits)-1)<<manBits);                 // 0x7C00 for s10e5
    static const unsigned int expBiasDiff     = (fp32ExpBias-expBias);                        // 112 for s10e5
    static const unsigned int sgnMask         = (sgnBit<<(manBits+expBits));                  // 0x8000 for s10e5
    static const unsigned int manexpInf       = (((1u<<expBits)-1)<<manBits);                 // 0x7C00 for s10e5
    static const unsigned int manexpNaN       = ((1u<<(expBits+manBits))-1);                  // 0x7FFF for s10e5
    static const unsigned int manexpMaxFloat  = ((manexpExpMask-manexpExpLsb)|manexpManMask); // 0x7Bff for s10e5

    // Constants for converting from/to packed float and fp32
    static const unsigned int convManexpShift  = (fp32ManBits-manBits);                   // 13 for s10e5
    static const unsigned int convManexpSubLsb = (1u<<(convManexpShift-1));               // 0x1000 for s10e5
    static const unsigned int convShift        = (fp32Bits-bits);                         // 16 for s10e5
    static const unsigned int convExpBiasSum   = (fp32ExpBias+expBias);                   // 142 for s10e5
    static const unsigned int convDenormStart  = ((fp32ExpBias-expBias+1)<<fp32ManBits);  // 0x38800000 for s10e5

    static unsigned int floatToCustomFloat(float f);
    unsigned int custom_floatToFloat() const;

    static unsigned int cgIntToCustom(float f);
    unsigned int __CGcustom_floatToInt() const;

public:
    // FLOAT-TO-HALF CONSTRUCTOR
    inline __CGcustom_float() {}
    __CGcustom_float(float val) {
        unsigned int ui = floatToCustomFloat(val);

        for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
            __CGcustom_float::v[i] = ui & 0xFF;
            ui >>= 8;
        }
    }
    // DOUBLE-TO-HALF CONSTRUCTOR
    __CGcustom_float(double val) {
        unsigned int ui = floatToCustomFloat(float(val));

        for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
            __CGcustom_float::v[i] = ui & 0xFF;
            ui >>= 8;
        }
    }
    // INT-TO-HALF CONSTRUCTOR
    // (should this be marked explicit?) requires half3(half(0),half(4),half(5))
    __CGcustom_float(int val) {
        unsigned int ui = floatToCustomFloat(float(val));

        for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
            __CGcustom_float::v[i] = ui & 0xFF;
            ui >>= 8;
        }
    }
    inline __CGcustom_float(const __CGcustom_float &rhs)
    {
        for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
            __CGcustom_float::v[i] = rhs.v[i];
        }
    }

    inline operator float()
    {
        unsigned int ui = custom_floatToFloat();
        float *fp = reinterpret_cast<float *>(&ui);
        float f = *fp;
        return f;
    }

    inline operator float() const
    {
        unsigned int ui = custom_floatToFloat();
        float *fp = reinterpret_cast<float *>(&ui);
        float f = *fp;
        return f;
    }

#if 0  // XXXmjk Do we need these?  What about fixed?
    inline operator int()
    {
        // Could do better...
        unsigned int ui = custom_floatToFloat();
        float *fp = reinterpret_cast<float *>(&ui);
        float f = *fp;
        return int(f);
    }

    inline operator int() const
    {
        // Could do better...
        unsigned int ui = custom_floatToFloat();
        float *fp = reinterpret_cast<float *>(&ui);
        float f = *fp;
        return int(f);
    }
#endif

    inline __CGcustom_float & operator = (const __CGcustom_float & val) {
        for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
            __CGcustom_float::v[i] = val.v[i];
        }
        return *this;
    }

    template <typename T>
    inline __CGcustom_float & operator += (const T & val) {
        float floatVersion = *this;
        floatVersion += val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_float & operator -= (const T & val) {
        float floatVersion = *this;
        floatVersion -= val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_float & operator *= (const T & val) {
        float floatVersion = *this;
        floatVersion *= val;
        *this = floatVersion;
        return *this;
    }
    template <typename T>
    inline __CGcustom_float & operator /= (const T & val) {
        float floatVersion = *this;
        floatVersion /= val;
        *this = floatVersion;
        return *this;
    }

    inline __CGcustom_float & operator ++ () {
        float floatVersion = *this;
        ++floatVersion;
        *this = floatVersion;
        return *this;
    }
    inline __CGcustom_float operator ++ (int) {
        __CGcustom_float oldValue = *this;
        float floatVersion = *this;
        ++floatVersion;
        *this = floatVersion;
        return oldValue;
    }
    inline __CGcustom_float & operator -- () {
        float floatVersion = *this;
        --floatVersion;
        *this = floatVersion;
        return *this;
    }
    inline __CGcustom_float operator -- (int) {
        __CGcustom_float oldValue = *this;
        float floatVersion = *this;
        --floatVersion;
        *this = floatVersion;
        return oldValue;
    }
};

template <unsigned int manBits, unsigned int expBits, bool sgnBit, bool satToMaxFloat>
unsigned int __CGcustom_float<manBits,expBits,sgnBit,satToMaxFloat>::custom_floatToFloat() const
{
    unsigned int f = 0;
    unsigned int manexp, data;

    for (int i=0; i<__CGcustom_float::bytesPerFloat; i++) {
        f |= __CGcustom_float::v[i] << i*8;
    }

    // Extract the mantissa and exponent.
    manexp = f & manexpMask;

    if (manexp < manexpExpLsb) {
        // Exponent == 0, implies 0.0 or Denorm.
        if (manexp == 0) {
            data = 0;
        } else {
            // Denorm -- shift the mantissa left until we find a leading one.
            // Each shift drops one off the final exponent.
            data = convDenormStart;
            do {
                data -= fp32ManexpExpLsb; // multiply by 1/2
                manexp *= 2;
            } while (!(manexp & manexpExpLsb));

            // Now shift the mantissa into the final location.
            data |= (manexp & manexpManMask) << convManexpShift;
        }
    } else if (manexp >= manexpExpMask) {
        // Exponent = ExpMax, implies Inf or NaN.
        if (manexp == manexpExpMask) {
            data = fp32ManexpInf;  // Inf
        } else {
            data = fp32ManexpNaN;  // NaN
        }
    } else {
        // Normal float -- (1) shift over mantissa/exponent, (2) add bias to
        // exponent, and (3)
        data = (manexp << convManexpShift);
        data += (expBiasDiff << fp32ManBits);
    }

    // Or in the sign bit and return the result.
    return data | (f & sgnMask) << convShift;
}

template <unsigned int manBits, unsigned int expBits, bool sgnBit, bool satToMaxFloat>
unsigned int __CGcustom_float<manBits,expBits,sgnBit,satToMaxFloat>::floatToCustomFloat(float f)
{
    unsigned int *uip = reinterpret_cast<unsigned int*>(&f);
    unsigned int ui = *uip;
    unsigned int data, exp, man;

    // Extract the exponent and the manBits MSBs of the mantissa from the fp32
    // number.
    exp = (ui >> fp32ManBits) & fp32ExpMask;
    man = (ui >> convManexpShift) & manexpManMask;

    // Round on type conversion.  Check mantissa Bit fp32ManExpShift in the 32-Bit number.
    // If set, round the mantissa up.  If the mantissa overflows, bump the
    // exponent by 1 and clear the mantissa.
    if (ui & convManexpSubLsb) {
        man++;
        if (man & manexpExpLsb) {
            man = 0;
            exp++;
        }
    }

    if (exp <= expBiasDiff) {
        // |x| < 2^-14, implies 0.0 or Denorm

        // If |x| < 2^-25, we will flush to zero.  Otherwise, we will or in
        // the leading one and Shift to the appropriate location.
        if (exp < (expBiasDiff - manBits)) {
            data = 0;           // 0.0
        } else {
            data = (man | manexpExpLsb) >> (expBiasDiff+1 - exp);
        }
    } else if (exp > convExpBiasSum) {
        // |x| > 2^15, implies overflow, an existing Inf, or NaN.  Fp32 NaN is any
        // non-zero mantissa with an exponent of +128 (255).  Note that our
        // rounding algorithm could have kicked the exponent up to 256.
        if (exp == fp32ExpMax) {
            if (man) {
                data = manexpNaN;
                // Return allows -NaN to return as NaN even if there is no sign Bit.
                return data | ((ui >> convShift) & sgnMask);
            } else {
                data = manexpInf;
            }
        } else {
            if (satToMaxFloat) {
                data = manexpMaxFloat;
            } else {
                // Values over MaxFloat map to Inf
                data = manexpInf;
            }
        }
    } else {
        exp -= expBiasDiff;
        data = (exp << manBits) | man;
    }

    if (sgnBit) {
        data |= ((ui >> convShift) & sgnMask);
    } else {
        if (ui & fp32SgnMask) {
            // Clamp negative values (except -NaN, see above) to zero.
            data = 0;           // 0.0
        }
    }

    return data;
}

typedef __CGcustom_float<10,5,1,0> __CGcustom_floatS10E5;
typedef __CGcustom_float_storage<10,5,1> __CGcustom_float_storageS10E5;

// Cg data type for fp30/fp40 profile
typedef __CGcustom_floatS10E5 half;
typedef __CGcustom_float_storageS10E5 __CGhalf_storage;

// Undefine helper #defines
#undef __CGmay_alias

#if defined(_MSC_VER) && !defined(__EDG__)  // Visual C++ but not EDG fakery
#pragma warning(pop)
#endif

} // namespace Cg

#endif // __Cg_half_hpp__
