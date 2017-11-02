/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//Define basic types from CGAL templates
#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Regular_triangulation_3.h>
#include <CGAL/Regular_triangulation_euclidean_traits_3.h>
#define ALPHASHAPES
#ifdef ALPHASHAPES
	#include <CGAL/Alpha_shape_vertex_base_3.h>
	#include <CGAL/Alpha_shape_cell_base_3.h>
	#include <CGAL/Alpha_shape_3.h>
#endif
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/circulator.h>
#include <CGAL/number_utils.h>
#include <boost/static_assert.hpp>

//This include from yade let us use Eigen types
#include <lib/base/Math.hpp>

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

namespace CGT {
//Robust kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//A bit faster, but gives crash eventualy
// typedef CGAL::Cartesian<double> K;

typedef CGAL::Regular_triangulation_euclidean_traits_3<K>   				Traits;
typedef K::Point_3									Point;
typedef Traits::Vector_3 								CVector;
typedef Traits::Segment_3								Segment;
#ifndef NO_REAL_CHECK
/** compilation inside yade: check that Real in yade is the same as Real we will define; otherwise it might make things go wrong badly (perhaps) **/
BOOST_STATIC_ASSERT(sizeof(Traits::RT)==sizeof(Real));
#endif
typedef Traits::RT									Real; //Dans cartesian, RT = FT
typedef Traits::Weighted_point								Sphere;
typedef Traits::Plane_3									Plane;
typedef Traits::Triangle_3								Triangle;
typedef Traits::Tetrahedron_3								Tetrahedron;

class SimpleCellInfo : public Point {
	public:
	//"id": unique identifier of each cell, independant of other numberings used in the fluid types.
	// Care to initialize it if you need it, there is no magic numbering to rely on
	unsigned int id;
	Real s;
	bool isFictious;//true if the cell has at least one fictious bounding sphere as a vertex
	SimpleCellInfo (void) {isFictious=false; s=0;Point::operator= (CGAL::ORIGIN);}
	SimpleCellInfo& setPoint(const Point &p) { Point::operator= (p); return *this; }
	SimpleCellInfo& setScalar(const Real &scalar) { s=scalar; return *this; }
	inline Real x (void) {return Point::x();}
	inline Real y (void) {return Point::y();}
	inline Real z (void) {return Point::z();}
	inline Real& f (void) {return s;}
	//virtual function that will be defined for all classes, allowing shared function (e.g. for display of periodic and non-periodic with the unique function saveVTK)
	bool isReal (void) {return !isFictious;}
};

class SimpleVertexInfo : public CVector {
protected:
	Real s;
	unsigned int i;
	Real vol;
public:
	bool isFictious;
	SimpleVertexInfo& setVector(const CVector &u) { CVector::operator= (u); return *this; }
	SimpleVertexInfo& setFloat(const float &scalar) { s=scalar; return *this; }
	SimpleVertexInfo& setId(const unsigned int &id) { i= id; return *this; }
	inline Real ux (void) {return CVector::x();}
	inline Real uy (void) {return CVector::y();}
	inline Real uz (void) {return CVector::z();}
	inline Real& f (void) {return s;}
	inline Real& v (void) {return vol;}
	inline const unsigned int& id (void) const {return i;}
	SimpleVertexInfo (void) {isFictious=false; s=0; i=0; vol=-1;}
	//virtual function that will be defined for all classes, allowing shared function (e.g. for display)
	bool isReal (void) {return !isFictious;}
};


template<class vertex_info, class cell_info>
class TriangulationTypes {

public:
typedef vertex_info								Vertex_Info;
typedef cell_info								Cell_Info;
typedef CGAL::Triangulation_vertex_base_with_info_3<Vertex_Info, Traits>	Vb_info;
typedef CGAL::Triangulation_cell_base_with_info_3<Cell_Info, Traits>		Cb_info;
#ifdef ALPHASHAPES
typedef CGAL::Alpha_shape_vertex_base_3<Traits,Vb_info> Vb;
typedef CGAL::Alpha_shape_cell_base_3<Traits,Cb_info>   Fb;
typedef CGAL::Triangulation_data_structure_3<Vb, Fb>				Tds;
#else
typedef CGAL::Triangulation_data_structure_3<Vb_info, Cb_info>			Tds;
#endif

typedef CGAL::Triangulation_3<K>						Triangulation;
typedef CGAL::Regular_triangulation_3<Traits, Tds>				RTriangulation;
#ifdef ALPHASHAPES
typedef CGAL::Alpha_shape_3<RTriangulation>  					AlphaShape;
#endif
typedef typename RTriangulation::Vertex_iterator                    		VertexIterator;
typedef typename RTriangulation::Vertex_handle                      		VertexHandle;
typedef typename RTriangulation::Finite_vertices_iterator                    	FiniteVerticesIterator;
typedef typename RTriangulation::Cell_iterator					CellIterator;
typedef typename RTriangulation::Finite_cells_iterator				FiniteCellsIterator;
typedef typename RTriangulation::Cell_circulator				CellCirculator;
typedef typename RTriangulation::Cell_handle					CellHandle;

typedef typename RTriangulation::Facet						Facet;
typedef typename RTriangulation::Facet_iterator					FacetIterator;
typedef typename RTriangulation::Facet_circulator				FacetCirculator;
typedef typename RTriangulation::Finite_facets_iterator				FiniteFacetsIterator;
typedef typename RTriangulation::Locate_type					LocateType;

typedef typename RTriangulation::Edge_iterator					EdgeIterator;
typedef typename RTriangulation::Finite_edges_iterator				FiniteEdgesIterator;
};

typedef TriangulationTypes<SimpleVertexInfo,SimpleCellInfo>			SimpleTriangulationTypes;

} // namespace CGT

typedef CGT::CVector CVector;
typedef CGT::Point Point;

/// Converters for Eigen and CGAL vectors
inline CVector makeCgVect ( const Vector3r& yv ) {return CVector ( yv[0],yv[1],yv[2] );}
inline Point makeCgPoint ( const Vector3r& yv ) {return Point ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
inline Vector3r makeVector3r ( const CVector& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
