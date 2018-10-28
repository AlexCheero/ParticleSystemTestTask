#pragma once
#include "Particle.h"
#include <mutex>

class ParticleController
{
	void* arena;
	Particle* particles;
	int particlesTotal;
	int particlesPerSystem;
	int firstActiveParticleIds[3] = { 0, 0, 0 };
	int nextInactiveParticleIds[3] = { 0, 0, 0 };
	int activeParticlesCounts[3] = { 0, 0, 0 };
	std::mutex emitMutex;

	std::mutex swapMutex;
	int renderBufferIndex = 2;
	int readyBufferIndex = 2;
	int currentBufferIndex = 0;
	int nextBufferIndex = 1;

	const float spawnProbability = 0.05f;

	int getNextId(int id, int steps = 1) { return (id + steps) % particlesTotal; }

	void SwapUpdateBuffer();
	void SwapRenderBuffer();

	int realUpdatedParticleId(int id) { return id + currentBufferIndex * particlesTotal; }
	int realRenderedParticleId(int id) { return id + renderBufferIndex * particlesTotal; }

	void UpdatePart(float dt, float time, int start, int end);
	void UpdateParticle(Particle& particle, float dt, float time);
public:
	ParticleController(int systemsCount, int particlesCount);
	~ParticleController();

	void Emit(int x, int y, float time);
	void Emit(Vector2 position, float time)
	{
		Emit(position._x, position._y, time);
	}

	void Update(float dt, float time);
	void Render();
};

