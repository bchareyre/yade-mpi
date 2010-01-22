/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/core/Omega.hpp>
#include <utility>
#include<yade/core/Scene.hpp>
#include<yade/lib-triangulation/Tesselation.h>
#include<boost/python.hpp>
#include<yade/pkg-dem/MicroMacroAnalyser.hpp>
#include<yade/extra/boost_python_len.hpp>

/*! \class TesselationWrapper
 * \brief Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities : currently volume and porosity of each Voronoï sphere.
 * More accessors in course of implementation. Feel free to suggest new ones.
 *
 * Usage :
 * TW.setState(0)
 * O.run(1000); O.wait()
 * TW.setState(1)
 * array = TW.getVolPoroDef(True) //syntax here?
 * 
 */


class TesselationWrapper : public GlobalEngine{
public:

	CGT::Tesselation* Tes;
	double mean_radius;
	unsigned int n_spheres;
	bool rad_divided;
	bool bounded;
	CGT::Point Pmin;
	CGT::Point Pmax;

	TesselationWrapper();
	~TesselationWrapper();
    
    	/// Insert a sphere, "id" will be used by some getters to retrieve spheres
    	bool insert(double x, double y, double z, double rad, unsigned int id);
	/// A faster version of insert, inserting all spheres in scene
	bool insertSceneSpheres(const Scene* scene);
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
	boost::python::dict getVolPoroDef(bool deformation);//{
// 		Scene* scene=Omega::instance().getScene().get();
// 		int dim1[]={scene->bodies->size()};
// 		int dim2[]={scene->bodies->size(),3};
// 		numpy_boost<Real,1> mass(dim1);
// 		numpy_boost<Real,2> vel(dim2);
// 		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
// 			if(!b) continue;
// 			mass[b->getId()]=b->state->mass;
// 			VECTOR3R_TO_NUMPY(vel[b->getId()],b->state->vel);
// 		}
// 		python::dict ret;
// 		ret["mass"]=mass;
// 		ret["vel"]=vel;
// 		return ret;
// 	}

	
private:
	/// edge iterators are used for returning tesselation "facets", i.e. spheres with a common branch in the triangulation, convert CGAL::edge to int pair (b1->id, b2->id)
	CGT::Finite_edges_iterator facet_begin;
	CGT::Finite_edges_iterator facet_end;
	CGT::Finite_edges_iterator facet_it;
	MicroMacroAnalyser mma;
public:	
	REGISTER_ATTRIBUTES(GlobalEngine,(n_spheres));
	REGISTER_CLASS_NAME(TesselationWrapper);
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TesselationWrapper);
//} // namespace CGT
