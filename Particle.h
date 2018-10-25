#pragma once
#include "Vector2.h"
#include "test.h"
#include <tuple>
// #include "ParticleController.h"

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
	float spawnProbability = 0.05f;
	ParticleSettings _settings;

public:
	static bool ValidatePosition(Vector2 position)
	{
		if (position._x < 0 || position._x > test::SCREEN_WIDTH)
			return false;
		if (position._y < 0 || position._y > test::SCREEN_HEIGHT)
			return false;
		return true;
	}

	bool alive = false;

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

	Vector2 Update(float dt)
	{
		if (!alive)
			return { -1, -1 };

		if (!ValidatePosition(_settings.position))
		{
			Kill();
			return { -1, -1 };
		}

		if (_settings.lifeTime <= 0)
		{
			if (rand() % 101 + 1 <= spawnProbability * 100)
			{
				Vector2 spawnPosition { _settings.position._x, _settings.position._y };
				Kill();
				return spawnPosition;
			}

			Kill();
			return { -1, -1 };
		}

		_settings.position += _settings.velocity * dt;
		_settings.velocity._y -= _settings.gravity * dt;
		_settings.lifeTime -= dt;
		return { -1, -1 };
	}

	void Render()
	{
		if (!alive)
			return;
		platform::drawPoint(_settings.position._x, _settings.position._y, _settings.color.r(),
			_settings.color.g(), _settings.color.b(), _settings.color.a());
	}
};

