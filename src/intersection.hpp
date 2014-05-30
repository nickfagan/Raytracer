#ifndef CS488_INTERSECTION_HPP
#define CS488_INTERSECTION_HPP

#include "material.hpp"
#include "algebra.hpp"

struct Intersection {
  double t;
  Point3D point;
  Vector3D normal;
  Material* material;

  Intersection() : t(-1) {}

  Intersection(double _t, Point3D p, Vector3D n, Material* m)
	: t(_t), 
	point(p), 
	normal(n), 
	material(m) {}
};

#endif
