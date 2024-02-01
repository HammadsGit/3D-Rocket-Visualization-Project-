
#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f result{};

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.v[i * 4 + j] = 0.0;
			for (int k = 0; k < 4; ++k) {
				result.v[i * 4 + j] += aLeft.v[i * 4 + k] * aRight.v[k * 4 + j];
			}
		}
	}
	return result;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f result{};
	for (int i = 0; i < 4; ++i) {
		result[i] = 0.0;
		for (int j = 0; j < 4; ++j) {
			result[i] += aLeft.v[i * 4 + j] * aRight[j];
		}
	}
	return result;
}

constexpr
Mat44f operator*( Mat44f const& aLeft, float aRight) noexcept
{
	Mat44f result = aLeft;
	for (int i = 0; i < 16; i++) {
		result.v[i] = result.v[i] * aRight;
	}

	return result;
}

// Functions:

Mat44f invert( Mat44f const& aM ) noexcept;

inline
Mat44f transpose( Mat44f const& aM ) noexcept
{
	Mat44f ret;
	for( std::size_t i = 0; i < 4; ++i )
	{
		for( std::size_t j = 0; j < 4; ++j )
			ret(j,i) = aM(i,j);
	}
	return ret;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f rotation{ {
		1.f, 0.f, 0.f, 0.f,
		0.f, cos(aAngle), -sin(aAngle), 0.f,
		0.f, sin(aAngle), cos(aAngle), 0.f,
		0.f, 0.f, 0.f, 1.f
		} };

	return rotation;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f rotation{ {
	cos(aAngle), 0.f, sin(aAngle), 0.f,
	0.f, 1.f, 0.f, 0.f,
	-sin(aAngle), 0, cos(aAngle), 0.f,
	0.f, 0.f, 0.f, 1.f
	} };

	return rotation;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f rotation{ {
			cos(aAngle), -sin(aAngle), 0.f, 0.f,
			sin(aAngle), cos(aAngle), 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
			} };

	return rotation;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f translation{ {
			1.f, 0.f, 0.f, aTranslation.x,
			0.f, 1.f, 0.f, aTranslation.y,
			0.f, 0.f, 1.f, aTranslation.z,
			0.f, 0.f, 0.f, 1.f
			} };

	return translation;
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	Mat44f scaling{ {
		aSX, 0.f, 0.f, 0.f,
		0.f, aSY, 0.f, 0.f,
		0.f, 0.f, aSZ, 0.f,
		0.f, 0.f, 0.f, 1.f
		} };

	return scaling;
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float s = 1 / (tan(aFovInRadians / 2));
	float sx = s / aAspect;
	float sy = s;
	float a = -1 * ((aFar + aNear) / (aFar - aNear));
	float b = -2 * ((aFar * aNear) / (aFar - aNear));
	Mat44f perspective{ {
		sx, 0.f, 0.f, 0.f,
		0.f, sy, 0.f, 0.f,
		0.f, 0.f, a, b,
		0.f, 0.f, -1.f, 0.f
		} };

	return perspective;
}




#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
