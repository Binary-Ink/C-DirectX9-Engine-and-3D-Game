#pragma once

#include "Engine.h"

namespace Batman
{

	class BitmapFont : public Sprite {

	private:
		//widths for characters
		int p_widths[256];
	public:
		//constructor / dstructor
		BitmapFont();
		virtual ~BitmapFont(void) { }

		//Print to screen
		void Print(int x, int y, std::string text,
			Color color = Color(255, 255, 255, 255));

		//getters / setters (character width)
		int getCharWidth() { return (int)size.x; }
		int getCharHeight() { return (int)size.y; }
		void setCharWidth(int width) { size.x = width; }
		void setCharSize(int width, int height) {
			setCharWidth(width);
			size.y = height;

		}

		//load width data
		bool loadWidthData(std::string filename);
	};
}


