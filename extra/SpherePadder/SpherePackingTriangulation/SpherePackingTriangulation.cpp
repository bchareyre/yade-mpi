#include "SpherePackingTriangulation.hpp"

using namespace std;

SpherePackingTriangulation::SpherePackingTriangulation()
{
  cerr << "OK SpherePackingTriangulation::SpherePackingTriangulation()" << endl;
  volumeAreComputed = false;
}
    
Vertex_handle SpherePackingTriangulation::get_vertex_handle(unsigned int i)
{
  std::map<unsigned int,Vertex_handle>::const_iterator ip = id_vh_link.find(i);
  if (ip != id_vh_link.end()) return ((Vertex_handle) ip->second);
  return 0;       
}
    
void SpherePackingTriangulation::record(unsigned int i, Vertex_handle vh)
{
  id_vh_link.insert(std::map<unsigned int,Vertex_handle>::value_type(i,vh));
}
    
    
    
bool SpherePackingTriangulation::insert_node(Real x, Real y, Real z, unsigned int id, bool isVirtual)
{
  Vertex_handle Vh;
  Vh = tri.insert (Point(x,y,z));

  if (Vh != NULL)
  {
    Vh->info().id        = id;
    Vh->info().isVirtual = isVirtual;
    record(id,Vh);
    return true;
  }
  return false;
}
   

void SpherePackingTriangulation::init_current_sphere() 
{ 
  sphere_iterator = tri.finite_vertices_begin(); 
}
  
void SpherePackingTriangulation::init_current_tetrahedron() 
{ 
  tetrahedron_iterator = tri.finite_cells_begin(); 
}
  
  
bool SpherePackingTriangulation::next_tetrahedron()
{
  if (++tetrahedron_iterator == tri.finite_cells_end()) return false;
  return true;
}
  
bool SpherePackingTriangulation::next_sphere()
{
  if (++sphere_iterator == tri.finite_vertices_end()) return false;
  return true;
}
  
float SpherePackingTriangulation::current_tetrahedron_get_volume()
{
  Cell_handle cell = tetrahedron_iterator;
  return (Tetrahedron(cell->vertex(0)->point(), cell->vertex(1)->point(),
          cell->vertex(2)->point(), cell->vertex (3)->point())).volume();
}

// warning order of radii is the same as id
void SpherePackingTriangulation::current_tetrahedron_get_circumcenter(Real R1,Real R2,Real R3,Real R4,Real& x, Real& y, Real& z)
{
  Cell_handle cell = tetrahedron_iterator;

   Real p1x = cell->vertex(0)->point().x();
   Real p1y = cell->vertex(0)->point().y();
   Real p1z = cell->vertex(0)->point().z();

   Real p2x = cell->vertex(1)->point().x();
   Real p2y = cell->vertex(1)->point().y();
   Real p2z = cell->vertex(1)->point().z();

   Real p3x = cell->vertex(2)->point().x();
   Real p3y = cell->vertex(2)->point().y();
   Real p3z = cell->vertex(2)->point().z();

   Real p4x = cell->vertex(3)->point().x();
   Real p4y = cell->vertex(3)->point().y();
   Real p4z = cell->vertex(3)->point().z();

    CGAL::weighted_circumcenterC3 (
                                   p1x, p1y, p1z, R1*R1,
                                   p2x, p2y, p2z, R2*R2,
                                   p3x, p3y, p3z, R3*R3,
                                   p4x, p4y, p4z, R4*R4,
                                   x, y, z
                                  );
}

void SpherePackingTriangulation::current_tetrahedron_get_nodes(unsigned int & id1, unsigned int & id2, unsigned int & id3, unsigned int & id4)
{
  Cell_handle cell = tetrahedron_iterator;
  id1 = (cell->vertex(0))->info().id;
  id2 = (cell->vertex(1))->info().id;
  id3 = (cell->vertex(2))->info().id;
  id4 = (cell->vertex(3))->info().id;
}

