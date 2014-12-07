#ifndef I_INFO_HPP
#define I_INFO_HPP

#include "algebra.hpp"
#include "material.hpp"

class IInfo
{
  public:
    IInfo() { t = -1; }

    double t;
    Point3D point;
    Vector3D normal;
    Material* material;
    
    void operator=(const IInfo &iInfo )
    {
        t = iInfo.t;
        point = iInfo.point;
        normal = iInfo.normal;
        material = iInfo.material;
    }
};

#endif
