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
	return ( Tetrahedron ( cell->vertex ( 0 )->point().point(), cell->vertex ( 1 )->point().point(),
						 cell->vertex ( 2 )->point().point(), cell->vertex ( 3 )->point().point() ) ).volume();
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
	for ( FiniteCellsIterator cell = Tri->finite_cells_begin(); cell != cellEnd; cell++ ) cell->info().setPoint(circumCenter(cell));
// 	{
// 
// 		const Sphere& S0 = cell->vertex ( 0 )->point();
// 		const Sphere& S1 = cell->vertex ( 1 )->point();
// 		const Sphere& S2 = cell->vertex ( 2 )->point();
// 		const Sphere& S3 = cell->vertex ( 3 )->point();
// 		Real x,y,z;
// 		CGAL::weighted_circumcenterC3 (
// 			S0.point().x(), S0.point().y(), S0.point().z(), S0.weight(),
// 			S1.point().x(), S1.point().y(), S1.point().z(), S1.weight(),
// 			S2.point().x(), S2.point().y(), S2.point().z(), S2.weight(),
// 			S3.point().x(), S3.point().y(), S3.point().z(), S3.weight(),
// 			x, y, z );
// 		cell->info().setPoint(Point(x,y,z));
// 	}
	computed = true;
}

template<class TT>
Point _Tesselation<TT>::circumCenter (const Sphere& S0, const Sphere& S1, const Sphere& S2, const Sphere& S3)
{
        Real x,y,z;
        CGAL::weighted_circumcenterC3 (
                S0.point().x(), S0.point().y(), S0.point().z(), S0.weight(),
                S1.point().x(), S1.point().y(), S1.point().z(), S1.weight(),
                S2.point().x(), S2.point().y(), S2.point().z(), S2.weight(),
                S3.point().x(), S3.point().y(), S3.point().z(), S3.weight(),
                x, y, z );
        return Point(x,y,z);
}

//construct the circumCenter of a facet vs. an external sphere of weight wExt (wExt=alpha, typically)
template<class TT>
Point _Tesselation<TT>::circumCenter (const CellHandle& cell, const short facet, const double wExt, bool& violate, Sphere& SAlpha, CVector& normal)
{
        const Sphere& S0 = cell->vertex ( facetVertices[facet][0] )->point();
        const Sphere& S1 = cell->vertex ( facetVertices[facet][1] )->point();
        const Sphere& S2 = cell->vertex ( facetVertices[facet][2] )->point();
        const Sphere& Sin = cell->vertex (facet)->point();
	CVector surface = 0.5*cross_product ( S0.point()-S1.point(), S0.point()-S2.point() );
	//check if the surface vector is inward or outward
	double dotP = surface* ( S0.point()-Sin.point() );
	if ( dotP<0 ) surface=-surface;
	double area = sqrt ( surface.squared_length() );
	normal = surface/area; //unit normal
// 	p1 = setCircumCenter(cell1);//starting point of the polygon
	Point vv=setCircumCenter(cell);
	double h1 = ( S0.point()-vv ) *normal; //orthogonal distance from Voronoi vertex to the plane in which the spheres lie, call the intersection V
	Point p2 = vv+ h1*normal; 
	double sqR = ( p2-S0.point() ).squared_length(); //squared distance between V and the center of sphere 0
	double temp = wExt + S0.weight() -sqR;
	Point OAlpha = p2+sqrt(temp)*normal;//center of the alpha sphere
	SAlpha=Sphere(OAlpha,wExt);
	p2=circumCenter(SAlpha,S0,S1,S2);
        violate = ((p2-vv)*normal<0);
// 	cerr<<"circumCenter(0) "<<OAlpha<<" "<<vv<<" "<< S0.point() <<" "<<S1.point()  <<" "<< S2.point() <<" "<< Sin.point() <<" "<< S0.point() << " "<<  S0.weight()<<" "<<normal<<" " <<violate<<endl;
	return p2;
}

template<class TT>
Point _Tesselation<TT>::circumCenter (const CellHandle& cell, const short facet, const Sphere& sExt, bool& violate)
{
        const Sphere& S0 = cell->vertex ( facetVertices[facet][0] )->point();
        const Sphere& S1 = cell->vertex ( facetVertices[facet][1] )->point();
        const Sphere& S2 = cell->vertex ( facetVertices[facet][2] )->point();
        const Sphere& Sin = cell->vertex (facet)->point();
        CVector surface = 0.5*cross_product ( S0.point()-S1.point(), S0.point()-S2.point() );
        //check if the surface vector is inward or outward
        double dotP = surface* ( S0.point()-Sin.point() );
        if ( dotP<0 ) surface=-surface;
//         double area = sqrt ( surface.squared_length() );
//         CVector normal = surface/area; //unit normal
//      p1 = setCircumCenter(cell1);//starting point of the polygon
        Point vv=setCircumCenter(cell);
        Point p2=circumCenter(S0,S1,S2,sExt);
        violate = ((p2-vv)*(S0.point()-Sin.point())<0);
	return p2;
}

template<class TT>
Point _Tesselation<TT>::circumCenter (const CellHandle& cell)
{
	const Sphere& S0 = cell->vertex ( 0 )->point();
	const Sphere& S1 = cell->vertex ( 1 )->point();
	const Sphere& S2 = cell->vertex ( 2 )->point();
	const Sphere& S3 = cell->vertex ( 3 )->point();
	return circumCenter (S0, S1, S2, S3);
}
template<class TT>
Point _Tesselation<TT>::setCircumCenter (const CellHandle& cell,bool force) {
	if (force or cell->info()==CGAL::ORIGIN) cell->info().setPoint(circumCenter(cell));
	return (Point) cell->info();
}

template<class TT>
std::vector<int> _Tesselation<TT>::getAlphaVertices(double alpha)
{
	cerr<<"Warning: this is extremely slow - only for experiments"<<endl;
	RTriangulation temp(*Tri);
	AlphaShape as (temp);
	if (!alpha) alpha=as.find_alpha_solid();
	as.set_alpha(alpha);
	std::list<VertexHandle> alphaVertices;
	as.get_alpha_shape_vertices(std::back_inserter(alphaVertices), AlphaShape::REGULAR);
	std::vector<int> res;
	for (auto v=alphaVertices.begin(); v!=alphaVertices.end(); v++) res.push_back((*v)->info().id());
	return res;
}	
	
