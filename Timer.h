#pragma once
#include "Engine.h"
#include <boost/timer.hpp>				//boost libray

namespace Batman
{
	class Timer
	{
	private:
		boost::timer p_timer;			//timer instance
		double timer_start;				//start time
		double stopwatch_start;			//stopwatch start

	public:
		Timer(void);					//constructor
		~Timer(void);					//destructor
		double getElapsed();			//get elapsed time
		void Rest(double ms);			//rest for specified time
		void Reset();					//reset timer
		bool Stopwatch(double ms);		//true after defined time

	};
}
