#include "Engine.h"
#include <boost/timer.hpp>

namespace Batman
{
	//constructor - call reset mehtod
	Timer::Timer(void)
	{
		Reset();
	}

	//destructor
	Timer::~Timer(void) {}

	double Timer::getElapsed()
	{
		//set elapsed as boost timer elapsed value
		//converted to ms from seconds
		double elapsed = p_timer.elapsed() * 1000;
		return elapsed;
	}

	void Timer::Rest(double ms)
	{
		//start at current elapsed time
		double start = getElapsed();

		//loop with sleep command for as
		//many ms as specified
		while (start + ms > getElapsed())
		{
			Sleep(1);
		}
	}

	void Timer::Reset()
	{
		//set stopwatch time at current
		//elapsed time
			stopwatch_start = getElapsed();
	}

	bool Timer::Stopwatch(double ms)
	{
		//checks to see if ms time has passed 
		if (getElapsed() > stopwatch_start + (double)ms)
		{
			Reset();
			return true;
		}
		else return false;
	}
};