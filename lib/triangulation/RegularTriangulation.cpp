// Triangulation_3/example_regular.C


#include "RegularTriangulation.h"

namespace CGT {

typedef Real*			p_Real;

//Conversion de Weight vers Real
W_TO_DOUBLE			W_to_Real;


long New_segments ( Real** ppCoordonnes )
{	
	RTriangulation T;
	// insertion of points on a 3D grid
	std::vector<Vertex_handle> V;

	//int current = 0;
	for (int z=0 ; z<3 ; z++)
		for (int y=0 ; y<3 ; y++)
			for (int x=0 ; x<3 ; x++)
			
			{
				Point p( x+z/40, y-x/20, z+y/30);
				Real w = (0.2); // let's say this is the weight.
				Sphere wp(p, w);
				V.push_back(T.insert(wp));
			}

			assert( T.is_valid() );
			assert( T.dimension() == 3 );

			std::cout << "Number of finite edges : " << T.number_of_finite_edges() << std::endl;

			//W = new Real [6];
			long L = New_liste_edges(T, ppCoordonnes);

			return L;
}
//int Regular_Triangule( )
//{	
//	RTriangulation T;
//	// insertion of points on a 3D grid
//	std::vector<Vertex_handle> V;
//
//	for (Real z=0.02 ; z<5 ; z++)
//		for (Real y=0 ; y<5 ; y++)
//			for (Real x=-0.3 ; x<5 ; x++)
//			{
//				Bare_point p(x, y, z);
//				Weight w = (x+y-z*y*x)*2.0; // let's say this is the weight.
//				Weighted_point wp(p, w);
//				V.push_back(T.insert(wp));
//			}
//
//			assert( T.is_valid() );
//			assert( T.dimension() == 3 );
//
//			std::cout << "Number of vertices : " << T.number_of_vertices() << std::endl;
//
//			Real* W;
//			//W = new Real [6];
//			long L = New_liste_edges(T, &W);
//
//			return 0;
//}


//long New_liste_edges (RTriangulation T, Weight** Coordonnes)
//{
//	long j = 6 * T.number_of_finite_edges();
//	long k = 0;
//	if (Coordonnes == NULL) Coordonnes = new p_Weight;
//	if ((*Coordonnes)!= NULL) delete [] (*Coordonnes);
//	p_Weight coor;
//	coor = new Weight [j];
//	Edge_iterator ed_it;
//	ed_it = T.edges_begin();
//    Bare_point p;
//	Segment seg;
//	Real X;
//	
//	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
//	
//	for (long i=0; i < T.number_of_finite_edges(); i++)
//	{
//		if (!T.is_infinite(*ed_it))
//		{
//			seg = T.segment(*ed_it);
//			p = seg.point ( 0 );
//			coor[k++] = p.x();
//			coor[k++] = p.y();
//			coor[k++] = p.z();
//			p = seg.point ( 1 );
//			coor[k++] = p.x();
//			coor[k++] = p.y();
//			coor[k++] = p.z();
//			//std::cout  << p.z() << std::endl;
//			X = W_to_Real(p.z());
//		}
//		++ed_it;
//	}
//	*Coordonnes = coor;
//
//	std::cout << "fini";
//    return j;
//}

long New_liste_edges (RTriangulation &T, Real** ppCoordonnes)
{
	//Real_timer H2;
	Delete_liste_edges(ppCoordonnes);
	//long j = 6 * T.number_of_finite_edges();
	long k = 0;
	*ppCoordonnes = new Real [6 * T.number_of_finite_edges()];
	//coor = new Real [j];
	Edge_iterator ed_it;
	ed_it = T.edges_begin();
    Point p;
	Segment seg;
	long Ne = T.number_of_edges();
	//Real X;
	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
	//H2.top("initialisation de la liste de segments");
	for (long i=0; i < Ne; i++)
	{
		if (!T.is_infinite(*ed_it))
		{
			//H2.top("reste de la boucle");
			seg = T.segment(*ed_it);
			//H2.top("segment");
			p = seg.point ( 0 );
			(*ppCoordonnes)[k++] = p.x();
			//X = W_to_Real( p.x() );
			(*ppCoordonnes)[k++] = p.y();
			(*ppCoordonnes)[k++] = p.z();
			p = seg.point ( 1 );
			(*ppCoordonnes)[k++] = p.x();
			(*ppCoordonnes)[k++] = p.y();
			(*ppCoordonnes)[k++] = p.z();
			//H2.top("copie en elle m�me");
		}
		++ed_it;		
	}
	//H2.top("convertir les segments en tableau de Reals");
	//*ppCoordonnes = coor;
	std::cout << "fini";
    return T.number_of_finite_edges();
}

long New_liste_edges (Triangulation &T, Real** ppCoordonnes)
{
	//Real_timer H2;
	Delete_liste_edges(ppCoordonnes);
	//long j = 6 * T.number_of_finite_edges();
	long k = 0;
	*ppCoordonnes = new Real [6 * T.number_of_finite_edges()];
	//coor = new Real [j];
	Triangulation::Edge_iterator ed_it;
	ed_it = T.edges_begin();
    Point p;
	Segment seg;
	long Ne = T.number_of_edges();
	//Real X;
	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
	//H2.top("initialisation de la liste de segments");
	for (long i=0; i < Ne; i++)
	{
		if (!T.is_infinite(*ed_it))
		{
			//H2.top("reste de la boucle");
			seg = T.segment(*ed_it);
			//H2.top("segment");
			p = seg.point ( 0 );
			(*ppCoordonnes)[k++] = p.x();
			//X = W_to_Real( p.x() );
			(*ppCoordonnes)[k++] = p.y();
			(*ppCoordonnes)[k++] = p.z();
			p = seg.point ( 1 );
			(*ppCoordonnes)[k++] = p.x();
			(*ppCoordonnes)[k++] = p.y();
			(*ppCoordonnes)[k++] = p.z();
			//H2.top("copie en elle m�me");
		}
		++ed_it;		
	}
	//H2.top("convertir les segments en tableau de Reals");
	//*ppCoordonnes = coor;
	std::cout << "fini";
    return T.number_of_finite_edges();
}

void Delete_liste_edges (Real** ppCoordonnes, long N_edges)
{
	if (ppCoordonnes != NULL)
	{
		delete [] (*ppCoordonnes);
		//N_edges *= 6;		//Nombre de points
		//for (int i = 0; i <  N_edges; i++)
		//{
		//	delete ppCoordonnes[i];		
		//}
	}
	else std::cout << "Real** ppCoordonnees doit �tre d�fini " << std::endl;
}

} //namespace CGT
