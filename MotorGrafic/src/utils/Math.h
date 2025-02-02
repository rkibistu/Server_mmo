#pragma once
#include <ostream>

#define PI 3.14159

namespace rml {

	class Vector2 {

	public:
		Vector2() : x(0.0), y(0.0f) {}
		Vector2(float x, float y) : x(x), y(y) {}
		Vector2(const Vector2& other) : x(other.x), y(other.y) {}

		void SetZero() { x = 0; y = 0; }
		float Length();
		Vector2& Normalize();
		Vector2 operator + (Vector2& vector);
		Vector2& operator += (Vector2& vector);
		Vector2 operator - ();
		Vector2 operator - (Vector2& vector);
		Vector2& operator -= (Vector2& vector);
		Vector2 operator * (float k);
		Vector2& operator *= (float k);
		Vector2 operator / (float k);
		Vector2& operator /= (float k);
		Vector2& operator = (const Vector2& vector);
		bool operator == (const Vector2& other);
		bool operator != (const Vector2& other);
		Vector2 Modulate(Vector2& vector);
		float Dot(Vector2& vector);

		//acces elements
		float operator [] (unsigned int idx);

		friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);

		float x;
		float y;
	};

	class Vector3 {

	public:
		Vector3() : x(0.0), y(0.0f), z(0.0f) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

		void SetZero() { x = 0; y = 0; z = 0; }
		float Length();
		Vector3& Normalize();
		Vector3 operator + (const Vector3& vector) const;
		Vector3& operator += (const Vector3& vector);
		Vector3 operator - ();
		Vector3 operator - (const Vector3& vector);
		Vector3& operator -= (const Vector3& vector);
		Vector3 operator * (float k);
		Vector3& operator *= (float k);
		Vector3 operator / (float k);
		Vector3& operator /= (float k);
		Vector3& operator = (const Vector3& vector);
		bool operator == (const Vector3& other);
		bool operator != (const Vector3& other);
		Vector3 Modulate(Vector3& vector);
		float Dot(const Vector3& vector);
		Vector3 Cross(const Vector3& vector);

		//acces elements
		float operator [] (unsigned int idx);

		friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);

		float x;
		float y;
		float z;
	};

	class Matrix;

	class Vector4 {
	public:
		//Constructors
		Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Vector4(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1.0f) {}
		Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		Vector4(float* pArg) : x(pArg[0]), y(pArg[1]), z(pArg[2]), w(pArg[3]) {}
		Vector4(Vector3& vector) : x(vector.x), y(vector.y), z(vector.z), w(1.0f) {}
		Vector4(Vector3& vector, float _w) : x(vector.x), y(vector.y), z(vector.z), w(_w) {}
		Vector4(Vector4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}
		Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

		//Vector's operations
		float Length();
		Vector4& Normalize();
		Vector4 operator + (const Vector4& vector);
		Vector4& operator += (const Vector4& vector);
		Vector4 operator - ();
		Vector4 operator - (const Vector4& vector);
		Vector4& operator -= (const Vector4& vector);
		Vector4 operator * (const float k);
		Vector4& operator *= (const float k);
		Vector4 operator / (const float k);
		Vector4& operator /= (const float k);
		Vector4& operator = (const Vector4& vector);
		bool operator == (const Vector4& other);
		bool operator != (const Vector4& other);
		Vector4 Modulate(const Vector4& vector);
		float Dot(const Vector4& vector);

		//matrix multiplication
		Vector4 operator * (const Matrix& m);

		//access to elements
		float operator [] (unsigned int idx);

		friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);

		//data members
		float x;
		float y;
		float z;
		float w;
	};

	//Matrix 4 X 4
	class Matrix {
	public:
		Matrix() {}
		Matrix(float val);
		Matrix(Matrix& mat);


		// Matrix operations
		Matrix& SetZero();
		Matrix& SetIdentity();

		Matrix& SetRotationX(float angle);
		Matrix& SetRotationY(float angle);
		Matrix& SetRotationZ(float angle);
		Matrix& SetRotationAngleAxis(float angle, float x, float y, float z);

		Matrix& SetScale(float scale);
		Matrix& SetScale(float scaleX, float scaleY, float scaleZ);
		Matrix& SetScale(float* pScale);
		Matrix& SetScale(Vector3& scaleVec);

		Matrix& SetTranslation(float x, float y, float z);
		Matrix& SetTranslation(float* pTrans);
		Matrix& SetTranslation(Vector3& vec);

		Matrix& SetPerspective(float fovY, float aspect, float nearPlane, float farPlane);
		Matrix InversePerspective(float fovY, float aspect, float nearPlane, float farPlane);
		Matrix& SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

		Matrix Transpose();

		Matrix operator + (const Matrix& mat);
		Matrix& operator += (const Matrix& mat);
		Matrix operator - (const Matrix& mat);
		Matrix& operator -= (const Matrix& mat);

		Matrix operator * (const Matrix& mat);
		Matrix operator * (const float k);
		Matrix& operator *= (const float k);

		Matrix& operator = (const Matrix& mat);

		Vector4 operator * (Vector4& vec);

		//data members
		float m[4][4];
	};
}