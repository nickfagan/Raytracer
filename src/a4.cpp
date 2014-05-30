#include <math.h>
#include "a4.hpp"
#include "raytracer.hpp"

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  RayTracer rayTracer(root, filename, width, height, eye, view, up, fov, ambient, lights);
  rayTracer.run();
}
