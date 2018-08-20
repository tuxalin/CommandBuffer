/* 
 * Copyright 2005 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_iostream_hpp__
#define __Cg_iostream_hpp__

/*
 * iostream-based output of Cg-style vector data types.
 *
 * This file is intended to be included AFTER you include <Cg/fixed.hpp>, 
 * <Cg/half.hpp>, <Cg/vector.hpp>, and <Cg/matrix.hpp> you include.
 *
 * Possible enhancements:
 * - Should there be a <Cg/ostream.hpp> and <Cg/istream.hpp> headers?  One
 *   for output, the other for input?
 *
 * INFORMATION: the C++ standard library declares a function named "fixed" in 
 * <ios> (included by <iostream>) that conflicts with the Cg's "fixed" name
 * for its fixed-point datatype.
 */

#include <iostream>

namespace Cg {

#ifdef __Cg_half_hpp__
std::ostream & operator << (std::ostream & o, const half & v);
#endif

#ifdef __Cg_fixed_hpp__
std::ostream & operator << (std::ostream & o, const fixed & v);
#endif

#ifdef __Cg_vector_hpp__
template <typename T, int N, typename Tstore>
std::ostream & operator << (std::ostream & o, const __CGvector_usage<T,N,Tstore> & v) {
    o << "{ ";
    int i;
    for( i = 0; i < N-1; i++ ) {
        o << v[i] << ", ";
    }
    o << v[i] << " }";
    return o;
}
#endif

#ifdef __Cg_matrix_hpp__
template <typename T, int ROWS, int COLS> 
std::ostream & operator << (std::ostream & o, const __CGmatrix_usage<T,ROWS,COLS> & v) {
    o << "{ ";
    int i;
    for( i = 0; i < ROWS-1; i++ ) {
        o << v[i] << ", ";
    }
    o << v[i] << " }";
    return o;
}
#endif

} // namespace Cg

#endif // __Cg_iostream_hpp__
