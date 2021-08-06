#include "Vector.h"
#include <cmath>
#include <string>

Vector2D::Vector2D()
{
	x = 0.0f;
	y = 0.0f;
}

Vector2D::Vector2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

float Vector2D::getX()
{
	return this->x;
}

float Vector2D::getY()
{
	return this->y;
}

void Vector2D::setX(float newX)
{
	this->x = newX;
}

void Vector2D::setY(float newY)
{
	this->y = newY;
}

void Vector2D::changeXDirection()
{
	this->x = -this->x;
}

void Vector2D::changeYDirection()
{
	this->y = -this->y;
}

Vector2D& Vector2D::Add(const Vector2D& addVec)
{
	this->x += addVec.x;
	this->y += addVec.y;

	return *this;
}

Vector2D& Vector2D::Subtract(const Vector2D& subVec)
{
	this->x -= subVec.x;
	this->y -= subVec.y;

	return *this;
}

Vector2D& Vector2D::Multiply(const Vector2D& multVec)
{
	this->x *= multVec.x;
	this->y *= multVec.y;

	return *this;
}

Vector2D& Vector2D::Divide(const Vector2D& divVec)
{
	this->x /= divVec.x;
	this->y /= divVec.y;

	return *this;
}

Vector2D& operator+(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Add(vec2);
}

Vector2D& operator-(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Subtract(vec2);
}

Vector2D& operator*(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Multiply(vec2);
}

Vector2D& operator/(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Divide(vec2);
}

Vector2D& Vector2D::operator+=(const Vector2D& vec)
{
	return this->Add(vec);
}

Vector2D& Vector2D::operator-=(const Vector2D& vec)
{
	return this->Subtract(vec);
}

Vector2D& Vector2D::operator*=(const Vector2D& vec)
{
	return this->Multiply(vec);
}

Vector2D& Vector2D::operator/=(const Vector2D& vec)
{
	return this->Divide(vec);
}

float Vector2D::find2DDotProduct(const Vector2D& vec)
{
	return (this->x * vec.x) + (this->y * vec.y);
}

float Vector2D::findMagnitude()
{
	printf("mag + %f\n", sqrt(pow(this->x, 2) + pow(this->y, 2)));
	return sqrt(pow(this->x,2) + pow(this->y,2));
}