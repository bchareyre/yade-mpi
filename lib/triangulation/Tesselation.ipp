/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//FIXME: handle that a better way
#define MAX_ID 200000

namespace CGT {

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::vector;
using std::ifstream;

template<class TT>
_Tesselation<TT>::_Tesselation ( void )
{
	Tri = new RTriangulation;
	Tes = Tri;
	computed=false;
	maxId = -1;
	TotalFiniteVoronoiVolume=0;
	area=0;
	TotalInternalVoronoiPorosity=0;
	TotalInternalVoronoiVolume=0;
	redirected = false;
	//FIXME : find a better way to avoid segfault when insert() is used before resizing this vector
	vertexHandles.resize(MAX_ID+1,NULL);
}
template<class TT>
_Tesselation<TT>::_Tesselation ( RTriangulation &T ) : Tri ( &T ), Tes ( &T ), computed ( false )
{
	std::cout << "Tesselation(RTriangulation &T)" << std::endl;
	compute();
}

template<class TT>
_Tesselation<TT>::~_Tesselation ( void )
{
	if ( Tri ) Tri->~RTriangulation();
}
template<class TT>
void _Tesselation<TT>::Clear ( void )
{
	Tri->clear();
	redirected = false;
	vertexHandles.clear();
	vertexHandles.resize(MAX_ID+1,NULL);
	maxId=0;
}
template<class TT>
typename _Tesselation<TT>::VertexHandle _Tesselation<TT>::insert ( Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious )
{
	VertexHandle Vh;
	Vh = Tri->insert(Sphere(Point(x,y,z),pow(rad,2)));
	if ( Vh!=NULL )
	{
		Vh->info().setId(id);
		Vh->info().isFictious = isFictious;
		assert (vertexHandles.size()>id);
		vertexHandles[id] = Vh;
		/*if ( !isFictious ) */maxId = std::max ( maxId, (int) id );
	}
	else cout << id <<  " : Vh==NULL!!"<< " id=" << id << " Point=" << Point ( x,y,z ) << " rad=" << rad << endl;
	return Vh;
}
template<class TT>
typename _Tesselation<TT>::VertexHandle _Tesselation<TT>::move ( Real x, Real y, Real z, Real rad, unsigned int id )
{
	bool fictious = vertexHandles[id]->info().isFictious;
	VertexHandle Vh;
	Vh = Tri->move_point ( vertexHandles[id], Sphere ( Point ( x,y,z ),pow ( rad,2 ) ) );
	if ( Vh!=NULL )
	{
		vertexHandles[id] = Vh;
		Vh->info().setId(id);
		Vh->info().isFictious = fictious;
	}
	else cerr << "Vh==NULL" << " id=" << id << " Point=" << Point ( x,y,z ) << " rad=" << rad << endl;
	return Vh;
}


template<class TT>
bool _Tesselation<TT>::redirect ( void )
{
	if ( !redirected )
	{
		//Set size of the redirection vector
		if ( (unsigned int)maxId+1 != vertexHandles.size() ) vertexHandles.resize ( maxId+1,NULL );
		maxId = 0;
		FiniteVerticesIterator verticesEnd = Tri->finite_vertices_end ();
		for ( FiniteVerticesIterator vIt = Tri->finite_vertices_begin (); vIt !=  verticesEnd; vIt++ )
		{
			vertexHandles[vIt->info().id()]= vIt;
			maxId = max(maxId, (int) vIt->info().id());
		}
		if ( (unsigned int)maxId+1 != vertexHandles.size() ) vertexHandles.resize ( maxId+1 );
		redirected = true;
	} else return false;
	return true;
}

template<class TT>
bool _Tesselation<TT>::remove ( unsigned int id )
{
	redirect();
	Tri->remove ( vertexHandles[id] );
	return true;
}

template<class TT>
void _Tesselation<TT>::voisins ( VertexHandle v, VectorVertex& Output_vector )
{ Tri->incident_vertices ( v, back_inserter ( Output_vector ) ); }

template<class TT>
typename _Tesselation<TT>::RTriangulation & _Tesselation<TT>::Triangulation ( void )
{ return *Tri; }

template<class TT>
Real _Tesselation<TT>::Volume ( FiniteCellsIterator cell )
{
	return ( Tetrahedron ( cell->vertex ( 0 )->point(), cell->vertex ( 1 )->point(),
						 cell->vertex ( 2 )->point(), cell->vertex ( 3 )->point() ) ).volume();
}

template<class TT>
Plane _Tesselation<TT>::Dual ( VertexHandle S1, VertexHandle S2 )
{
	Segment seg ( S1->point(), S2->point() );
	Real r = 0.5* ( 1.0 + ( ( S1->point() ).weight() * ( S1->point() ).weight() - ( S2->point() ).weight() * ( S2->point() ).weight() ) /seg.squared_length() );
	return Plan ( S1->point() + seg.to_vector() *r, seg.to_vector() );
}

template<class TT>
Point _Tesselation<TT>::Dual ( const CellHandle &cell )
{
	return cell->info();
}

template<class TT>
void _Tesselation<TT>::compute ()
{
	if (!redirected) redirect();
	FiniteCellsIterator cellEnd = Tri->finite_cells_end();
	for ( FiniteCellsIterator cell = Tri->finite_cells_begin(); cell != cellEnd; cell++ )
	{

		const Sphere& S0 = cell->vertex ( 0 )->point();
		const Sphere& S1 = cell->vertex ( 1 )->point();
		const Sphere& S2 = cell->vertex ( 2 )->point();
		const Sphere& S3 = cell->vertex ( 3 )->point();
		Real x,y,z;
		CGAL::weighted_circumcenterC3 (
			S0.point().x(), S0.point().y(), S0.point().z(), S0.weight(),
			S1.point().x(), S1.point().y(), S1.point().z(), S1.weight(),
			S2.point().x(), S2.point().y(), S2.point().z(), S2.weight(),
			S3.point().x(), S3.point().y(), S3.point().z(), S3.weight(),
			x, y, z );
		cell->info().setPoint(Point(x,y,z));
	}
	computed = true;
	
//	AlphaShape as (*Tri);
//	as.set_alpha(as.find_alpha_solid());
//	cerr << "Alpha shape computed" <<endl;
//	std::list<CellHandle> cells,cells2;
//	as.get_alpha_shape_cells(std::back_inserter(cells),
  //             AlphaShape::EXTERIOR);
//	as.get_alpha_shape_cells(std::back_inserter(cells2),
  //             AlphaShape::INTERIOR);
//	std::cerr<< "num exterior cells "<< cells.size() <<" vs. "<<cells2.size() <<std::endl;
}

template<class TT>
Segment _Tesselation<TT>::Dual ( FiniteFacetsIterator &f_it )
{
	return Segment ( f_it->first->info(), ( f_it->first->neighbor ( f_it->second ) )->info() );
}

template<class TT>
double _Tesselation<TT>::computeVFacetArea ( FiniteEdgesIterator ed_it )
{
	CellCirculator cell0 = Tri->incident_cells ( *ed_it );
	CellCirculator cell2 = cell0;

	if ( Tri->is_infinite ( cell2 ) ){
		++cell2;
		while ( Tri->is_infinite ( cell2 ) && cell2!=cell0 ) ++cell2;
		if ( cell2==cell0 ) return 0;
	}
	cell0=cell2++;
	CellCirculator cell1=cell2++;
	Real area = 0;

	while ( cell2!=cell0 ){
	  	area+= sqrt(std::abs (( Triangle ( cell0->info(), cell1->info(), cell2->info() ) ).squared_area())) ;
		++cell1;
		++cell2;
	}
	return area;
}

template<class TT>
void _Tesselation<TT>::AssignPartialVolume ( FiniteEdgesIterator& ed_it )
{
	//EdgeIterator ed_it
	CellCirculator cell0=Tri->incident_cells ( *ed_it );
	CellCirculator cell2=cell0;
	if ( Tri->is_infinite ( cell2 ) )
	{
		++cell2;
		while ( Tri->is_infinite ( cell2 ) && cell2!=cell0 ) ++cell2;
		if ( cell2==cell0 ) return;
	}
	cell0=cell2++;
	CellCirculator cell1=cell2++;
	bool isFictious1 = ( ed_it->first )->vertex ( ed_it->second )->info().isFictious;
	bool isFictious2 = ( ed_it->first )->vertex ( ed_it->third )->info().isFictious;
	Real r;
	while ( cell2!=cell0 )
	{
		if ( !Tri->is_infinite ( cell1 )  && !Tri->is_infinite ( cell2 ) )
		{
			if ( !isFictious1 )
			{
				r = std::abs ( ( Tetrahedron ( ed_it->first->vertex ( ed_it->second )->point(), cell0->info(), cell1->info(), cell2->info() ) ).volume() );
				( ed_it->first )->vertex ( ed_it->second )->info().v() += r;
				TotalFiniteVoronoiVolume+=r;
			}
			if ( !isFictious2 )
			{
				r = std::abs ( ( Tetrahedron ( ed_it->first->vertex ( ed_it->third )->point(), cell0->info(),  cell1->info(), cell2->info() ) ).volume() );
				ed_it->first->vertex ( ed_it->third )->info().v() +=r;
				TotalFiniteVoronoiVolume+=r;
			}
		}
		++cell1; ++cell2;
	}
}

template<class TT>
void _Tesselation<TT>::ResetVCellVolumes ( void )
{
	for ( VertexIterator  vIt = Tri->vertices_begin (); vIt !=  Tri->vertices_end (); vIt++ ) vIt->info().v() =0;
	TotalFiniteVoronoiVolume=0;
	TotalInternalVoronoiPorosity=0;
}

template<class TT>
void _Tesselation<TT>::computeVolumes ( void )
{
	if ( !computed ) compute();
	ResetVCellVolumes();
	for ( FiniteEdgesIterator ed_it=Tri->finite_edges_begin(); ed_it!=Tri->finite_edges_end();ed_it++ )
	{
		AssignPartialVolume ( ed_it );
	}
	//FIXME: find a way to compute a volume correctly for spheres of the boarders.
}

template<class TT>
void _Tesselation<TT>::computePorosity ( void )  //WARNING : This function will erase real volumes of cells
{
	computeVolumes();
	FiniteVerticesIterator verticesEnd = Tri->finite_vertices_end ();
	for ( FiniteVerticesIterator vIt = Tri->finite_vertices_begin (); vIt !=  verticesEnd; vIt++ )
	{
		if ( vIt->info().v() && !vIt->info().isFictious )
		{
			Real r = 4.188790 * std::pow ( ( vIt->point().weight() ),1.5 );// 4/3*PI*R³ = 4.188...*R³
			TotalInternalVoronoiPorosity+=r;
			TotalInternalVoronoiVolume += vIt->info().v();
			vIt->info().v() =
				( vIt->info().v() - r )
				/ vIt->info().v();
		}
	}
	TotalInternalVoronoiPorosity= ( TotalInternalVoronoiVolume-TotalInternalVoronoiPorosity ) /TotalInternalVoronoiVolume;
}


template<class TT>
bool _Tesselation<TT>::is_internal ( FiniteFacetsIterator &facet )
{
	return ( !Tri->is_infinite ( facet->first ) &&  !Tri->is_infinite ( facet->first->neighbor ( facet->second ) ) );
}
	
	

template<class Tesselation>
typename Tesselation::VertexHandle PeriodicTesselation<Tesselation>::insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious, int duplicateOfId)
{
	VertexHandle Vh;
	if (!Tri) cerr<<"!Tri!"<<endl;
	Vh = Tri->insert(Sphere(Point(x,y,z),pow(rad,2)));
	if ( Vh!=NULL )
	{
		Vh->info() = id;
		Vh->info().isFictious = isFictious;
		if (duplicateOfId<0) {
			assert (vertexHandles.size()>id);
			vertexHandles[id] = Vh;
			maxId = std::max ( maxId, (int) id );
			Vh->info().isGhost=0;
		} else Vh->info().isGhost=1;
	}
	else cerr << " : Vh==NULL!!" << " id=" << id << " Point=" << Point ( x,y,z ) << " rad=" << rad<<" fictious="<<isFictious<< endl;
	return Vh;
}

template<class Tesselation>
bool PeriodicTesselation<Tesselation>::redirect ( void )
{
	if ( !redirected )
	{
		//Set size of the redirection vector
		if ( (unsigned int)maxId+1 != vertexHandles.size() ) vertexHandles.resize ( maxId+1,NULL );
		cout << "!redirected" << endl;
		maxId = 0;
		FiniteVerticesIterator verticesEnd = Tri->finite_vertices_end ();
		for ( FiniteVerticesIterator vIt = Tri->finite_vertices_begin (); vIt !=  verticesEnd; vIt++ )
		{
			if (vIt->info().isGhost) continue;
			vertexHandles[vIt->info().id()]= vIt;
			maxId = max(maxId, (int) vIt->info().id());
		}
		if ( (unsigned int)maxId+1 != vertexHandles.size() ) vertexHandles.resize ( maxId+1 );
		redirected = true;
	} else return false;
	return true;
}

} //namespace CGT
