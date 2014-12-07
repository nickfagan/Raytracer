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
    
    ~SceneObj();
    
    std::string name;
    Material* material;
    std::vector<SceneObj*> children;
    
    void intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo);
    virtual IInfo getIntersection(Point3D rayOrigin, Vector3D rayDir);
    virtual Type getType();
    Matrix4x4 getTrans();
    Matrix4x4 getInvtrans();
    void setTrans(Matrix4x4 m);
    void rotate(std::string axis, double angle);
    void scale(Vector3D amount);
    void translate(Vector3D amount);
    bool intersectCube(Point3D rayOrigin, Vector3D rayDir, Point3D minPoint, Point3D maxPoint, IInfo& iInfo);
    
protected:
    Matrix4x4 trans;
    Matrix4x4 invtrans;
};

class SphereObj : public SceneObj
{
public:
    virtual IInfo getIntersection(Point3D rayOrigin, Vector3D rayDir);
    virtual Type getType();
};

class CubeObj : public SceneObj
{
public:
    virtual IInfo getIntersection(Point3D rayOrigin, Vector3D rayDir);
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
    
    virtual IInfo getIntersection(Point3D rayOrigin, Vector3D rayDir);
    virtual Type getType();
    void calculateTriangles();
    
private:
    Point3D minPoint;
    Point3D maxPoint;
    
    void setMinMax(Point3D point);
};

#endif
