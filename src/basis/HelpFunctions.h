#pragma once

#ifndef M_PI
#define M_PI  3.14159265358979
#endif

#include <vector>
#include <base/Math.hpp>

inline static FW::Mat3f formBasis(const FW::Vec3f& n)
{
	FW::Vec3f q = n;
	float qmin = FW::min(abs(q.x), abs(q.y));
	qmin = FW::min(qmin, abs(q.z));

	for(int i = 0 ; i < 3 ; i++)
	{
		if(abs(q[i]) == qmin)
		{
			q[i] = 1;
		}
	}
	FW::Vec3f t = cross(q, n).normalized();
	FW::Vec3f b = cross(n, t).normalized();

	FW::Mat3f r;
	r.setCol(0, t);
	r.setCol(1, b);
	r.setCol(2, n);

	return r;
}

static inline FW::Vec2f toUnitDisk(FW::Vec2f onSquare)
{
	float phi, r,u,v;
	float a = 2 *  onSquare.x-1;
	float b = 2 *  onSquare.y-1;

	if (a > -b)
	{ // region 1 or 2
		if (a > b)
		{ // region 1, also |a| > |b|
			r = a;
			phi = (FW_PI/4 ) * (b/a);
		}
		else
		{ // region 2, also |b| > |a|
			r = b;
			phi = (FW_PI/4) * (2 - (a/b));
		}
	}
	else
	{ // region 3 or 4
		if (a < b)
		{ // region 3, also |a| >= |b|, a != 0
			r = -a;
			phi = (FW_PI/4) * (4 + (b/a));
		}
		else
		{ // region 4, |b| >= |a|, but a==0 and b==0 could occur.
		r = -b;
		if (b != 0)
		{
			phi = (FW_PI/4) * (6 - (a/b));
		}
		else{
			phi = 0;
		}
		}
	}


	u = r * cos( phi );
	v = r * sin( phi );
	return FW::Vec2f(u,v);
}

inline static FW::Mat4f makeMat4f( float m00, float m01, float m02, float m03,
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
