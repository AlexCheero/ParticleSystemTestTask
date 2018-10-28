#pragma once

class Vector2
{
public:
	float x;
	float y;

	Vector2() : x(0), y(0) {}
	Vector2(float xCoord, float yCoord) : x(xCoord), y(yCoord) {}

	Vector2& operator += (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2& operator *= (const float& f)
	{
		x *= f;
		y *= f;
		return *this;
	}
};

inline Vector2 operator * (const Vector2& v, const float s)
{
	return Vector2(v.x * s, v.y * s);
}