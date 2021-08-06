#pragma once

class Vector2D
{
private:
	float x;
	float y;

public:

	Vector2D();
	Vector2D(float x, float y);
	
	float getX();
	float getY();

	void setX(float newX);
	void setY(float newX);

	void changeXDirection();
	void changeYDirection();

	Vector2D& Add(const Vector2D& vec);
	Vector2D& Subtract(const Vector2D& vec);
	Vector2D& Multiply(const Vector2D& vec);
	Vector2D& Divide(const Vector2D& vec);

	friend Vector2D& operator+(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator-(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator/(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator*(Vector2D& vec1, const Vector2D& vec2);

	Vector2D& operator+=(const Vector2D& vec);
	Vector2D& operator-=(const Vector2D& vec);
	Vector2D& operator/=(const Vector2D& vec);
	Vector2D& operator*=(const Vector2D& vec);

	float find2DDotProduct(const Vector2D& vec);
	Vector2D& find2DCrossProduct(const Vector2D& vec);
	float findMagnitude();

	

};