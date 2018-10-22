
#include <math.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "./nvToolsExt.h"

#include "test.h"
#include "ParticleSystem.h"


// Just some hints on implementation
// You could remove all of them

static std::atomic<float> globalTime;
static volatile bool workerMustExit = false;

ParticleSystem ps;

// some code

void WorkerThread(void)
{
	while (!workerMustExit)
	{
		nvtxRangePush(__FUNCTION__);

		static float lastTime = 0.f;
		float time = globalTime.load();
		float delta = time - lastTime;
		lastTime = time;

		// some code

		float dt = delta / 1000; //delta time in seconds
		ps.Update(dt);

		if (delta < 10)
			std::this_thread::sleep_for(std::chrono::milliseconds(10 - static_cast<int>(delta*1000.f)));

		nvtxRangePop();
	}
}


void test::init(void)
{
	// some code

	std::thread workerThread(WorkerThread);
	workerThread.detach(); // Glut + MSVC = join hangs in atexit()

	// some code
}

void test::term(void)
{
	// some code

	workerMustExit = true;

	// some code
}

void test::render(void)
{
	ps.Render();

	// some code

	// for (size_t i=0; i< .... ; ++i)
	//	platform::drawPoint(x, y, r, g, b, a);

	// some code
}

void test::update(int dt)
{
	// some code

	float time = globalTime.load();
	globalTime.store(time + dt);

	// some code
}

void test::on_click(int x, int y)
{
	// some code
	ps.Emit(x, SCREEN_HEIGHT - y);
}