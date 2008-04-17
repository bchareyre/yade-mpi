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


typedef CGAL::Cartesian<double> K ;
typedef CGAL::Regular_triangulation_euclidean_traits_3<K>   Traits;
typedef K::Point_3											Point;
//typedef Traits::Bare_point 									Point;
typedef Traits::Vector_3 									Vecteur;
typedef Traits::Segment_3									Segment;
typedef Traits::RT											Real; //Dans cartesian, RT = FT
typedef Traits::Weighted_point								Sphere;
typedef Traits::Line_3										Droite;
typedef Traits::Plane_3										Plan;
typedef Traits::Triangle_3									Triangle;
typedef Traits::Tetrahedron_3								Tetraedre;


class Cell_Info : public Point/*, public Vecteur*/ {

	Real s;// stockage d'une valeur scalaire (ex. d�viateur) pour affichage

public:
	bool isFictious;
	Cell_Info& operator= (const Point &p) { Point::operator= (p); return *this; }
	//Info& operator= (const Vecteur &u) { Vecteur::operator= (u); return *this; }
	Cell_Info& operator= (const float &scalar) { s=scalar; return *this; }
	inline Real x (void) {return Point::x();}
	inline Real y (void) {return Point::y();}
	inline Real z (void) {return Point::z();}
	//inline Real ux (void) {return Vecteur::x();}
	//inline Real uy (void) {return Vecteur::y();}
	//inline Real uz (void) {return Vecteur::z();}
	inline Real& f (void) {return s;}

	//operator Point& (void) {return (Point&) *this;}
};

class Vertex_Info : /*public Point,*/ public Vecteur {

	Real s;// stockage d'une valeur scalaire (ex. d�viateur) pour affichage
	unsigned int i;
	Real vol;

public:
	bool isFictious;
	//Info& operator= (const Point &p) { Point::operator= (p); return *this; }
	Vertex_Info& operator= (const Vecteur &u) { Vecteur::operator= (u); return *this; }
	Vertex_Info& operator= (const float &scalar) { s=scalar; return *this; }
	Vertex_Info& operator= (const unsigned int &id) { i= id; return *this; }
	//inline Real x (void) {return Point::x();}
	//inline Real y (void) {return Point::y();}
	//inline Real z (void) {return Point::z();}
	inline Real ux (void) {return Vecteur::x();}
	inline Real uy (void) {return Vecteur::y();}
	inline Real uz (void) {return Vecteur::z();}
	inline Real& f (void) {return s;}
	inline Real& v (void) {return vol;}
	inline unsigned int& id (void) {return i;}

	//operator Point& (void) {return (Point&) *this;}
};

//typedef struct info_v //Donn�e stock�e pour chaque cellule
//{
//	Point p; //Sommet de Voronoi
//	Vecteur u; //... et son d�placement
//} Info;
//
//Point& operator(info ) 




#endif
