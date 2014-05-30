#include "scene.hpp"
#include <iostream>
#include "a2.hpp"

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
  m_id = 0;
}

SceneNode::~SceneNode()
{
}

void SceneNode::intersect(Ray ray, Intersection &intersection, bool light_ray)
{
  ray.origin = get_inverse() * ray.origin;
  ray.direction = get_inverse() * ray.direction;

  recurse_intersect(ray, intersection, light_ray);
  if(intersection.t != -1) {
    intersection.point = get_transform() * intersection.point;
    intersection.normal = get_transform().transpose() * intersection.normal;
  }
}

void SceneNode::transform(Point3D &point, Vector3D &vector, const Matrix4x4& matrix) {
  point = matrix*point;
  vector = matrix*vector;
  vector.normalize();
}

void SceneNode::recurse_intersect(Ray ray, Intersection &intersection, bool light_ray) {
  ChildList::const_iterator begin = m_children.begin();
  ChildList::const_iterator end = m_children.end();

  while( begin != end ) {
      SceneNode *node = *begin;

      node->intersect(ray, intersection, light_ray);
      if(light_ray && intersection.t != -1) break;

      begin++;
  }
}

void SceneNode::rotate(char axis, double angle)
{
  set_transform(m_trans*rotation(angle, axis));
}

void SceneNode::scale(const Vector3D& amount)
{
  set_transform(m_trans*scaling(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
  set_transform(m_trans*translation(amount));
}

bool SceneNode::is_joint() const
{
  return false;
}

void SceneNode::set_id(int id) {
  m_id = id;
}

int SceneNode::get_id() const {
  return m_id;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;

  if(init < m_joint_x.min) m_joint_x.init = m_joint_x.min;
  if(init > m_joint_x.max) m_joint_x.init = m_joint_x.max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;

  if(init < m_joint_y.min) m_joint_y.init = m_joint_y.min;
  if(init > m_joint_y.max) m_joint_y.init = m_joint_y.max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::intersect(Ray ray, Intersection &intersection, bool light_ray)
{
  double t;
  Point3D intersection_point; 
  Vector3D intersection_normal; 

  ray.origin = get_inverse() * ray.origin;
  ray.direction = get_inverse() * ray.direction;

  //SceneNode::intersect(ray, light_ray);
  t = m_primitive->intersect(ray.origin, ray.direction, intersection_point, intersection_normal);

  if(t != -1 && (intersection.t == -1 || intersection.t > t)) {
    intersection_point = get_transform() * intersection_point;
    intersection_normal = get_transform().transpose() * intersection_normal;

    intersection.t = t;
    intersection.point = intersection_point;
    intersection.normal = intersection_normal;
    intersection.material = m_material;
  }

  //recurse_intersect(ray_point, ray_dir, intersection, light_ray);
}













