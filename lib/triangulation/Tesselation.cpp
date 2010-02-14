#include "Tesselation.h"
#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
//Tesselation TESSELATION = Tesselation();


using namespace std;

namespace CGT {

Tesselation::Tesselation ( void )
{
//  std::cout << "Tesselation(void)" << std::endl;
	Tri = new RTriangulation;
	Tes = Tri;
	computed = false;
	max_id = -1;
	TotalFiniteVoronoiVolume=0;
	TotalInternalVoronoiPorosity=0;
	TotalInternalVoronoiVolume=0;
	redirected = false;
	//FIXME : find a better way to avoid segfault when insert() is used before resizing this vector
	vertexHandles.resize(1000000);
}

Tesselation::Tesselation ( RTriangulation &T )
		: Tri ( &T ), Tes ( &T ), computed ( false )
{
	std::cout << "Tesselation(RTriangulation &T)" << std::endl;
	Compute();
}


Tesselation::~Tesselation ( void )
{
	if ( Tri ) Tri->~RTriangulation();
}

void Tesselation::Clear ( void )
{
	Tri->clear();
	max_id=0;
	redirected = false;
	vertexHandles.clear();
}

Vertex_handle Tesselation::insert ( Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious )
{
	Vertex_handle Vh;
	Vh = Tri->insert(Sphere(Point(x,y,z),pow(rad,2)));
	if ( Vh!=NULL )
	{
		Vh->info() = id;
		Vh->info().isFictious = isFictious;
		vertexHandles[id] = Vh;
		/*if ( !isFictious ) */max_id = std::max ( max_id, (int) id );
	}
	else cout << id <<  " : Vh==NULL!!" << endl;
	return Vh;
}

Vertex_handle Tesselation::move ( Real x, Real y, Real z, Real rad, unsigned int id )
{
	bool fictious = vertexHandles[id]->info().isFictious;
	Vertex_handle Vh;
	Vh = Tri->move_point ( vertexHandles[id], Sphere ( Point ( x,y,z ),pow ( rad,2 ) ) );
	if ( Vh!=NULL )
	{
		vertexHandles[id] = Vh;
		Vh->info() = id;
		Vh->info().isFictious = fictious;
	}
	else cerr << "Vh==NULL" << " id=" << id << " Point=" << Point ( x,y,z ) << " rad=" << rad << endl;
	return Vh;
}



bool Tesselation::redirect ( void )
{
	if ( !redirected )
	{
		//Set size of the redirection vector
		if ( (unsigned int)max_id+1 != vertexHandles.size() ) vertexHandles.resize ( max_id+1 );
		//cout << "!redirected" << endl;
		max_id = 0;
		Finite_vertices_iterator vertices_end = Tri->finite_vertices_end ();
		for ( Finite_vertices_iterator V_it = Tri->finite_vertices_begin (); V_it !=  vertices_end; V_it++ )
		{
			vertexHandles[V_it->info().id()]= V_it;
			max_id = max(max_id, (int) V_it->info().id());
			//if ( ! ( V_it->info().isFictious ) ) vertexHandles[V_it->info().id() ]= V_it;
			//std::cout<< "Cell " << V_it->info().id() << ": v=" << V_it->info().v() << std::endl;
		}
		if ( (unsigned int)max_id+1 != vertexHandles.size() ) vertexHandles.resize ( max_id+1 );
		redirected = true;
	} else return false;
	return true;
}


bool Tesselation::remove ( unsigned int id )
{
	redirect();
	Tri->remove ( vertexHandles[id] );
	return true;
}


Finite_cells_iterator Tesselation::finite_cells_begin ( void )
{ return Tri->finite_cells_begin(); }

Finite_cells_iterator Tesselation::finite_cells_end ( void )
{ return Tri->finite_cells_end(); }

void Tesselation::Voisins ( Vertex_handle v, Vector_Vertex& Output_vector )
{ Tri->incident_vertices ( v, back_inserter ( Output_vector ) ); }

RTriangulation& Tesselation::Triangulation ( void )
{ return *Tri; }

Real Tesselation::Volume ( Finite_cells_iterator cell )
{
	return ( Tetraedre ( cell->vertex ( 0 )->point(), cell->vertex ( 1 )->point(),
						 cell->vertex ( 2 )->point(), cell->vertex ( 3 )->point() ) ).volume();
}

Plan Tesselation::Dual ( Vertex_handle S1, Vertex_handle S2 )
{
	Segment seg ( S1->point(), S2->point() );
	Real r = 0.5* ( 1.0 + ( ( S1->point() ).weight() * ( S1->point() ).weight() - ( S2->point() ).weight() * ( S2->point() ).weight() ) /seg.squared_length() );
	return Plan ( S1->point() + seg.to_vector() *r, seg.to_vector() );
}

Point Tesselation::Dual ( const Cell_handle &cell )
{
	//Sphere S0 = cell->vertex(0)->point();
	//Plan P12 = PlanRadical(S0 , cell->vertex(1)->point());
	//Plan P13 = PlanRadical(S0 , cell->vertex(2)->point());
	//Plan P14 = PlanRadical(S0 , cell->vertex(3)->point());
	//std::cout << std::endl << "Plans : " << std::endl << P12 << std::endl << P13 << std::endl << P14 << std::endl << std::endl;
	//cell->info() = Intersect(P12, P13, P14);
	//std::cout << "voro calcule : " << cell->info() << std::endl;
	return cell->info();
}

void Tesselation::Compute ()
{

//  std::cout << "Tesselation::Compute ()" << std::endl;
	redirect();
	Finite_cells_iterator cell_end = Tri->finite_cells_end();
	for ( Finite_cells_iterator cell = Tri->finite_cells_begin(); cell != cell_end; cell++ )
	{

		const Sphere& S0 = cell->vertex ( 0 )->point();
		const Sphere& S1 = cell->vertex ( 1 )->point();
		const Sphere& S2 = cell->vertex ( 2 )->point();
		const Sphere& S3 = cell->vertex ( 3 )->point();
		Real x,y,z;
//   Plan P12 = PlanRadical(S0 , cell->vertex(1)->point());
//   Plan P13 = PlanRadical(S0 , cell->vertex(2)->point());
//   Plan P14 = PlanRadical(S0 , cell->vertex(3)->point());
//   cell->info() = Intersect(P12, P13, P14);
		CGAL::weighted_circumcenterC3 (
			S0.point().x(), S0.point().y(), S0.point().z(), S0.weight(),
			S1.point().x(), S1.point().y(), S1.point().z(), S1.weight(),
			S2.point().x(), S2.point().y(), S2.point().z(), S2.weight(),
			S3.point().x(), S3.point().y(), S3.point().z(), S3.weight(),
			x, y, z );
		cell->info() =Point ( x,y,z );
		//cout << "voronoi cell : " <<  cell->vertex(0)->info().id() << " "
		//  <<  cell->vertex(1)->info().id() << " "
		//  <<  cell->vertex(2)->info().id() << " "
		//  <<  cell->vertex(3)->info().id() << "(center : " << (Point) cell->info() << ")" << endl;
	}

	
	computed = true;
}

long Tesselation::New_liste_adjacent_edges ( Vertex_handle vertex0, Real** Coordonnes )
{
	Delete_liste_edges ( Coordonnes );
	*Coordonnes = new Real [600];
	long k = 0;
	Finite_edges_iterator ed_it;

	for ( ed_it = Tri->finite_edges_begin(); ed_it != Tri->finite_edges_end(); ed_it++ )
	{
		if ( ed_it->first->vertex ( ed_it->second ) == vertex0 || ed_it->first->vertex ( ed_it->third ) == vertex0 )
		{
			Point p;
			Facet_circulator facet0, facet;
			facet0 =  Tri->incident_facets ( *ed_it );
			bool pass = false;
			for ( facet = facet0; ! ( facet == facet0 && pass ) ; facet++ )
			{
				pass = true;
				if ( !Tri->is_infinite ( ( *facet ).first ) && !Tri->is_infinite ( ( *facet ).first->neighbor ( ( *facet ).second ) ) )
				{
					cout << "p.x()     = " << p.x() << "p.y()     = " << p.y() << "p.z()     = " << p.z() << endl;
					p = ( *facet ).first->info();
					( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
					p = ( *facet ).first->neighbor ( ( *facet ).second )->info();
					( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
				}
			}
		}
	}


	return k/6;
}


long Tesselation::New_liste_short_edges2 ( Real** Coordonnes )
{
	Delete_liste_edges ( Coordonnes );
	//long j = 6 * T.number_of_finite_edges();

	long Nff = Tri->number_of_finite_facets();
	*Coordonnes = new Real [6 * Nff];

	long k = 0;
	Point p, p2;
	Finite_facets_iterator f_it;  //une facette = { cell, long }
	Cell_handle cell;
// long id_facet ;
// Locate_type lt;
// int n, m;
	CGAL::Object result;    //utilis� pour le retour du pr�dicat CGAL::intersection

	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
	//H2.top("initialisation de la liste de segments");
	for ( f_it = Tri->finite_facets_begin(); f_it != Tri->finite_facets_end(); f_it++ )
	{
		if ( is_short ( f_it ) )
		{
			p = f_it->first->info();
			( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
			Cell_handle cell = f_it->first->neighbor ( f_it->second );

			p = cell->info();
			( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
		}
	}
	return k/6;
}

bool Tesselation::is_short ( Finite_facets_iterator f_it )
{
	Cell_handle cell1 = f_it->first;
	Cell_handle cell2 = cell1->neighbor ( f_it->second );
//  return ( !Tri->is_infinite(cell1->neighbor(1)) &&  !Tri->is_infinite(cell1->neighbor(2))
//    && !Tri->is_infinite(cell1->neighbor(3))  && !Tri->is_infinite(cell1->neighbor(0))
//    && !Tri->is_infinite(cell2->neighbor(1)) &&  !Tri->is_infinite(cell2->neighbor(2))
//    && !Tri->is_infinite(cell2->neighbor(3))  && !Tri->is_infinite(cell2->neighbor(0)) );
	return ( !Tri->is_infinite ( cell1 ) &&  !Tri->is_infinite ( cell2 ) );

}

long Tesselation::New_liste_short_edges ( Real** Coordonnes )
{
	Delete_liste_edges ( Coordonnes );
	//long j = 6 * T.number_of_finite_edges();

	long Nff = Tri->number_of_finite_facets();
	*Coordonnes = new Real [6 * Nff];

	long k = 0;
	int n = 0;
	Point p, p2;
	Point v1, v2, v3;
	Finite_facets_iterator f_it;  //une facette = { cell, long }
	Cell_handle cell, location;
	long id_facet ;
	CGAL::Object result;    //utilis� pour le retour du pr�dicat CGAL::intersection

	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
	//H2.top("initialisation de la liste de segments");
	for ( f_it = Tri->finite_facets_begin(); f_it != Tri->finite_facets_end(); f_it++ )
	{
		id_facet = f_it->second;    //indice de la facette dans la cellule
		cell = f_it->first->neighbor ( id_facet );   //Deuxi�me cellule adjacente
		if ( !Tri->is_infinite ( f_it->first ) && !Tri->is_infinite ( cell ) )
		{
			p = f_it->first->info();
			p2 = cell->info();

			location = Tri->locate ( p );
			if ( !Tri->is_infinite ( location ) )
			{
				( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();

				location = Tri->locate ( p2 );
				if ( !Tri->is_infinite ( location ) ) { p = p2; }
				else
				{
					n = 0;
					v1 = f_it->first->vertex ( n == id_facet ? ++n : n )->point(); n++;
					v2 = f_it->first->vertex ( n == id_facet ? ++n : n )->point();  n++;
					v3 = f_it->first->vertex ( n == id_facet ? ++n : n )->point(); n++;
					Plan P1 ( v1, v2, v3 );

					result = CGAL::intersection ( P1, Droite ( p, p2 ) );
					if ( !CGAL::assign ( p, result ) ) std::cout << "pas de point d'intersection!!!!!!!!!" << std::endl;
				}
				( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
			}
			else
			{
				location = Tri->locate ( p2 );
				if ( !Tri->is_infinite ( location ) )
				{
					( *Coordonnes ) [k++] = p2.x(); ( *Coordonnes ) [k++] = p2.y(); ( *Coordonnes ) [k++] = p2.z();
					n = 0;
					n = 0;
					v1 = f_it->first->vertex ( n == id_facet ? ++n : n )->point(); n++;
					v2 = f_it->first->vertex ( n == id_facet ? ++n : n )->point();  n++;
					v3 = f_it->first->vertex ( n == id_facet ? ++n : n )->point(); n++;
					Plan P1 ( v1, v2, v3 );

					result = CGAL::intersection ( P1, Droite ( p, p2 ) );
					if ( !CGAL::assign ( p, result ) ) std::cout << "pas de point d'intersection!!!!!!!!!" << std::endl;
					( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
				}
			}
		}
	}
	return k/6;
}



long Tesselation::New_liste_edges ( Real** Coordonnes )
{
	Delete_liste_edges ( Coordonnes );
	//long j = 6 * T.number_of_finite_edges();

	long Nff = Tri->number_of_finite_facets();
	*Coordonnes = new Real [6 * Nff];

	long k = 0;
	Point p;

	// const Edge & e : (e.first, e.second, e.third) == const Cell_handle& c, int i, int j
	//H2.top("initialisation de la liste de segments");
	for ( Finite_facets_iterator f_it = Tri->finite_facets_begin(); f_it != Tri->finite_facets_end(); f_it++ )
	{
		if ( !Tri->is_infinite ( f_it->first ) )
		{
			p = f_it->first->info();
			//}
			( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();

			Cell_handle cell = f_it->first->neighbor ( f_it->second );
			if ( !Tri->is_infinite ( cell ) )
			{
				p = cell->info();
			}
			( *Coordonnes ) [k++] = p.x(); ( *Coordonnes ) [k++] = p.y(); ( *Coordonnes ) [k++] = p.z();
		}
	}
	return Nff;
}


Segment Tesselation::Dual ( Finite_facets_iterator &f_it )
{
	return Segment ( f_it->first->info(), ( f_it->first->neighbor ( f_it->second ) )->info() );
}

void Tesselation::AssignPartialVolume ( Finite_edges_iterator& ed_it )
{
	//Edge_iterator ed_it
	Cell_circulator cell0=Tri->incident_cells ( *ed_it );
	Cell_circulator cell2=cell0;
	if ( Tri->is_infinite ( cell2 ) )
	{
		++cell2;
		while ( Tri->is_infinite ( cell2 ) && cell2!=cell0 ) ++cell2;
		if ( cell2==cell0 ) return;
	}
	cell0=cell2++;
	Cell_circulator cell1=cell2++;
	//std::cout << "edge : " << ed_it->first->vertex ( ed_it->second )->info().id() << "-" << ed_it->first->vertex ( ed_it->third )->info().id() << std::endl;
	bool isFictious1 = ( ed_it->first )->vertex ( ed_it->second )->info().isFictious;
	bool isFictious2 = ( ed_it->first )->vertex ( ed_it->third )->info().isFictious;
	Real r;

	//cout << "cell0 : " <<  cell0->vertex(0)->info().id() << " "
	//   <<  cell0->vertex(1)->info().id() << " "
	//   <<  cell0->vertex(2)->info().id() << " "
	//   <<  cell0->vertex(3)->info().id() << "(center : " << (Point) cell0->info() << ")" <<   endl;

	while ( cell2!=cell0 )
	{
		if ( !Tri->is_infinite ( cell1 )  && !Tri->is_infinite ( cell2 ) )
		{
			// cout << "cell1 : " <<  cell1->vertex(0)->info().id() << " "
			//   <<  cell1->vertex(1)->info().id() << " "
			//   <<  cell1->vertex(2)->info().id() << " "
			//   <<  cell1->vertex(3)->info().id() << "(center : " << (Point) cell1->info() << ")" << endl;
			// cout << "cell2 : " <<  cell2->vertex(0)->info().id() << " "
			//   <<  cell2->vertex(1)->info().id() << " "
			//   <<  cell2->vertex(2)->info().id() << " "
			//   <<  cell2->vertex(3)->info().id() << "(center : " << (Point) cell2->info() << ")" << endl;

			//std::cout << "assign tetra : (" << ed_it->first->vertex ( ed_it->second )->point() << "),(" << cell0->info() << "),(" << cell1->info() << "),(" <<cell2->info() << ")" << std::endl;
			if ( !isFictious1 )
			{
				r = std::abs ( ( Tetraedre ( ed_it->first->vertex ( ed_it->second )->point(), cell0->info(), cell1->info(), cell2->info() ) ).volume() );
				//std::cout << "assigned1=" << r << " on " << ed_it->first->vertex ( ed_it->second )->info().id() << std::endl;
				( ed_it->first )->vertex ( ed_it->second )->info().v() += r;
				TotalFiniteVoronoiVolume+=r;
			}
			//std::cout << "assign tetra : (" << ed_it->first->vertex ( ed_it->third )->point() << "),(" << cell0->info() << "),(" << cell1->info() << "),(" <<cell2->info() << ")" << std::endl;
			if ( !isFictious2 )
			{
				r = std::abs ( ( Tetraedre ( ed_it->first->vertex ( ed_it->third )->point(), cell0->info(),  cell1->info(), cell2->info() ) ).volume() );
				//std::cout << "assigned2=" << r << " on " << ed_it->first->vertex ( ed_it->third )->info().id() << std::endl;
				ed_it->first->vertex ( ed_it->third )->info().v() +=r;
				TotalFiniteVoronoiVolume+=r;
			}
		}
		++cell1; ++cell2;
	}
	//std::cout << "fin AssignPartialVolume" << std::endl;
}

void Tesselation::ResetVCellVolumes ( void )
{
	for ( Vertex_iterator  V_it = Tri->vertices_begin (); V_it !=  Tri->vertices_end (); V_it++ )
	{
		V_it->info().v() =0;
	}
	TotalFiniteVoronoiVolume=0;
	TotalInternalVoronoiPorosity=0;
}

void Tesselation::ComputeVolumes ( void )
{
	if ( !computed ) Compute();
	ResetVCellVolumes();
	for ( Finite_edges_iterator ed_it=Tri->finite_edges_begin(); ed_it!=Tri->finite_edges_end();ed_it++ )
	{
		AssignPartialVolume ( ed_it );
	}

	//Delete volume for spheres on the boarders of the packing
	//FIXME: find a way to compute a volume correctly for spheres of the boarders.

//  Vector_Vertex boarder_vertices;
//  Voisins(Tri->infinite_vertex (), boarder_vertices);
//  unsigned int l = boarder_vertices.size();
//  for (unsigned int i=0; i<l; ++i)  boarder_vertices[i]->info().v()=0;
//  cout << "TotalVolume : " << TotalFiniteVoronoiVolume << endl;
}

void Tesselation::ComputePorosity ( void )  //WARNING : This function will erase real volumes of cells
{
	// and replace it with porosity
	ComputeVolumes();
	//Real rr=0;
	Finite_vertices_iterator vertices_end = Tri->finite_vertices_end ();
	for ( Finite_vertices_iterator V_it = Tri->finite_vertices_begin (); V_it !=  vertices_end; V_it++ )
	{
		if ( V_it->info().v() && !V_it->info().isFictious )
		{
			Real r = 4.188790 * std::pow ( ( V_it->point().weight() ),1.5 );// 4/3*PI*R³ = 4.188...*R³
			TotalInternalVoronoiPorosity+=r;
			//rr+=V_it->info().v();
			TotalInternalVoronoiVolume += V_it->info().v();
			V_it->info().v() =
				( V_it->info().v() - r )
				/ V_it->info().v();
			//std::cout << "Cell " << V_it->info().id() << ": V_it->point().weight()=" << V_it->point().weight() << std::endl;
			std::cout << "Cell " << V_it->info().id() << ": v=" << V_it->info().v() << " radius=" << sqrt ( V_it->point().weight() ) << " volume sphere ="<< r << std::endl;
		}
	}
	std::cout << "total internal solid = " << TotalInternalVoronoiPorosity << std::endl;
	std::cout << "TotalFiniteVoronoiVolume = " << TotalFiniteVoronoiVolume << std::endl;
	std::cout << "TotalInternalVoronoiVolume = " << TotalInternalVoronoiVolume << std::endl;

	TotalInternalVoronoiPorosity= ( TotalInternalVoronoiVolume-TotalInternalVoronoiPorosity ) /TotalInternalVoronoiVolume;
	std::cout << "TotalInternalVoronoiPorosity = " << TotalInternalVoronoiPorosity << std::endl;
}



bool Tesselation::is_internal ( Finite_facets_iterator &facet )
{
	return ( !Tri->is_infinite ( facet->first ) &&  !Tri->is_infinite ( facet->first->neighbor ( facet->second ) ) );
}

// Real& Tesselation::Volume(unsigned int id)
// {
//  return vertexHandles[id]->info().v();
// }

} //namespace CGT
