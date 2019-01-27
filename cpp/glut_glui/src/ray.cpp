#include "ray.h"

Ray::Ray()
	: origin(0.0f, 0.0f, 0.0f),
	direction(),
	tMax(RAY_T_MAX)
{
	history.push_back(origin);
}

Ray::Ray(const Ray& r)
	: origin(r.origin),
	direction(r.direction),
	tMax(r.tMax)
{
	history.push_back(r.origin);
}

Ray::Ray(const Point& origin, const Vector& direction, float tMax)
	: origin(origin),
	direction(direction),
	tMax(tMax)
{
	history.push_back(origin);
}

Ray::~Ray()
{
}

Ray& Ray::operator =(const Ray& r)
{
	origin = r.origin;
	direction = r.direction;
	tMax = r.tMax;
	return *this;
}

Point Ray::calculate(float t) const
{
	return origin + direction * t;
}

void Ray::addPoint(Point p) {
	history.push_back(p);
	origin = p + RAY_T_MIN * direction;
}

void Ray::draw(void) {
	glPushMatrix();
		glBegin(GL_LINES);
			int i = 0;
			if (history.size() > 1) {
				glColor3f(1.0f, 1.0f, 0.0f);
				for (i = 0; i < history.size() - 1; i++) {
					glVertex3f(history.at(i).x, history.at(i).y, history.at(i).z);
					glVertex3f(history.at(i + 1).x, history.at(i + 1).y, history.at(i + 1).z);
				}
			}
			
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(history.at(i).x, history.at(i).y, history.at(i).z);
			glVertex3f(history.at(i).x + 1000.0f * direction.x,
				history.at(i).y + 1000.0f * direction.y,
				history.at(i).z + 1000.0f * direction.z);

		glEnd();
	glPopMatrix();
}

Intersection::Intersection()
	: ray(),
	t(RAY_T_MAX),
	pShape(NULL)
{
}

Intersection::Intersection(const Intersection& i)
	: ray(i.ray),
	t(i.t),
	pShape(i.pShape)
{
}

Intersection::Intersection(const Ray& ray)
	: ray(ray),
	t(ray.tMax),
	pShape(NULL)
{
}

Intersection::~Intersection()
{
}

Intersection& Intersection::operator =(const Intersection& i)
{
	ray = i.ray;
	t = i.t;
	pShape = i.pShape;
	return *this;
}

bool Intersection::intersected() const
{
	return (pShape != NULL);
}

Point Intersection::position() const
{
	return ray.calculate(t);
}
