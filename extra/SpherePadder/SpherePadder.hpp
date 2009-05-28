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
#include <list>

# define BEGIN_FUNCTION(arg) if (verbose) cout << "+--> " << (arg) << endl << flush
# define END_FUNCTION        if (verbose) cout << "+-- Done <--+\n\n" << flush

#define FAIL_DET            0x01
#define FAIL_DELTA          0x02
#define FAIL_RADIUS         0x04
#define FAIL_OVERLAP        0x08
#define FAIL_GAP            0x10
#define FAIL_RADIUS_RANGE   0x20
#define FAIL_NaN            0x40

typedef unsigned int id_type; // [0 -> 4,294,967,295] if 32 bits
enum SphereType
{
  AT_NODE,
  AT_SEGMENT,
  AT_FACE,
  AT_TETRA_CENTER,
  AT_TETRA_VERTEX,
  INSERTED_BY_USER,
  FROM_TRIANGULATION,
  VIRTUAL
};

struct Criterion
{
  bool nb_spheres;
  bool solid_fraction;
  id_type nb_spheres_max;
  double solid_fraction_max;
  double x,y,z,R;
};

struct Sphere
{
  double     x,y,z,R;
  SphereType type;
};

struct Neighbor
{
  id_type i,j;
};

struct neighbor_with_distance
{
  id_type sphereId;
  double  distance;
  bool    priority;
};

struct tetra_porosity
{
  id_type id1,id2,id3,id4;
  double  volume;
  double  void_volume;
};

// not used
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
                
	vector<vector<id_type> >    combination;
    SpherePackingTriangulation  triangulation;
    vector<tetra_porosity>      tetra_porosities;
	Criterion                   criterion;
    
	double       distance_spheres (id_type i, id_type j); // deprecated
	double       distance_spheres (Sphere& S1,Sphere& S2);
	double       squared_distance_centre_spheres(id_type i, id_type j); // deprecated
    double       distance_centre_spheres(Sphere& S1, Sphere& S2);
    double       distance_vector3 (double V1[],double V2[]);
	void         build_sorted_list_of_neighbors(id_type sphereId, vector<neighbor_with_distance> & neighbor);
	void         build_sorted_list_of_neighbors(Sphere & S, vector<neighbor_with_distance> & neighbor);
    double       spherical_triangle (double point1[],double point2[],double point3[],double point4[]);
    double       solid_volume_of_tetrahedron(Sphere& S1, Sphere& S2, Sphere& S3, Sphere& S4);
    void         place_at_nodes ();
    void         place_at_segment_middle ();
    void         place_at_faces ();
    void         place_at_tetra_centers ();
    void         place_at_tetra_vertexes ();
    void         cancel_overlaps ();
	unsigned int iter_densify(unsigned int nb_check = 20);
	void         repack_null_radii();
    
    // some key functions 
	unsigned int place_fifth_sphere(id_type s1, id_type s2, id_type s3, id_type s4, Sphere& S);
	unsigned int place_sphere_4contacts (id_type sphereId, unsigned int nb_combi_max = 15);//  (deprecated)
	unsigned int place_sphere_4contacts (Sphere& S, unsigned int nb_combi_max = 15);
	unsigned int check_overlaps(Sphere & S, id_type excludedId);
	
    double       rmin,rmax,rmoy;
    double       ratio; // rmax/rmin
    double       max_overlap_rate;
	id_type      n1,n2,n3,n4,n5,n_densify,nzero;
	unsigned int max_iter_densify;
	double       virtual_radius_factor;
	bool         RadiusDataIsOK,RadiusIsSet;
	unsigned int zmin;
	double       gap_max;
        
    TetraMesh *      mesh;
    vector <Sphere>  sphere;
    CellPartition    partition;
	list <id_type>   bounds;
    
	bool verbose;
	bool Must_Stop;

  public:
   
    bool meshIsPlugged;

	void ShutUp() { verbose = false; }
	void Speak()  { verbose = true; }
	
	void setRadiusRatio (double r, double rapp = 0.125);
	void setRadiusRange (double min, double max);
	void setMaxOverlapRate (double r) { max_overlap_rate = fabs(r); }
	void setVirtualRadiusFactor (double f) {virtual_radius_factor = fabs(f);}
	void setMaxNumberOfSpheres (id_type max);
	void setMaxSolidFractioninProbe (double max, double x, double y,double z, double R);

	id_type getNumberOfSpheres ();
	double getMeanSolidFraction (double x, double y, double z, double R);
	
    void plugTetraMesh (TetraMesh * mesh);
    void save_mgpost (const char* name);
	void save_tri_mgpost (const char* name);
    void save_Rxyz (const char* name);
    
    SpherePadder ();

	// Check functions only for debug (very slow!!)
	void detect_overlap ();

	//! \brief 5-step padding (for details see Jerier et al.)
    void pad_5 ();

	//! \brief Place virtual spheres at mesh boudaries and modify the position and radius of overlapping spheres in such a way that theu are in contact with the boundary plans.
	void place_virtual_spheres ();

	//! \brief Make the packing denser by filling void spaces detected by building a Delaunay triangulation (with CGAL)
	void densify ();

	//! \brief Insert a sphere (x,y,z,R) within the packing. Overlapping spheres are cancelled.
    void insert_sphere (double x, double y, double z, double R);
    
    // FOR ANALYSIS
	void save_granulo (const char* name);
	void rdf (unsigned int Npoint, unsigned int Nrmean);
};



