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

void ParticleController::Emit(int x, int y)
{
	std::lock_guard<std::mutex> lock(emitMutex);
	for (int i = 0; i < particlesPerSystem; i++)
	{
		particles[realUpdatedParticleId(nextInactiveParticleId)].Init(ParticleSettings(x, y));
		nextInactiveParticleId = getNextId(nextInactiveParticleId);
	}
}

void ParticleController::Update(float dt)
{
	for (int i = 0; i < particlesTotal; i++)
	{
		Vector2 spawnPosition;
		if (particles[realUpdatedParticleId(i)].Update(dt, spawnPosition))
			Emit(spawnPosition);
	}

	SwapUpdateBuffer();
}

void ParticleController::SwapUpdateBuffer()
{
	std::lock_guard<std::mutex> lock(swapMutex);
	readyBufferIndex = currentBufferIndex;
	std::swap(currentBufferIndex, nextBufferIndex);
	
	std::memcpy(&particles[currentBufferIndex * particlesTotal], &particles[readyBufferIndex * particlesTotal], sizeof Particle * particlesTotal);
}

void ParticleController::Render()
{
	for (int i = 0; i < particlesTotal; i++)
	{
		particles[realRenderedParticleId(i)].Render();
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
