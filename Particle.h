#pragma once

#include "Vector2.h"
#include "test.h"

#include <tuple>

class Color
{
private:
	float r;
	float g;
	float b;
	float a;

public:
	Color(float rCol = 1, float gCol = 1, float bCol = 1, float aCol = 1) : r(rCol), g(gCol), b(bCol), a(aCol) {}

	float R() { return r; }
	float G() { return g; }
	float B() { return b; }
	float A() { return a; }
};

const double PI = 3.14159;
const float maxInitialVelocity = 100;
const float minLifeTime = 0.5f;
const float maxLifeTime = 5;
const float minGravity = 50;
const float maxGravity = 200;

class ParticleSettings
{
private:
	static float Randomize(float from, float to)
	{
		return from + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}

public:
	Vector2 position;
	Vector2 velocity;
	float lifeTime;
	float birthTime;
	float gravity;
	Color color;

	ParticleSettings() : lifeTime(0), gravity(0) { }

	ParticleSettings(int x, int y, float time)
		: position(x, y), lifeTime(Randomize(minLifeTime, maxLifeTime)), birthTime(time), gravity(Randomize(minGravity, maxGravity)),
		  color(Randomize(0, 1), Randomize(0, 1), Randomize(0, 1), Randomize(0.5f, 1))
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
private:
	float spawnProbability = 0.05f;
	ParticleSettings _settings;
	bool alive = false;

public:
	const ParticleSettings& GetSettings() const { return _settings; }

	bool IsAlive() { return alive; }

	bool IsVisible(const Vector2& position)
	{
		if (position.x < 0 || position.x > test::SCREEN_WIDTH)
			return false;
		if (position.y < 0 || position.y > test::SCREEN_HEIGHT)
			return false;
		return true;
	}

	bool IsVisible() { return IsVisible(_settings.position);  }

	bool IsDeadByTime(float time) { return _settings.birthTime > 0 && time > _settings.birthTime + _settings.lifeTime; }

	bool IsExactlyDead(float time) { return _settings.birthTime > 0 && time > _settings.birthTime + maxLifeTime; }

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

	void UpdatePosition(float dt)
	{
		_settings.position += _settings.velocity * dt;
		_settings.velocity.y -= _settings.gravity * dt;
	}

	void Render()
	{
		if (!alive || !IsVisible())
			return;
		platform::drawPoint(_settings.position.x, _settings.position.y, _settings.color.R(),
			_settings.color.G(), _settings.color.B(), _settings.color.A());
	}
};