template<class TT>
void _Tesselation<TT>::testAlphaShape(double alpha)
{
// 	if (not computed) compute();
	
	RTriangulation temp(*Tri);
	AlphaShape as (temp);
	if (!alpha) alpha=as.find_alpha_solid();
	as.set_alpha(alpha);	
	cerr << "Alpha shape computed. alpha_solid=" <<alpha <<endl;
	
	std::list<CellHandle> cells,cells2,cells3,cells4;
	std::list<Facet> facets,facets2,facets3;
	std::list<VertexHandle> alphaVertices;
	std::list<CVector> normals;
	std::list<CVector> normals2;
	std::list<Edge> edges0,edges1,edges2,edges3;
	as.get_alpha_shape_cells(std::back_inserter(cells),AlphaShape::REGULAR);
	as.get_alpha_shape_cells(std::back_inserter(cells2), AlphaShape::EXTERIOR);
	as.get_alpha_shape_cells(std::back_inserter(cells3),AlphaShape::INTERIOR);
	as.get_alpha_shape_cells(std::back_inserter(cells4),AlphaShape::SINGULAR);
	as.get_alpha_shape_facets(std::back_inserter(facets), AlphaShape::REGULAR);
	as.get_alpha_shape_vertices(std::back_inserter(alphaVertices), AlphaShape::REGULAR);
	as.get_alpha_shape_edges(std::back_inserter(edges0), AlphaShape::INTERIOR);
	as.get_alpha_shape_edges(std::back_inserter(edges1), AlphaShape::REGULAR);
	as.get_alpha_shape_edges(std::back_inserter(edges2), AlphaShape::SINGULAR);
	as.get_alpha_shape_edges(std::back_inserter(edges3), AlphaShape::EXTERIOR);
	
	int finitEdges=0;
	for ( FiniteEdgesIterator ed_it=Tri->finite_edges_begin(); ed_it!=Tri->finite_edges_end();ed_it++ ) ++finitEdges;
	
	std::cerr<< "num regular cells "<< cells.size() <<" vs. "<<cells2.size() <<" vs. "<<cells3.size()<<" vs. "<<cells4.size()<<std::endl;
	std::cerr<< "num regular facets "<< facets.size() << std::endl;
	std::cerr<< "num edges "<< edges0.size() <<" "<< edges1.size() <<" "<< edges2.size() <<" "<< edges3.size() <<"(finite ones:"<<finitEdges<<")" << std::endl;
// 	for (auto v=alphaVertices.begin(); v!=alphaVertices.end();v++){
// 		std::cerr<< "alpha vertex:"<<(*v)->info().id()<<std::endl;
// 	}
	for (auto e=edges1.begin(); e!=edges1.end();e++){
		std::cerr<< "alpha edge:"<<e->first->vertex(e->second)->info().id()<<" "<<e->first->vertex(e->third)->info().id() <<" "<< alphaVoronoiFaceArea(*e,as,*Tri) <<std::endl;
	}
// 	for (auto c=cells.begin(); c!=cells.end();c++){
// 		(*c)->info().setPoint(circumCenter(*c));
// 		std::cerr<< "alpha cell:"<<(Point) (*c)->info()<<std::endl;
// 	}
	
	for (auto f=facets.begin(); f!=facets.end();f++){
		const int& idx = f->second;//index of the facet within cell defined by f->first
// 		std::cerr << f->first->vertex(facetVertices[idx][0])->info().id()
// 			<<" "<< f->first->vertex(facetVertices[idx][1])->info().id()
// 			<<" "<< f->first->vertex(facetVertices[idx][2])->info().id()  << std::endl;
		CVector surface = 0.5*cross_product(f->first->vertex(facetVertices[idx][0])->point().point()-f->first->vertex(facetVertices[idx][1])->point().point(),
			f->first->vertex(facetVertices[idx][0])->point().point()-f->first->vertex(facetVertices[idx][2])->point().point());
		//largest sphere
		double maxWeight = std::max(f->first->vertex(facetVertices[idx][0])->point().weight(),max(f->first->vertex(facetVertices[idx][1])->point().weight(), f->first->vertex(facetVertices[idx][2])->point().weight()));
		Point pp;
		Point vv;
 		if (as.classify(f->first)==AlphaShape::INTERIOR) {
			pp= f->first->vertex(f->second)->point().point();
			if (not computed) f->first->info().setPoint(circumCenter(f->first));
// 			std::cerr<< "alpha cell:"<<(Point) f->first->info()<<std::endl;
			vv = f->first->info();
// 			std::cerr << "vv="<<vv<<std::endl;
// 			if (not computed) f->first->info().setPoint(Tri->dual(f->first));
// 			std::cerr << "found as.classify(f->first)==Alpha_shape_3::INTERIOR"<<std::endl;
		}
		else {
			if (as.classify(f->first->neighbor(f->second))!=AlphaShape::INTERIOR) std::cerr<<"_____________BIG PROB. HERE ___________"<<std::endl;

			pp= f->first->neighbor(f->second)->vertex(Tri->mirror_index(f->first,f->second))->point().point();
			if (not computed) f->first->neighbor(f->second)->info().setPoint(circumCenter(f->first->neighbor(f->second)));
// 			std::cerr<< "alpha cell:"<<(Point) f->first->neighbor(f->second)->info()<<std::endl;
			vv = f->first->neighbor(f->second)->info();
// 			std::cerr << "vv="<<vv<<std::endl;
// 			if (not computed) f->first->neighbor(f->second)->info().setPoint(Tri->dual(f->first->neighbor(f->second)));
// 			std::cerr << "not an Alpha_shape_3::INTERIOR"<<std::endl;
		}
		//check if the surface vector is inward or outward
		double dotP = surface*(f->first->vertex(facetVertices[f->second][0])->point().point()-pp);
		if (dotP<0) surface=-surface;
		double area = sqrt(surface.squared_length());
		CVector normal = surface/area; //unit normal
// 		std::cerr <<"dotP="<<dotP<<std::endl<<"surface: "<<surface<<std::endl;
		
		double h1 = (f->first->vertex(facetVertices[idx][0])->point().point()-vv)*surface/area; //orthogonal distance from Voronoi vertex to the plane in which the spheres lie, call the intersection V
		Point V = vv + h1*normal;
		double distLiu = sqrt((V-Point(0,0,0)).squared_length());
		double sqR = (V-f->first->vertex(facetVertices[idx][0])->point().point()).squared_length(); //squared distance between V and the center of sphere 0 
		double temp = alpha + f->first->vertex(facetVertices[idx][0])->point().weight() -sqR;
		if (temp<0) {temp=0; std::cerr<<"NEGATIVE TEMP!"<<std::endl;}
		if (temp>maxWeight) temp=maxWeight; //if alpha vertex is too far, crop
		double h2 = sqrt(temp);// this is now the distance from Voronoi vertex to "alpha" vertex (after cropping if needed)
		V = V+h2*normal;
		std::cerr <<"dist alpha center:"<<sqrt((V-Point(0,0,0)).squared_length())<<"(vs. Liu:"<< distLiu << ")"<<std::endl;
	}
}

