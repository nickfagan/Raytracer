#include "primitive.hpp"
#include "polyroots.hpp"
#include "limits.h"
#include <iostream>

Primitive::~Primitive()
{
}

double Primitive::intersect_cube(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal, Point3D min_pos, Point3D max_pos) {
  double tNear = INT_MIN;
  double tFar = INT_MAX;

  double min_x = min_pos[0];
  double min_y = min_pos[1];
  double min_z = min_pos[2];
  double max_x = max_pos[0];
  double max_y = max_pos[1];
  double max_z = max_pos[2];

  if ((ray_dir[0] == 0) && (ray_point[0] < min_x) && (ray_point[0] > max_x))
  {
    return -1;
  }
  else
  {
    int dir = -1;
    double t1 = (min_x - ray_point[0]) / ray_dir[0];
    double t2 = (max_x - ray_point[0]) / ray_dir[0];
    
    if(t1 > t2) {
      double temp = t1;
      t1 = t2;
      t2 = temp;
      dir = 1;
    }

    if(t1 > tNear) {
      intersection_normal = Vector3D(dir*1, 0, 0);
      tNear = t1;
    }

    tFar = (t2 < tFar) ? t2 : tFar;

    if ((tNear > tFar) || (tFar < 0))
        return -1;
  }

  if ((ray_dir[1] == 0) && (ray_point[1] < min_y) && (ray_point[1] > max_y))
  {
    //parallel
    return -1;
  }
  else
  {
    int dir = -1;
    double t1 = (min_y - ray_point[1]) / ray_dir[1];
    double t2 = (max_y - ray_point[1]) / ray_dir[1];
    
    if(t1 > t2) {
      double temp = t1;
      t1 = t2;
      t2 = temp;
      dir = 1;
    }

    if(t1 > tNear) {
      intersection_normal = Vector3D(0, dir*1, 0);
      tNear = t1;
    }

    tFar = (t2 < tFar) ? t2 : tFar;

    if ((tNear > tFar) || (tFar < 0))
        return -1;
  }

  if ((ray_dir[2] == 0) && (ray_point[2] < min_z) && (ray_point[2] > max_z))
  {
    //parallel
    return -1;
  }
  else
  {
    int dir = -1;
    double t1 = (min_z - ray_point[2]) / ray_dir[2];
    double t2 = (max_z - ray_point[2]) / ray_dir[2];
    
    if(t1 > t2) {
      double temp = t1;
      t1 = t2;
      t2 = temp;
      dir = 1;
    }

    if(t1 > tNear) {
      intersection_normal = Vector3D(0, 0, dir*1);
      tNear = t1;
    }

    tFar = (t2 < tFar) ? t2 : tFar;

    if ((tNear > tFar) || (tFar < 0))
        return -1;
  }

  if(tNear < 0.1) {
    return -1;
  }

  intersection_point = ray_point + tNear*ray_dir;

  return tNear;
}

double Primitive::intersect_sphere(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal, Point3D pos, int radius) {
  Vector3D v = Vector3D(ray_point[0] - pos[0], ray_point[1] - pos[1], ray_point[2] - pos[2]);

  double a, b, c;

  a = ray_dir.dot(ray_dir);
  b = 2*v.dot(ray_dir);
  c = v.dot(v) - radius*radius;

  double roots[2];
  size_t result = quadraticRoots(a, b, c, roots);

  if(result == 0 || roots[0] < 0.1 || roots[1] < 0.1) {
    return -1; //No intersection
  }

  double t = (roots[0] < roots[1]) ? roots[0] : roots[1];

  intersection_point = ray_point + t*ray_dir;
  intersection_normal = intersection_point - pos;

  return t;
}

Sphere::~Sphere()
{
}

double Sphere::intersect(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal)
{
  ray_dir.normalize();
  return intersect_sphere(ray_point, ray_dir, intersection_point, intersection_normal, Point3D(0, 0, 0), 1);
}

Cube::~Cube()
{
}

double Cube::intersect(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal)
{
  ray_dir.normalize();
  return intersect_cube(ray_point, ray_dir, intersection_point, intersection_normal, Point3D(-0.5, -0.5, -0.5), Point3D(0.5, 0.5, 0.5));
}

NonhierSphere::~NonhierSphere()
{
}

double NonhierSphere::intersect(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal)
{
  ray_dir.normalize();
  return intersect_sphere(ray_point, ray_dir, intersection_point, intersection_normal, m_pos, m_radius);
}

NonhierBox::~NonhierBox()
{
}

double NonhierBox::intersect(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal)
{
  ray_dir.normalize();
  return intersect_cube(ray_point, ray_dir, intersection_point, intersection_normal, m_pos, Point3D(m_pos[0]+m_size,m_pos[1]+m_size,m_pos[2]+m_size));
}














