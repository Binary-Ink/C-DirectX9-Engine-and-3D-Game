#pragma once

#include "Engine.h"

namespace Batman
{
	class LogFile
	{
	public:
		//Constructor
		LogFile();
		//Destructor
		~LogFile();
		//Print operators
		void Print(std::string s);
		void Print(char c[]);
		/* overloaded operators for streaming to file 
			(Hence ofstream for file stream output)
			using the << makes it avalogous to cout
			the different data types all need to be included
			separately hence the different inputs
		*/
		std::ofstream& operator << (char c[]);
		std::ofstream& operator << (std::string s);
		std::ofstream& operator << (double d);
		std::ofstream& operator << (float f);
		std::ofstream& operator << (int i);
		std::ofstream& operator << (bool b);
	};
};
//call the class debug and make it globall accessible
//in the batman namespace
static Batman::LogFile debug;