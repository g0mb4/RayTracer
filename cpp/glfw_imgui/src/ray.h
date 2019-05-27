#ifndef RAY_H
#define RAY_H

#include <Windows.h>

#include <GL/GL.h>
#include <vector>

#include "vectormath.h"

// In order to prevent bouncing rays self-intersecting
#define RAY_T_MIN 0.0001f

// 'Infinite' distance, used as a default value
//#define RAY_T_MAX 1.0e30f
#define RAY_T_MAX 1000.0f

struct Ray
{
	Point origin; // start
	Vector direction;
	double tMax;
	bool valid;
	double energy;

	Ray();
	Ray(const Ray& r);
	Ray(const Point& origin, const Vector& direction, double tMax = RAY_T_MAX);

	virtual ~Ray() {};

	Ray& operator =(const Ray& r);

	Point calculate(double t) const;

	void draw(void);
	void addPoint(Point p);

	std::vector<Point> history;
};

class Shape;

struct Intersection
{
	Ray ray;
	double t;
	Shape *pShape;

	Intersection();
	Intersection(const Intersection& i);
	Intersection(const Ray& ray);

	virtual ~Intersection();

	Intersection& operator =(const Intersection& i);

	bool intersected() const;
	Point position() const;
};

#endif // RAY_H
