#pragma once

#include "Particle.h"

#include <mutex>
#include <algorithm>
#include <vector>
#include <future>

class ParticleController
{
private:
	void* arena = nullptr;
	Particle* particles = nullptr;
	int particlesTotal = 0;
	int particlesPerSystem = 0;
	int firstActiveParticleIds[3] = { 0, 0, 0 };
	int nextInactiveParticleIds[3] = { 0, 0, 0 };
	int activeParticlesCounts[3] = { 0, 0, 0 };
	
	std::mutex mutex;
	int renderBufferIndex = 2;
	int readyBufferIndex = 2;
	int currentBufferIndex = 0;
	int nextBufferIndex = 1;

	std::vector<std::future<void>> futures;

	const float spawnProbability = 0.05f;

	int getNextId(int id, int steps = 1) { return (id + steps) % particlesTotal; }

	void SwapUpdateBuffer();
	void SwapRenderBuffer();

	int realUpdatedParticleId(int id) { return id + currentBufferIndex * particlesTotal; }
	int realRenderedParticleId(int id) { return id + renderBufferIndex * particlesTotal; }

	int GetThreadsCount(int particleCount)
	{
		const int minPerThread = 64;
		const int desirableThreads = particleCount / minPerThread;
		int hardwareThreads = std::thread::hardware_concurrency();
		hardwareThreads -= 2;//for update and render threads
		const int maxThreads = hardwareThreads > 0 ? hardwareThreads : 2;
		return std::min(desirableThreads, maxThreads);
	}

	void UpdatePart(float dt, float time, int start, int end);
	void UpdateParticle(Particle& particle, float dt, float time);
public:
	ParticleController(int systemsCount, int particlesCount);
	~ParticleController();

	void Emit(int x, int y, float time);
	void Emit(Vector2 position, float time)
	{
		Emit(position.x, position.y, time);
	}

	void Update(float dt, float time);
	void Render();
};

