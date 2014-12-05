#include <math.h>

#include "scene.hpp"
#include "image.hpp"

using namespace std;

#define BACKGROUND_COLOR Colour(0.4, 0.4, 0.4)
#define reflectivity_DEPTH 3
#define PRCT 10

void Scene::runRaytracer(string outfile, int width, int height)
{
  viewDir.normalize();
  upDir.normalize();
  rightDir = viewDir.cross(upDir);
  rightDir.normalize();
  tanf = tan((fieldOfView/2)*M_PI/180);
  aspect = (double)width/(double)height;

  //TODO: this
  ambient = Colour(0.2, 0.2, 0.2);

  Image image(width, height, 3);

  int curPrct = 0;
  int pixelY, pixelX;
  for(pixelY = 0; pixelY < height; pixelY++) {
    for(pixelX = 0; pixelX < width; pixelX++) {
      double r, g, b;
      r = g = b = 0;

      Point3D origin = eyePos;
      Vector3D direction = viewDir + ((pixelX/width)-0.5)*tanf*aspect*rightDir + ((pixelY/height)-0.5)*tanf*upDir;
      direction.normalize();

      Colour pixel_colour = trace(origin, direction, 1);

      r += pixel_colour.R();
      g += pixel_colour.G();
      b += pixel_colour.B();

      image(pixelX, height-pixelY-1, 0) = r;
      image(pixelX, height-pixelY-1, 1) = g;
      image(pixelX, height-pixelY-1, 2) = b;
    }
    
    if(((double)(pixelY+1)/(double)height)*100 >= curPrct + PRCT) {
      curPrct += PRCT;
      cout << curPrct << '%' << " complete" << endl; 
    }
  }

  image.savePng(outfile);
}

Colour Scene::trace(Point3D origin, Vector3D direction, int depth) {

  IInfo iInfo;
  if(root->intersect(origin, direction, iInfo)) {
    return BACKGROUND_COLOR;
  }

  Colour pixelColor = Colour(0);
  Colour diffused = iInfo.material->diffused;
  Colour specular = iInfo.material->specular;
  double reflectivity = iInfo.material->reflectivity;
  double shininess = iInfo.material->shininess;

  pixelColor = diffused*ambient;

/*******************************/
/* Apply lighting and shadows
/*******************************/

  for (int i = 0; i < lights.size(); i++) {
    Vector3D lightDir = lights[i]->position() - iInfo.point;
    double lightDistance = lightDir.length();
    lightDir.normalize();

    double lambentCoef = lightDir.dot(iInfo.normal);
    if(lambentCoef < 0)  {
      continue;	// intersection point not facing light
    }

    IInfo lightIInfo;
    if(root->intersect(iInfo.point, lightDir, lightIInfo) && lightIInfo.t < lightDistance)
    {
      continue;	// intersection point is in another objects shadow
    }

    Vector3D r = lightDir - 2.0f * lightDir.dot(iInfo.normal) * iInfo.normal;
    r.normalize();

    double dot = r.dot(direction);
    dot = (dot < 0) ? 0 : dot;

    double attenuation = 1 / (lights[i]->falloff[0] + lights[i]->falloff[1]*lightDistance + lights[i]->falloff[2]*lightDistance*lightDistance);

    // Adjust pixel colour to account for this lights lambent light + specular reflectivity
    pixelColor = pixelColor + diffused * lambentCoef * lights[i]->colour() * attenuation
                            + specular * powf(dot, shininess) * lights[i]->colour() * attenuation;
  }

/*******************************/
/* Apply reflectivity
/*******************************/

  // If the object is reflective, get the color of the reflected object and apply that to the surface color
  if(reflectivity > 0 && depth < reflectivity_DEPTH) {
    Point3D refOrigin = iInfo.point;
    Vector3D refDir = direction - 2*direction.dot(iInfo.normal)*iInfo.normal;
    refDir.normalize();
    pixelColor = pixelColor * (1.0-reflectivity) + trace(refOrigin, refDir, depth + 1) * reflectivity;
  }

  return pixelColor;
}
