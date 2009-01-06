/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERE_PADDER_HPP
#define SPHERE_PADDER_HPP

#include "TetraMesh.hpp"

struct Sphere
{
	double                x,y,z,R;
	unsigned int          type; // FIXME utiliser un enum ??
	unsigned int          tetraOwner;
	vector<unsigned int>  owner; // a documenter... FIXME necessaire ?
	// type = 0 => owner = nodeId
	// type = 1 => owner = segId
};

struct Neighbor
{
	unsigned int i,j;	
};

class SpherePadder
{
protected:
	
	double distance_spheres (unsigned int i, unsigned int j);
        double distance_centre_spheres(Sphere& S1, Sphere& S2);
	void place_at_nodes ();
	void place_at_segment_middle ();
	void place_at_faces_barycentre ();
	void cancel_overlap ();
	void place_at_tetra_centers ();
	
public:
	double rmin,rmax,rmoy,dr;
	double ratio;
	double max_overlap; // ATTENTION negatif 
	
	TetraMesh * mesh;
	vector<Sphere> sphere;

	void read_data (const char* filename);
	void plugTetraMesh (TetraMesh * mesh);
	void save_mgpost (const char* name);
	//void save_Rxyz (const char* name);
	
	void pad_5 ();		
};

#endif // SPHERE_PADDER_HPP
