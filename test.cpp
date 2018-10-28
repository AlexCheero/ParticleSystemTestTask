#include <thread>
#include <mutex>
#include <atomic>

#include "./nvToolsExt.h"

#include "test.h"
#include "ParticleController.h"

static std::atomic<float> globalTime;
static volatile bool workerMustExit = false;

std::unique_ptr<ParticleController> particle_controller;
int maxEffectsCount = 2048;
int maxParticlesPerEffect = 64;

void WorkerThread(void)
{
	while (!workerMustExit)
	{
		nvtxRangePush(__FUNCTION__);

		static float lastTime = 0.f;
		float time = globalTime.load();
		float delta = time - lastTime;
		lastTime = time;

		float dt = delta / 1000; //delta time in seconds
		particle_controller->Update(dt, time / 1000/*time in seconds*/);

		if (delta < 10)
			std::this_thread::sleep_for(std::chrono::milliseconds(10 - static_cast<int>(delta*1000.f)));

		nvtxRangePop();
	}
}


void test::init(void)
{
	particle_controller = std::make_unique<ParticleController>(maxEffectsCount, maxParticlesPerEffect);
	std::thread workerThread(WorkerThread);
	workerThread.detach(); // Glut + MSVC = join hangs in atexit()
}

void test::term(void)
{
	workerMustExit = true;
}

void test::render(void)
{
	particle_controller->Render();
}

void test::update(int dt)
{
	float time = globalTime.load();
	globalTime.store(time + dt);
}

void test::on_click(int x, int y)
{
	particle_controller->Emit(x, SCREEN_HEIGHT - y, globalTime.load() / 1000 /*time in seconds*/);
}