template<class TT>
void _Tesselation<TT>::setAlphaFaces(std::vector<AlphaFace>& faces, double alpha)
{
	RTriangulation temp(*Tri);
	AlphaShape as (temp);
	if (!alpha) {
		as.set_alpha(as.find_alpha_solid());
		/*cerr << "Alpha shape computed. alpha_solid=" <<as.find_alpha_solid() <<endl;*/}
	else as.set_alpha(alpha);
	
	std::list<Facet> facets;
	std::list<CVector> normals;
	as.get_alpha_shape_facets(std::back_inserter(facets), AlphaShape::REGULAR);// get the list of "contour" facets
	faces.resize(facets.size()); int k=0;

	for (auto f=facets.begin(); f!=facets.end();f++){
		int idx = f->second;//index of the facet within cell defined by f->first
		CVector normal = 0.5*cross_product(f->first->vertex(facetVertices[idx][0])->point().point()-f->first->vertex(facetVertices[idx][1])->point().point(),
			f->first->vertex(facetVertices[idx][0])->point().point()-f->first->vertex(facetVertices[idx][2])->point().point());
		Point pp;
 		if (as.classify(f->first)==AlphaShape::INTERIOR) pp= f->first->vertex(f->second)->point().point();
		else pp= f->first->neighbor(f->second)->vertex(Tri->mirror_index(f->first,f->second))->point().point();
		//check if the normal vector is inward or outward
		double dotP = normal*(f->first->vertex(facetVertices[f->second][0])->point().point()-pp);
		if (dotP<0) normal=-normal;
		// set the face in the global list 
		for (int ii=0; ii<3;ii++) faces[k].ids[ii]= f->first->vertex(facetVertices[idx][ii])->info().id();
		faces[k++].normal = normal;
	}
}

template<class TT>
std::vector<Vector3r> _Tesselation<TT>::getExtendedAlphaGraph (double alpha, double shrinkedAlpha, bool fixedAlpha)
{
	std::vector<Vector3r> vSegments;
	RTriangulation temp ( *Tri );
	AlphaShape as ( temp );
	double minAlpha=as.find_alpha_solid();
	if ( !alpha ) as.set_alpha ( minAlpha );
	else {
		as.set_alpha ( alpha );
		if (alpha<minAlpha) cerr<<"TesselationWrapper: Using alpha<minAlpha will not work. Consider using default alpha (=0)"<<endl;
	}	
	if (fixedAlpha) {//insert one sphere per regular facet, with a fixed size shrinkedAlpha
		std::list<Facet> facets;
		as.get_alpha_shape_facets(std::back_inserter(facets), AlphaShape::REGULAR);
		for ( auto fp=facets.begin(); fp!=facets.end(); fp++ ) {
			Facet f = *fp;    
			if (as.classify(f.first)!=AlphaShape::INTERIOR) f=as.mirror_facet(f);
			Sphere sph; bool b; CVector n;
			//FIXME: suboptimal, we are calculating/returning a point for no good
			/*Point p =*/ circumCenter(f.first,f.second,alpha,b,sph,n);
			VertexHandle Vh = Tri->insert(Sphere(sph.point(), shrinkedAlpha));
			if ( Vh!=NULL ) Vh->info().isFictious = true;
			else cerr << " : __Vh==NULL__ :(" << endl;}
			

	} else {//insert one sphere per exterior/infinite cell, the radius is derived from the alpha value of the corresponding cell or 4*alpha for infinite cells  

		double alphaRad=sqrt(alpha);
		double deltaAlpha=alphaRad-sqrt(shrinkedAlpha);
		std::list<Facet> facets;// the infinite ones
		as.get_alpha_shape_facets(std::back_inserter(facets), AlphaShape::REGULAR);
		for ( auto fp=facets.begin(); fp!=facets.end(); fp++ ) {
			Facet f = *fp;    
			if (as.classify(f.first)!=AlphaShape::INTERIOR) f=as.mirror_facet(f);
			const CellHandle& outerCell = f.first->neighbor(f.second);
			if (as.is_infinite(outerCell)) {
				Sphere sph; bool b; CVector n;
				/*Point p =*/ circumCenter(f.first,f.second,alpha*4,b,sph,n);
				VertexHandle Vh = Tri->insert(Sphere(sph.point(), pow(2*alphaRad-deltaAlpha,2)));
				if ( Vh!=NULL ) Vh->info().isFictious = true;
				else cerr << " : __Vh==NULL__ :(" << endl;
			} else {
				
				if (!outerCell->info().isFictious) {
					outerCell->info().isFictious=true;
					Point p = setCircumCenter(outerCell);
			double weight = (p-outerCell->vertex(0)->point().point()).squared_length() - outerCell->vertex(0)->point().weight();
			VertexHandle Vh = Tri->insert(Sphere(p, pow(sqrt(weight)-deltaAlpha,2) ));
			if ( Vh!=NULL ) Vh->info().isFictious = true;
			else cerr << " : __Vh==NULL__ :(" << endl;
				}
			}
		}
			
			
		}

		
	
				for (auto e = Tri->finite_facets_begin(); e != Tri->finite_facets_end(); e++){
				short countFictious=e->first->vertex(facetVertices[e->second][0])->info().isFictious
				+ e->first->vertex(facetVertices[e->second][1])->info().isFictious
				+ e->first->vertex(facetVertices[e->second][2])->info().isFictious;
				if (countFictious==1){ vSegments.push_back(makeVector3r(setCircumCenter(e->first))); 
					vSegments.push_back(makeVector3r(setCircumCenter(e->first->neighbor(e->second)))); }
			}
			return vSegments;
}


