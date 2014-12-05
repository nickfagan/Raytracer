#include <math.h>

#include "scene_obj.hpp"
#include "polyroots.hpp"

using namespace std;

/****************************************************************************/
/* Scene Obj Functions */
/****************************************************************************/

double SceneObj::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{
  rayOrigin = invtrans * rayOrigin;
  rayDir = invtrans * rayDir;

  for(int i = 0; i < children.size(); i++)
  {
    children[i]->intersect(rayOrigin, rayDir, iInfo);
  }

  if(iInfo.t != -1)
  {
    iInfo.point = trans * iInfo.point;
    iInfo.normal = trans.transpose() * iInfo.normal;
    iInfo.normal.normalize(); //TODO: only once
  }
}

SceneObj::Type SceneObj::getType()
{
  return SceneObj::NONE;
}

Matrix4x4 SceneObj::getTrans()
{
  return trans;
}

Matrix4x4 SceneObj::getInvtrans()
{
  return invtrans;
}

void SceneObj::setTrans(Matrix4x4 m)
{
  trans = m;
  invtrans = m.invert();
}

void SceneObj::rotate(string axis, double angle)
{
  Matrix4x4 r;

  double sinA = sin(angle*M_PI/180);
  double cosA = cos(angle*M_PI/180);
  
  if(axis == "x")
  {
    r = {{1, 0, 0, 0},
         {0, cosA, -1*sinA, 0},
         {0, sinA, cosA, 0},
         {0, 0, 0, 1}};
  }
  else if(axis == "y")
  {
    r = {{cosA, 0, sinA, 0},
         {0, 1, 0, 0},
         {-1*sinA, 0, cosA, 0},
         {0, 0, 0, 1}};
  }
  else if(axis == "z")
  {
    r = {{cosA, -1*sinA, 0, 0},
         {sinA, cosA, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1}};
  }

  setTrans(trans*r);
}

void SceneObj::scale(Vector3D amount)
{
  Matrix4x4 s;
  s = {{amount[0], 0, 0, 0},
       {0, amount[1], 0, 0},
       {0, 0, amount[2], 0},
       {0, 0, 0, 1}};

  setTrans(trans*s);
}

void SceneObj::translate(Vector3D amount)
{
  Matrix4x4 t;
  t = {{1, 0, 0, amount[0]},
       {0, 1, 0, amount[1]},
       {0, 0, 1, amount[2]},
       {0, 0, 0, 1}};

  setTrans(trans*t);
}

/****************************************************************************/
/* Sphere Obj Functions */
/****************************************************************************/

double SphereObj::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{
  SceneObj::intersect(rayOrigin, rayDir, iInfo);

  rayOrigin = invtrans * rayOrigin;
  rayDir = invtrans * rayDir;

  Vector3D v = Vector3D(rayOrigin[0] - 0, rayOrigin[1] - 0, rayOrigin[2] - 0);
  double a = rayDir.dot(rayDir);
  double b = 2*v.dot(rayDir);
  double c = v.dot(v) - 1;

  double roots[2];
  size_t result = quadraticRoots(a, b, c, roots);

  if(result == 0 || roots[0] < 0.1 || roots[1] < 0.1) {
    return -1; //No intersection
  }

  double t = (roots[0] < roots[1]) ? roots[0] : roots[1];

  iInfo.point = rayOrigin + t*rayDir;
  iInfo.normal = iInfo.point - Point3D(0, 0, 0);
}

SceneObj::Type SphereObj::getType()
{
  return SceneObj::SPHERE;
}

/****************************************************************************/
/* Cube Obj Functions */
/****************************************************************************/

double CubeObj::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{

}

SceneObj::Type CubeObj::getType()
{
  return SceneObj::CUBE;
}

/****************************************************************************/
/* Mesh Obj Functions */
/****************************************************************************/

double MeshObj::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{

}

SceneObj::Type MeshObj::getType()
{
  return SceneObj::MESH;
}

