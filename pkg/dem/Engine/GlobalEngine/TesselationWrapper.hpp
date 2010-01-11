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
//namespace CGT {


/*! \class TesselationWrapper
 * \brief Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities : currently volume and porosity of each Voronoï sphere.
 * More accessors in course of implementation. Feel free to suggest new ones.
 * 
 */
class TesselationWrapper : public GlobalEngine{
public:

	CGT::Tesselation* Tes;
	double mean_radius;
	unsigned int n_spheres;
	bool rad_divided;
	bool bounded;

	TesselationWrapper();
	~TesselationWrapper();
    
    	/// Insert a sphere, "id" will be used by some getters to retrieve spheres
    	bool insert(double x, double y, double z, double rad, unsigned int id);
	/// A faster version of insert
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
	
private:
	/// edge iterators are used for returning tesselation "facets", i.e. spheres with a common branch in the triangulation, convert CGAL::edge to int pair (b1->id, b2->id)
	CGT::Finite_edges_iterator facet_begin;
	CGT::Finite_edges_iterator facet_end;
	CGT::Finite_edges_iterator facet_it;
public:	
	REGISTER_ATTRIBUTES(GlobalEngine,(n_spheres));
	REGISTER_CLASS_NAME(TesselationWrapper);
	REGISTER_BASE_CLASS_NAME(GlobalEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TesselationWrapper);
//} // namespace CGT
