
#ifndef _def_Empilement
#define _def_Empilement

#include "RegularTriangulation.h"
#include "Tesselation.h"
//#include "Def_types.h"

#include <iostream>
//#include <tchar.h>
#include <time.h>			// Pour initialisation du tirage al�atoire
#include <list>


#define		_Nmax_sphere_defaut	5000
#define		_Rad_moyen		0.2
#define		_Rad_ecart		0.1

namespace CGT{
//// Macro to get a random integer with a specified range 
//#define getrandom(min, max) \
//    ((rand()%(int)(((max) + 1)-(min)))+ (min))

double Rand_d (void);	//Tirage d'un double entre 0 et 1
Sphere Rand_sph (double rad_max = 0.01); //Tirage d'une sph�re dans une bo�te (0,0,0) -> (1,1,1) 
Sphere Rand_sph (double Xmin, double Xmax,
				 double Ymin, double Ymax,
				 double Zmin, double Zmax,
				 double Rad_min, double Rad_max); //Tirage d'une sph�re dans une bo�te Lmin -> Lmax 


class Empilement;
void Triangule ( Empilement &E, RTriangulation &T);


class Empilement
{
public :
	typedef struct {Point base; Point sommet;}			Boite;
	typedef long										ID;
	typedef struct Gr {ID n; Sphere S;}					SphereID;
	typedef std::list <SphereID>						Liste_SphereID;
	typedef Liste_SphereID::iterator					SphereID_iterator;

	const static long Nmax_sphere_defaut = _Nmax_sphere_defaut;
	

private :
	Liste_SphereID Liste_S2;
	SphereID_iterator S_it_current;
	long Nmax_sphere;
	long N_sphere;
	double X_min, X_max, Y_min, Y_max, Z_min, Z_max;
	Sphere **Tableau_Sphere;
	
public:
	

	Empilement(void);
	Empilement(int N_S);
	Empilement(int N_S, Boite boite);
	~Empilement(void);

	ID add_sphere(Sphere S);
	ID add_sphere(Real x, Real y, Real z, Real r ); 
	Sphere* sphere (ID n);
	bool sphere_first () {S_it_current = Liste_S2.begin(); return !Liste_S2.empty();}
	bool sphere_next () { if (Liste_S2.end() != S_it_current) return(++S_it_current != Liste_S2.end()); return false;}
	Sphere* sphere_current () {return &(S_it_current->S);}
	Sphere  sphere_to_wpoint () {Real w = CGAL::square (S_it_current->S.weight()); return Sphere(S_it_current->S.point(), w);}
	long N_Spheres ()	{return N_sphere;}
	SphereID_iterator SphereID_begin () {return Liste_S2.begin();}
	SphereID_iterator SphereID_end () {return Liste_S2.end();} 

	bool sphere_last () {return (S_it_current == Liste_S2.end());}
    
    RTriangulation Triangulation (void);
};



//		EXEMPLE ENTREE/SORTIE
// file          : examples/Triangulation_2/terrain.C
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
//#include <CGAL/Delaunay_triangulation_2.h>
//
//#include <fstream>
//
//struct K : CGAL::Exact_predicates_inexact_constructions_kernel {};
//
//typedef CGAL::Triangulation_euclidean_traits_xy_3<K>  Gt;
//typedef CGAL::Delaunay_triangulation_2<Gt> Delaunay;
//
//typedef K::Point_3   Point;
//
//int main()
//{
//  std::ifstream in("data/terrain.cin");
//  std::istream_iterator<Point> begin(in);
//  std::istream_iterator<Point> end;
//
//  Delaunay dt;
//  dt.insert(begin, end);
//  std::cout << dt.number_of_vertices() << std::endl;
//  return 0;
//}
// 

} //namespace CGT
#endif