#include "light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0),
    intensity(1),
    size(1)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

int Light::getLightType() {
  return 0;
}

int BoxLight::getLightType() {
  return 1;
}

/*SceneNode* Light::getSceneNode() {
  GeometryNode* node = new GeometryNode("light", new NonhierSphere(position, 0.5));
  node->set_material(new LightMaterial(colour));
  return node;
}

SceneNode* BoxLight::getSceneNode() {
  NonhierBox* box = new NonhierBox(position, 1);
  GeometryNode* node = new GeometryNode("light", box);
  node->scale(Vector3D(size[0], size[1], size[2]));
  node->set_material(new LightMaterial(colour));
  return node;
}*/