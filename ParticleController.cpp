#include "ParticleController.h"

ParticleController::ParticleController(int systemsCount, int particlesCount)
{
	particlesPerSystem = particlesCount;
	particlesTotal = systemsCount * particlesCount;
	const int realParticlesCount = particlesTotal * 3; //2 update and 1 render buffer
	arena = malloc(sizeof(Particle) * realParticlesCount);
	Particle** ptrs = new Particle*[realParticlesCount];
	for (int i = 0; i < realParticlesCount; i++)
		ptrs[i] = new(reinterpret_cast<Particle*>(arena) + i)Particle;
	particles = ptrs[0];
	delete[]ptrs;

	futures.reserve(GetThreadsCount(particlesTotal));
}

ParticleController::~ParticleController()
{
	for (int i = 0; i < particlesTotal; i++)
		particles[i].~Particle();
	free(arena);
	arena = nullptr;
	particles = nullptr;
}

void ParticleController::Emit(int x, int y, float time)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (int i = 0; i < particlesPerSystem; i++)
	{
		Particle& current = particles[realUpdatedParticleId(nextInactiveParticleIds[currentBufferIndex])];
		current.Init(ParticleSettings(x, y, time));

		nextInactiveParticleIds[currentBufferIndex] = getNextId(nextInactiveParticleIds[currentBufferIndex]);

		if (nextInactiveParticleIds[currentBufferIndex] == firstActiveParticleIds[currentBufferIndex])
			firstActiveParticleIds[currentBufferIndex] = getNextId(firstActiveParticleIds[currentBufferIndex]);
		else
			activeParticlesCounts[currentBufferIndex]++;
	}
}

void ParticleController::Update(float dt, float time)
{
	const int overallCount = activeParticlesCounts[currentBufferIndex];
	const int threadCount = GetThreadsCount(overallCount);
	const int countPerThread = overallCount / (threadCount + 1);

	for (int i = 0; i < threadCount; i++)
	{
		int start = i * countPerThread;
		int end = start + countPerThread;
		if (futures.size() < i + 1)
			futures.push_back(std::async(&ParticleController::UpdatePart, this, dt, time, start, end));
		else
			futures[i] = std::async(&ParticleController::UpdatePart, this, dt, time, start, end);
	}

	UpdatePart(dt, time, countPerThread * threadCount, overallCount);
	for (int i = 0; i < threadCount; i++)
		futures[i].get();

	SwapUpdateBuffer();
}

void ParticleController::UpdatePart(float dt, float time, int start, int end)
{
	const int first = firstActiveParticleIds[currentBufferIndex];
	for (int index = start; index < end; index++)
	{
		const int i = getNextId(first, index);

		Particle& current = particles[realUpdatedParticleId(i)];
		UpdateParticle(current, dt, time);
	}
}

void ParticleController::UpdateParticle(Particle& particle, float dt, float time)
{
	if (!particle.IsAlive())
	{
		if (particle.IsExactlyDead(time))
		{
			std::lock_guard<std::mutex> lock(mutex);
			firstActiveParticleIds[currentBufferIndex] = getNextId(firstActiveParticleIds[currentBufferIndex]);
			activeParticlesCounts[currentBufferIndex]--;
		}
		return;
	}

	if (particle.IsDeadByTime(time))
	{
		if (rand() % 101 + 1 <= spawnProbability * 100)
		{
			const Vector2 spawnPosition = { particle.GetSettings().position.x, particle.GetSettings().position.y };
			if (particle.IsVisible(spawnPosition))
				Emit(spawnPosition, time);
			particle.Kill();
		}

		particle.Kill();
		return;
	}

	particle.UpdatePosition(dt);
}

void ParticleController::SwapUpdateBuffer()
{
	std::lock_guard<std::mutex> lock(mutex);
	readyBufferIndex = currentBufferIndex;
	std::swap(currentBufferIndex, nextBufferIndex);
	
	std::memcpy(&particles[currentBufferIndex * particlesTotal], &particles[readyBufferIndex * particlesTotal], sizeof Particle * particlesTotal);
	nextInactiveParticleIds[currentBufferIndex] = nextInactiveParticleIds[readyBufferIndex];
	firstActiveParticleIds[currentBufferIndex] = firstActiveParticleIds[readyBufferIndex];
	activeParticlesCounts[currentBufferIndex] = activeParticlesCounts[readyBufferIndex];
}

void ParticleController::Render()
{
	for (int index = 0; index < activeParticlesCounts[renderBufferIndex]; index++)
	{
		const int i = getNextId(firstActiveParticleIds[renderBufferIndex], index);
		Particle& current = particles[realRenderedParticleId(i)];
		current.Render();
	}

	SwapRenderBuffer();
}

void ParticleController::SwapRenderBuffer()
{
	std::lock_guard<std::mutex> lock(mutex);
	if (renderBufferIndex != readyBufferIndex)
	{
		nextBufferIndex = renderBufferIndex;
		renderBufferIndex = readyBufferIndex;
	}
}
