#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

struct Timer
{
	Timer()
	{
		QueryPerformanceFrequency(&tf);
		QueryPerformanceCounter(&t0);
		frameNum = 0;
		lastFrame = 0;
		fps = 0;
		frameTime = 0.0f;
	}

	float Getfps()
	{
		// Timer
		++frameNum;
		LARGE_INTEGER t1;
		QueryPerformanceCounter(&t1);
		if(t1.QuadPart - t0.QuadPart >= tf.QuadPart/16) 
		{
			double frameCount = static_cast<double>(frameNum - lastFrame);
			double timePerFrame = (t1.QuadPart - t0.QuadPart) / frameCount;

			frameTime = static_cast<FLOAT>(timePerFrame / tf.QuadPart);
			fps = static_cast<UINT>((tf.QuadPart / timePerFrame) + 0.5);
			t0 = t1;
			lastFrame = frameNum;
		}

	}

	LARGE_INTEGER t0, tf;
	UINT frameNum ;
	UINT lastFrame ;
	UINT fps;
	FLOAT frameTime;
};
// Timer

#endif