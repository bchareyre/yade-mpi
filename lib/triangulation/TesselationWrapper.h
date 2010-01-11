//
// C++ Interface: TesselationWrapper
//
// Description: 
//
//
// Author: chareyre <bruno.chareyre@hmg.inpg.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TESSELATIONWRAPPER_H
#define TESSELATIONWRAPPER_H
#include "Tesselation.h"
/**
	@author chareyre <bruno.chareyre@hmg.inpg.fr>
*/


#include <utility>

namespace CGT {

/*! \class TesselationWrapper
 * \brief Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities like volume of each Voronoï sphere.
 * More accessors in course of implementation. Feel free to suggest new ones.
 * 
 */
//  class Tesselation;
 
class TesselationWrapper {
public:

	Tesselation* Tes;
	double mean_radius;
	unsigned int n_spheres;
	bool rad_divided;
	bool bounded;

	TesselationWrapper();

	~TesselationWrapper();
    
    	/// Insert a sphere, "id" will be used by some getters to retrieve spheres
    	bool insert(double x, double y, double z, double rad, unsigned int id);
	/// A faster version of insert
	//bool insertSceneSpheres(const Scene* scene);
	/// Move one sphere to the new position and maintain triangulation (invalidates the tesselation)
	bool move (double x, double y, double z, double rad, unsigned int id);
	
    	void checkMinMax(double x, double y, double z, double rad);//for experimentation purpose	
    	void clear(void);
    	void clear2(void);
    	    	
	///Add axis aligned bounding planes (modelised as spheres with (almost) infinite radius)
  	void 	AddBoundingPlanes (void); 
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
	/// set facet = (body1->id,body2->id), returns facet_it==facet_begin
	bool nextFacet (std::pair<unsigned int, unsigned int>& facet);
	
private:
	///iterators on triangulation edges, which are facets in the tesselation 
	Finite_edges_iterator facet_begin;
	Finite_edges_iterator facet_end;
	Finite_edges_iterator facet_it;

};

} // namespace CGT
#endif
