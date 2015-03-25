#ifndef __MATRIX_TRANSFORM_H_
#define __MATRIX_TRANSFORM_H_

#ifndef assert
#include <assert.h>
#endif

class matrix4
{
public:
    union
    {
        double e[16];
        double e2[4][4];
        struct
        {
            double m00, m01, m02, m03;  //x y z w
            double m10, m11, m12, m13;  //x y z w
            double m20, m21, m22, m23;  //x y z w
            double m30, m31, m32, m33;  //x y z w
        };
    };
};

/*-----------------------------------------------------------------------------
	Functions for transformation of geometry data (using 4x4 matrices).
	-By 4E534B
----------------------------------------------------------------------------*/

// Retruns the value of a 2x2 determinant (either enter each value, or give an array)
#define DET2(a, b, c, d)					(a * d - b * c)
#define DET3(a, b, c, d, e, f, g, h, i)		(a * DET2(e, f, h, i) - b * DET2(d, f, g, i) + c * DET2(d, e, g, h))

// Retruns the value of a 3x3 determinant (either enter each value, or give an array)
#define DET2F(f)							DET2(f[0], f[1], f[2], f[3])
#define DET3F(f)							DET3(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8])

// Returns a Null (all entries zero) matrix.
inline void Mat4Null(matrix4 &mRet) {
	mRet.m00 = mRet.m01 = mRet.m02 = mRet.m03 = 
	mRet.m10 = mRet.m11 = mRet.m12 = mRet.m13 = 
	mRet.m20 = mRet.m21 = mRet.m22 = mRet.m23 = 
	mRet.m30 = mRet.m31 = mRet.m32 = mRet.m33 = 0.0f;
}

// Returns an Identity (diagonal entries one) matrix.
inline void Mat4Identity(matrix4 &mRet) {
	Mat4Null(mRet);

	mRet.m00 = mRet.m11 = mRet.m22 = mRet.m33 = 1.0f;
}

// To get the determinant of a matrix by eliminating i'th row and j'th column.
inline float Mat4Determinant_EliminateRC(const matrix4 &m, const unsigned int i, const unsigned int j) {
	float f[9];
	unsigned int x = 0;

	for (unsigned int _i = 0; _i < 4; _i++) {
		for (unsigned int _j = 0; _j < 4; _j++) {
			if ((_i != i) && (_j != j)) {
				f[x] = m.e2[_i][_j];
				x++;

				assert(x < 10);	// Don't go any further if this is not the case!
			}
		}
	}

	assert(x == 9);		// We must have processed 9 elements (not less, not more)
	return DET3F(f);
}

// To get the determinant of a matrix.
inline float Mat4Determinant(const matrix4 &m) {
	float fRet = 0;
	float f = 0;

	fRet += m.e2[0][0] * Mat4Determinant_EliminateRC(m, 0, 0);
	fRet += m.e2[0][1] * Mat4Determinant_EliminateRC(m, 0, 1);
	fRet += m.e2[0][2] * Mat4Determinant_EliminateRC(m, 0, 2);
	fRet += m.e2[0][3] * Mat4Determinant_EliminateRC(m, 0, 3);

	return fRet;
}

// Returns whether an matrix is an identity matrix or not (ie |mat| = 1 or 
// non diagonal entries 0, diagonal entries 1)
inline bool Mat4IsIdentity(const matrix4 &m) {
	bool b1 = ((
			(m.m01 == m.m02) && (m.m02 == m.m03) && (m.m10 == m.m12) && (m.m12 == m.m13) &&
			(m.m20 == m.m21) && (m.m21 == m.m23) && (m.m30 == m.m31) && (m.m31 == m.m32) && (m.m32 == 0.0f)
		) && (
			(m.m00 == m.m11) && (m.m11 == m.m22) && (m.m22 == m.m33) && (m.m33 == 1)
		));

	bool b2 = (Mat4Determinant(m) == 1.0f);

	return (b1 || b2);
}

