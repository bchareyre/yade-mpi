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
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
#include <CGAL/Triangulation_cell_base_with_info_3.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/circulator.h>
#include <CGAL/number_utils.h>
#include <boost/static_assert.hpp>

//This include from yade let us use Eigen types
#include <yade/lib/base/Math.hpp>

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
	bool isFictious;
	SimpleCellInfo (void) {isFictious=false; s=0;}
	SimpleCellInfo& operator= (const Point &p) { Point::operator= (p); return *this; }
	SimpleCellInfo& operator= (const float &scalar) { s=scalar; return *this; }
	inline Real x (void) {return Point::x();}
	inline Real y (void) {return Point::y();}
	inline Real z (void) {return Point::z();}
	inline Real& f (void) {return s;}
	//virtual function that will be defined for all classes, allowing shared function (e.g. for display of periodic and non-periodic with the unique function saveVTK)
	virtual bool isReal (void) {return !isFictious;}
};

class SimpleVertexInfo : public CVector {
protected:
	Real s;
	unsigned int i;
	Real vol;
public:
	bool isFictious;
	SimpleVertexInfo& operator= (const CVector &u) { CVector::operator= (u); return *this; }
	SimpleVertexInfo& operator= (const float &scalar) { s=scalar; return *this; }
	SimpleVertexInfo& operator= (const unsigned int &id) { i= id; return *this; }
	inline Real ux (void) {return CVector::x();}
	inline Real uy (void) {return CVector::y();}
	inline Real uz (void) {return CVector::z();}
	inline Real& f (void) {return s;}
	inline Real& v (void) {return vol;}
	inline const unsigned int& id (void) const {return i;}
	SimpleVertexInfo (void) {isFictious=false; s=0; i=0; vol=-1;}
	//virtual function that will be defined for all classes, allowing shared function (e.g. for display)
	virtual bool isReal (void) {return !isFictious;}
};

class FlowCellInfo : public SimpleCellInfo {

	public:
	//For vector storage of all cells
	unsigned int index;
	int volumeSign;
	bool Pcondition;
	Real invVoidV;
	Real t;
	int fict;
 	Real volumeVariation;
	double pression;
	 //average relative (fluid - facet translation) velocity defined for a single cell as 1/Volume * SUM_ON_FACETS(x_average_facet*average_facet_flow_rate)
	CVector averageCellVelocity;
	// Surface vectors of facets, pointing from outside toward inside the cell
	std::vector<CVector> facetSurfaces;
	//Ratio between fluid surface and facet surface 
	std::vector<Real> facetFluidSurfacesRatio;
	// Reflects the geometrical property of the cell, so that the force by cell fluid on grain "i" is pressure*unitForceVectors[i]
	std::vector<CVector> unitForceVectors;
	// Store the area of triangle-sphere intersections for each facet (used in forces definition)
	std::vector<CVector> facetSphereCrossSections;
	std::vector<CVector> cellForce;
	std::vector<double> rayHydr;
	std::vector<double> modulePermeability;
	// Partial surfaces of spheres in the double-tetrahedron linking two voronoi centers. [i][j] is for sphere facet "i" and sphere facetVertices[i][j]. Last component for 1/sum_surfaces in the facet.
	double solidSurfaces [4][4];

	FlowCellInfo (void)
	{
		modulePermeability.resize(4, 0);
		cellForce.resize(4);
		facetSurfaces.resize(4);
		facetFluidSurfacesRatio.resize(4);
		facetSphereCrossSections.resize(4);
		unitForceVectors.resize(4);
		for (int k=0; k<4;k++) for (int l=0; l<3;l++) solidSurfaces[k][l]=0;
		rayHydr.resize(4, 0);
// 		isInside = false;
		invSumK=0;
		isFictious=false; Pcondition = false; isGhost = false;
// 		isInferior = false; isSuperior = false; isLateral = false; isExternal=false;
		isvisited = false;
		index=0;
		volumeSign=0;
		s=0;
		volumeVariation=0;
		pression=0;
		invVoidV=0;
 		fict=0;
		isGhost=false;
	}	
	bool isGhost;
	double invSumK;
	bool isvisited;
	
	FlowCellInfo& operator= (const std::vector<double> &v) { for (int i=0; i<4;i++) modulePermeability[i]= v[i]; return *this; }
	FlowCellInfo& operator= (const Point &p) { Point::operator= (p); return *this; }
	FlowCellInfo& operator= (const float &scalar) { s=scalar; return *this; }
	
