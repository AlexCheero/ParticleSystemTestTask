#pragma once
#include "Vector2.h"
#include "Particle.h"
#include <array>

const double PI = 3.14159;

class ParticleSystem
{
	std::array<Particle, 64> particles;

	float maxInitialVelocity = 100;
	float maxLifeTime = 5;
	float minGravity = 50;
	float maxGravity = 200;

	static float Randomize(float from, float to)
	{
		return from + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}

	ParticleSettings RandomizeSettings(int x, int y)
	{
		Vector2 pos(x, y);
		float angle = Randomize(0, 360) * PI / 180.0;
		Vector2 vel(cos(angle), sin(angle));
		vel *= Randomize(0, maxInitialVelocity);

		float lifeTime = Randomize(0, maxLifeTime);
		float grav = Randomize(minGravity, maxGravity);
		Color col(Randomize(0, 1), Randomize(0, 1), Randomize(0, 1), Randomize(0, 1));
		return { pos, vel, lifeTime, grav, col };
	}

public:
	ParticleSystem() {}
	~ParticleSystem()
	{
		for (Particle& particle : particles)
			particle.alive = false;
	}

	void Emit(int x, int y)
	{
		for (Particle& particle : particles)
			particle.Init(RandomizeSettings(x, y));
	}

	void Update(float dt)
	{
		for (Particle& particle : particles)
			particle.Update(dt);
	}

	void Render()
	{
		for (Particle& particle : particles)
			particle.Render();
	}
};

