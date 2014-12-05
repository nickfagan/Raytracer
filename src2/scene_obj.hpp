#ifndef SCENE_OBJ_HPP
#define SCENE_OBJ_HPP

#include <vector>
#include <string>

#include "iinfo.hpp"
#include "algebra.hpp"
#include "material.hpp"

class SceneObj
{
  public:
    enum Type {NONE, SPHERE, CUBE, MESH};

    std::string name;
    Material* material;
    std::vector<SceneObj*> children;

    Matrix4x4 getTrans();
    Matrix4x4 getInvtrans();
    void setTrans(Matrix4x4 m);
    void rotate(std::string axis, double angle);
    void scale(Vector3D amount);
    void translate(Vector3D amount);
    virtual double intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo); // Transform the ray, call intersect on all children 
    virtual Type getType();

  protected:
    Matrix4x4 trans;
    Matrix4x4 invtrans;
};

class SphereObj : public SceneObj
{
  public:
    virtual double intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo);
    virtual Type getType();
};

class CubeObj : public SceneObj
{
  public:
    virtual double intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo);
    virtual Type getType();
};

class MeshObj : public SceneObj
{
  public:
    typedef std::vector<int> Face;
    typedef std::vector<Point3D> Triangle;
  
    std::vector<Point3D> verts;
    std::vector<Face> faces;
    std::vector<Triangle> triangles;

    virtual double intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo);
    virtual Type getType();
};

#endif
