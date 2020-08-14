#ifndef SHAPE_H
#define SHAPE_H

#include <Windows.h>

#include <GL/GL.h>
#include <GL/GLU.h>

#include <vector>

#include "color.h"
#include "ray.h"
#include "vectormath.h"

enum ShapeType { T_NONE, T_SET, T_PLANE, T_ELLIPSOID, T_CUBE };

class Shape {
public:
    Shape(void) {
    }
    virtual ~Shape() {
    }

    virtual bool intersect(Intersection & intersection) = 0;
    virtual bool doesIntersect(const Ray & ray) = 0;

    virtual void draw(void) = 0;

    int type;
    Color color;
};

class ShapeSet: public Shape {
public:
    ShapeSet(void) {
        type = T_SET;
    };

    virtual ~ShapeSet(){};

    void addShape(Shape * shape);

    void clear(void);

    virtual bool intersect(Intersection & intersection);
    virtual bool doesIntersect(const Ray & ray);
    virtual void draw(void){};

    std::vector<Shape *> shapes;
};

class Plane: public Shape {
public:
    Plane(const Point & center, float size, const Vector & normal,
          float reflection, const Color & color);

    virtual ~Plane(){};

    virtual bool intersect(Intersection & intersection);
    virtual bool doesIntersect(const Ray & ray);
    virtual void draw(void);

    Point position;
    Vector normal;
    float reflection;

private:
    Vector _t, _b;
    Point _p1, _p2, _p3, _p4;
    float _size;
};

class Ellipsoid: public Shape {
public:
    Ellipsoid(const Point & centre, const Point & radius, float reflection,
              const Color & color);

    virtual ~Ellipsoid(void) {
        gluDeleteQuadric(quad);
    };

    virtual bool intersect(Intersection & intersection);
    virtual bool doesIntersect(const Ray & ray);
    virtual void draw(void);

    Point centre;
    Point radius;
    float reflection;

    bool isInside(const Point p);

    GLUquadric * quad;
};

class Cube: public Shape {
public:
    Cube(const Point & centre, float sideLength, const Color & color);

    virtual ~Cube(void){};

    virtual bool intersect(Intersection & intersection) {
        return false;
    };
    virtual bool doesIntersect(const Ray & ray) {
        return false;
    };
    virtual void draw(void);

    Point centre;
    float halfSideLength;
};

#endif // SHAPE_H
