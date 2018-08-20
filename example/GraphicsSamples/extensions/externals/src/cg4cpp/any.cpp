/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/any.hpp>

namespace Cg {

bool1 any(bool1 v)
{
    return any<1>(v);
}

bool1 any(bool2 v)
{
    return any<2>(v);
}

bool1 any(bool3 v)
{
    return any<3>(v);
}

bool1 any(bool4 v)
{
    return any<4>(v);
}

} // namespace Cg

