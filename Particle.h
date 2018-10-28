#pragma once
#include "Vector2.h"
#include "test.h"
#include <tuple>

class Color
{
	std::tuple<float, float, float, float> colorTuple;

public:
	Color(float r = 1, float g = 1, float b = 1, float a = 1) : colorTuple(r, g, b, a) {}

	float r() { return std::get<0>(colorTuple); }

	float g() { return std::get<1>(colorTuple); }
	
	float b() { return std::get<2>(colorTuple); }
	
	float a() { return std::get<3>(colorTuple); }
};

const double PI = 3.14159;
const float maxInitialVelocity = 100;
const float minLifeTime = 0.5f;
const float maxLifeTime = 5;
const float minGravity = 50;
const float maxGravity = 200;

class ParticleSettings
{
	static float Randomize(float from, float to)
	{
		return from + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}

public:
	Vector2 position;
	Vector2 velocity;
	float lifeTime;
	float maxTime;
	float gravity;
	Color color;

	ParticleSettings() : lifeTime(0), gravity(0) { }

	ParticleSettings(int x, int y)
		: position(x, y), lifeTime(Randomize(minLifeTime, maxLifeTime)), maxTime(maxLifeTime), gravity(Randomize(minGravity, maxGravity)),
		  // color(Randomize(0, 1), Randomize(0, 1), Randomize(0, 1), Randomize(0.5f, 1))
		  color(1, 1, 1, 1)
	{
		float angle = Randomize(0, 360) * PI / 180.0;
		velocity = Vector2(cos(angle), sin(angle));
		velocity *= Randomize(0, maxInitialVelocity);
	}

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
	bool alive = false;

	static bool IsVisible(Vector2 position)
	{
		if (position._x < 0 || position._x > test::SCREEN_WIDTH)
			return false;
		if (position._y < 0 || position._y > test::SCREEN_HEIGHT)
			return false;
		return true;
	}

public:
	const ParticleSettings& GetSettings() const { return _settings; }

	bool isExactlyDead() { return _settings.maxTime <= 0; }

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

	bool Update(float dt, Vector2& spawnPosition)
	{
		_settings.maxTime -= dt;
		if (!alive)
			return false;

		if (!IsVisible(_settings.position))
		{
			Kill();
			return false;
		}

		if (_settings.lifeTime <= 0)
		{
			if (rand() % 101 + 1 <= spawnProbability * 100)
			{
				spawnPosition = { _settings.position._x, _settings.position._y };
				Kill();
				return IsVisible(spawnPosition);
			}

			Kill();
			return false;
		}

		_settings.position += _settings.velocity * dt;
		_settings.velocity._y -= _settings.gravity * dt;
		_settings.lifeTime -= dt;
		return false;
	}

	void Render()
	{
		if (!alive)
			return;
		platform::drawPoint(_settings.position._x, _settings.position._y, _settings.color.r(),
			_settings.color.g(), _settings.color.b(), _settings.color.a());
	}
};

