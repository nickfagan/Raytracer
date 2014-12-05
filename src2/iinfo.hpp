#ifndef I_INFO_HPP
#define I_INFO_HPP

#include "algebra.hpp"
#include "material.hpp"

class IInfo
{
  public:
    double t;
    Point3D point;
    Vector3D normal;
    Material* material;
};

#endif
