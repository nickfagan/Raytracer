#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  virtual double intersect(Point3D ray_point, Vector3D ray_dir, Point3D &intersection_point, Vector3D &intersection_normal);

  typedef std::vector<int> Face;
  typedef std::vector<Point3D> Triangle;
  
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  std::vector<Triangle> m_triangles;

  double m_min_x, m_max_x;
  double m_min_y, m_max_y;
  double m_min_z, m_max_z;

  void set_min_max(Point3D point);

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
