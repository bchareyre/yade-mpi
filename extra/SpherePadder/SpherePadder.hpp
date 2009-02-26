/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "TetraMesh.hpp"
#include "CellPartition.hpp"
#include "SpherePackingTriangulation/SpherePackingTriangulation.hpp"
#include <time.h>
#include <set>

# define BEGIN_FUNCTION(arg) if (trace_functions) cerr << (arg) << "... " << flush
# define END_FUNCTION        if (trace_functions) cerr << "Done\n" << flush

enum SphereType {AT_NODE, AT_SEGMENT, AT_FACE, AT_TETRA_CENTER, AT_TETRA_VERTEX, VIRTUAL, INSERED_BY_USER, FROM_TRIANGULATION};

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

struct tetra_porosity
{
  unsigned int id1,id2,id3,id4;
  double volume;
  double void_volume;
};

class CompareNeighborId
{
  public:
  bool operator()(Neighbor& N1, Neighbor& N2)
  {
    if (N1.i < N2.i) return true;
    if ((N1.i == N2.i) && (N1.j < N2.j)) return true;
    return false;
  }
};

class SpherePadder
{
  protected:
                
    vector<vector<unsigned int> > combination;
    SpherePackingTriangulation    triangulation;
    vector<tetra_porosity>        tetra_porosities;
    
    double       distance_spheres (unsigned int i, unsigned int j);
    double       distance_centre_spheres(Sphere& S1, Sphere& S2);
    double       distance_vector3 (double V1[],double V2[]);
    double       spherical_triangle (double point1[],double point2[],double point3[],double point4[]);
    double       solid_volume_of_tetrahedron(Sphere& S1, Sphere& S2, Sphere& S3, Sphere& S4);
    void         place_at_nodes ();
    void         place_at_segment_middle ();
    void         place_at_faces ();
    void         place_at_tetra_centers ();
    void         place_at_tetra_vertexes ();
    void         cancel_overlaps ();
    void         place_virtual_spheres ();
    
    // 
    unsigned int place_fifth_sphere(unsigned int s1, unsigned int s2, unsigned int s3, unsigned int s4, Sphere& S);
    unsigned int place_sphere_4contacts (unsigned int sphereId, unsigned int nb_combi_max = 30);
    
    // Check functions
    void         detect_overlap ();
    
    double       rmin,rmax,rmoy,dr;
    double       ratio;
    double       max_overlap_rate;
    unsigned int n1,n2,n3,n4,n5,n_densify;
    unsigned int nb_iter_max;
        
    TetraMesh *      mesh;
    vector <Sphere>  sphere;
    CellPartition    partition;
   
    // FOR ANALYSIS
    set<Neighbor,CompareNeighborId> neighbor; // non utilise pour le moment    
    bool probeIsDefined;
    vector<unsigned int> sphereInProbe;
    double xProbe,yProbe,zProbe,RProbe;
    ofstream compacity_file;
    
    double compacity_in_probe(unsigned int ninsered);
    void check_inProbe(unsigned int i);
    
    bool trace_functions;
    void save_granulo(const char* name);
 
  public:
   
    bool meshIsPlugged;
   
    void plugTetraMesh (TetraMesh * mesh);
    void save_mgpost (const char* name);
    void save_Rxyz   (const char* name);
        
    double virtual_radius_factor;
    
    SpherePadder();
        
    // High level pading functions
    void pad_5 ();
    void tetra_pad();
	void densify();

    // void insert_sphere(double x, double y, double z, double R);
    // void densify ();     
    
    // FOR ANALYSIS
    void add_spherical_probe(double Rfact = 1.0);   
};