	inline Real& volume (void) {return t;}
	inline const Real& invVoidVolume (void) const {return invVoidV;}
	inline Real& invVoidVolume (void) {return invVoidV;}
	inline Real& dv (void) {return volumeVariation;}
	inline int& fictious (void) {return fict;}
	inline double& p (void) {return pression;}
	//For compatibility with the periodic case
	inline const double shiftedP (void) const {return pression;}
	inline const std::vector<double>& kNorm (void) const {return modulePermeability;}
	inline std::vector<double>& kNorm (void) {return modulePermeability;}
	inline std::vector< CVector >& facetSurf (void) {return facetSurfaces;}
	inline std::vector<CVector>& force (void) {return cellForce;}
	inline std::vector<double>& Rh (void) {return rayHydr;}
	inline CVector& averageVelocity (void) {return averageCellVelocity;}
};

class FlowVertexInfo : public SimpleVertexInfo {
	CVector grainVelocity;
	Real volumeIncidentCells;
public:
	FlowVertexInfo& operator= (const CVector &u) { CVector::operator= (u); return *this; }
	FlowVertexInfo& operator= (const float &scalar) { s=scalar; return *this; }
	FlowVertexInfo& operator= (const unsigned int &id) { i= id; return *this; }
	CVector forces;
	bool isGhost;
	FlowVertexInfo (void) {isGhost=false;}
	inline CVector& force (void) {return forces;}
	inline CVector& vel (void) {return grainVelocity;}
	inline Real& volCells (void) {return volumeIncidentCells;}
	inline const CVector ghostShift (void) {return CGAL::NULL_VECTOR;}
};

class PeriodicCellInfo : public FlowCellInfo
{	
	public:
	static CVector gradP;
	//for real cell, baseIndex is the rank of the cell in cellHandles. For ghost cells, it is the baseIndex of the corresponding real cell.
	//Unlike ordinary index, baseIndex is also indexing cells with imposed pressures
	int baseIndex;
	int period[3];
	static CVector hSize[3];
	static CVector deltaP;
	int ghost;
	Real* _pression;
	PeriodicCellInfo (void){
		_pression=&pression;
		period[0]=period[1]=period[2]=0;
		baseIndex=-1;
		volumeSign=0;}
	~PeriodicCellInfo (void) {}
	PeriodicCellInfo& operator= (const Point &p) { Point::operator= (p); return *this; }
	PeriodicCellInfo& operator= (const float &scalar) { s=scalar; return *this; }
	
	inline const double shiftedP (void) const {return isGhost? (*_pression)+pShift() :(*_pression) ;}
	inline const double pShift (void) const {return deltaP[0]*period[0] + deltaP[1]*period[1] +deltaP[2]*period[2];}
// 	inline const double p (void) {return shiftedP();}
	inline void setP (const Real& p) {pression=p;}
	virtual bool isReal (void) {return !(isFictious || isGhost);}
};

class PeriodicVertexInfo : public FlowVertexInfo {
	public:
	PeriodicVertexInfo& operator= (const CVector &u) { CVector::operator= (u); return *this; }
	PeriodicVertexInfo& operator= (const float &scalar) { s=scalar; return *this; }
	PeriodicVertexInfo& operator= (const unsigned int &id) { i= id; return *this; }
	int period[3];
	//FIXME: the name is misleading, even non-ghost can be out of the period and therefore they need to be shifted as well
	inline const CVector ghostShift (void) {
		return period[0]*PeriodicCellInfo::hSize[0]+period[1]*PeriodicCellInfo::hSize[1]+period[2]*PeriodicCellInfo::hSize[2];}
	PeriodicVertexInfo (void) {isFictious=false; s=0; i=0; period[0]=period[1]=period[2]=0; isGhost=false;}
	virtual bool isReal (void) {return !(isFictious || isGhost);}
};


/// 	
	
template<class vertex_info, class cell_info>
class TriangulationTypes {

public:
typedef vertex_info								Vertex_Info;
typedef cell_info								Cell_Info;
typedef CGAL::Triangulation_vertex_base_with_info_3<Vertex_Info, Traits>	Vb_info;
typedef CGAL::Triangulation_cell_base_with_info_3<Cell_Info, Traits>		Cb_info;
typedef CGAL::Triangulation_data_structure_3<Vb_info, Cb_info>			Tds;

typedef CGAL::Triangulation_3<K>						Triangulation;
typedef CGAL::Regular_triangulation_3<Traits, Tds>				RTriangulation;

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
typedef TriangulationTypes<FlowVertexInfo,FlowCellInfo>				FlowTriangulationTypes;
typedef TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo>			PeriFlowTriangulationTypes;

} // namespace CGT
