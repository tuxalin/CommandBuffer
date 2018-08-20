// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2014 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#ifndef NV_NVFOUNDATION_NVMAT33_H
#define NV_NVFOUNDATION_NVMAT33_H
/** \addtogroup foundation
@{
*/

#include "NvVec3.h"
#include "NvQuat.h"

#if !NV_DOXYGEN
namespace nvidia
{
#endif
/*!
\brief 3x3 matrix class

Some clarifications, as there have been much confusion about matrix formats etc in the past.

Short:
- Matrix have base vectors in columns (vectors are column matrices, 3x1 matrices).
- Matrix is physically stored in column major format
- Matrices are concaternated from left

Long:
Given three base vectors a, b and c the matrix is stored as

|a.x b.x c.x|
|a.y b.y c.y|
|a.z b.z c.z|

Vectors are treated as columns, so the vector v is

|x|
|y|
|z|

And matrices are applied _before_ the vector (pre-multiplication)
v' = M*v

|x'|   |a.x b.x c.x|   |x|   |a.x*x + b.x*y + c.x*z|
|y'| = |a.y b.y c.y| * |y| = |a.y*x + b.y*y + c.y*z|
|z'|   |a.z b.z c.z|   |z|   |a.z*x + b.z*y + c.z*z|


Physical storage and indexing:
To be compatible with popular 3d rendering APIs (read D3d and OpenGL)
the physical indexing is

|0 3 6|
|1 4 7|
|2 5 8|

index = column*3 + row

which in C++ translates to M[column][row]

The mathematical indexing is M_row,column and this is what is used for _-notation
so _12 is 1st row, second column and operator(row, column)!

*/
class NvMat33
{
  public:
	//! Default constructor
	NV_CUDA_CALLABLE NV_FORCE_INLINE NvMat33()
	{
	}

	//! identity constructor
	NV_CUDA_CALLABLE NV_INLINE NvMat33(NvIDENTITY r)
	: column0(1.0f, 0.0f, 0.0f), column1(0.0f, 1.0f, 0.0f), column2(0.0f, 0.0f, 1.0f)
	{
		NV_UNUSED(r);
	}

	//! zero constructor
	NV_CUDA_CALLABLE NV_INLINE NvMat33(NvZERO r) : column0(0.0f), column1(0.0f), column2(0.0f)
	{
		NV_UNUSED(r);
	}

	//! Construct from three base vectors
	NV_CUDA_CALLABLE NvMat33(const NvVec3& col0, const NvVec3& col1, const NvVec3& col2)
	: column0(col0), column1(col1), column2(col2)
	{
	}

	//! constructor from a scalar, which generates a multiple of the identity matrix
	explicit NV_CUDA_CALLABLE NV_INLINE NvMat33(float r)
	: column0(r, 0.0f, 0.0f), column1(0.0f, r, 0.0f), column2(0.0f, 0.0f, r)
	{
	}

	//! Construct from float[9]
	explicit NV_CUDA_CALLABLE NV_INLINE NvMat33(float values[])
	: column0(values[0], values[1], values[2])
	, column1(values[3], values[4], values[5])
	, column2(values[6], values[7], values[8])
	{
	}

	//! Construct from a quaternion
	explicit NV_CUDA_CALLABLE NV_FORCE_INLINE NvMat33(const NvQuat& q)
	{
		const float x = q.x;
		const float y = q.y;
		const float z = q.z;
		const float w = q.w;

		const float x2 = x + x;
		const float y2 = y + y;
		const float z2 = z + z;

		const float xx = x2 * x;
		const float yy = y2 * y;
		const float zz = z2 * z;

		const float xy = x2 * y;
		const float xz = x2 * z;
		const float xw = x2 * w;

		const float yz = y2 * z;
		const float yw = y2 * w;
		const float zw = z2 * w;

		column0 = NvVec3(1.0f - yy - zz, xy + zw, xz - yw);
		column1 = NvVec3(xy - zw, 1.0f - xx - zz, yz + xw);
		column2 = NvVec3(xz + yw, yz - xw, 1.0f - xx - yy);
	}

	//! Copy constructor
	NV_CUDA_CALLABLE NV_INLINE NvMat33(const NvMat33& other)
	: column0(other.column0), column1(other.column1), column2(other.column2)
	{
	}