template<class TT>
void _Tesselation<TT>::setExtendedAlphaCaps ( std::vector<AlphaCap>& faces, double alpha, double shrinkedAlpha, bool fixedAlpha)
{
	std::vector<CVector> areas;
	//initialize area vectors in a list accessed via ids (hence the size), later refactored into a shorter list in "faces"
	areas.resize ( maxId+1,CVector ( 0,0,0 ) );// from 0 to maxId
	
	RTriangulation temp ( *Tri );
	AlphaShape as ( temp );
	double minAlpha=as.find_alpha_solid();
	if ( !alpha ) as.set_alpha ( minAlpha );
	else {
		as.set_alpha ( alpha );
		if (alpha<minAlpha) cerr<<"TesselationWrapper: Using alpha<minAlpha will not work. Consider using default alpha (=0)"<<endl;
	}
	std::list<Facet> facets;
	as.get_alpha_shape_facets(std::back_inserter(facets), AlphaShape::REGULAR);
	if (fixedAlpha) {//insert one sphere per regular facet, with a fixed size shrinkedAlpha	
		for ( auto fp=facets.begin(); fp!=facets.end(); fp++ ) {
			Facet f = *fp;    
			if (as.classify(f.first)!=AlphaShape::INTERIOR) f=as.mirror_facet(f);
			Sphere sph; bool b; CVector n;
			circumCenter(f.first,f.second,alpha,b,sph,n);
			VertexHandle Vh = Tri->insert(Sphere(sph.point(), shrinkedAlpha));
			if ( Vh!=NULL ) Vh->info().isFictious = true;
			else cerr << " : __Vh==NULL__ :(" << endl;}
	} else {//insert one sphere per exterior/infinite cell, the radius is derived from the alpha value of the corresponding cell or 4*alpha for infinite cells		
		double alphaRad=sqrt(alpha);
		double deltaAlpha=alphaRad-sqrt(shrinkedAlpha);
		for ( auto fp=facets.begin(); fp!=facets.end(); fp++ ) {
			Facet f = *fp;    
			if (as.classify(f.first)!=AlphaShape::INTERIOR) f=as.mirror_facet(f);
			const CellHandle& outerCell = f.first->neighbor(f.second);
			if (as.is_infinite(outerCell)) {
				Sphere sph; bool b; CVector n;
				/*Point p =*/ circumCenter(f.first,f.second,alpha*4,b,sph,n);
				VertexHandle Vh = Tri->insert(Sphere(sph.point(), pow(2*alphaRad-deltaAlpha,2)));
				if ( Vh!=NULL ) Vh->info().isFictious = true;
				else cerr << " : __Vh==NULL__ :(" << endl;
			} else if (!outerCell->info().isFictious) {
					outerCell->info().isFictious=true;
					Point p = setCircumCenter(outerCell);
					double weight = (p-outerCell->vertex(0)->point().point()).squared_length() - outerCell->vertex(0)->point().weight();
					VertexHandle Vh = Tri->insert(Sphere(p, pow(sqrt(weight)-deltaAlpha,2) ));
					if ( Vh!=NULL ) Vh->info().isFictious = true;
					else cerr << " : __Vh==NULL__ :(" << endl;
			}
		}
	}
	for (auto e = Tri->finite_facets_begin(); e != Tri->finite_facets_end(); e++){
		short countFictious=e->first->vertex(facetVertices[e->second][0])->info().isFictious
		+ e->first->vertex(facetVertices[e->second][1])->info().isFictious
		+ e->first->vertex(facetVertices[e->second][2])->info().isFictious;
		if (countFictious==1){
			short ftx=0; while (!e->first->vertex(facetVertices[e->second][ftx])->info().isFictious) ftx++;
			const Point& fictV = e->first->vertex(facetVertices[e->second][ftx])->point().point();
			const int& id1 = e->first->vertex(facetVertices[e->second][ftx>0? ftx-1:2])->info().id();
			const int& id2 = e->first->vertex(facetVertices[e->second][ftx<2? ftx+1:0])->info().id();
			const Point& p1 = e->first->vertex(facetVertices[e->second][ftx>0? ftx-1:2])->point().point();
			const Point& p2 = e->first->vertex(facetVertices[e->second][ftx<2? ftx+1:0])->point().point();
			CVector u = setCircumCenter(e->first)-setCircumCenter(e->first->neighbor(e->second));
			int makeClockWise = u*cross_product(p2-p1,fictV-p1)>0? 1:-1;
			areas[id1]=areas[id1]+(makeClockWise*0.5)*cross_product(u,setCircumCenter(e->first)-p1);
			areas[id2]=areas[id2]-(makeClockWise*0.5)*cross_product(u,setCircumCenter(e->first)-p2);
		}
	}
	
	faces.clear(); faces.reserve(1.5*6*pow(as.number_of_vertices(),0.6666));
	for (short id=0; id<=maxId; id++) {
		if (areas[id]!=CVector(0,0,0)) {
			AlphaCap cap; cap.id=id; cap.normal=areas[id];
// 			cerr << "cap: "<<cap.id<<" "<<cap.normal<<endl;
			faces.push_back(cap);
		}
	}

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
CVector _Tesselation<TT>::alphaVoronoiFaceArea (const Edge& ed_it, const AlphaShape& as, const RTriangulation& Tro)
{
	//Overall, we calculate the area vector of the polygonal Voronoi face between two spheres, this is done by integrating x×dx
 
        double alpha = as.get_alpha();
	CellCirculator cell0,cell1,cell2; 
	cell0 = as.incident_cells ( ed_it );
	cell2 = cell0;
	while ( as.classify(cell2)!=AlphaShape::INTERIOR ) {++cell2; if (cell2==cell0) cerr<<"infinite loop on an edge, probably singular"<<endl;}
	cell1=cell2;

	//pA,pB are the spheres of the edge, (p1,p2) are iterating over the vertices of the vornonoi face, p12 can be an intermediate point for EXTERIOR-EXTERIOR parts of the contour 
	Point pA,pB,p1,p2,p12,vv0,vv;
        p1 = setCircumCenter(cell1);//starting point of the polygon
	CVector branch, normal;
	CVector branchArea(0,0,0);
	pA = ( ed_it.first )->vertex ( ed_it.second )->point().point();//one sphere
	pB = ( ed_it.first )->vertex ( ed_it.third )->point().point();//another sphere
	CVector AB = pB-pA;
	bool interior1 = true;//keep track of last cell's status
	bool interior2; 
	do {
		++cell2;
		interior2 = (as.classify(cell2)==AlphaShape::INTERIOR);
		if (interior2) {//easy
			setCircumCenter(cell2);
			p2 = cell2->info();
			branch=p2-p1;
			branchArea = branchArea + cross_product(branch, p1-CGAL::ORIGIN);
			cerr<<"branchArea(1) "<<branch<<" cross "<<p1<<endl;
		} else {//tricky, we have to construct the face between INTERIOR-EXTERIOR, or even EXTERIOR-EXTERIOR
			CellCirculator baseCell=cell1;
			//handle EXTERIOR-EXTERIOR by checking the n+1 cell
			if (!interior1) {baseCell=cell2; baseCell++; vv0=vv;if(as.classify(baseCell)!=AlphaShape::INTERIOR) cerr<<"3 consecutive EXTERIOR cells in a loop";}
			vv = setCircumCenter(baseCell);
			// finding the facet from baseCell to cell2 ...
			int idx=0; while (baseCell->neighbor(idx)!=cell2) {idx++; if(idx>3) cerr<<"HUUUUUUUH";}
			// ... then its surface vector
			CVector surface = 0.5*cross_product(baseCell->vertex(facetVertices[idx][0])->point().point()-baseCell->vertex(facetVertices[idx][1])->point().point(),
			baseCell->vertex(facetVertices[idx][0])->point().point()-baseCell->vertex(facetVertices[idx][2])->point().point());
			//largest sphere
			double maxWeight = std::max(baseCell->vertex(facetVertices[idx][0])->point().weight(),max(baseCell->vertex(facetVertices[idx][1])->point().weight(), baseCell->vertex(facetVertices[idx][2])->point().weight()));
			//check if the surface vector is inward or outward
			double dotP = surface*(baseCell->vertex(facetVertices[idx][0])->point().point()-baseCell->vertex(idx)->point().point());
			if (dotP<0) surface=-surface;
			double area = sqrt(surface.squared_length());
			normal = surface/area; //unit normal
			double h1 = (baseCell->vertex(facetVertices[idx][0])->point().point()-vv)*normal; //orthogonal distance from Voronoi vertex to the plane in which the spheres lie, call the intersection V
			p2 = vv + h1*normal;
			double sqR = (p2-baseCell->vertex(facetVertices[idx][0])->point().point()).squared_length(); //squared distance between V and the center of sphere 0
                        double temp = alpha + baseCell->vertex(facetVertices[idx][0])->point().weight() -sqR;
                        if (temp<0) {temp=0; std::cerr<<"NEGATIVE TEMP!"<<std::endl;}
                        if (temp>maxWeight) temp=maxWeight; //if alpha vertex is too far, crop
                        double h2 = sqrt(temp);// this is now the distance from Voronoi vertex to "alpha" vertex (after cropping if needed)
		        p2 = p2+h2*normal;
		
		        bool coplanar=false;
		
		        if (!(interior1 or interior2))  {
			        //VERSION 1,intersection of orthogonal planes from two branches
			        CVector tangent = cross_product(AB,p1-vv0);
			        tangent = tangent/sqrt(tangent.squared_length());//this is orthogonal to the _previous_ branch segment of the polygonal contour
			        double dotP = tangent*normal;
			        coplanar=(abs(dotP)<1e-2);
				CVector p1mp2=p1-p2;
			        if (!coplanar) {
					//make sure the construction is not singular (no intermediate vertex)
					if ((p1mp2*(p1-vv0)>0) and (p1mp2*normal<0)) {
					p12=p1-(p1mp2)*normal/dotP*tangent;
					
// 					if (((p12-p1)*p1mp2)*(p12-p2)*p1mp2)<0) {//make sure the construction is not singular (no intermediate vertex)
			        cerr<<"p12="<<p12<<" with p1="<< p1<<", p2="<<p2 <<" tangent="<<tangent<<" "<<AB <<" "<<p1-pA <<endl;
			        //VERSION 2... a different p12 (possibly parallelogram?)
			
			        //Whatever the method:
			        branchArea = branchArea+cross_product(p12-p1, p1-CGAL::ORIGIN);
				cerr<<"branchArea(2) "<<p12-p1<<" cross "<<p1<<endl;
			        p1 = p12;}
				}
                        }
                        cerr<<"branchArea(3) "<<p2-p1<<" cross "<<p1<<endl;
		        branchArea = branchArea+cross_product(p2-p1, p1-CGAL::ORIGIN);
			
		}
		cell1=cell2; p1=p2; interior1=interior2;
		
	} while (cell2!=cell0);
	return 0.5*branchArea;//0.5 because the above integral is twice the area
}

template<class TT>
CVector _Tesselation<TT>::alphaVoronoiPartialCapArea ( const Edge& ed_it, const AlphaShape& as, std::vector<Vector3r>& vSegments )
{
	//Overall, a partial area vector based on only the outer part of a polygonal Voronoi face (see alphaVoronoiFaceArea), looping on REGULAR edges incident to a boundary sphere and using this function for each of them should give the contour integral of x×dx for the polygonal cap of the sphere.
	double alpha = as.get_alpha();
	CellCirculator cell0,cell1,cell2,cell3;
	cell0 = as.incident_cells ( ed_it );
	cell1 = cell2 = cell0;
        bool violate1, violate2;
	++cell2;

	while ( as.classify(cell1) !=AlphaShape::INTERIOR or as.classify(cell2) ==AlphaShape::INTERIOR){//we want interior->exterior starting sequence
		++cell1;
		if ( cell0==cell2++ ) cerr<<"infinite loop on an edge, probably singular"<<endl;
	}
	cell0=cell1++;//keep the starting sequence as 0->1, cell1 is now equal to cell2
	cell3=cell2; cell3++;//we have now cell0 < cell1=cell2 < cell3
	
	while ( as.classify ( cell3 ) !=AlphaShape::INTERIOR){//we want an exterior->interior end-point, could be that cell3=cell0 and that's ok
		++cell2; ++cell3;
		if ( cell2==cell0 ) cerr<<"infinite loop on an edge, probably singular(2)"<<endl;
	}
	//now cell0 < cell1 < ... < cell2 < cell3
	
	const Sphere& sA = ( ed_it.first )->vertex ( ed_it.second )->point();//one sphere
        const Sphere& sB = ( ed_it.first )->vertex ( ed_it.third )->point();//another sphere
        const Point& pA = sA.point();//one sphere
	const Point& pB = sB.point();//another sphere

	//pA,pB are the spheres of the edge, (p1,p2) are iterating over the vertices of the vornonoi face, p12 is an intermediate point for convex parts
	Point p1,p2,p12,vv0,vv3;
	vv0=setCircumCenter(cell0);//start point
	vv3=setCircumCenter(cell3);//end point
	
	
	//   p1 = setCircumCenter ( cell1 ); //starting point of the polygon
	CVector branch, normal1, normal2;
	CVector branchArea(0,0,0);
	
	CVector AB = pB-pA;
	Sphere SAlpha1, SAlpha2;

        Sphere SAlphaSmall1, SAlphaSmall2;
        Sphere sC1,sC2;//the 3rd spheres of each facet, not part of the edge
	bool first=true;
	int idx1,idx2;
        
	do { //this do-while will run twice and it should return the second time, we work with the "exit" and "enter" regions of a polyline wrt. the alpha contour
			CellCirculator baseCell= first ? cell0 : cell3;//it plays the role of the internal cell
			CellCirculator outerCell= first ? cell1 : cell2;//it plays the role of the external cell

			// finding the facet from baseCell to outerCell ...
			int& idx=first?idx1:idx2;
                        idx=0;
			while ( baseCell->neighbor ( idx ) !=outerCell ) {
				idx++;
				if ( idx>3 ) cerr<<"HUUUUUUUH";}
			short thirdSphere=0; 
			while	( baseCell->vertex(facetVertices[idx][thirdSphere]) == (ed_it.first)->vertex(ed_it.second) or baseCell->vertex(facetVertices[idx][thirdSphere]) == (ed_it.first)->vertex(ed_it.third)) {thirdSphere++; if ( thirdSphere>3 ) cerr<<"HUAAAUUH";}
			Sphere& sC=first ? sC1:sC2;
			sC = baseCell->vertex(facetVertices[idx][thirdSphere])->point();//forming the regular facet with sA and sB

			// ... then its surface vector
			//FIXME: for many cases this cross product is not needed if we set/use voronoi centers of alpha cells
			

                        Sphere& SAlpha = first ? SAlpha1:SAlpha2;
                        Point& p = first ? p1 : p2;
                        bool& violate= first ? violate1 : violate2;
                        Sphere& SAlphaSmall = first? SAlphaSmall1:SAlphaSmall2;
                        CVector& normal = first? normal1:normal2;
                        
                        p = circumCenter(baseCell,idx,alpha,violate,SAlpha,normal);
                        
                        SAlphaSmall = Sphere(SAlpha.point(),pow(sqrt(SAlpha.weight())-0.5,2));
			p = circumCenter(baseCell,idx,SAlphaSmall,violate);
			first=!first;
	} while ( !first );
	first=true;
                        
	do {
	  CellCirculator baseCell= first ? cell0 : cell3;//it plays the role of the internal cell
			CellCirculator outerCell= first ? cell1 : cell2;//it plays the role of the external cell

			// finding the facet from baseCell to outerCell ...
			int& idx=first?idx1:idx2;
			Sphere& sC=first ? sC1:sC2;
			Sphere& SAlpha = first ? SAlpha1:SAlpha2;
                        Point& p = first ? p1 : p2;
                        bool& violate= first ? violate1 : violate2;
                        Sphere& SAlphaSmall = first? SAlphaSmall1:SAlphaSmall2;
			Sphere& SAlphaSmallInv = first? SAlphaSmall2:SAlphaSmall1;

                        std::vector<Point> pi; /*pi.reserve(6);*///"interior" points
//                         cerr<<"violate "<<violate<<endl;
			 bool infCenter=false;
			 Point pppInv = circumCenter(SAlphaSmall,sA,sB,SAlphaSmallInv);
// 			if (!violate) {
					 
					  if ((p-pppInv)*(sA.point()-sC.point())<0) {
					    infCenter=true;
                                            /////////////////
 
					    p=pppInv;  
					  }
// 			}
                        if (violate)
                        {
                         	Point ppp = circumCenter(baseCell->vertex(idx)->point(),sA,sB,SAlphaSmall);
                                if ((ppp-pppInv)*(sA.point()-sC.point())<0) infCenter=true;
                                            
                                
                                p = ppp;
				if ((p-ppp)*(sA.point()-sC.point())<0) {
					
                                        Point ppp2 = circumCenter(baseCell->vertex(idx)->point(),sB,sC,SAlphaSmall);
                                        Point ppp3 = circumCenter(baseCell->vertex(idx)->point(),sA,sC,SAlphaSmall);
					if (infCenter) {
					  Point ppp2B = circumCenter(baseCell->vertex(idx)->point(),sB,SAlphaSmallInv,SAlphaSmall);
					  Point ppp3B = circumCenter(baseCell->vertex(idx)->point(),sA,SAlphaSmallInv,SAlphaSmall);
					  Vector3r u = makeVector3r(ppp2)-makeVector3r(ppp2B); Vector3r v = makeVector3r(ppp3)-makeVector3r(ppp3B); 
					  Vector3r w = makeVector3r(ppp2)-makeVector3r(ppp3);
					  for (int j=0;j<8;j++) {
						  vSegments.push_back(makeVector3r(ppp2B)+j*(2/23.)*u); vSegments.push_back(makeVector3r(ppp2B)+(2.*j+1)*(1/23.)*u);
						  vSegments.push_back(makeVector3r(ppp3B)+j*(2/23.)*v); vSegments.push_back(makeVector3r(ppp3B)+(2.*j+1)*(1/23.)*v);
						  vSegments.push_back(makeVector3r(ppp3)+j*(2/23.)*w); vSegments.push_back(makeVector3r(ppp3)+(2.*j+1)*(1/23.)*w);
					  }
					  
					} else {
                                         
                                        Vector3r u = makeVector3r(ppp2)-makeVector3r(ppp); Vector3r v = makeVector3r(ppp3)-makeVector3r(ppp); 
					  for (int j=0;j<3;j++) {
						  vSegments.push_back(makeVector3r(ppp)+j*(2/5.)*u); vSegments.push_back(makeVector3r(ppp)+(2.*j+1)*(1/5.)*u);
						  vSegments.push_back(makeVector3r(ppp)+j*(2/5.)*v); vSegments.push_back(makeVector3r(ppp)+(2.*j+1)*(1/5.)*v);
					  }
					}
                                         
//                                         vSegments.push_back(makeVector3r(ppp)); vSegments.push_back(makeVector3r(ppp2)); 
                                        
//                                         vSegments.push_back(makeVector3r(ppp)); vSegments.push_back(makeVector3r(ppp3)); 
                                }
                                else {}
//                                 p = circumCenter(sC,sA,sB,SAlpha);
                         	for (short k=0;k<3;k++) {                                 	
					const int fct = facetVertices[idx][k];
					const int* f = facetVertices[fct];
                                        const CellHandle& ncell =  baseCell->neighbor(fct);
					short nfacet = as.mirror_index(baseCell,fct);
					Point pp;
                                        if (0 /*as.is_infinite(ncell)*/) {
					   pp = circumCenter(baseCell->vertex(f[0])->point(),baseCell->vertex(f[1])->point(),baseCell->vertex(f[2])->point(),SAlpha);
                                        cerr <<"INFINITE CELL"<<endl;
					  
					} else {
                                       
                                        bool violate2;
                                        pp = circumCenter(ncell,nfacet,SAlpha,violate);
					cerr<<"violate2 "<<violate<<endl;
					}
					
					
                                        /*if (!violate2)*/ pi.push_back(pp);
//                                         else {

                         	}
                         	
                        } else {
//                          	pi.push_back(p2);
                        }
                        

		first=!first;
// 		interior1=interior2;
	}
// 	while ( as.classify ( cell3 ) !=AlphaShape::INTERIOR)
	while ( !first );
 
         
        vSegments.push_back(makeVector3r(p1)); vSegments.push_back(makeVector3r(p2)); 
         
                                CVector tangent = cross_product ( AB,p1-vv0 );
                                CVector p1mp2=p1-p2;
//                              bool clockWise = ( tangent*p1mp2>0 );//not sure it works
                                bool clockWise = ( cross_product(p1-vv0,p2-p1)*AB>0 );
                                tangent = tangent/sqrt ( tangent.squared_length() ); //this is orthogonal to the _previous_ branch segment of the polygonal contour
                                double dotP = tangent*normal1;
 
                                branchArea = branchArea+cross_product ( p2-p1, p1-CGAL::ORIGIN );

                                //check the orientation (we need to accumulate along the polyline with a given direction)
                                if ( clockWise ) return 0.5*branchArea;
                                else return -0.5*branchArea;
//                         }
        
     
	cerr << "WE SHOULD NEVER REACH HERE" <<endl;
	return CVector(0,0,0);
}

template<class TT>
CVector _Tesselation<TT>::alphaVoronoiPartialCapArea ( Facet facet, const AlphaShape& as, double shrinkedAlpha, std::vector<Vector3r>& vSegments )
{
	//Overall, a partial area vector based on only the outer part of a polygonal Voronoi face (see alphaVoronoiFaceArea), looping on REGULAR edges incident to a boundary sphere and using this function for each of them should give the contour integral of x×dx for the polygonal cap of the sphere.
	double alpha = as.get_alpha();
	Facet adjactF [4]; 
	bool violate [4];
	Point circumC [4];
	Point edgeC [3];
	Point mirrorC [3];
	Sphere alphaSph [4];
	Sphere thirdSph [3];
	Sphere mirrorSph [3];
	CVector norml [4];
	CVector dir [3];
	bool internal=true; short externalEdge;
	
	if (as.classify(facet.first)!=AlphaShape::INTERIOR) facet=as.mirror_facet(facet);
	adjactF[0]=facet;
	
	Point p = circumCenter(facet.first,facet.second,alpha,violate[0],alphaSph[0],norml[0]);
	alphaSph[0] = Sphere(alphaSph[0].point(), shrinkedAlpha);
	circumC[0] = circumCenter(facet.first,facet.second,alphaSph[0],violate[0]);
	
	for (int k=0;k<3;k++) {
		FacetCirculator f; 
		const short& ii= facetVertices[facet.second][k];		const Sphere& sii = facet.first->vertex(ii)->point();
		const short& jj= facetVertices[facet.second][k>1?0:(k+1)]; const Sphere& sjj = facet.first->vertex(jj)->point();
		const short& kk= facetVertices[facet.second][k>0?(k-1):2]; thirdSph[k]=facet.first->vertex(kk)->point();
// 		const Sphere& skk = facet.first->vertex(kk)->point();
		f = as.incident_facets(facet.first,//cell
			ii,//i
			jj,//j
			facet);//start
		FacetCirculator f0 = f;
		
		do {f++; if (f==f0) cerr <<"PROB PROB1"<<endl; } while (as.classify(*f)!=AlphaShape::REGULAR); 
		if (as.classify(f->first)==AlphaShape::INTERIOR) adjactF[k]=(*f);
		else adjactF[k]=as.mirror_facet(*f);
		const Facet& af = adjactF[k];
		
		Point p = circumCenter(af.first,af.second,alpha,violate[k+1],alphaSph[k+1],norml[k+1]);
		short mirrorVtx = 0; while (af.first->vertex(facetVertices[af.second][mirrorVtx])== facet.first->vertex(jj) or af.first->vertex(facetVertices[af.second][mirrorVtx])== facet.first->vertex(jj)) mirrorVtx++;
		
		alphaSph[k+1] = Sphere(alphaSph[k+1].point(),shrinkedAlpha);
		circumC[k+1] = circumCenter(af.first,af.second,alphaSph[k+1],violate[k+1]);
		edgeC[k] = circumCenter(alphaSph[0],sii,sjj,alphaSph[k+1]);
		mirrorSph[k] = af.first->vertex(facetVertices[af.second][mirrorVtx])->point();
		mirrorC[k] = circumCenter(alphaSph[0],sii,sjj,mirrorSph[k]);
		
		dir[k]=cross_product(sii.point()-sjj.point(),sii.point()-alphaSph[0].point());
		dir[k]=((sii.point()-thirdSph[k].point())*dir[k])*dir[k];
		if (((edgeC[k]-circumC[0])*dir[k])<0) {internal=false; externalEdge=k;}
// 		cerr<<"edge: "<<alphaSph[0]<<" "<< sii<<" "<<sjj <<" "<<alphaSph[k+1] <<endl;
	}
	if (internal) {
	for (int k=0;k<3;k++) {
		const short& ii= facetVertices[facet.second][k]; const Sphere& sii = facet.first->vertex(ii)->point();
		const short& jj= facetVertices[facet.second][k>1?0:(k+1)]; const Sphere& sjj = facet.first->vertex(jj)->point();
		const short& kk= facetVertices[facet.second][k>0?(k-1):2]; const Sphere& skk = facet.first->vertex(kk)->point();
		if (((mirrorSph[k].point()-skk.point())*dir[k])>0 and ((mirrorC[k]-edgeC[k])*dir[k])<0) {
			Point p1 = circumCenter(alphaSph[0],sjj,mirrorSph[k],alphaSph[k+1]);
			Point p2 = circumCenter(alphaSph[0],mirrorSph[k],sii,alphaSph[k+1]);
			if (((mirrorC[k]-circumC[0])*dir[k])>0) {
				int NN=3; CVector u=mirrorC[k]-circumC[0]; CVector v=mirrorC[k]-p1;
				CVector w=mirrorC[k]-p2;
				for (int j=0;j<NN;j++) {
					vSegments.push_back(makeVector3r(circumC[0]+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(circumC[0]+(2.*j+1)*(1/(2.*NN-1.))*u));
					vSegments.push_back(makeVector3r(p1+j*(2/(2.*NN-1.))*v)); vSegments.push_back(makeVector3r(p1+(2.*j+1)*(1/(2.*NN-1.))*v));
					vSegments.push_back(makeVector3r(p2+j*(2/(2.*NN-1.))*w)); vSegments.push_back(makeVector3r(p2+(2.*j+1)*(1/(2.*NN-1.))*w));
				}
			} else {
				Point p1b = circumCenter(alphaSph[0],sjj,mirrorSph[k],skk);
				Point p2b = circumCenter(alphaSph[0],mirrorSph[k],sii,skk);
				int NN=5; CVector u=p1b-p2b; CVector v=p1b-p1;
				CVector w=p2b-p2;
				for (int j=0;j<NN;j++) {
					vSegments.push_back(makeVector3r(p2b+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(p2b+(2.*j+1)*(1/(2.*NN-1.))*u));
					vSegments.push_back(makeVector3r(p1+j*(2/(2.*NN-1.))*v)); vSegments.push_back(makeVector3r(p1+(2.*j+1)*(1/(2.*NN-1.))*v));
					vSegments.push_back(makeVector3r(p2+j*(2/(2.*NN-1.))*w)); vSegments.push_back(makeVector3r(p2+(2.*j+1)*(1/(2.*NN-1.))*w));
				}
			}
		} else {
			if (((edgeC[k]-circumC[0])*dir[k])>0) {
// 				cerr<<"TEST1: "<<edgeC[k]<<" "<<circumC[0]<<" "<<dir[k]<<endl;
				
				int NN=11; CVector u=edgeC[k]-circumC[0];
				for (int j=0;j<NN;j++) {
				vSegments.push_back(makeVector3r(circumC[0]+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(circumC[0]+(2.*j+1)*(1/(2.*NN-1.))*u));}
				
			} else {
				Point p1 = circumCenter(alphaSph[0],sjj,skk,alphaSph[k+1]);
				Point p2 = circumCenter(alphaSph[0],skk,sii,alphaSph[k+1]);
	// 			vSegments.push_back(makeVector3r(edgeC[k])); vSegments.push_back(makeVector3r(p1));
	// 			vSegments.push_back(makeVector3r(edgeC[k])); vSegments.push_back(makeVector3r(p2));
				int NN=7; CVector u=edgeC[k>1?0:(k+1)]-p1; CVector v=edgeC[k>0?(k-1):2]-p2;
				for (int j=0;j<NN;j++) {
					vSegments.push_back(makeVector3r(p1+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(p1+(2.*j+1)*(1/(2.*NN-1.))*u));
					vSegments.push_back(makeVector3r(p2+j*(2/(2.*NN-1.))*v)); vSegments.push_back(makeVector3r(p2+(2.*j+1)*(1/(2.*NN-1.))*v));
				}
			}
		}
	} 
	}
	else {
		const short& k = externalEdge;
		const short& ii= facetVertices[facet.second][k]; const Sphere& sii = facet.first->vertex(ii)->point();
		const short& jj= facetVertices[facet.second][k>1?0:(k+1)]; const Sphere& sjj = facet.first->vertex(jj)->point();
		const short& kk= facetVertices[facet.second][k>0?(k-1):2]; const Sphere& skk = facet.first->vertex(kk)->point();
		Point p1 = circumCenter(alphaSph[0],sjj,skk,alphaSph[k+1]);
		Point p2 = circumCenter(alphaSph[0],skk,sii,alphaSph[k+1]);
		Point& p1b = edgeC[k>1?0:(k+1)];
		Point& p2b = edgeC[k>0?(k-1):2];
		
		Point mirror1 = circumCenter(alphaSph[0],sjj,skk,mirrorSph[k]);
		Point mirror2 = circumCenter(alphaSph[0],sii,skk,mirrorSph[k]);
		
		bool ext1 = ((mirror1-p1)*dir[k]<0);
		bool ext2 = ((mirror2-p2)*dir[k]<0);
		
		if (ext1 && ext2) {
			Point p1c = circumCenter(alphaSph[0],sjj,alphaSph[k+1],mirrorSph[k]);
			vSegments.push_back(makeVector3r(p1c)); vSegments.push_back(makeVector3r(mirror1));
			p1 = mirror1;
			Point p2c = circumCenter(alphaSph[0],sii,alphaSph[k+1],mirrorSph[k]);
			vSegments.push_back(makeVector3r(p2c)); vSegments.push_back(makeVector3r(mirror2));
			p2 = mirror2;
			vSegments.push_back(makeVector3r(p2)); vSegments.push_back(makeVector3r(p1));
		}
		else if (ext2 && !ext1) {
			Point p2c = circumCenter(alphaSph[0],sii,alphaSph[k+1],mirrorSph[k]);
			Point p2d = circumCenter(alphaSph[0],skk,sii,mirrorSph[k]);
			vSegments.push_back(makeVector3r(p2c)); vSegments.push_back(makeVector3r(mirror2));
			p2 = mirror2;
			vSegments.push_back(makeVector3r(p2)); vSegments.push_back(makeVector3r(p2d));
		} else if (ext1 && !ext2) {
			Point p1c = circumCenter(alphaSph[0],sjj,alphaSph[k+1],mirrorSph[k]);
			Point p1d = circumCenter(alphaSph[0],skk,sjj,mirrorSph[k]);
			vSegments.push_back(makeVector3r(p1c)); vSegments.push_back(makeVector3r(mirror1));
			p1 = mirror1;
			vSegments.push_back(makeVector3r(p1)); vSegments.push_back(makeVector3r(p1d));
		}
		
		
		
		int NN=4; CVector u=p1b-p1; CVector v=p2b-p2;
		for (int j=0;j<NN;j++) {
			vSegments.push_back(makeVector3r(p1+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(p1+(2.*j+1)*(1/(2.*NN-1.))*u));
			vSegments.push_back(makeVector3r(p2+j*(2/(2.*NN-1.))*v)); vSegments.push_back(makeVector3r(p2+(2.*j+1)*(1/(2.*NN-1.))*v));
		}
		
		
	}
	if (0 /*internal*/) {
		CVector u=edgeC[0]-circumC[0]; CVector v=edgeC[1]-circumC[0]; CVector w=edgeC[2]-circumC[0]; 
		int NN=8;
		for (int j=0;j<NN;j++) {
			if (internal){
			vSegments.push_back(makeVector3r(circumC[0]+j*(2/(2.*NN-1.))*u)); vSegments.push_back(makeVector3r(circumC[0]+(2.*j+1)*(1/(2.*NN-1.))*u));}
			vSegments.push_back(makeVector3r(circumC[0]+j*(2/(2.*NN-1.))*v)); vSegments.push_back(makeVector3r(circumC[0]+(2.*j+1)*(1/(2.*NN-1.))*v));
			vSegments.push_back(makeVector3r(circumC[0]+j*(2/(2.*NN-1.))*w)); vSegments.push_back(makeVector3r(circumC[0]+(2.*j+1)*(1/(2.*NN-1.))*w));
		}
		cerr<<"Segments(2):"<< circumC[0]<<" "<<edgeC[0]<<" "<<edgeC[1]<<" "<<edgeC[2]<<endl;
	}
	if (0 /*violate[0]*/){
		Point iCircumC [3];
		Point iEdgeC [3];
		std::vector<Point> ll;
// 		const Sphere& facet.first->vertex(facetVertices[facet.second][k])->point();
		for (short k=0; k<3; k++) {
			iCircumC[k]= circumCenter(	facet.first->vertex(facet.second)->point(),
							facet.first->vertex(facetVertices[facet.second][k])->point(),
							facet.first->vertex(facetVertices[facet.second][k>1?0:(k+1)])->point(),
							alphaSph[0]);
			iEdgeC[k]= circumCenter(	alphaSph[0],
							facet.first->vertex(facetVertices[facet.second][k])->point(),
							facet.first->vertex(facetVertices[facet.second][k>1?0:(k+1)])->point(),
							alphaSph[k+1]);
		}
		
		for (short k=0; k<3; k++) {
			if ((iCircumC[k]-iEdgeC[k])*(iEdgeC[k]-thirdSph[k].point())>0) {
				ll.push_back(circumCenter(	facet.first->vertex(facet.second)->point(),
							facet.first->vertex(facetVertices[facet.second][k])->point(),
							alphaSph[0],
							alphaSph[k+1]));
				ll.push_back(circumCenter(	facet.first->vertex(facet.second)->point(),
							alphaSph[0],
							facet.first->vertex(facetVertices[facet.second][k>1?0:(k+1)])->point(),
							alphaSph[k+1]));
			} else {
				ll.push_back(iCircumC[k]);
			}
		}
		if (ll.size()>0) vSegments.push_back(makeVector3r(ll[0]));
		for (short n =0;n<(ll.size()-1);n++) {vSegments.push_back(makeVector3r(ll[n+1])); vSegments.push_back(makeVector3r(ll[n+1]));}
		if (ll.size()>0) vSegments.push_back(makeVector3r(ll[0]));
	}
	return CVector(0,0,0);
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
				r = std::abs ( ( Tetrahedron ( ed_it->first->vertex ( ed_it->second )->point().point(), cell0->info(), cell1->info(), cell2->info() ) ).volume() );
				( ed_it->first )->vertex ( ed_it->second )->info().v() += r;
				TotalFiniteVoronoiVolume+=r;
			}
			if ( !isFictious2 )
			{
				r = std::abs ( ( Tetrahedron ( ed_it->first->vertex ( ed_it->third )->point().point(), cell0->info(),  cell1->info(), cell2->info() ) ).volume() );
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
	else cerr << " : Vh==NULL!!" << " id=" << id << " Point=" << Point ( x,y,z ) << " rad=" << rad<<" fictious="<<isFictious<<", isGhost="<< bool(duplicateOfId>=0)<<endl;
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
