#pragma once

#ifndef M_PI
#define M_PI  3.14159265358979
#endif

#include <vector>
#include <base/Math.hpp>

// http://stackoverflow.com/questions/2270726/how-to-determine-the-size-of-an-array-of-strings-in-c
template <typename T, size_t N>
char (&static_sizeof_array( T(&)[N] ))[N]; // declared, not defined
#define SIZEOF_ARRAY( x ) sizeof(static_sizeof_array(x))

// Axis must be unit length. Angle in radians.
inline FW::Mat4f rotation4f( const FW::Vec3f& axis, float angle )
{
    FW::Mat3f R = FW::Mat3f::rotation(axis, angle);
    // Copy to homogeneous matrix
    FW::Mat4f H;
    H.setIdentity();
    for (unsigned i = 0; i < 3; ++i)
        for (unsigned j = 0; j < 3; ++j)
            H(i, j) = R(i, j);
    return H;
}

inline FW::Mat4f translation(const FW::Vec3f& t)
{
    FW::Mat4f M;
    M.setIdentity();
    M.setCol(3, t.toHomogeneous());
    return M;
}


inline FW::Mat4f makeMat4f( float m00, float m01, float m02, float m03,
                            float m10, float m11, float m12, float m13,
                            float m20, float m21, float m22, float m23,
                            float m30, float m31, float m32, float m33)
{
    FW::Mat4f A;
    A.m00 = m00; A.m01 = m01; A.m02 = m02; A.m03 = m03;
    A.m10 = m10; A.m11 = m11; A.m12 = m12; A.m13 = m13;
    A.m20 = m20; A.m21 = m21; A.m22 = m22; A.m23 = m23;
    A.m30 = m30; A.m31 = m31; A.m32 = m32; A.m33 = m33;
    return A;
}

inline FW::Mat3f makeMat3f( float m00, float m01, float m02,
                            float m10, float m11, float m12,
                            float m20, float m21, float m22)
{
    FW::Mat3f A;
    A.m00 = m00; A.m01 = m01; A.m02 = m02;
    A.m10 = m10; A.m11 = m11; A.m12 = m12;
    A.m20 = m20; A.m21 = m21; A.m22 = m22;
    return A;
}

inline FW::Mat3f vecMultTran(float x, float y, float z)
{
	FW::Mat3f returnMatrix = makeMat3f(	x*x, x*y, x*z,
									y*x, y*y, y*z,
									z*x, z*y, z*z);
	return returnMatrix;
}
