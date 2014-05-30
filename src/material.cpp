#include "material.hpp"

Material::Material(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

Material::~Material()
{
}

Colour Material::get_kd() {
  return m_kd;
}

Colour Material::get_ks() {
  return m_ks;
}

double Material::get_shininess() {
  return m_shininess;
}

double Material::get_reflection() {
  return 0.0;
}

bool Material::isLight() {
  return false;
}

Light* Material::getLight() {
  return NULL;
}

RefMaterial::RefMaterial(const Colour& kd, const Colour& ks, double shininess, double reflection)
  : Material(kd, ks, shininess), m_reflection(reflection)
{
}

RefMaterial::~RefMaterial()
{
}

double RefMaterial::get_reflection() {
  return m_reflection;
}

LightMaterial::LightMaterial(const Colour& kd, Light* l) : Material(kd, Colour(0.0, 0.0, 0.0), 0), mLight(l)
{
}

LightMaterial::~LightMaterial()
{
}

bool LightMaterial::isLight() {
  return true;
}

Light* LightMaterial::getLight() {
  return mLight;
}