	//! Assignment operator
	NV_CUDA_CALLABLE NV_FORCE_INLINE NvMat33& operator=(const NvMat33& other)
	{
		column0 = other.column0;
		column1 = other.column1;
		column2 = other.column2;
		return *this;
	}

	//! Construct from diagonal, off-diagonals are zero.
	NV_CUDA_CALLABLE NV_INLINE static NvMat33 createDiagonal(const NvVec3& d)
	{
		return NvMat33(NvVec3(d.x, 0.0f, 0.0f), NvVec3(0.0f, d.y, 0.0f), NvVec3(0.0f, 0.0f, d.z));
	}

	/**
	\brief returns true if the two matrices are exactly equal
	*/
	NV_CUDA_CALLABLE NV_INLINE bool operator==(const NvMat33& m) const
	{
		return column0 == m.column0 && column1 == m.column1 && column2 == m.column2;
	}

	//! Get transposed matrix
	NV_CUDA_CALLABLE NV_FORCE_INLINE NvMat33 getTranspose() const
	{
		const NvVec3 v0(column0.x, column1.x, column2.x);
		const NvVec3 v1(column0.y, column1.y, column2.y);
		const NvVec3 v2(column0.z, column1.z, column2.z);

		return NvMat33(v0, v1, v2);
	}

	//! Get the real inverse
	NV_CUDA_CALLABLE NV_INLINE NvMat33 getInverse() const
	{
		const float det = getDeterminant();
		NvMat33 inverse;

		if(det != 0)
		{
			const float invDet = 1.0f / det;

			inverse.column0[0] = invDet * (column1[1] * column2[2] - column2[1] * column1[2]);
			inverse.column0[1] = invDet * -(column0[1] * column2[2] - column2[1] * column0[2]);
			inverse.column0[2] = invDet * (column0[1] * column1[2] - column0[2] * column1[1]);

			inverse.column1[0] = invDet * -(column1[0] * column2[2] - column1[2] * column2[0]);
			inverse.column1[1] = invDet * (column0[0] * column2[2] - column0[2] * column2[0]);
			inverse.column1[2] = invDet * -(column0[0] * column1[2] - column0[2] * column1[0]);

			inverse.column2[0] = invDet * (column1[0] * column2[1] - column1[1] * column2[0]);
			inverse.column2[1] = invDet * -(column0[0] * column2[1] - column0[1] * column2[0]);
			inverse.column2[2] = invDet * (column0[0] * column1[1] - column1[0] * column0[1]);

			return inverse;
		}
		else
		{
			return NvMat33(NvIdentity);
		}
	}

	//! Get determinant
	NV_CUDA_CALLABLE NV_INLINE float getDeterminant() const
	{
		return column0.dot(column1.cross(column2));
	}

	//! Unary minus
	NV_CUDA_CALLABLE NV_INLINE NvMat33 operator-() const
	{
		return NvMat33(-column0, -column1, -column2);
	}

	//! Add
	NV_CUDA_CALLABLE NV_INLINE NvMat33 operator+(const NvMat33& other) const
	{
		return NvMat33(column0 + other.column0, column1 + other.column1, column2 + other.column2);
	}

	//! Subtract
	NV_CUDA_CALLABLE NV_INLINE NvMat33 operator-(const NvMat33& other) const
	{
		return NvMat33(column0 - other.column0, column1 - other.column1, column2 - other.column2);
	}

	//! Scalar multiplication
	NV_CUDA_CALLABLE NV_INLINE NvMat33 operator*(float scalar) const
	{
		return NvMat33(column0 * scalar, column1 * scalar, column2 * scalar);
	}

	friend NvMat33 operator*(float, const NvMat33&);

	//! Matrix vector multiplication (returns 'this->transform(vec)')
	NV_CUDA_CALLABLE NV_INLINE NvVec3 operator*(const NvVec3& vec) const
	{
		return transform(vec);
	}

	// a <op>= b operators

	//! Matrix multiplication
	NV_CUDA_CALLABLE NV_FORCE_INLINE NvMat33 operator*(const NvMat33& other) const
	{
		// Rows from this <dot> columns from other
		// column0 = transform(other.column0) etc
		return NvMat33(transform(other.column0), transform(other.column1), transform(other.column2));
	}

	//! Equals-add
	NV_CUDA_CALLABLE NV_INLINE NvMat33& operator+=(const NvMat33& other)
	{
		column0 += other.column0;
		column1 += other.column1;
		column2 += other.column2;
		return *this;
	}

