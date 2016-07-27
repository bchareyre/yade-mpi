/*************************************************************************
*  Copyright (C) 2013 by Bruno Chareyre    <bruno.chareyre@hmg.inpg.fr>  *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Cartesian.h>
#include <iostream>
#include <fstream>
/////////////////////////////////////////////////////////////////////
/*
TYPES:
Triangulation_vertex_base_with_id_3: we redefine a vertex base including an index for each vertex (available in CGAL in 2D but not in 3D),
MeniscusPhysicalData: the physical variables describing a capillary bridge, with a few algebraic operators for computing weighted averages
Meniscus: a structure combining MeniscusPhysicalData with some cached data allowing faster operations in multiple queries (pointer to the last cell found and its normals)

FUNCTIONS:
getIncidentVtxWeights: an interpolation algorithm which is 20x faster than the natural neighbor interpolation of CGAL.
			returns a list of vertices incident to a query point and their respective weights
interpolate: uses the results of getIncidentVtxWeights combined with a data array to return a weighted average,
			may be used with arbitrary data types provided they have the required algebraic operators
main: example usage
*/
/////////////////////////////////////////////////////////////////////

namespace CGAL {



//Vertex base including an index for each vertex, adapted from CGAL::Triangulation_vertex_base_with_id_2
template < typename GT,  typename Vb = Triangulation_vertex_base_with_info_3<unsigned,GT> >
class Triangulation_vertex_base_with_id_3 : public Vb
{
  int _id;
public:
  typedef typename Vb::Cell_handle                   Cell_handle;
  typedef typename Vb::Point                         Point;
  template < typename TDS3 >
  struct Rebind_TDS {
    typedef typename Vb::template Rebind_TDS<TDS3>::Other          Vb3;
    typedef Triangulation_vertex_base_with_id_3<GT, Vb3>   Other;
  };

  Triangulation_vertex_base_with_id_3(): Vb() {}
  Triangulation_vertex_base_with_id_3(const Point & p): Vb(p) {}
  Triangulation_vertex_base_with_id_3(const Point & p, Cell_handle c): Vb(p, c) {}
  Triangulation_vertex_base_with_id_3(Cell_handle c): Vb(c) {}
  unsigned int id() const { return this->info(); }
  unsigned int& id()       { return this->info(); }
};

// The function returning vertices and their weights for an arbitrary point in R3 space.
// The returned triplet contains:
// 	- the output iterator pointing to the filled vector of vertices
// 	- the sum of weights for normalisation
// 	- a bool telling if the query point was located inside the convex hull of the data points
template <class Dt, class OutputIterator>
Triple< OutputIterator,  // iterator with value type std::pair<Dt::Vertex_handle, Dt::Geom_traits::FT>
    typename Dt::Geom_traits::FT,  // Should provide 0 and 1
    bool >
getIncidentVtxWeights(const Dt& dt,
            const typename Dt::Geom_traits::Point_3& Q,
            OutputIterator nn_out, typename Dt::Geom_traits::FT & norm_coeff,
            std::vector<typename Dt::Geom_traits::Vector_3>& normals,
            typename Dt::Cell_handle& start = CGAL_TYPENAME_DEFAULT_ARG Dt::Cell_handle())
{
  //helpful array for permutations
  const int comb [6] = {1, 2, 3, 0, 1, 2};
  typedef typename Dt::Geom_traits Gt;
  typedef typename Dt::Cell_handle Cell_handle;
  typedef typename Dt::Locate_type Locate_type;
  typedef typename Gt::FT Coord_type;
  CGAL_triangulation_precondition (dt.dimension()== 3);
  Locate_type lt; int li, lj;
  Cell_handle c = dt.locate( Q, lt, li, lj, start);
  bool updateNormals = (c!=start || normals.size()<4);
  if (updateNormals) normals.clear();
  if ( lt == Dt::VERTEX )
    {
      *nn_out++= std::make_pair(c->vertex(li),Coord_type(1));
      return make_triple(nn_out,norm_coeff=Coord_type(1),true);
    }
  else if (dt.is_infinite(c))
    return make_triple(nn_out, Coord_type(1), false);//point outside the convex-hull
  norm_coeff=0;
  for ( int k=0;k<4;k++ )
  {
    if (updateNormals) {
    normals.push_back(cross_product(c->vertex(comb[k])->point()-c->vertex(comb[k+1])->point(),c->vertex(comb[k])->point()-c->vertex(comb[k+2])->point()));
      normals[k] = normals[k]/
      ((c->vertex(k)->point()-c->vertex(comb[k])->point())*normals[k]);
    }
    Coord_type closeness = ((Q-c->vertex(comb[k])->point())*normals[k]);
      Coord_type w = closeness;
    *nn_out++= std::make_pair(c->vertex(k),w);
    norm_coeff += w;
  }
  start = c;
  return make_triple(nn_out,norm_coeff,true);
}



} //END NAMESPACE CGAL

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K>::Geom_traits		Traits;
typedef CGAL::Triangulation_vertex_base_with_id_3<Traits>	Vb;
typedef CGAL::Triangulation_cell_base_3<Traits>			Cb;
typedef CGAL::Triangulation_data_structure_3<Vb, Cb>		Tds;
typedef CGAL::Delaunay_triangulation_3<Traits,Tds>		DT;
typedef std::vector< std::pair< DT::Vertex_handle, K::FT> >	Vertex_weight_vector;

template <class Dt, class DataOwner>
typename DataOwner::Data interpolate1 (const Dt& dt, const typename Dt::Geom_traits::Point_3& Q, DataOwner& owner, const std::vector<typename DataOwner::Data>& rawData, bool reset)
{
    K::FT norm;
    Vertex_weight_vector coords;    
    if (reset) owner.cell = dt.cells_begin();
    CGAL::Triple<std::back_insert_iterator<Vertex_weight_vector>,K::FT, bool> result = CGAL::getIncidentVtxWeights(dt, Q,std::back_inserter(coords), norm, owner.normals , owner.cell);
     
    typename DataOwner::Data data = typename DataOwner::Data();//initialize null solution
    if (!result.third) return data;// out of the convex hull, we return the null solution
    //else, we compute the weighted sum
    for (unsigned int k=0; k<coords.size(); k++) data += (rawData[coords[k].first->id()]*coords[k].second);
    if (!data.ending) return data*(1./result.second);
    else return typename DataOwner::Data();
}
template <class Dt, class DataOwner>
typename DataOwner::Data interpolate2 (const Dt& dt, const typename Dt::Geom_traits::Point_3& Q, DataOwner& owner, const std::vector<typename DataOwner::Data>& rawData, bool reset)
{
    K::FT norm;
    Vertex_weight_vector coords;
    if (reset) owner.cell = dt.cells_begin();
    CGAL::Triple<std::back_insert_iterator<Vertex_weight_vector>,K::FT, bool> result = CGAL::getIncidentVtxWeights(dt, Q,std::back_inserter(coords), norm, owner.normals , owner.cell);

    typename DataOwner::Data data = typename DataOwner::Data();//initialize null solution
    if (!result.third) return data;// out of the convex hull, we return the null solution
    //else, we compute the weighted sum
    for (unsigned int k=0; k<coords.size(); k++) data += (rawData[coords[k].first->id()]*coords[k].second);
    if (!data.ending) return data*(1./result.second);
    else return typename DataOwner::Data();
}

