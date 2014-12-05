#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>

#include "iinfo.hpp"
#include "algebra.hpp"
#include "light.hpp"
#include "material.hpp"
#include "scene_obj.hpp"

class Scene
{
  public:
    Point3D eyePos;
    Vector3D viewDir;
    Vector3D upDir;
    int fieldOfView;
    Colour ambient;
    std::vector<Material*> materials;
    std::vector<Light*> lights;
    SceneObj* root;

    bool intersect(Point3D rayOrigin, Vector3D rayDir, IInfo* iInfo);
    void runRaytracer(std::string outfile, int width, int height);
  private:
    Vector3D rightDir;
    double tanf;
    double aspect;

    Colour trace(Point3D origin, Vector3D direction, int depth);
};

#endif
