/* 
 * Copyright 2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/half.hpp>
#include <Cg/fixed.hpp>
#include <Cg/double.hpp>
#include <Cg/all.hpp>

namespace Cg {

bool1 all(bool1 v)
{
    return all<1>(v);
}

bool1 all(bool2 v)
{
    return all<2>(v);
}

bool1 all(bool3 v)
{
    return all<3>(v);
}

bool1 all(bool4 v)
{
    return all<4>(v);
}

} // namespace Cg

