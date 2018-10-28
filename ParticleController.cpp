#include "ParticleController.h"

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
	Particle* first = &particles[realUpdatedParticleId(nextInactiveParticleIds[currentBufferIndex])];
	for (int i = 0; i < particlesPerSystem; i++)
	{
		Particle& current = particles[realUpdatedParticleId(nextInactiveParticleIds[currentBufferIndex])];
		current.Init(ParticleSettings(x, y, time));

		if (current.GetSettings().lifeTime > first->GetSettings().lifeTime)
			std::swap(*first, current);

		nextInactiveParticleIds[currentBufferIndex] = getNextId(nextInactiveParticleIds[currentBufferIndex]);
	}
}

void ParticleController::Update(float dt, float time)
{
	for (int i = 0; i < particlesTotal; i++)
	{
		Particle& current = particles[realUpdatedParticleId(i)];
		UpdateParticle(current, dt, time);

		//skip inactive effect
		if (i % particlesPerSystem == 0 && current.GetSettings().lifeTime == 0)
		{
			// activeParticlesCounts[currentBufferIndex]
			i += particlesPerSystem - 1;
		}
	}

	SwapUpdateBuffer();
}

void ParticleController::UpdateParticle(Particle& particle, float dt, float time)
{
	if (!particle.IsAlive())
		return;

	if (particle.IsDeadByTime(time))
	{
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
}

void ParticleController::Render()
{
	for (int i = 0; i < particlesTotal; i++)
	{
		Particle& current = particles[realRenderedParticleId(i)];
		current.Render();

		//skip inactive effect
		if (i % particlesPerSystem == 0 && current.GetSettings().lifeTime == 0)
			i += particlesPerSystem - 1;
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
