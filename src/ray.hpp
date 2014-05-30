#ifndef RAY_HPP
#define RAY_HPP

#include "algebra.hpp"

struct Ray {
  Point3D origin;
  Vector3D direction;

  Ray(Point3D o, Vector3D d) { origin = o; direction = d; }
};

#endif
