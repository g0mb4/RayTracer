#include "ray.h"

Ray::Ray()
	: origin(0.0f, 0.0f, 0.0f),
	direction(),
	tMax(RAY_T_MAX),
	energy(1.0f),
	valid(true)
{
	history.push_back(origin);
}

Ray::Ray(const Ray& r)
	: origin(r.origin),
	direction(r.direction),
	tMax(r.tMax),
	energy(r.energy),
	valid(true)
{
	history.clear();
	for (unsigned int i = 0; i < r.history.size(); i++) {
		history.push_back(r.history[i]);
	}
}

Ray::Ray(const Point& origin, const Vector& direction, double tMax)
	: origin(origin),
	direction(direction),
	tMax(tMax),
	energy(1.0f),
	valid(true)
{
	history.push_back(origin);
}

Ray& Ray::operator =(const Ray& r)
{
	origin = r.origin;
	direction = r.direction;
	tMax = r.tMax;
	energy = r.energy;
	valid = r.valid;

	history.clear();
	for (unsigned int i = 0; i < r.history.size(); i++) {
		history.push_back(r.history[i]);
	}

	return *this;
}

Point Ray::calculate(double t) const
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
			try {
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
			} catch (const std::exception& e) { // caught by reference to base
				printf("error: %s\n", e.what());
			}
			
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