	//! Equals-sub
	NV_CUDA_CALLABLE NV_INLINE NvMat33& operator-=(const NvMat33& other)
	{
		column0 -= other.column0;
		column1 -= other.column1;
		column2 -= other.column2;
		return *this;
	}

	//! Equals scalar multiplication
	NV_CUDA_CALLABLE NV_INLINE NvMat33& operator*=(float scalar)
	{
		column0 *= scalar;
		column1 *= scalar;
		column2 *= scalar;
		return *this;
	}

	//! Equals matrix multiplication
	NV_CUDA_CALLABLE NV_INLINE NvMat33& operator*=(const NvMat33& other)
	{
		*this = *this * other;
		return *this;
	}

	//! Element access, mathematical way!
	NV_CUDA_CALLABLE NV_FORCE_INLINE float operator()(unsigned int row, unsigned int col) const
	{
		return (*this)[col][row];
	}

	//! Element access, mathematical way!
	NV_CUDA_CALLABLE NV_FORCE_INLINE float& operator()(unsigned int row, unsigned int col)
	{
		return (*this)[col][row];
	}

	// Transform etc

	//! Transform vector by matrix, equal to v' = M*v
	NV_CUDA_CALLABLE NV_FORCE_INLINE NvVec3 transform(const NvVec3& other) const
	{
		return column0 * other.x + column1 * other.y + column2 * other.z;
	}

	//! Transform vector by matrix transpose, v' = M^t*v
	NV_CUDA_CALLABLE NV_INLINE NvVec3 transformTranspose(const NvVec3& other) const
	{
		return NvVec3(column0.dot(other), column1.dot(other), column2.dot(other));
	}

	NV_CUDA_CALLABLE NV_FORCE_INLINE const float* front() const
	{
		return &column0.x;
	}

	NV_CUDA_CALLABLE NV_FORCE_INLINE NvVec3& operator[](unsigned int num)
	{
		return (&column0)[num];
	}
	NV_CUDA_CALLABLE NV_FORCE_INLINE const NvVec3& operator[](unsigned int num) const
	{
		return (&column0)[num];
	}

	// Data, see above for format!

	NvVec3 column0, column1, column2; // the three base vectors
};

// implementation from NvQuat.h
NV_CUDA_CALLABLE NV_INLINE NvQuat::NvQuat(const NvMat33& m)
{
	float tr = m(0, 0) + m(1, 1) + m(2, 2), h;
	if(tr >= 0)
	{
		h = NvSqrt(tr + 1);
		w = 0.5f * h;
		h = 0.5f / h;

		x = (m(2, 1) - m(1, 2)) * h;
		y = (m(0, 2) - m(2, 0)) * h;
		z = (m(1, 0) - m(0, 1)) * h;
	}
	else
	{
		unsigned int i = 0;
		if(m(1, 1) > m(0, 0))
			i = 1;
		if(m(2, 2) > m(i, i))
			i = 2;
		switch(i)
		{
		case 0:
			h = NvSqrt((m(0, 0) - (m(1, 1) + m(2, 2))) + 1);
			x = 0.5f * h;
			h = 0.5f / h;

			y = (m(0, 1) + m(1, 0)) * h;
			z = (m(2, 0) + m(0, 2)) * h;
			w = (m(2, 1) - m(1, 2)) * h;
			break;
		case 1:
			h = NvSqrt((m(1, 1) - (m(2, 2) + m(0, 0))) + 1);
			y = 0.5f * h;
			h = 0.5f / h;

			z = (m(1, 2) + m(2, 1)) * h;
			x = (m(0, 1) + m(1, 0)) * h;
			w = (m(0, 2) - m(2, 0)) * h;
			break;
		case 2:
			h = NvSqrt((m(2, 2) - (m(0, 0) + m(1, 1))) + 1);
			z = 0.5f * h;
			h = 0.5f / h;

			x = (m(2, 0) + m(0, 2)) * h;
			y = (m(1, 2) + m(2, 1)) * h;
			w = (m(1, 0) - m(0, 1)) * h;
			break;
		default: // Make compiler happy
			x = y = z = w = 0;
			break;
		}
	}
}

#if !NV_DOXYGEN
} // namespace nvidia
#endif

/** @} */
#endif // #ifndef NV_NVFOUNDATION_NVMAT33_H
