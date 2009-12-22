//Ce fichier d�finit l'objet Empilement, un tableau
//de sph�re (soit des "points" pond�r�s au sens de CGAL)

#include "Empilement.h"
namespace CGT {
typedef Empilement::Boite				Boite;
typedef Empilement::ID					ID;
typedef Empilement::SphereID			SphereID;
typedef Empilement::Liste_SphereID		Liste_SphereID;
typedef Empilement::SphereID_iterator	SphereID_iterator;

// D�finition de la constante de classe
//int Empilement::Nmax_sphere_defaut=_Nmax_sphere_defaut;

double Rand_d (void)		  // G�n�ration d'un double al�atoire entre 0 et 1 (pr�cision ajustable)
{
	double a=rand();
	a=(a/RAND_MAX+rand())/(RAND_MAX+1.f);
	return a;
}

Sphere Rand_sph (double Rad_max)
{
	return Sphere(	Point(Rand_d(), Rand_d(), Rand_d()), Rand_d()*Rad_max );
}

Sphere Rand_sph (double Xmin, double Xmax, double Ymin, double Ymax,
				 double Zmin, double Zmax,
				 double Rad_min, double Rad_max)
{
	return Sphere(	Point(Xmin + Rand_d()*(Xmax-Xmin), Ymin + Rand_d()*(Ymax-Ymin),
					Zmin + Rand_d()*(Zmax-Zmin) ), Rad_min + Rand_d()*(Rad_max-Rad_min));	
}



Empilement::Empilement(void)
{
	Nmax_sphere = Nmax_sphere_defaut;
	srand( (unsigned)time( NULL ) );
	X_min=0; X_max=0; Y_min=0; Y_max=0; Z_min=0; Z_max=0;
	N_sphere=0;
	S_it_current = Liste_S2.begin();
	Tableau_Sphere = new Sphere* [Nmax_sphere];
	for (long j=0; j<Nmax_sphere; j++) Tableau_Sphere[j]=NULL;
}

Empilement::Empilement(const int N_s)
{	 
	Nmax_sphere= N_s;
	srand( (unsigned)time( NULL ) );
	X_min=0; X_max=0; Y_min=0; Y_max=0; Z_min=0; Z_max=0;
	N_sphere=0;
	S_it_current = Liste_S2.begin();
	Tableau_Sphere = new Sphere* [Nmax_sphere];
	for (long j=0; j<Nmax_sphere; j++) Tableau_Sphere[j]=NULL;
}

Empilement::Empilement(int N_s, Boite boite)
{
	Nmax_sphere = Nmax_sphere_defaut;
	srand( (unsigned)time( NULL ) );
	X_min=0; X_max=0; Y_min=0; Y_max=0; Z_min=0; Z_max=0;
	N_sphere=0;
	S_it_current = Liste_S2.begin();
	X_min =  CGAL::min( boite.base.x() , boite.sommet.x() ) ;
	X_max =  CGAL::max( boite.base.x() , boite.sommet.x() ) ;
	Y_min =  CGAL::min( boite.base.y() , boite.sommet.y() ) ;
	Y_max =  CGAL::max( boite.base.y() , boite.sommet.y() ) ;
	Z_min =  CGAL::min( boite.base.z() , boite.sommet.z() ) ;
	Z_max =  CGAL::max( boite.base.z() , boite.sommet.z() ) ;
	Tableau_Sphere = new Sphere* [Nmax_sphere];
	for (long j=0; j<Nmax_sphere; j++) Tableau_Sphere[j]=NULL;		
	for (int j=0; j < N_s; j++)
	{
		double rayon = (Rand_d()-0.5) * _Rad_ecart + _Rad_moyen;
		add_sphere(	( X_min + (X_max-X_min)*Rand_d() ),
					( Y_min + (Y_max-Y_min)*Rand_d() ),
					( Z_min + (Z_max-Z_min)*Rand_d() ),
					rayon);
	}
	

		 
}

Empilement::~Empilement(void)
{	
	delete [] Tableau_Sphere;
	//if (dyn_cast!=true) delete [] );  
}

ID Empilement::add_sphere(Sphere S) 
{	
	if (N_sphere < Nmax_sphere)
	{
		N_sphere++;
		SphereID SI = {N_sphere, S};
		Liste_S2.push_back(SI);
		Tableau_Sphere[N_sphere] = &(Liste_S2.back().S);
        return N_sphere;
	}
	else {
		std::cout << "N max spheres depasse" << std::endl; 
		return 0;}
}

ID Empilement::add_sphere(Real x, Real y, Real z, Real r ) 
{	
	return add_sphere(Sphere(Point(x,y,z),r));
}

Sphere* Empilement::sphere (ID n)
{
	if (Tableau_Sphere[n-1] != NULL) return Tableau_Sphere[n-1] ;
	std::cout << "la sphere n'existe pas" << std::endl;
	return NULL;
	exit (1);
}

RTriangulation Empilement::Triangulation (void)
{
	RTriangulation Tri;
	Triangule (*this, Tri);
	return Tri;
}

void Triangule ( Empilement &E, RTriangulation &T)
{	
	int current = 0;
	Vertex_handle Vh;
	//Sphere* it;
	std::cout << "top1" << std::endl;
	//CGAL_NTS square
	for (Empilement::SphereID_iterator SID = E.SphereID_begin(); SID != E.SphereID_end() ; SID++)
	{
		std::cout << "top1.5" << std::endl;
		Vh = T.insert(SID->S);
		std::cout << "top2" << std::endl;
		if (Vh->is_valid())
		{
			Vh->info() = ((const unsigned int) SID->n);
			std::cout << "top3" << std::endl;
		}
		else std::cout << "cellule pas valide!!!" << std::endl;
	}
	std::cout << "top4" << std::endl;
	std::cout << T.number_of_vertices()<< std::endl;

	/*for (bool S = E.sphere_first(); !E.sphere_last() ; E.sphere_next())
			{				
				T.insert( E.sphere_to_wpoint() );
			}*/
}





//void Triangule ( Empilement &E, RTriangulation &T)
//{	
//	int current = 0;
//	Vertex_handle Vh;
//	//Sphere* it;
//	std::cout << "top1" << std::endl;
//	//CGAL_NTS square
//
//		std::cout << "top1.5" << std::endl;
//		Vh = T.insert(Sphere(Point(0,0,0),0.3));
//		std::cout << "top2" << std::endl;
//		if (Vh->is_valid())
//		{
//			Vh->info() = (1);
//			std::cout << "top3" << std::endl;
//		}
//		else std::cout << "cellule pas valide!!!" << std::endl;
//		std::cout << "top1.5" << std::endl;
//		Vh = T.insert(Sphere(Point(0.2,0,0),0.3));
//		std::cout << "top2" << std::endl;
//		if (Vh->is_valid())
//		{
//			Vh->info() = (2);
//			std::cout << "top3" << std::endl;
//		}
//		else std::cout << "cellule pas valide!!!" << std::endl;
//		std::cout << "top1.5" << std::endl;
//		Vh = T.insert(Sphere(Point(1,0.4,0),0.4));
//		std::cout << "top2" << std::endl;
//		if (Vh->is_valid())
//		{
//			Vh->info() = (3);
//			std::cout << "top3" << std::endl;
//		}
//		else std::cout << "cellule pas valide!!!" << std::endl;
//		std::cout << "top1.5" << std::endl;
//		Vh = T.insert(Sphere(Point(0,1.1,.2),0.3));
//		std::cout << "top2" << std::endl;
//		if (Vh->is_valid())
//		{
//			Vh->info() = (4);
//			std::cout << "top3" << std::endl;
//		}
//		else std::cout << "cellule pas valide!!!" << std::endl;
//
//
//
//	/*E1.add_sphere(0,0,0,0.3);
//	E1.add_sphere(0.2,0,0,0.3);
//	E1.add_sphere(1,0.4,0,0.4);
//	E1.add_sphere(0,1,0,0.3);
//	E1.add_sphere(0,0,1,0.3);
//	E1.add_sphere(-1,0,-0.5,0.3);
//	E1.add_sphere(-0.6,-1,-0.6,0.3);
//	E1.add_sphere(1,-1,0.5,0.3);
//	E1.add_sphere(0.5,0,-1,0.3);*/
//
//	std::cout << "top4" << std::endl;
//	std::cout << T.number_of_vertices()<< std::endl;
//
//	/*for (bool S = E.sphere_first(); !E.sphere_last() ; E.sphere_next())
//			{				
//				T.insert( E.sphere_to_wpoint() );
//			}*/
//}
}