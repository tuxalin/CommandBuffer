/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_inout_hpp__
#define __Cg_inout_hpp__

#include <Cg/vector.hpp>

#include <cstring>  // for std::memcpy

namespace Cg {

/// WRAP TYPES

// Wrapper for classes and structures
template <typename T>
class __CGwrap_type : public T {
protected:
    typedef struct {
        T val;  // Wrap a value of type T in a structure.
    } Wrapper;
public:
    inline __CGwrap_type & operator = (const T & v) {
        T::operator=(v);
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        this->T::operator++();
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        this->T::operator++(1);
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        this->T::operator--();
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        this->T::operator--(1);
        return orig;
    }
};

// Wrapper for built-in float
template <>
class __CGwrap_type<float> {
protected:
    typedef float T;
    typedef T Wrapper;
    float val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for built-in int
template <>
class __CGwrap_type<int> {
protected:
    typedef int T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for built-in unsigned int
template <>
class __CGwrap_type<unsigned int> {
protected:
    typedef unsigned int T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for built-in bool
template <>
class __CGwrap_type<bool> {
protected:
    typedef bool T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
};

template <>
class __CGwrap_type<char> {
protected:
    typedef char T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for built-in short
template <>
class __CGwrap_type<short> {
protected:
    typedef short T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for built-in double
template <>
class __CGwrap_type<double> {
protected:
    typedef double T;
    typedef T Wrapper;
    T val;
public:
    inline operator T & () {
        return val;
    }
    inline operator const T & () const {
        return val;
    }
    inline operator T () const {  // no const needed for function
        return val;
    }
    inline T & operator = (const T & v) { 
        this->val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->val;
        return orig;
    }
};

// Wrapper for non-const arrays
template <typename T, size_t N>
class __CGwrap_type<T[N]> {
protected:
    typedef struct {
        T val[N];  // Wrap a value of type T in a structure.
    } Wrapper;
    Wrapper wrapper;
public:
    inline T & operator [] (int i) { 
        return wrapper.val[i];
    }
    inline const T & operator [] (int i) const {
        return wrapper.val[i];
    }
    inline operator T & () {
        return wrapper.val;
    }
    inline T & operator = (const T & v) { 
        this->wrapper.val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->wrapper.val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->wrapper.val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->wrapper.val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->wrapper.val;
        return orig;
    }
};

// Wrapper for non-const arrays
template <typename T, size_t N>
class __CGwrap_type<const T[N]> {
protected:
    typedef struct {
        T val[N];  // Wrap a value of type T in a structure.
    } Wrapper;
    Wrapper wrapper;
public:
    inline const T & operator [] (int i) const {
        return wrapper.val[i];
    }
    inline operator T & () {
        return wrapper.val;
    }
    inline T & operator = (const T & v) { 
        this->wrapper.val = v;
        return *this;
    }
    inline __CGwrap_type & operator ++ () { /* prefix */
        ++this->wrapper.val;
        return *this;
    }
    inline __CGwrap_type operator ++ (int) { /* postfix */
        __CGwrap_type orig = *this;
        ++this->wrapper.val;
        return orig;
    }
    inline __CGwrap_type & operator -- () { /* prefix */
        --this->wrapper.val;
        return *this;
    }
    inline __CGwrap_type operator -- (int) { /* postfix */
        __CGwrap_type orig = *this;
        --this->wrapper.val;
        return orig;
    }
};

/// INOUT

// InOut for generic types
template <typename T>
class InOut : public __CGwrap_type<T> {
private:
    typedef typename __CGwrap_type<T>::Wrapper Wrapper;
    Wrapper *from;
public:
    InOut(T & f) : from(reinterpret_cast<Wrapper*>(&f)) {
        std::memcpy(this, from, sizeof(T));
    }
    ~InOut() {
        std::memcpy(from, reinterpret_cast<Wrapper*>(this), sizeof(T));
    }
    inline InOut & operator = (const T & v) {
        __CGwrap_type<T>::operator=(v);
        return *this;
    }
};

// InOut for vector and swizzled types
template <typename T, int NN>
class InOut<__CGvector<T,NN> > : public __CGwrap_type<__CGvector<T,NN> > {
private:
    typedef typename __CGwrap_type<__CGvector<T,NN> >::Wrapper Wrapper;
    Wrapper *from;
    unsigned int swizMask;
public:
    // Constructor that registers standard copy-out routine
    InOut(__CGvector<T,NN> & f) : from(reinterpret_cast<Wrapper*>(&f)),
                                  swizMask(0x03020100) {
        for (int i=0; i<NN; i++) {
            (*this)[i] = f[i];
        }
    }
    template <int STORAGE, unsigned int SMASK>
    InOut(__CGswizzle<T,STORAGE,NN,SMASK> & f) : from(reinterpret_cast<Wrapper*>(&f)),
                                                 swizMask(SMASK) {
        for (int i=0; i<NN; i++) {
            (*this)[i] = f[i];
        }
    }
    ~InOut() {
        T *src = reinterpret_cast<T*>(this);
        T *dst = reinterpret_cast<T*>(from);

        for (int i=0; i<NN; i++) {
            unsigned int s = (swizMask >> (8*i)) & 0xFF;
            dst[s] = src[i];
        }
    }
    template <typename Tstore>
    inline InOut & operator = (const __CGvector_usage<T,NN,Tstore> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
    inline InOut & operator = (const __CGvector<T,NN> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
};

/// IN

// In for generic types
template <typename T>
class In : public __CGwrap_type<T> {
private:
    // No reference
public:
    In(T f) {
        *this = f;
    }
    // No destructor defined.
    inline In & operator = (const T & v) {
        __CGwrap_type<T>::operator=(v);
        return *this;
    }
};

// In for vector and swizzled types
template <typename T, int NN>
class In<__CGvector<T,NN> > : public __CGwrap_type<__CGvector<T,NN> > {
private:
    // No reference
public:
    // Constructor that registers standard copy-out routine
    In(const __CGvector<T,NN> & f) {
        for (int i=0; i<NN; i++) {
            (*this)[i] = f[i];
        }
    }
    template <int STORAGE, unsigned int SMASK>
    In(const __CGswizzle<T,STORAGE,NN,SMASK> & f) {
        for (int i=0; i<NN; i++) {
            (*this)[i] = f[i];
        }
    }
    ~In() { }
    template <typename Tstore>
    inline In & operator = (const __CGvector_usage<T,NN,Tstore> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
    inline In & operator = (const __CGvector<T,NN> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
};

/// OUT

// Out for generic types
template <typename T>
class Out : public __CGwrap_type<T>  {
private:
    typedef typename __CGwrap_type<T>::Wrapper Wrapper;
    Wrapper *from;
public:
    Out(T & f) : from(reinterpret_cast<Wrapper*>(&f)) { }
    ~Out() {
        std::memcpy(from, reinterpret_cast<Wrapper*>(this), sizeof(T));
    }
    inline Out & operator = (const T & v) {
        __CGwrap_type<T>::operator=(v);
        return *this;
    }
};

// Out for vector and swizzled types
template <typename T, int NN>
class Out<__CGvector<T,NN> > : public __CGwrap_type<__CGvector<T,NN> > {
private:
    typedef typename __CGwrap_type<__CGvector<T,NN> >::Wrapper Wrapper;
    Wrapper *from;
    unsigned int swizMask;
public:
    // Constructor that registers standard copy-out routine
    Out(__CGvector<T,NN> & f) : from(reinterpret_cast<Wrapper*>(&f)),
                                swizMask(0x03020100) { 
        // Nothing
    }
    template <int STORAGE, unsigned int SMASK>
    Out(__CGswizzle<T,STORAGE,NN,SMASK> & f) : from(reinterpret_cast<Wrapper*>(&f)),
                                               swizMask(SMASK) {
        // Nothing
    }
    ~Out() {
        T *src = reinterpret_cast<T*>(this);
        T *dst = reinterpret_cast<T*>(from);

        for (int i=0; i<NN; i++) {
            unsigned int s = (swizMask >> (8*i)) & 0xFF;
            dst[s] = src[i];
        }
    }
    template <typename Tstore>
    inline Out & operator = (const __CGvector_usage<T,NN,Tstore> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
    inline Out & operator = (const __CGvector<T,NN> & v) {
        __CGwrap_type<__CGvector<T,NN> >::operator=(v);
        return *this;
    }
};

/// These __CGtype_trait definitions make sure In, Out, and InOut types participate in operator overloading

// In and In with conventional types
template <typename T>
struct __CGtype_trait<In<T> > : __CGtype_trait<T> {
};
template <typename T1, typename T2>
struct __CGtype_trait<In<T1>,In<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<T1,In<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<In<T1>,T2> : __CGtype_trait<T1,T2> {
};

// InOut and InOut with conventional types
template <typename T>
struct __CGtype_trait<InOut<T> > : __CGtype_trait<T> {
};
template <typename T1, typename T2>
struct __CGtype_trait<InOut<T1>,InOut<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<T1,InOut<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<InOut<T1>,T2> : __CGtype_trait<T1,T2> {
};

// Out and Out with conventional types
template <typename T>
struct __CGtype_trait<Out<T> > : __CGtype_trait<T> {
};
template <typename T1, typename T2>
struct __CGtype_trait<Out<T1>,Out<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<T1,Out<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<Out<T1>,T2> : __CGtype_trait<T1,T2> {
};

// In and InOut mixes
template <typename T1, typename T2>
struct __CGtype_trait<In<T1>,InOut<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<InOut<T1>,In<T2> > : __CGtype_trait<T1,T2> {
};

// In and Out mixes
template <typename T1, typename T2>
struct __CGtype_trait<In<T1>,Out<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<Out<T1>,In<T2> > : __CGtype_trait<T1,T2> {
};

// Out and InOut mixes
template <typename T1, typename T2>
struct __CGtype_trait<Out<T1>,InOut<T2> > : __CGtype_trait<T1,T2> {
};
template <typename T1, typename T2>
struct __CGtype_trait<InOut<T1>,Out<T2> > : __CGtype_trait<T1,T2> {
};

} // namespace Cg

#endif // __Cg_inout_hpp__
