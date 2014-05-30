#include <iostream>
#include <math.h>

#include "a2.hpp"
// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.

#define PI 3.14159265

Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 r;

  double sinA = sin(angle*PI/180);
  double cosA = cos(angle*PI/180);
  
  switch(axis) {
    case 'x':
      r = {{1, 0, 0, 0},
           {0, cosA, -1*sinA, 0},
           {0, sinA, cosA, 0},
           {0, 0, 0, 1}};
      break;
    case 'y':
      r = {{cosA, 0, sinA, 0},
           {0, 1, 0, 0},
           {-1*sinA, 0, cosA, 0},
           {0, 0, 0, 1}};
      break;
    case 'z':
      r = {{cosA, -1*sinA, 0, 0},
           {sinA, cosA, 0, 0},
           {0, 0, 1, 0},
           {0, 0, 0, 1}};
      break;
  }

  return r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
  Matrix4x4 t;
  t = {{1, 0, 0, displacement[0]},
       {0, 1, 0, displacement[1]},
       {0, 0, 1, displacement[2]},
       {0, 0, 0, 1}};

  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
  Matrix4x4 s;
  
  s = {{scale[0], 0, 0, 0},
       {0, scale[1], 0, 0},
       {0, 0, scale[2], 0},
       {0, 0, 0, 1}};

  return s;
}




















