#ifndef SHAPE_H
#define SHAPE_H

#include <Windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <vector>

#include "vectormath.h"
#include "ray.h"
#include "color.h"

enum { T_NONE, T_SET, T_PLANE, T_SPHERE, T_ELLIPSOID };

class Shape
{
public:
	Shape(void) {}
	virtual ~Shape() { }

	virtual bool intersect(Intersection& intersection) = 0;
	virtual bool doesIntersect(const Ray& ray) = 0;

	virtual void draw(void) = 0;

	int type;
	Color color;
};

class ShapeSet : public Shape
{
public:
	ShapeSet(void) { type = T_SET; };

	virtual ~ShapeSet() {};

	void addShape(Shape* shape);

	void clear(void);

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void) {};

	std::vector<Shape*> shapes;
};

class Plane : public Shape
{
public:
	Plane(const Point& center, float size, const Vector& normal, const Color& color);

	virtual ~Plane() {};

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void);

	Point position;
	Vector normal;

private:
	Vector _t, _b;
	Point _p1, _p2, _p3, _p4;
	float _size;
};

class Sphere : public Shape
{
public:
	Sphere(const Point& centre, float radius, float reflection, const Color& color);

	virtual ~Sphere(void) { gluDeleteQuadric(quad); };

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void);

	Point centre;
	float radius;
	float reflection;

	GLUquadric * quad;
};

class Ellipsoid : public Shape
{
public:
	Ellipsoid(const Point& centre, const Point& radius, float reflection, const Color& color);

	virtual ~Ellipsoid(void) { gluDeleteQuadric(quad); };

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void);

	Point centre;
	Point radius;
	float reflection;

	GLUquadric * quad;
};

#endif // SHAPE_H
