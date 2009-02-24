#ifndef SPHERE_PACKING_TRIANGULATION_HPP
#define SPHERE_PACKING_TRIANGULATION_HPP

//#include <vector>
#include <map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/circulator.h>
#include <CGAL/number_utils.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_3                                     Point;
typedef K::Vector_3                                    Vecteur;
typedef K::RT                                          Real; 

class Cell_Info : public Point
{
  public:
    Cell_Info& operator= (const Point &p) { Point::operator= (p); return *this; }
    Real scalar;
    bool isVirtual; // utile ?
};

class Vertex_Info :  public Vecteur 
{
  public:
    
    unsigned int  id;
    bool          isVirtual;
};

typedef Vertex_Info                                                  vertex_info;  
typedef Cell_Info                                                    cell_info;
typedef CGAL::Triangulation_vertex_base_with_info_3<vertex_info, K>  Vb_info;
typedef CGAL::Triangulation_cell_base_with_info_3<cell_info, K>      Cb_info;
typedef CGAL::Triangulation_data_structure_3<Vb_info, Cb_info>       Tds;
typedef CGAL::Delaunay_triangulation_3<K, Tds>                       Triangulation;

typedef K::Tetrahedron_3                          Tetrahedron;

typedef Triangulation::Vertex_iterator            Vertex_iterator;
typedef Triangulation::Vertex_handle              Vertex_handle;
typedef Triangulation::Finite_vertices_iterator   Finite_vertices_iterator;
//typedef RTriangulation::Finite_vertex_handle       Finite_vertices_handle;

typedef Triangulation::Cell_iterator              Cell_iterator;
typedef Triangulation::Finite_cells_iterator      Finite_cells_iterator;
typedef Triangulation::Cell_circulator            Cell_circulator;
typedef Triangulation::Cell_handle                Cell_handle;

typedef Triangulation::Facet                      Facet;
typedef Triangulation::Facet_iterator             Facet_iterator;
typedef Triangulation::Facet_circulator           Facet_circulator;
typedef Triangulation::Finite_facets_iterator     Finite_facets_iterator;
typedef Triangulation::Locate_type                Locate_type;

typedef Triangulation::Edge_iterator              Edge_iterator;
typedef Triangulation::Finite_edges_iterator      Finite_edges_iterator;


class SpherePackingTriangulation
{
  protected:
    
    Triangulation tri;

    Finite_cells_iterator     tetrahedron_iterator;
    Finite_vertices_iterator  sphere_iterator;
    bool                      volumeAreComputed;
    std::map<unsigned int,Vertex_handle> id_vh_link;  
    
    Vertex_handle get_vertex_handle(unsigned int i);
    void record(unsigned int i,  Vertex_handle vh);
    
  public:
  
    SpherePackingTriangulation();
 
    bool insert_node(Real x, Real y, Real z, unsigned int id, bool isVirtual = false);
  
    void init_current_sphere();
    void init_current_tetrahedron();
   
    bool next_tetrahedron();
    bool next_sphere(); 
  
    float current_tetrahedron_get_volume();
    void  current_tetrahedron_get_nodes(unsigned int & id1, unsigned int & id2, unsigned int & id3, unsigned int & id4);
    
};



#endif // SPHERE_PACKING_TRIANGULATION_HPP
