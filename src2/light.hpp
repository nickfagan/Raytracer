#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <string>

#include "algebra.hpp"
#include "scene_obj.hpp"

class Light
{
  public:
    std::string name;
    double* falloff;
    double intensity;
    SceneObj* obj;

    ~Light() 
    {
      delete falloff;
      delete obj;
    }

    Point3D position()
    {
      return obj->getTrans() * Point3D();
    }

    Colour colour()
    {
      return obj->material->diffused;
    }
};

#endif
