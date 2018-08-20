/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/iostream.hpp>

#include <iostream>

namespace Cg {

std::ostream & operator << (std::ostream & o, const half & v) {
    const float fv = v;
    o << fv;
    return o;
}

std::ostream & operator << (std::ostream & o, const fixed & v) {
    const float fv = v;
    o << fv;
    return o;
}

} // namespace Cg

