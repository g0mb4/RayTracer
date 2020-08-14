#include "ray.h"

Ray::Ray(): origin(0.0f, 0.0f, 0.0f), direction(), energy(0.0f), valid(true) {
    history.push_back(origin);
}

Ray::Ray(const Ray & r):
    origin(r.origin), direction(r.direction), energy(r.energy), valid(true) {
    history.clear();
    for (unsigned int i = 0; i < r.history.size(); i++) {
        history.push_back(r.history[i]);
    }
}

Ray::Ray(const Point & origin, const Vector & direction):
    origin(origin), direction(direction), energy(0.0f), valid(true) {
    history.push_back(origin);
}

Ray & Ray::operator=(const Ray & r) {
    origin = r.origin;
    direction = r.direction;
    energy = r.energy;
    valid = r.valid;

    history.clear();
    for (unsigned int i = 0; i < r.history.size(); i++) {
        history.push_back(r.history[i]);
    }

    return *this;
}

bool Ray::equals(const Ray & r) {
    return origin.equals(r.origin) && direction.equals(r.direction) &&
           energy == r.energy;
}

Point Ray::calculate(double t) const {
    return origin + direction * t;
}

void Ray::addPoint(Point p) {
    history.push_back(p);
    // printf("    added: %s\n",p.to_str().c_str());
    origin = p;
    // origin = p + RAY_T_MIN * direction;

    // printf("    new: %s\n", origin.to_str().c_str());
}

void Ray::draw(void) const {
    glPushMatrix();
    glBegin(GL_LINES);
    int i = 0;
    try {
        if (history.size() > 1) {
            glColor3f(1.0f, 1.0f, 0.0f);
            for (i = 0; i < history.size() - 1; i++) {
                glVertex3f(history.at(i).x, history.at(i).y, history.at(i).z);
                glVertex3f(history.at(i + 1).x, history.at(i + 1).y,
                           history.at(i + 1).z);
            }
        }

        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(history.at(i).x, history.at(i).y, history.at(i).z);
        glVertex3f(history.at(i).x + 1000.0f * direction.x,
                   history.at(i).y + 1000.0f * direction.y,
                   history.at(i).z + 1000.0f * direction.z);
    } catch (const std::exception & e) { // caught by reference to base
        printf("error: %s\n", e.what());
    }

    glEnd();
    glPopMatrix();
}

std::string Ray::to_str(void) const {
    char pastshape[32];
    //T_NONE, T_SET, T_PLANE, T_ELLIPSOID, T_CUBE
    /*
    switch (pastCollision) { 
    case T_NONE:
        strncpy(pastshape, "T_NONE", sizeof(pastshape));
        break;
    case T_SET:
        strncpy(pastshape, "T_SET", sizeof(pastshape));
        break;
    case T_PLANE:
        strncpy(pastshape, "T_PLANE", sizeof(pastshape));
        break;
    case T_ELLIPSOID:
        strncpy(pastshape, "T_ELLIPSOID", sizeof(pastshape));
        break;
    case T_CUBE:
        strncpy(pastshape, "T_CUBE", sizeof(pastshape));
        break;
    }*/

    char buf[4096];

    snprintf(buf, sizeof(buf), "  p: %s d: %s e: %f %s %s\n",
             origin.to_str().c_str(), direction.to_str().c_str(), energy,
             energy == 0.0 ? "Z" : " ", valid ? "valid" : "invalid");

    std::string s(buf);
    for (auto v: history) {
        snprintf(buf, sizeof(buf), "    %s\n", v.to_str().c_str());
        s.append(buf);
    }

    return s;
}

Intersection::Intersection(): ray(), t(RAY_T_MAX), pShape(NULL) {
}

Intersection::Intersection(const Intersection & i):
    ray(i.ray), t(i.t), pShape(i.pShape) {
}

Intersection::Intersection(const Ray & ray):
    ray(ray), t(RAY_T_MAX), pShape(NULL) {
}

Intersection::~Intersection() {
}

Intersection & Intersection::operator=(const Intersection & i) {
    ray = i.ray;
    t = i.t;
    pShape = i.pShape;
    return *this;
}

bool Intersection::intersected() const {
    return (pShape != NULL);
}

Point Intersection::position() const {
    return ray.calculate(t);
}
