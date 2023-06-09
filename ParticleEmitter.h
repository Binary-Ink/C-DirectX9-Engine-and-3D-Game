#pragma once
#include "Engine.h"
namespace Batman {

	class ParticleEmitter {
	private:
		//STL vector for particles
		std::vector<Batman::Sprite*> p_particles;

		//texture pointer
		Texture *p_image;

		//position vars
		Vector2 p_position;
		double p_direction;
		double p_length;

		//colour / amount / alpha variation

		int p_max;
		int p_alphaMin, p_alphaMax;
		int p_minR, p_minG, p_minB, p_maxR, p_maxG, p_maxB;

		//spread vars
		int p_spread;
		double p_velocity;
		double p_scale;
	public:
		//constructor / destructor
		ParticleEmitter();
		virtual ~ParticleEmitter();

		//load particle image
		bool Load(std::string imageFile);

		//render / update
		void Render(bool rotate);
		void Update(float deltaTime);

		//dynamica11y add particles
		void Add();

		//getters and setters

		//position

		void setPosition(double x, double y) { p_position.Set(x, y); }
		void setPosition(Vector2 vec) { p_position = vec; }

		Vector2 getPosition() { return p_position; }

		//direction

		void setDirection(double angle) { p_direction = angle; }
		double getDirection() { return p_direction; }

		//particle numbers
		int getCount() { return (int)p_particles.size(); }
		void setMax(int num) { p_max = num; }

		//a1pha / colour / spread

		void setAlphaRange(int nin, int max);
		void setColorRange(int r1, int g1, int b1, int r2, int g2, int b2);
		void setSpread(int value) { p_spread = value; }
		void setLength(double value) { p_length = value; }
		void setVelocity(double value) { p_velocity = value; }
		void setScale(double value) { p_scale = value; }
	};
}

