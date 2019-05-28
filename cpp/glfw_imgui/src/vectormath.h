#ifndef __VECTORMATH_H__
#define __VECTORMATH_H__

#include <string>

#ifndef NULL
	#define NULL 0
#endif

#ifndef PI
	#define PI 3.1415926f
#endif

struct Vector
{
	double x, y, z;

	Vector();
	Vector(const Vector& v);
	Vector(double x, double y, double z);
	Vector(double f);

	virtual ~Vector();

	double length2();
	double length();

	Vector normal();
	double normal_len();
	
	Vector& operator =(const Vector& v);
	Vector& operator +=(const Vector& v);
	Vector& operator -=(const Vector& v);
	Vector& operator *=(double f);
	Vector& operator /=(double f);
	Vector operator -() const;

	std::string to_str(void) const;
};

double dot(Vector v1, Vector v2);
Vector cross(Vector v1, Vector v2);

inline Vector operator +(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z);
}

inline Vector operator -(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z);
}

inline Vector operator *(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x * v2.x,
		v1.y * v2.y,
		v1.z * v2.z);
}

inline Vector operator *(const Vector& v, double f)
{
	return Vector(v.x * f,
		v.y * f,
		v.z * f);
}

inline Vector operator *(double f, const Vector& v)
{
	return Vector(f * v.x,
		f * v.y,
		f * v.z);
}

inline Vector operator /(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x / v2.x,
		v1.y / v2.y,
		v1.z / v2.z);
}

inline Vector operator /(const Vector& v, double f)
{
	return Vector(v.x / f,
		v.y / f,
		v.z / f);
}

inline Vector operator /(double f, const Vector& v)
{
	return Vector(f / v.x, 
		f / v.y,
		f / v.z);
}

typedef Vector Point;

struct Vector2
{
	double u, v;

	Vector2();
	Vector2(const Vector2& v);
	Vector2(double u, double v);
	Vector2(double f);

	virtual ~Vector2();

	Vector2& operator =(const Vector2& v);
};

#endif // MATHS_H
