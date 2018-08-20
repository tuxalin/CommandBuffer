
/* 
 * Copyright 2008 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 */

#include <Cg/double.hpp>
#include <Cg/inverse.hpp>
#include <Cg/abs.hpp>

namespace Cg {

template <typename T, int N>
static inline __CGmatrix<T,N,N> inverse(__CGmatrix<T,N,N> & input)
{
    __CGmatrix<T,N,N> matrix(input), scratch, result;
    __CGvector<int,N> perm;
    const int n = N;
    int pivot;

    // Make permuation vector
    for (int i=0; i<n; i++) {
        perm[i] = i;
    }
    // Initialize scratch to identity matrix
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            scratch[i][j] = (i==j) ? T(1) : T(0);
        }
    }

    int isave = 0;
    for (int j=0; j<n; j++) {
        pivot = perm[j];

        for (int i=j+1; i<n; i++) {
            int jrow = perm[i];
            if (abs(matrix[pivot][j]) < abs(matrix[jrow][j])) {
                pivot = jrow;
                isave = i;
            }
        }

        // If abs(matrix[pivot][j] is too small matrix probably singular

        // Swap indices if not the pivot
        if (pivot != perm[j]) {
            perm[isave] = perm[j];
            perm[j] = pivot;
        }

        // Normalize rows
        for (int kcol=j+1; kcol<n; kcol++) {
            matrix[pivot][kcol] /= matrix[pivot][j];
        }
        for (int kcol=0; kcol<n; kcol++) {
            scratch[pivot][kcol] /= matrix[pivot][j];
        }

        // Reduce rows
        for (int i=j+1; i<n; i++) {
            int jrow = perm[i];
            for (int kcol=j+1; kcol<n; kcol++) {
                matrix[jrow][kcol] -= matrix[pivot][kcol] * matrix[jrow][j];
            }
            for (int kcol=0; kcol<n; kcol++) {
                scratch[jrow][kcol] -= scratch[pivot][kcol] * matrix[jrow][j];
            }
        }
    }

    // Subsitute back
    for (int i=n-1; i>=0; i--) {
        int jrow = perm[i];
        for (int kcol=0; kcol<n; kcol++) {
            for (int j=i+1; j<n; j++) {
                scratch[jrow][kcol] -= matrix[jrow][j] * scratch[perm[j]][kcol];
            }
        }
    }

    // De-permute result
    for (int i=0; i<n; i++) {
        int jrow = perm[i];
        result[i] = scratch[jrow];
    }

    return __CGmatrix<T,N,N>(result);
}

float1x1 inverse(float1x1 a)
{
    return inverse<float,1>(a);
}
float2x2 inverse(float2x2 a)
{
    return inverse<float,2>(a);
}
float3x3 inverse(float3x3 a)
{
    return inverse<float,3>(a);
}
float4x4 inverse(float4x4 a)
{
    return inverse<float,4>(a);
}

double1x1 inverse(double1x1 a)
{
    return inverse<double,1>(a);
}
double2x2 inverse(double2x2 a)
{
    return inverse<double,2>(a);
}
double3x3 inverse(double3x3 a)
{
    return inverse<double,3>(a);
}
double4x4 inverse(double4x4 a)
{
    return inverse<double,4>(a);
}

} // namespace Cg
