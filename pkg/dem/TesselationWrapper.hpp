/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/core/Omega.hpp>
#include <utility>
#include<yade/core/Scene.hpp>
#include<yade/lib/triangulation/Tesselation.h>
#include<boost/python.hpp>
#include<yade/extra/boost_python_len.hpp>
#include<yade/pkg/dem/MicroMacroAnalyser.hpp>

/*! \class TesselationWrapper
 * \brief Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities : currently volume and porosity of each Voronoï sphere.
 * More accessors in course of implementation. Feel free to suggest new ones.
 *
 * Example usage script :
 *
 *tt=TriaxialTest()
 *tt.generate("test.xml")
 *O.load("test.xml")
 *O.run(100) //for unknown reasons, this procedure crashes at iteration 0
 *TW=TesselationWrapper()
 *TW.triangulate() //compute regular Delaunay triangulation, don't construct tesselation
 *TW.computeVolumes() //will silently tesselate the packing
 *TW.volume(10) //get volume associated to sphere of id 10
 *
 */


class TesselationWrapper : public GlobalEngine{
public:

	CGT::Tesselation* Tes;
	double mean_radius, inf;
	bool rad_divided;
	bool bounded;
	CGT::Point Pmin;
	CGT::Point Pmax;

	~TesselationWrapper();

    	/// Insert a sphere, "id" will be used by some getters to retrieve spheres
    	bool insert(double x, double y, double z, double rad, unsigned int id);
	/// A faster version of insert, inserting all spheres in scene (first erasing current triangulation  if reset=true)
	void insertSceneSpheres(bool reset=true);
	/// Move one sphere to the new position (x,y,z) and maintain triangulation (invalidates the tesselation)
	bool move (double x, double y, double z, double rad, unsigned int id);

    	void checkMinMax(double x, double y, double z, double rad);//for experimentation purpose
	/// Reset the triangulation
    	void clear(void);
    	void clear2(void);

	/// Add axis aligned bounding planes (modelised as spheres with (almost) infinite radius)
  	void 	AddBoundingPlanes (void);
	/// Force boudaries at positions not equal to precomputed ones
 	void	AddBoundingPlanes(double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz, double dt);
	void 	RemoveBoundingPlanes (void);
	///Compute voronoi centers then stop (don't compute anything else)
 	void	ComputeTesselation (void);
 	void	ComputeTesselation( double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz, double dt);

	///Compute Voronoi vertices + volumes of all cells
	///use ComputeTesselation to force update, e.g. after spheres positions have been updated
  	void	ComputeVolumes	(void);
	///Get volume of the sphere inserted with indentifier "id""
	double	Volume	(unsigned int id);

	/// number of facets in the tesselation (finite branches of the triangulation)
	unsigned int NumberOfFacets(bool initIters=false);
	/// set first and last facets, set facet_it = facet_begin
	void InitIter(void);
	/// set facet = next pair (body1->id,body2->id), returns facet_it==facet_end
	bool nextFacet (std::pair<unsigned int, unsigned int>& facet);

	/// make the current state the initial (0) or final (1) configuration for the definition of displacement increments, use only state=0 if you just want to get only volmumes and porosity
	void setState (bool state=0);
	/// return python array containing voronoi volumes, per-particle porosity, and optionaly per-particle deformation, if states 0 and 1 have been assigned
	boost::python::dict getVolPoroDef(bool deformation);//FIXME ; unexplained crash for now


public:
	/// edge iterators are used for returning tesselation "facets", i.e. spheres with a common branch in the triangulation, convert CGAL::edge to int pair (b1->id, b2->id)
	CGT::Finite_edges_iterator facet_begin;
	CGT::Finite_edges_iterator facet_end;
	CGT::Finite_edges_iterator facet_it;
	MicroMacroAnalyser mma;

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(TesselationWrapper,GlobalEngine,"Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities : currently volume and porosity of each Voronoï sphere. Example script :\n tt=TriaxialTest()\ntt.generate('test.xml')\nO.load('test.xml')\nO.run(100) //for unknown reasons, this procedure crashes at iteration 0\nTW=TesselationWrapper()\nTW.triangulate() //compute regular Delaunay triangulation, don't construct tesselation\nTW.computeVolumes() //will silently tesselate the packing\nTW.volume(10) //get volume associated to sphere of id 10",
	((unsigned int,n_spheres,0,,"|ycomp|"))
	,/*ctor*/
  	Tes = new CGT::Tesselation;
	clear();
	facet_begin = Tes->Triangulation().finite_edges_begin();
	facet_end = Tes->Triangulation().finite_edges_end();
	facet_it = Tes->Triangulation().finite_edges_begin();
	inf=1e10;
	,/*py*/
	.def("triangulate",&TesselationWrapper::insertSceneSpheres,(python::arg("reset")=true),"triangulate spheres of the packing")
 	.def("setState",&TesselationWrapper::setState,(python::arg("state")=0),"Make the current state the initial (0) or final (1) configuration for the definition of displacement increments, use only state=0 if you just want to get only volmumes and porosity.")
 	.def("volume",&TesselationWrapper::Volume,(python::arg("id")=0),"Returns the volume of Voronoi's cell of a sphere.")
 	.def("computeVolumes",&TesselationWrapper::ComputeVolumes,"Compute volumes of all Voronoi's cells.")
	.def("getVolPoroDef",&TesselationWrapper::getVolPoroDef,(python::arg("deformation")=false),"Return a table with per-sphere computed quantities. Include deformations on the increment defined by states 0 and 1 if deformation=True (make sure to define states 0 and 1 consistently).")
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TesselationWrapper);
//} // namespace CGT