// Multiply one matrix by another
inline void Mat4Multiply(matrix4 &m, const matrix4 &m1, const matrix4 &m2) {
	matrix4 mOut;

	Mat4Null(mOut);

	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 4; j++)
			for (unsigned int k = 0; k < 4; k++)
				mOut.e2[i][j] += m1.e2[i][k] * m2.e2[k][j];

	m = mOut;
}

// Multiply a matrix by a scalar
template<class T> inline void Mat4MultiplyScalar(matrix4 &mOut, const matrix4 &m, const T tScalar) {
	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 4; j++)
			mOut.e2[i][j] = m.e2[i][j] * tScalar;
}

// To get the transpose of a matrix.
inline void Mat4Transpose(matrix4 &mOut, const matrix4 &m) {
	matrix4 mRet;

	for (unsigned int i = 0; i < 4; i ++)
		for (unsigned int j = 0; j < 4; j ++)
			mRet.e2[j][i] = m.e2[i][j];

	mOut = mRet;
}

// To get the inverse of a matrix (such that (inv * mat) OR (mat * inv) = identity matrix).
inline void Mat4Inverse(matrix4 &mOut, const matrix4 &m) {
	const float fDet = Mat4Determinant(m);

	if (fDet != 0) {
		matrix4 mAdj;

		for (unsigned int i = 0; i < 4; i ++)
			for (unsigned int j = 0; j < 4; j ++)
				mAdj.e2[i][j] = powf(-1, float(i + j)) * Mat4Determinant_EliminateRC(m, i, j);

		Mat4Transpose(mAdj, mAdj);
		Mat4MultiplyScalar(mOut, mAdj, 1 / fDet);
	} else {
		Mat4Null(mOut);	// can't get an inverse (Null matrix?)
	}
}

// To get a transform matrix by translation.
template<class T> inline void Mat4Transform_Translation(matrix4 &mRet, const T x, const T y, const T z) {
	Mat4Identity(mRet);

	mRet.m30 = (float)x;
	mRet.m31 = (float)y;
	mRet.m32 = (float)z;
}

// To get a transform matrix by translation (array of numbers).
template<class T> inline void Mat4Transform_Translation(matrix4 &mOut, const T *pos)  {
	Mat4Transform_Translation(mOut, (*pos)[0], (*pos)[1], (*pos)[2]);
}

// To get a transform matrix by rotation on X axis.
template<class T> inline void Mat4Transform_RotateX(matrix4 &mRet, const T val) {
	Mat4Identity(mRet);

	mRet.m12  = float( sin(val) * mRet.m11						);
	mRet.m21  = float(-sin(val) * mRet.m22						);
	mRet.m11 *= float(cos(val)									);
	mRet.m22 *= float(cos(val)									);
	mRet.m31  = float(cos(val) * mRet.m31 - sin(val) * mRet.m32	);
	mRet.m32  = float(cos(val) * mRet.m32 + sin(val) * mRet.m31	);
}

// To get a transform matrix by rotation on Y axis
template<class T> inline void Mat4Transform_RotateY(matrix4 &mRet, const T val) {
	Mat4Identity(mRet);

	mRet.m20  = float( sin(val) * mRet.m22						);
	mRet.m02  = float(-sin(val) * mRet.m00						);
	mRet.m00 *= float(cos(val)									);
	mRet.m22 *= float(cos(val)									);
	mRet.m30  = float(cos(val) * mRet.m30 + sin(val) * mRet.m32	);
	mRet.m32  = float(cos(val) * mRet.m32 - sin(val) * mRet.m30	);
}

