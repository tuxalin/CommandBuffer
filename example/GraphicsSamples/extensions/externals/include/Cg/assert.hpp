/* 
 * Copyright 2005-2006 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#ifndef __Cg_assert_hpp__
#define __Cg_assert_hpp__

#ifndef __CG_NO_ASSERT
#include <assert.h>
#define __CGassert(x) assert(x)
#else
#define __CGassert(x)
#endif

#endif // __Cg_assert_hpp__
