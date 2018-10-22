#pragma once
#include <corecrt_math.h>

class Vector2
{
public:
	float _x;
	float _y;

	Vector2() : _x(0), _y(0) {}
	Vector2(float x, float y) : _x(x), _y(y) {}

	Vector2& operator += (const Vector2& v)
	{
		_x += v._x;
		_y += v._y;
		return *this;
	}

	Vector2& operator *= (const float& f)
	{
		_x *= f;
		_y *= f;
		return *this;
	}
};

inline Vector2 operator * (const Vector2& v, const float s)
{
	return Vector2(v._x * s, v._y * s);
}