// To get a transform matrix by rotation on Z axis
template<class T> inline void Mat4Transform_RotateZ(matrix4 &mRet, const T val) {
	Mat4Identity(mRet);

	mRet.m01  = float( sin(val) * mRet.m00						);
	mRet.m10  = float(-sin(val) * mRet.m11						);
	mRet.m00 *= float(cos(val)									);
	mRet.m11 *= float(cos(val)									);
	mRet.m31  = float(cos(val) * mRet.m31 + sin(val) * mRet.m30	);
	mRet.m30  = float(cos(val) * mRet.m30 - sin(val) * mRet.m31	);
}

// To get a transform matrix by scaling on X, Y, and Z axii.
template<class T> inline void Mat4Transform_Scaling(matrix4 &mRet, const T x, const T y, const T z) {
	Mat4Identity(mRet);

	mRet.m00 = float(mRet.m00 * x);	mRet.m30 = float(mRet.m30 * x);
	mRet.m11 = float(mRet.m11 * y);	mRet.m31 = float(mRet.m31 * y);
	mRet.m22 = float(mRet.m22 * z);	mRet.m32 = float(mRet.m32 * z);
}

// To get a transform matrix by scaling on X, Y, and Z axii (by an array of numbers).
template<class T> inline void Mat4Transform_Scaling(matrix4 &mOut, const T *pos) {
	Mat4Transform_Scaling(mOut, (*pos)[0], (*pos)[1], (*pos)[2]);
}

// To get the transformed components of a 4D vector from a transform matrix.
template<class T> inline void Mat4Transform(const matrix4 &m, T &x, T &y, T &z, T &w) {
	const T vX = x; const T vY = y; const T vZ = z;

	x = (T)m.m00 * vX + (T)m.m10 * vY + (T)m.m20 * vZ + (T)m.m30;
	y = (T)m.m01 * vX + (T)m.m11 * vY + (T)m.m21 * vZ + (T)m.m31;
	z = (T)m.m02 * vX + (T)m.m12 * vY + (T)m.m22 * vZ + (T)m.m32;
	w = (T)m.m03 * vX + (T)m.m13 * vY + (T)m.m23 * vZ + (T)m.m33;
}

// To get the array of transformed components of a 4D vector from a transform matrix.
template<class T> inline void Mat4Transform(const matrix4 &m, T *pos) {
	Mat4Transform(m, (*pos)[0], (*pos)[1], (*pos)[2], (*pos)[3]);
}

// To get the transformed components of a 3D vector from a transform matrix.
template<class T> inline void Mat4Transform_Coord(const matrix4 &m, T &x, T &y, T &z) {
	const T vX = x; const T vY = y; const T vZ = z;

	x = (T)m.m00 * vX + (T)m.m10 * vY + (T)m.m20 * vZ + (T)m.m30;
	y = (T)m.m01 * vX + (T)m.m11 * vY + (T)m.m21 * vZ + (T)m.m31;
	z = (T)m.m02 * vX + (T)m.m12 * vY + (T)m.m22 * vZ + (T)m.m32;
}

// To get the array of transformed components of a 3D vector from a transform matrix.
template<class T> inline void Mat4Transform_Coord(const matrix4 &m, T *pos) {
	Mat4Transform_Coord(m, (*pos)[0], (*pos)[1], (*pos)[2]);
}

// To get the transformed components of a 3D normal from a transform matrix.
template<class T> inline void Mat4Transform_Normal(const matrix4 &m, T &x, T &y, T &z) {
	const T vX = x; const T vY = y; const T vZ = z;

	x = (T)m.m00 * vX + (T)m.m10 * vY + (T)m.m20 * vZ;
	y = (T)m.m01 * vX + (T)m.m11 * vY + (T)m.m21 * vZ;
	z = (T)m.m02 * vX + (T)m.m12 * vY + (T)m.m22 * vZ;
}

// To get the array of transformed components of a 3D normal from a transform matrix.
template<class T> inline void Mat4Transform_Normal(const matrix4 &m, T *pos) {
	Mat4Transform_Normal(m, (*pos)[0], (*pos)[1], (*pos)[2]);
}

#endif	//	__MATRIX_TRANSFORM_H_