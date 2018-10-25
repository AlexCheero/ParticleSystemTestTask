#pragma once
#include <cstdlib>
#include "Particle.h"

class ParticleSettingsFactory
{
	const double PI = 3.14159;

	float maxInitialVelocity = 100;
	float minLifeTime = 0.5f;
	float maxLifeTime = 5;
	float minGravity = 50;
	float maxGravity = 200;

	static float Randomize(float from, float to)
	{
		return from + static_cast<float>(rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}

public:
	ParticleSettings RandomizeSettingsForPosition(int x, int y)
	{
		Vector2 pos(x, y);
		float angle = Randomize(0, 360) * PI / 180.0;
		Vector2 vel(cos(angle), sin(angle));
		vel *= Randomize(0, maxInitialVelocity);
	
		float lifeTime = Randomize(minLifeTime, maxLifeTime);
		float grav = Randomize(minGravity, maxGravity);
		Color col(Randomize(0, 1), Randomize(0, 1), Randomize(0, 1), Randomize(0.5f, 1));
		return { pos, vel, lifeTime, grav, col };
	}
};

class ParticleController
{
	void* arena;
	Particle* particles;
	int particlesTotal;
	int particlesPerSystem;
	int nextInactiveParticleId = 0;
	int firstActiveParticleId = 0;
	ParticleSettingsFactory settingsFactory;

	int getNextId(int id, int steps = 1) { return (id + steps) % particlesTotal; }
	int getActiveParticlesCount()
	{
		return firstActiveParticleId < nextInactiveParticleId ?
			   nextInactiveParticleId - firstActiveParticleId :
			   particlesTotal - firstActiveParticleId + nextInactiveParticleId;
	}
public:
	ParticleController(int systemsCount, int particlesCount)
	{
		particlesPerSystem = particlesCount;
		particlesTotal = systemsCount * particlesCount;
		arena = malloc(sizeof(Particle) * particlesTotal);
		Particle** ptrs = new Particle*[particlesTotal];
		for (int i = 0; i < particlesTotal; i++)
			ptrs[i] = new(reinterpret_cast<Particle*>(arena) + i)Particle;
		particles = ptrs[0];
		delete[]ptrs;
	}

	~ParticleController()
	{
		for (int i = 0; i < particlesTotal; i++)
			particles[i].~Particle();
		free(arena);
		arena = nullptr;
		particles = nullptr;
	}

	void Emit(int x, int y)
	{
		for (int i = 0; i < particlesPerSystem; i++)
		{
			particles[nextInactiveParticleId].Init(settingsFactory.RandomizeSettingsForPosition(x, y));
			nextInactiveParticleId = getNextId(nextInactiveParticleId);
		}
	}

	void Emit(Vector2 position)
	{
		Emit(position._x, position._y);
	}

	void Update(float dt)
	{
		for (int i = 0; i < particlesTotal; i++)
		{
			Vector2 spawnPosition = particles[i].Update(dt);
			if (Particle::ValidatePosition(spawnPosition))
			{
				Emit(spawnPosition);
				particles[i].Kill();
			}
		}
	}

	void Render()
	{
		for (int i = 0; i < particlesTotal; i++)
			particles[i].Render();
	}
};

