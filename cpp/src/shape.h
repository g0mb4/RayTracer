#ifndef SHAPE_H
#define SHAPE_H

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <vector>

#include "vectormath.h"
#include "ray.h"

enum { T_NONE, T_SET, T_PLANE, T_SPHERE };

class Shape
{
public:
	Shape(void) {}
	virtual ~Shape() { }

	virtual bool intersect(Intersection& intersection) = 0;
	virtual bool doesIntersect(const Ray& ray) = 0;

	virtual void draw(void) = 0;

	int type;
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
	Point position;
	Vector normal;

public:
	Plane(const Point& center, float size, const Vector& normal);

	virtual ~Plane() {};

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void);

private:
	Vector _t, _b;
	Point _p1, _p2, _p3, _p4;
	float _size;
};

class Sphere : public Shape
{
public:
	Point centre;
	float radius;
	float reflection;

public:
	Sphere(const Point& centre, float radius, float reflection);

	virtual ~Sphere() {};

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);
	virtual void draw(void);
};

#endif // SHAPE_H
