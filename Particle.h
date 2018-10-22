#pragma once
#include "Vector2.h"
#include "test.h"
#include <tuple>

class Color
{
	std::tuple<float, float, float, float> colorTuple;

public:
	Color(float r, float g, float b, float a) : colorTuple(r, g, b, a) {}

	float r() { return std::get<0>(colorTuple); }

	float g() { return std::get<1>(colorTuple); }
	
	float b() { return std::get<2>(colorTuple); }
	
	float a() { return std::get<3>(colorTuple); }
};

class ParticleSettings
{
public:
	Vector2 position;
	Vector2 velocity;
	float lifeTime;
	float gravity;
	Color color;

	ParticleSettings(Vector2 pos = Vector2(), Vector2 vel = Vector2(), float time = 0, float grav = 1000.f, Color col = Color(1, 1, 1, 1))
		: position(pos), velocity(vel), lifeTime(time), gravity(grav), color(col)
	{}

	void Reset()
	{
		position = Vector2();
		velocity = Vector2();
		lifeTime = 0;
	}
};

class Particle
{
	ParticleSettings _settings;

public:
	bool alive;

	Particle() {}
	~Particle();

	void Kill()
	{
		alive = false;
		_settings.Reset();
	}

	void Init(ParticleSettings settings)
	{
		alive = true;
		_settings = settings;
	}

	void Update(float dt)
	{
		if (!alive)
			return;

		_settings.position += _settings.velocity * dt;
		_settings.velocity._y -= _settings.gravity * dt;
	}

	void Render()
	{
		if (!alive)
			return;
		platform::drawPoint(_settings.position._x, _settings.position._y, _settings.color.r(),
			_settings.color.g(), _settings.color.b(), _settings.color.a());
	}
};

