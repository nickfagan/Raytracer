#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include "algebra.hpp"

class Material
{
  public:
    std::string name;
    Colour diffused;
    Colour specular;
    int shininess;
    double reflectivity;
    double transparency;
    double refractionI;
};

#endif
