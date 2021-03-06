#include "camera.h"

#include <cmath>

PerspectiveCamera::PerspectiveCamera(double fov, double aspectRatio,
                                     Point origin, Point target, Vector up):
    origin(origin) {
    forward = (target - origin).normal();
    right = cross(forward, up).normal();
    up = cross(right, forward);

    h = tan(fov);
    w = h * aspectRatio;
}

PerspectiveCamera::PerspectiveCamera(Point origin, Vector direction, Vector up,
                                     double fov, double aspectRatio):
    origin(origin) {
    forward = direction.normal();
    right = cross(forward, up).normal();
    up = cross(right, forward);

    h = tan(fov);
    w = h * aspectRatio;
}

Ray PerspectiveCamera::makeRay(Vector2 point) const {
    Vector direction = forward + point.u * w * right + point.v * h * up;

    return Ray(origin, direction.normal());
}
