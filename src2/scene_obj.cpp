#include <math.h>

#include "scene_obj.hpp"
#include "polyroots.hpp"

using namespace std;

/****************************************************************************/
/* Scene Obj Functions */
/****************************************************************************/

SceneObj::~SceneObj()
{
    for(int i = 0; i < children.size(); i++)
    {
        delete children[i];
    }
}

void SceneObj::intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo)
{
    // Transform the ray
    rayOrigin = invtrans * rayOrigin;
    rayDir = invtrans * rayDir;
    
    // Get the closest intersection info for thei scene objects children
    for(int i = 0; i < children.size(); i++)
    {
        children[i]->intersect(rayOrigin, rayDir, iInfo);
    }
    
    // Get the intersection info for this scene object
    IInfo thisIInfo = getIntersection(rayOrigin, rayDir);
    
    // Check if this scene objs intersection point is closer than its childrens
    if(thisIInfo.t != -1 && (iInfo.t == -1 || thisIInfo.t < iInfo.t))
    {
        iInfo = thisIInfo;
    }
    
    // If we found an intersection point for this obj then transform it
    if(iInfo.t != -1)
    {
        iInfo.point = trans * iInfo.point;
        iInfo.normal = trans.transpose() * iInfo.normal;
    }
}

IInfo SceneObj::getIntersection(Point3D rayOrigin, Vector3D rayDir)
{
    return IInfo();
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

bool SceneObj::intersectCube(Point3D rayOrigin, Vector3D rayDir, Point3D minPoint, Point3D maxPoint, IInfo& iInfo) {
    double tNear = INT_MIN;
    double tFar = INT_MAX;
    
    double minX = minPoint[0];
    double minY = minPoint[1];
    double minZ = minPoint[2];
    double maxX = maxPoint[0];
    double maxY = maxPoint[1];
    double maxZ = maxPoint[2];
    
    if ((rayDir[0] == 0) && (rayOrigin[0] < minX) && (rayOrigin[0] > maxX))
    {
        return -1;
    }
    else
    {
        int dir = -1;
        double t1 = (minX - rayOrigin[0]) / rayDir[0];
        double t2 = (maxX - rayOrigin[0]) / rayDir[0];
        
        if(t1 > t2)
        {
            double temp = t1;
            t1 = t2;
            t2 = temp;
            dir = 1;
        }
        
        if(t1 > tNear)
        {
            iInfo.normal = Vector3D(dir*1, 0, 0);
            tNear = t1;
        }
        
        tFar = (t2 < tFar) ? t2 : tFar;
        
        if ((tNear > tFar) || (tFar < 0))
            return false;
    }
    
    if ((rayDir[1] == 0) && (rayOrigin[1] < minY) && (rayOrigin[1] > maxY))
    {
        //parallel
        return false;
    }
    else
    {
        int dir = -1;
        double t1 = (minY - rayOrigin[1]) / rayDir[1];
        double t2 = (maxY - rayOrigin[1]) / rayDir[1];
        
        if(t1 > t2)
        {
            double temp = t1;
            t1 = t2;
            t2 = temp;
            dir = 1;
        }
        
        if(t1 > tNear)
        {
            iInfo.normal = Vector3D(0, dir*1, 0);
            tNear = t1;
        }
        
        tFar = (t2 < tFar) ? t2 : tFar;
        
        if ((tNear > tFar) || (tFar < 0))
            return false;
    }
    
    if ((rayDir[2] == 0) && (rayOrigin[2] < minZ) && (rayOrigin[2] > maxZ))
    {
        //parallel
        return false;
    }
    else
    {
        int dir = -1;
        double t1 = (minZ - rayOrigin[2]) / rayDir[2];
        double t2 = (maxZ - rayOrigin[2]) / rayDir[2];
        
        if(t1 > t2) {
            double temp = t1;
            t1 = t2;
            t2 = temp;
            dir = 1;
        }
        
        if(t1 > tNear) {
            iInfo.normal = Vector3D(0, 0, dir*1);
            tNear = t1;
        }
        
        tFar = (t2 < tFar) ? t2 : tFar;
        
        if ((tNear > tFar) || (tFar < 0))
            return false;
    }
    
    if(tNear < 0.1) {
        return false;
    }
    
    iInfo.t = tNear;
    iInfo.point = rayOrigin + tNear*rayDir;
    
    return true;
}


/****************************************************************************/
/* Sphere Obj Functions */
/****************************************************************************/

IInfo SphereObj::getIntersection(Point3D rayOrigin, Vector3D rayDir)
{
    Vector3D v = Vector3D(rayOrigin[0] - 0, rayOrigin[1] - 0, rayOrigin[2] - 0);
    double a = rayDir.dot(rayDir);
    double b = 2*v.dot(rayDir);
    double c = v.dot(v) - 1;
    
    double roots[2];
    size_t result = quadraticRoots(a, b, c, roots);
    
    IInfo iInfo;
    if(!(result == 0 || roots[0] < 0.1 || roots[1] < 0.1)) {
        double t = (roots[0] < roots[1]) ? roots[0] : roots[1];
        iInfo.t = t;
        iInfo.point = rayOrigin + t*rayDir;
        iInfo.normal = iInfo.point - Point3D(0, 0, 0);
        iInfo.material = material;
    }
    
    return iInfo;
}

SceneObj::Type SphereObj::getType()
{
    return SceneObj::SPHERE;
}

/****************************************************************************/
/* Cube Obj Functions */
/****************************************************************************/

IInfo CubeObj::getIntersection(Point3D rayOrigin, Vector3D rayDir)
{
    IInfo iInfo;
    intersectCube(rayOrigin, rayDir, Point3D(-0.5, -0.5, -0.5), Point3D(0.5, 0.5, 0.5), iInfo);
    return iInfo;
}

SceneObj::Type CubeObj::getType()
{
    return SceneObj::CUBE;
}

/****************************************************************************/
/* Mesh Obj Functions */
/****************************************************************************/

IInfo MeshObj::getIntersection(Point3D rayOrigin, Vector3D rayDir)
{
    
    IInfo tmp;
    if(!intersectCube(rayOrigin, rayDir, minPoint, maxPoint, tmp)) {
        return IInfo();
    }
    
    IInfo iInfo;
    
    for(int i = 0; i < triangles.size(); i++) {
        Point3D p1 = triangles[i][0];
        Point3D p2 = triangles[i][1];
        Point3D p3 = triangles[i][2];
        
        Vector3D v0 = p2 - p1;
        Vector3D v1 = p3 - p1;
        Vector3D n = v0.cross(v1);
        
        double dot1 = n.dot(rayDir);
        
        // Plane is parallel to ray
        if (dot1 == 0) continue;
        
        double t = -n.dot(rayOrigin - p1) / dot1;
        
        // Intersection point is behind ray point
        if (t < 0.1)
            continue;
        
        Point3D iPoint = rayOrigin + t * rayDir;
        
        Vector3D point_to_plane = iPoint - p1;
        double dot2 = v0.dot(v0);
        double dot3 = v0.dot(v1);
        double dot4 = v1.dot(v1);
        double dot5 = point_to_plane.dot(v0);
        double dot6 = point_to_plane.dot(v1);
        double denom = dot3 * dot3 - dot2 * dot4;
        
        double a, b;
        a = (dot3 * dot6 - dot4 * dot5) / denom;
        if (a < 0.0 || a > 1.0)
            continue;
        b = (dot3 * dot5 - dot2 * dot6) / denom;
        if (b < 0.0 || (a + b) > 1.0) 
            continue;
        
        if(iInfo.t == -1 || iInfo.t > t) {
            iInfo.t = t;
            iInfo.point = iPoint;
            iInfo.normal = n;
            iInfo.material = material;
        }
    }
    
    return iInfo;
}

SceneObj::Type MeshObj::getType()
{
    return SceneObj::MESH;
}

void MeshObj::calculateTriangles()
{
    minPoint[0] = minPoint[1] = minPoint[2] = INT_MAX;
    maxPoint[0] = maxPoint[1] = maxPoint[2] = INT_MIN;
    
    for(int i = 0; i < faces.size(); i++)
    {
        Point3D origin = verts[faces[i][0]];
        
        setMinMax(origin);
        
        for(int j = 1; j < faces[i].size() - 1; j++)
        {
            Triangle triangle;
            
            triangle.push_back(origin);
            triangle.push_back(verts[faces[i][j]]);
            triangle.push_back(verts[faces[i][j+1]]);
            
            setMinMax(verts[faces[i][j]]);
            setMinMax(verts[faces[i][j+1]]);
            
            triangles.push_back(triangle);
        }
    }
    
    minPoint[0] -= 0.1;
    minPoint[1] -= 0.1;
    minPoint[2] -= 0.1;
    maxPoint[0] += 0.1;
    maxPoint[1] += 0.1;
    maxPoint[2] += 0.1;
}

void MeshObj::setMinMax(Point3D point) {
    if(point[0] < minPoint[0]) {
        minPoint[0] = point[0];
    }
    if(point[1] < minPoint[1]) {
        minPoint[1] = point[1];
    }
    if(point[2] < minPoint[2]) {
        minPoint[2] = point[2];
    }
    
    if(point[0] > maxPoint[0]) {
        maxPoint[0] = point[0];
    }
    if(point[1] > maxPoint[1]) {
        maxPoint[1] = point[1];
    }
    if(point[2] > maxPoint[2]) {
        maxPoint[2] = point[2];
    }
}

