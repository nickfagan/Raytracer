#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>

#include "iinfo.hpp"
#include "algebra.hpp"
#include "light.hpp"
#include "material.hpp"
#include "scene_obj.hpp"
#include "image.hpp"

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
    int height;
    int width;

    void init(std::string outfile, int width, int height);
    void raytrace(Image* image, int pixelStart, int pixelEnd);
  private:
    Vector3D rightDir;
    double tanf;
    double aspect;

    bool intersect(Point3D rayOrigin, Vector3D rayDir, IInfo& iInfo);
    Colour trace(Point3D origin, Vector3D direction, int depth);
};

#endif
