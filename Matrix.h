#pragma once 

#include "Engine.h"

namespace Batman
{
	//struct inherits from D3DXMATRIX 
	struct Matrix :public D3DXMATRIX {
	public:
		//Constructors and Copy Constructors 
		Matrix();
		Matrix(const Matrix&);
		Matrix(const D3DXMATRIX&);
		Matrix& operator=(const Matrix&);
		Matrix& operator=(const D3DXMATRIX&);

		//set zero and identity matrices
		void setZero();
		void setIdentity();

		//Fill matrices with values 
		void Fill(int);
		void Fill(float);
		void Fill(double);

		//Translate (move object position) 
		void Translate(float x, float y, float z);
		void Translate(Vector3&);

		//Roitate on Yaw / Pitch / Roll 
		void rotateYawPitchRoll(float x, float y, float z);
		void rotateYawPitchRoll(Vector3&);

		//Rotate on axes 
		void rotateX(float);
		void rotateY(float);
		void rotateZ(float);

		//Scale 
		void Scale(float x, float y, float z);
		void Scale(Vector3&);
	};
};