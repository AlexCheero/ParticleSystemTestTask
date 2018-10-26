#pragma once
#include <cstdlib>
#include "Particle.h"
#include <mutex>

class ParticleController
{
	void* arena;
	Particle* particles;
	int particlesTotal;
	int particlesPerSystem;
	int nextInactiveParticleId = 0;
	int firstActiveParticleId = 0;
	std::mutex emitMutex;

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
		std::lock_guard<std::mutex> lock(emitMutex);
		for (int i = 0; i < particlesPerSystem; i++)
		{
			particles[nextInactiveParticleId].Init(ParticleSettings(x, y));
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
			Vector2 spawnPosition;
			if (particles[i].Update(dt, spawnPosition))
				Emit(spawnPosition);
		}
	}

	void Render()
	{
		for (int i = 0; i < particlesTotal; i++)
			particles[i].Render();
	}
};

