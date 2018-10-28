#include "ParticleController.h"
#include <future>

ParticleController::ParticleController(int systemsCount, int particlesCount)
{
	particlesPerSystem = particlesCount;
	particlesTotal = systemsCount * particlesCount;
	int realParticlesCount = particlesTotal * 3; //2 update and 1 render buffer
	arena = malloc(sizeof(Particle) * realParticlesCount);
	Particle** ptrs = new Particle*[realParticlesCount];
	for (int i = 0; i < realParticlesCount; i++)
		ptrs[i] = new(reinterpret_cast<Particle*>(arena) + i)Particle;
	particles = ptrs[0];
	delete[]ptrs;
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
	std::lock_guard<std::mutex> lock(emitMutex);
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
	int count = activeParticlesCounts[currentBufferIndex];
	int half = count / 2;

	std::future<void> first_half = std::async(&ParticleController::UpdatePart, this, dt, time, 0, half);
	UpdatePart(dt, time, half, count);
	first_half.get();

	SwapUpdateBuffer();
}

void ParticleController::UpdatePart(float dt, float time, int start, int end)
{
	int first = firstActiveParticleIds[currentBufferIndex];
	for (int index = start; index < end; index++)
	{
		int i = getNextId(first, index);

		Particle& current = particles[realUpdatedParticleId(i)];
		UpdateParticle(current, dt, time);
	}
}

void ParticleController::UpdateParticle(Particle& particle, float dt, float time)
{
	if (!particle.IsAlive())
		return;

	if (particle.IsDeadByTime(time))
	{
		{
			std::lock_guard<std::mutex> lock(swapMutex);
			firstActiveParticleIds[currentBufferIndex] = getNextId(firstActiveParticleIds[currentBufferIndex]);
			activeParticlesCounts[currentBufferIndex]--;
		}

		if (rand() % 101 + 1 <= spawnProbability * 100)
		{
			Vector2 spawnPosition = { particle.GetSettings().position._x, particle.GetSettings().position._y };
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
	std::lock_guard<std::mutex> lock(swapMutex);
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
		int i = getNextId(firstActiveParticleIds[renderBufferIndex], index);
		Particle& current = particles[realRenderedParticleId(i)];
		current.Render();
	}

	SwapRenderBuffer();
}

void ParticleController::SwapRenderBuffer()
{
	std::lock_guard<std::mutex> lock(swapMutex);
	if (renderBufferIndex != readyBufferIndex)
	{
		nextBufferIndex = renderBufferIndex;
		renderBufferIndex = readyBufferIndex;
	}
}
