//D�finit les types � partir des types standards de CGAL

#ifndef _Def_types
#define _Def_types
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

// #include<yade/lib/base/Math.hpp> // typedef for Real

namespace CGT{
//Robust kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//A bit faster, but gives crash eventualy
// typedef CGAL::Cartesian<double> K;

typedef CGAL::Regular_triangulation_euclidean_traits_3<K>   				Traits;
typedef K::Point_3									Point;
//typedef Traits::Bare_point 								Point;
typedef Traits::Vector_3 								Vecteur;
typedef Traits::Segment_3								Segment;
#ifndef NO_REAL_CHECK
/** compilation inside yade: check that Real in yade is the same as Real we will define; otherwise it might make things go wrong badly (perhaps) **/
BOOST_STATIC_ASSERT(sizeof(Traits::RT)==sizeof(Real));
#endif
typedef Traits::RT									Real; //Dans cartesian, RT = FT
typedef Traits::Weighted_point								Sphere;
typedef Traits::Line_3									Droite;
typedef Traits::Plane_3									Plan;
typedef Traits::Triangle_3								Triangle;
typedef Traits::Tetrahedron_3								Tetraedre;
// typedef std::vector<double>								VectorD;

//using namespace std;

class Cell_Info : public Point/*, public Vecteur*/ {

	public:
	Real s;// stockage d'une valeur scalaire (ex. d�viateur) pour affichage
	bool isFictious;
#ifdef FLOW_ENGINE
	//For vector storage of all cells
	unsigned int index;
	bool Pcondition;
	Real t;
	int fict;
 	Real VolumeVariation;
	double pression; //stockage d'une valeur de pression pour chaque cellule
	Vecteur Average_Cell_Velocity; //average velocity defined for a single cell as 1/Volume * SUM_ON_FACETS(x_average_facet*average_facet_flow_rate)

	// Surface vectors of facets, pointing from outside toward inside the cell
	std::vector<Vecteur> facetSurfaces;
	std::vector<Vecteur> facetVelocities;
	// Reflects the geometrical property of the cell, so that the force by cell fluid on grain "i" is pressure*unitForceVectors[i]
	std::vector<Vecteur> unitForceVectors;
	// Store the area of triangle-sphere intersections for each facet (used in forces definition)
	std::vector<Vecteur> facetSphereCrossSections;
	std::vector<Vecteur> cell_force;
	std::vector<double> RayHydr;
// 	std::vector<double> flow_rate;
	std::vector<double> module_permeability;
	// Partial surfaces of spheres in the double-tetrahedron linking two voronoi centers. [i][j] is for sphere facet "i" and sphere facetVertices[i][j]. Last component for 1/sum_surfaces in the facet.
	double solidSurfaces [4][4];
// 	std::vector<Vecteur> vec_forces;
	Cell_Info (void)
	{
		module_permeability.resize(4, 0);
		cell_force.resize(4);
		facetSurfaces.resize(4);
		facetSphereCrossSections.resize(4);
		facetVelocities.resize(4);
		unitForceVectors.resize(4);
		for (int k=0; k<4;k++) for (int l=0; l<3;l++) solidSurfaces[k][l]=0;
		RayHydr.resize(4, 0);
		isInside = false;
		inv_sum_k=0;
		isFictious=false; Pcondition = false; isInferior = false; isSuperior = false; isLateral = false; isvisited = false; isExternal=false;
		index=0;
	}	

	double inv_sum_k;
	bool isInside;
	bool isInferior;
	bool isSuperior;
	bool isLateral;
	bool isvisited;
	bool isExternal;
// 	bool isBizarre;
	Cell_Info& operator= (const std::vector<double> &v) { for (int i=0; i<4;i++) module_permeability[i]= v[i]; return *this; }
#endif
	Cell_Info& operator= (const Point &p) { Point::operator= (p); return *this; }
	//Info& operator= (const Vecteur &u) { Vecteur::operator= (u); return *this; }

	Cell_Info& operator= (const float &scalar) { s=scalar; return *this; }
// 	Cell_Info& operator= (const float &scalar) { VolumeVariation=scalar; return *this; }
	inline Real x (void) {return Point::x();}
	inline Real y (void) {return Point::y();}
	inline Real z (void) {return Point::z();}
	//inline Real ux (void) {return Vecteur::x();}
	//inline Real uy (void) {return Vecteur::y();}
	//inline Real uz (void) {return Vecteur::z();}
	inline Real& f (void) {return s;}
#ifdef FLOW_ENGINE
	inline Real& volume (void) {return t;}
	inline Real& dv (void) {return VolumeVariation;}
	inline int& fictious (void) {return fict;}
	inline double& p (void) {return pression;}

	inline std::vector<double>& k_norm (void) {return module_permeability;}
	inline std::vector< Vecteur >& facetSurf (void) {return facetSurfaces;}
	inline std::vector< Vecteur >& facetVelocity (void) {return facetVelocities;}
	inline std::vector<Vecteur>& force (void) {return cell_force;}
	inline std::vector<double>& Rh (void) {return RayHydr;}

	inline Vecteur& av_vel (void) {return Average_Cell_Velocity;}
// 	inline vector<Vecteur>& F (void) {return vec_forces;}
// 	inline vector<double>& Q (void) {return flow_rate;}
// 	inline vector<Real>& d (void) {return distance;}
#endif
	//operator Point& (void) {return (Point&) *this;}
};



class Vertex_Info : /*public Point,*/ public Vecteur {

	Real s;// stockage d'une valeur scalaire (ex. d�viateur) pour affichage
	unsigned int i;
	Real vol;
#ifdef FLOW_ENGINE
	Vecteur Grain_Velocity;
	Real volume_incident_cells;
#endif
public:
	bool isFictious;

	//Info& operator= (const Point &p) { Point::operator= (p); return *this; }
	Vertex_Info& operator= (const Vecteur &u) { Vecteur::operator= (u); return *this; }
	Vertex_Info& operator= (const float &scalar) { s=scalar; return *this; }
	Vertex_Info& operator= (const unsigned int &id) { i= id; return *this; }
// 	inline Real x (void) {return Point::x();}
// 	inline Real y (void) {return Point::y();}
// 	inline Real z (void) {return Point::z();}
	inline Real ux (void) {return Vecteur::x();}
	inline Real uy (void) {return Vecteur::y();}
	inline Real uz (void) {return Vecteur::z();}
	inline Real& f (void) {return s;}
	inline Real& v (void) {return vol;}
	inline const unsigned int& id (void) const {return i;}

#ifdef FLOW_ENGINE
	Vecteur forces;
	inline Vecteur& force (void) {return forces;}
	inline Vecteur& vel (void) {return Grain_Velocity;}
	inline Real& vol_cells (void) {return volume_incident_cells;}
#endif
	//operator Point& (void) {return (Point&) *this;}
};

//typedef struct info_v //Donn�e stock�e pour chaque cellule
//{
//	Point p; //Sommet de Voronoi
//	Vecteur u; //... et son d�placement
//} Info;
//
//Point& operator(info )




} // namespace CGT

#endif
