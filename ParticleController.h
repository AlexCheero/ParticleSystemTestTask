#pragma once
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

	std::mutex swapMutex;
	int renderBufferIndex = 2;
	int readyBufferIndex = 2;
	int currentBufferIndex = 0;
	int nextBufferIndex = 1;

	int getNextId(int id, int steps = 1) { return (id + steps) % particlesTotal; }
	int getActiveParticlesCount()
	{
		return firstActiveParticleId < nextInactiveParticleId ?
			   nextInactiveParticleId - firstActiveParticleId :
			   particlesTotal - firstActiveParticleId + nextInactiveParticleId;
	}

	void SwapUpdateBuffer();
	void SwapRenderBuffer();

	int realUpdatedParticleId(int id) { return id + currentBufferIndex * particlesTotal; }
	int realRenderedParticleId(int id) { return id + renderBufferIndex * particlesTotal; }
public:
	ParticleController(int systemsCount, int particlesCount);
	~ParticleController();

	void Emit(int x, int y);
	void Emit(Vector2 position)
	{
		Emit(position._x, position._y);
	}

	void Update(float dt);
	void Render();
};

