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

/**
	@author chareyre <bruno.chareyre@hmg.inpg.fr>
*/


#include <utility>


class Tesselation;


class TesselationWrapper{

private:

	Tesselation* Tes;
	double mean_radius;
	unsigned int n_spheres;
	bool rad_divided;
	bool bounded;
	
public:
    TesselationWrapper();

    ~TesselationWrapper();
    
    	///Insert a sphere
    	bool insert(double x, double y, double z, double rad, unsigned int id);
    	bool move (double x, double y, double z, double rad, unsigned int id);
    	void clear(void);
    	    	
	///Add axis aligned bounding planes (modelised as spheres with (almost) infinite radius)
	void 	AddBoundingPlanes (void); 
	void 	RemoveBoundingPlanes (void);
	///Compute voronoi centers then stop (don't compute anything else)
	void	ComputeTesselation (void);
	
	///Compute Voronoi vertices + volumes of all cells
	///use ComputeTesselation to force update, e.g. after spheres positions have been updated
	void	ComputeVolumes	(void);
	
	double	Volume	(unsigned int id);
	
	unsigned int NumberOfFacets(void);
	void nextFacet (std::pair<unsigned int, unsigned int>& facet); 
};

#endif
