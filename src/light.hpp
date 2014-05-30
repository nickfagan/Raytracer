#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "algebra.hpp"

// Represents a simple point light.
struct Light {
  Light();
  
  Colour colour;
  Point3D position;
  double falloff[3];
  double intensity;
  int size;

  int getSize() { return size; }
  Colour getLight() { return colour * intensity; }

  virtual int getLightType();
};

struct BoxLight : Light {

  virtual int getLightType();
};

#endif
