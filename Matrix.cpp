#include "Engine.h" 

namespace Batman {
	//BEGIN: Constructors 

	//standard contsructor 
	Matrix::Matrix() :D3DXMATRIX() {}

	//Copy constructor
	Matrix::Matrix(const Matrix& M) : D3DXMATRIX(M) {
		*this = M;
	}

	//Copy constructor on D3DMATRIX 
	Matrix::Matrix(const D3DXMATRIX& M) : D3DXMATRIX(M) {
		*this = M;
	}

	//Operator overload copy constructor 
	Matrix& Matrix::operator=(const Matrix& M) {
		//copies details from memory 
		memcpy(m, &M, sizeof(float) * 16);
		return *this;
	}

	//Operator overload copy constructor on D3DMATRIX 
	Matrix& Matrix::operator=(const D3DXMATRIX& M) 
	{ 
		//copies details from memory 
		memcpy(m, &M, sizeof(float)*16); 
		return *this; } 
	//END: Constructors

  //set all entries to 0 
	void Matrix::setZero() {
		//use fill method 
		Fill(0.0f); 
	}

	//Set Identity matrix 
	void Matrix::setIdentity() {
		//set most elements to zero 
		m[0][1] = m[0][2] = m[0][3] =
			m[1][0] = m[1][2] = m[1][3] =
			m[2][0] = m[2][1] = m[2][3] =
			m[3][0] = m[3][1] = m[3][2] = 0.0f;

		//set diagonals 00, 11, 22, 33 to 1 
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	}

		//Fill method int 
	void Matrix::Fill(int value) {
		//use existing with float cast 
		Fill((float)value);
	}
		//fill all elements (float value) 
		void Matrix::Fill(float value) {
			m[0][0] = m[0][1] = m[0][2] = m[0][3] =
				m[1][0] = m[1][1] = m[1][2] = m[1][3] =
				m[2][0] = m[2][1] = m[2][2] = m[2][3] =
				m[3][0] = m[3][1] = m[3][2] = m[3][3] = value;
		}

		//Fill method double
		void Matrix::Fill(double value) { 
			//use existing with float cast 
			Fill((float)value); 
			}

		//Translate (move on x, y, z) 
		void Matrix::Translate(float x, float y, float z) { 
			//use built in D3D method to perform translation 
			D3DXMatrixTranslation((D3DXMATRIX*)this, x, y, z); } 

			//Translate (move on x,y,z using vector) 
		void Matrix::Translate(Vector3& V) { 
			//use existing method with float casts 
			Translate((float)V.x, (float)V.y, (float)V.z); 
		}

		//Rotate using Yaw / Pitch / Roll 
		void Matrix::rotateYawPitchRoll(float x, float y, float z) { 
			//use built in method 
			D3DXMatrixRotationYawPitchRoll( (D3DXMATRIX*) this, x, y, z); } 

		//rotate using Yaw / Pitch / Roll (in vector format) 
		void Matrix::rotateYawPitchRoll(Vector3&V) { 
		//use built in method with float casts 
			rotateYawPitchRoll((float)V.x, (float)V.y, (float)V.z); 
	}

	//rotate on X axis 
		void Matrix::rotateX(float x) {
			//use built in method with D3DMATRIX cast 
			D3DXMatrixRotationX((D3DXMATRIX*)this, x);
		}

	//rotate on Y axis
		void Matrix::rotateY(float y) { 
			//use built in method with D3DMATRIX cast 
			D3DXMatrixRotationY( (D3DXMATRIX*) this, y ); 
			}

	//rotate on Z axis 
		void Matrix::rotateZ(float z) { 
			//use built in method with D3DMATRIX cast 
			D3DXMatrixRotationZ( (D3DXMATRIX*) this, z ); 
		}

	//Scale on x, y, z 
		void Matrix::Scale(float x, float y, float z) { 
			//use built in method with D3DMATRIX cast 
			D3DXMatrixScaling( (D3DXMATRIX*) this, x, y, z ); 
	}

	//scale using vector 
		void Matrix::Scale(Vector3& V) {
			//use existing method with float casts 
			Scale((float)V.x, (float)V.y, (float)V.z);

		}
}