#include "raytracer.hpp"
#include <math.h>
#include "a4.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#include <stdlib.h>     /* srand, rand */
#include <time.h> 	/* time */

#define RAND_01 ((double) rand() / (RAND_MAX))

#define RAY_DEPTH 8
#define DIST_RAY_NUM 32
#define ANTIALIAS 4.0

#define ENABLE_SOFT_SHADOWS false
#define ENABLE_ANTIALIASING false

RayTracer::RayTracer(SceneNode* root,
           const std::string& filename,
           int width, int height,
           const Point3D& eye, const Vector3D& view,
           const Vector3D& up, double fov,
           const Colour& ambient,
           const std::list<Light*>& lights) : m_ambient(ambient) {

  srand (time(NULL));

  // Initialize some variables
  m_view = view;
  m_view.normalize();
  m_up = up;
  m_up.normalize();
  m_right = m_view.cross(m_up);
  m_right.normalize();
  m_tanf = tan((fov/2)*M_PI/180);
  m_aspect = (double)width/(double)height;

  // Load the background png
  m_background_image.loadPng("background.png");

  // Get the ratio so we can use the right background pixels
  m_width_ratio = (double)m_background_image.width()/(double)width;
  m_height_ratio = (double)m_background_image.height()/(double)height;

  // Create the image to save to

  m_width = width;
  m_height = height;
  m_ambient = ambient;
  m_lights = lights;
  m_root = root;
  m_eye = eye;
  m_filename = filename;

  for (std::list<Light*>::const_iterator I = m_lights.begin(); I != m_lights.end(); ++I) {
    Light *light = *I;

    GeometryNode* node = NULL;
    switch(light->getLightType()) {
      case 0:
        node = new GeometryNode("light", new NonhierSphere(light->position, 1));
        break;
      case 1:
        node = new GeometryNode("light", new NonhierSphere(light->position, light->getSize()));
        break;
    }

    node->set_material(new LightMaterial(light->colour, light));
    
    m_root->add_child(node);
  }
}

RayTracer::~RayTracer() {

}

void RayTracer::run() {
  Image m_save_image(m_width, m_height, 3);
  int cur_percent = 0;

  for(m_pixel_y = 0; m_pixel_y < m_height; m_pixel_y++) {
    for(m_pixel_x = 0; m_pixel_x < m_width; m_pixel_x++) {
      double pixelSamples;
      double r, g, b;

      pixelSamples = ENABLE_ANTIALIASING ? ANTIALIAS : 1;
      r = g = b = 0;

      for(double i = 0; i < pixelSamples; i++) {
        for(double j = 0; j < pixelSamples; j++) {
          double rayX = m_pixel_x + i/pixelSamples;
          double rayY = m_pixel_y + j/pixelSamples;

          Ray ray(m_eye, m_view + ((rayX/m_width)-0.5)*m_tanf*m_aspect*m_right + ((rayY/m_height)-0.5)*m_tanf*m_up);
          ray.direction.normalize();
          Colour pixel_colour = rayTrace(ray, 1);

          r += pixel_colour.R();
          g += pixel_colour.G();
          b += pixel_colour.B();
        }
      }

      m_save_image(m_pixel_x, m_height-m_pixel_y-1, 0) = r/(pixelSamples*pixelSamples);
      m_save_image(m_pixel_x, m_height-m_pixel_y-1, 1) = g/(pixelSamples*pixelSamples);
      m_save_image(m_pixel_x, m_height-m_pixel_y-1, 2) = b/(pixelSamples*pixelSamples);
    }
    
    if(((double)m_pixel_y/(double)m_height)*100 >= cur_percent + 10) {
      cur_percent += 10;
      std::cout << cur_percent << '%' << " complete" << std::endl; 
    }
  }

  m_save_image.savePng(m_filename);
}

