#include <cmath>

#include "vectormath.h"

Vector::Vector()
	: x(0.0f), y(1.0f), z(0.0f)
{
}

Vector::Vector(const Vector& v)
	: x(v.x), y(v.y), z(v.z)
{
}

Vector::Vector(double x, double y, double z)
	: x(x), y(y), z(z)
{
}

Vector::Vector(double f)
	: x(f), y(f), z(f)
{
}

Vector::~Vector()
{
}

double Vector::length2()
{
	return x*x + y*y + z*z;
}

double Vector::length()
{
	return std::sqrt(length2());
}

Vector Vector::normal()
{
	Vector v(*this);
	v.normal_len();
	return v;
}

double Vector::normal_len()
{
	double l = length();

	*this /= l;

	return l;
}

double dot(Vector v1, Vector v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector cross(Vector v1, Vector v2)
{
	return Vector(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

Vector& Vector::operator =(const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vector& Vector::operator +=(const Vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector& Vector::operator -=(const Vector& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector& Vector::operator *=(double f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Vector& Vector::operator /=(double f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

Vector Vector::operator -() const
{
	return Vector(-x, -y, -z);
}

Vector2::Vector2()
	: u(0.0f),
	v(0.0f)
{
}

Vector2::Vector2(const Vector2 &v)
	: u(v.u), v(v.v)
{
}

Vector2::Vector2(double u, double v)
	: u(u), v(v)
{
}

Vector2::Vector2(double f)
	: u(f), v(f)
{
}

Vector2::~Vector2()
{
}

Vector2& Vector2::operator =(const Vector2& vec)
{
	u = vec.u;
	v = vec.v;
	return *this;
}
