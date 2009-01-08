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

enum SphereType {AT_NODE, AT_SEGMENT, AT_FACE, AT_TETRA_CENTER, AT_TETRA_VERTEX};

struct Sphere
{
	double        x,y,z,R;
        SphereType    type; 
	unsigned int  tetraOwner;
};

struct Neighbor
{
        unsigned int i,j;	
};

struct neighbor_with_distance
{
        unsigned int sphereId;
        double       distance;
};

class SpherePadder
{
protected:
        	
        vector<vector<unsigned int> > combination;
  
	double       distance_spheres (unsigned int i, unsigned int j);
        double       distance_centre_spheres(Sphere& S1, Sphere& S2);
        double       distance_vector3 (double V1[],double V2[]);
	void         place_at_nodes ();
        void         place_at_segment_middle ();
	void         place_at_faces ();
        void         place_at_tetra_centers ();
        void         place_at_tetra_vertexes ();
	void         cancel_overlap ();
        unsigned int place_fifth_sphere(unsigned int s1, unsigned int s2, unsigned int s3, unsigned int s4, Sphere& S);
        unsigned int place_sphere_4contacts (unsigned int sphereId);
        	 
	double rmin,rmax,rmoy,dr;
	double ratio;
	double max_overlap_rate;
        unsigned int n1,n2,n3,n4,n5,n_densify;
	
	TetraMesh * mesh;
	vector<Sphere> sphere;

 public:
   
	void plugTetraMesh (TetraMesh * mesh);
	void save_mgpost (const char* name);
	void save_Rxyz   (const char* name);
	
        SpherePadder();
        // TODO destructor that clean TetraMesh*
        
	void pad_5 ();
        // void densify ();	
};


#endif // SPHERE_PADDER_HPP