Colour RayTracer::rayTrace(Ray ray, int depth) {
  Intersection rayI;

  m_root->intersect(ray, rayI);

  // If no intersection was found, return background color
  if(rayI.t == -1) {
    if(depth == 1) {
      return get_background_colour();
    } else {
      return Colour(0.9205, 0.9607, 1.0);
    }
  }

  Colour pixel_color = Colour(0);
  Colour diffused = rayI.material->get_kd();
  Colour specular = rayI.material->get_ks();
  double reflection = rayI.material->get_reflection();
  double shininess = rayI.material->get_shininess();
  rayI.normal.normalize();

  if(rayI.material->isLight()) {
    return diffused;
  }

  pixel_color = diffused*m_ambient;

  // Apply all lighting to the surface color
  for (std::list<Light*>::const_iterator I = m_lights.begin(); I != m_lights.end(); ++I) {
    Light *light = *I;

    // Get light information
    Vector3D light_dir = light->position - rayI.point;
    double lightDistance = light_dir.length();
    light_dir.normalize();
    Ray lightRay(rayI.point, light_dir);

    // Check if the surface the intersection point is on is facing the light
    double lambent_coef = light_dir.dot(rayI.normal);
    if(lambent_coef < 0)  {
      continue;
    }

/**************************************************************************************
 ******** Calculate shadows
 **************************************************************************************/

    double lightAmount = 0;
    if(light->getLightType() == 0 || !ENABLE_SOFT_SHADOWS) {
      Intersection lightRayI;
      m_root->intersect(lightRay, lightRayI, true);
      if(lightRayI.t != -1 && lightRayI.t < lightDistance && lightRayI.material->getLight() != light ) {
        lightAmount = 0;
      } else {
        lightAmount = 1;
      }
    } else {
      for(int i = 0; i < DIST_RAY_NUM; i++) {
        double theta = RAND_01*(2.0 * M_PI); // theta == [0, 2*PI]
        double x = RAND_01*2.0 - 1.0;	 // x == [-1, 1]
        double s = sqrt(1.0 - x * x);
        Vector3D v(x, s * cos(theta), s * sin(theta));
        Point3D randomPoint = (light->position + (light->size*v));

        Vector3D sampleDir = randomPoint - rayI.point;
        sampleDir.normalize();
        Ray sampleRay(rayI.point, sampleDir);
      
        Intersection lightRayI;
        m_root->intersect(sampleRay, lightRayI, true);
        if(lightRayI.t != -1 && lightRayI.t < lightDistance && lightRayI.material->getLight() != light ) {
          continue;
        }
      
        lightAmount++;
      }

      lightAmount = lightAmount/DIST_RAY_NUM;
    }
    
    if(lightAmount == 0) continue;		// No light is reaching this point

/**************************************************************************************
 ******** Calculate the phong lighting
 **************************************************************************************/

    Vector3D r = lightRay.direction - 2.0f * (lightRay.direction.dot(rayI.normal)) * rayI.normal;
    r.normalize();

    double dot = r.dot(ray.direction);
    dot = (dot < 0) ? 0 : dot;

/**************************************************************************************
 ******** Adjust pixel colour
 **************************************************************************************/
    double attenuation = 1 / (light->falloff[0] + light->falloff[1]*lightDistance + light->falloff[2]*(lightDistance*lightDistance));

    // Adjust pixel colour to account for this lights lambent light + specular reflection
    pixel_color = pixel_color + lightAmount * (diffused * lambent_coef * light->getLight() * attenuation
                                              + specular * powf(dot, shininess) * light->getLight() * attenuation);
  }

/**************************************************************************************
 ******** Calculate reflections
 **************************************************************************************/

  // If the object is reflective, get the color of the reflected object and apply that to the surface color
  if(reflection > 0 && depth < 8) {
    Ray newRay(rayI.point, ray.direction - 2*ray.direction.dot(rayI.normal)*rayI.normal);
    newRay.direction.normalize();
    pixel_color = reflection*rayTrace(newRay, depth + 1) + ((double)1-reflection)*pixel_color;
  }

  return pixel_color;
}

Colour RayTracer::get_background_colour() {
  double r = m_background_image(m_pixel_x * m_width_ratio, (m_height-m_pixel_y-1) * m_height_ratio, 0);
  double g = m_background_image(m_pixel_x * m_width_ratio, (m_height-m_pixel_y-1) * m_height_ratio, 1);
  double b = m_background_image(m_pixel_x * m_width_ratio, (m_height-m_pixel_y-1) * m_height_ratio, 2);

  return Colour(r, g, b);
}








































