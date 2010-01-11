/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include"TesselationWrapper.hpp"
//#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
//#include<yade/lib-triangulation/KinematicLocalisationAnalyser.hpp>

//using namespace std;
YADE_PLUGIN((TesselationWrapper));
YADE_REQUIRE_FEATURE(CGAL)

//spatial sort traits to use with a pair of CGAL::sphere pointers and integer.
//template<class _Triangulation>
struct RTraits_for_spatial_sort : public CGT::RTriangulation::Geom_traits {
	//typedef typename _Triangulation::Geom_traits Gt;
	typedef CGT::RTriangulation::Geom_traits Gt;
	typedef std::pair<const CGT::Sphere*,body_id_t> Point_3;

	struct Less_x_3 {
		bool operator()(const Point_3& p,const Point_3& q) const {
			return Gt::Less_x_3()(* (p.first),* (q.first));
		}
	};
	struct Less_y_3 {
		bool operator()(const Point_3& p,const Point_3& q) const {
			return Gt::Less_y_3()(* (p.first),* (q.first));
		}
	};
	struct Less_z_3 {
		bool operator()(const Point_3& p,const Point_3& q) const {
			return Gt::Less_z_3()(* (p.first),* (q.first));
		}
	};
	Less_x_3  less_x_3_object() const {return Less_x_3();}
	Less_y_3  less_y_3_object() const {return Less_y_3();}
	Less_z_3  less_z_3_object() const {return Less_z_3();}
};


//function inserting points into a triangulation (where YADE::Sphere is converted to CGT::Sphere)
//and setting the info field to the bodies id.
//Possible improvements : use bodies pointers to avoid one copy, use aabb's lists to replace the shuffle/sort part
// template <class Triangulation>
void build_triangulation_with_ids(const shared_ptr<BodyContainer>& bodies, TesselationWrapper &TW)
{
	CGT::Tesselation& Tes = *(TW.Tes);
	CGT::RTriangulation& T = Tes.Triangulation();
	std::vector<CGT::Sphere> spheres;
	std::vector<std::pair<const CGT::Sphere*,body_id_t> > pointsPtrs;
	spheres.reserve(bodies->size());
	pointsPtrs.reserve(bodies->size());

	BodyContainer::iterator biBegin    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	BodyContainer::iterator bi = biBegin;

	body_id_t Ng = 0;
	TW.mean_radius = 0;
	for (; bi!=biEnd ; ++bi) {
		if ((*bi)->isDynamic) { //then it is a sphere (not a wall) FIXME : need test if isSphere
			const Sphere* s = YADE_CAST<Sphere*> ((*bi)->shape.get());
			const Vector3r& pos = (*bi)->state->pos;
			const Real rad = s->radius;
			CGT::Sphere sp(CGT::Point(pos[0],pos[1],pos[2]),rad*rad);
			spheres.push_back(sp);
			pointsPtrs.push_back(std::make_pair(&(spheres[Ng]/*.point()*/),(*bi)->getId()));
			Ng++;
			TW.mean_radius += rad;
		}
	}
	TW.mean_radius /= Ng;
	spheres.resize(Ng);
	pointsPtrs.resize(Ng);
	std::random_shuffle(pointsPtrs.begin(), pointsPtrs.end());
	spatial_sort(pointsPtrs.begin(),pointsPtrs.end(), RTraits_for_spatial_sort()/*, CGT::RTriangulation::Weighted_point*/);

	CGT::RTriangulation::Cell_handle hint;

	long Nt = 0;
	for (std::vector<std::pair<const CGT::Sphere*,body_id_t> >::const_iterator
			p = pointsPtrs.begin();p != pointsPtrs.end(); ++p) {
		CGT::RTriangulation::Locate_type lt;
		CGT::RTriangulation::Cell_handle c;
		int li, lj;
		c = T.locate(* (p->first), lt, li, lj, hint);
		CGT::RTriangulation::Vertex_handle v = T.insert(*(p->first),lt,c,li,lj);
		if (v==CGT::RTriangulation::Vertex_handle())
			hint=c;
		else {
			v->info() = (const unsigned int) p->second;
			//Vh->info().isFictious = false;//false is the default
			Tes.max_id = std::max(Tes.max_id,(const unsigned int) p->second);
			hint=v->cell();
			++Nt;
		}
	}
	cerr << " loaded : " << Ng<<", triangulated : "<<Nt<<", mean radius = " << TW.mean_radius<<endl;
}




//namespace CGT {

CREATE_LOGGER(TesselationWrapper);

static CGT::Point Pmin;
static CGT::Point Pmax;
static double inf = 1e10;
double pminx=0;
double pminy=0;
double pminz=0;
double pmaxx=0;
double pmaxy=0;
double pmaxz=0;
double thickness = 0;

//  Finite_edges_iterator facet_it; //an edge in a triangulation is a facet in corresponding tesselation, remember...
//That explain the name.

TesselationWrapper::TesselationWrapper()
{
	Tes = new CGT::Tesselation;
	inf = 1e10;
	clear();
	facet_begin = Tes->Triangulation().finite_edges_begin();
	facet_end = Tes->Triangulation().finite_edges_end();
	facet_it = Tes->Triangulation().finite_edges_begin();
}


TesselationWrapper::~TesselationWrapper()
{
	delete Tes;
}

void TesselationWrapper::clear(void)
{
	Tes->Clear();
	Pmin = CGT::Point(inf, inf, inf);
	Pmax = CGT::Point(-inf, -inf, -inf);
	mean_radius = 0;
	n_spheres = 0;
	rad_divided = false;
	bounded = false;
	facet_it = Tes->Triangulation().finite_edges_end();
}

void TesselationWrapper::clear2(void) //for testing purpose
{
	Tes->Clear();

//   Pmin = Point(inf, inf, inf);
//  Pmax = Point(-inf, -inf, -inf);
//  mean_radius = 0;
//  n_spheres = 0;
//  rad_divided = false;
// bounded = false;
//  facet_it = Tes->Triangulation().finite_edges_end ();
}

bool TesselationWrapper::insertSceneSpheres(const Scene* scene)
{
	const shared_ptr<BodyContainer>& bodies = scene->bodies;
	build_triangulation_with_ids(bodies, *this);
	return 1;
}

double TesselationWrapper::Volume(unsigned int id) {return Tes->Volume(id);}

bool TesselationWrapper::insert(double x, double y, double z, double rad, unsigned int id)
{
	using namespace std;
	Pmin = CGT::Point(min(Pmin.x(), x-rad),
					  min(Pmin.y(), y-rad),
					  min(Pmin.z(), z-rad));
	Pmax = CGT::Point(max(Pmax.x(), x+rad),
					  max(Pmax.y(), y+rad),
					  max(Pmax.z(), z+rad));
	mean_radius += rad;
	++n_spheres;
	return (Tes->insert(x,y,z,rad,id)!=NULL);
}

void TesselationWrapper::checkMinMax(double x, double y, double z, double rad)
{
	using namespace std;
	Pmin = CGT::Point(min(Pmin.x(), x-rad),
					  min(Pmin.y(), y-rad),
					  min(Pmin.z(), z-rad));
	Pmax = CGT::Point(max(Pmax.x(), x+rad),
					  max(Pmax.y(), y+rad),
					  max(Pmax.z(), z+rad));
	mean_radius += rad;
	++n_spheres;
}


bool TesselationWrapper::move(double x, double y, double z, double rad, unsigned int id)
{
	using namespace std;

	Pmin = CGT::Point(min(Pmin.x(), x-rad),
					  min(Pmin.y(), y-rad),
					  min(Pmin.z(), z-rad));
	Pmax = CGT::Point(max(Pmax.x(), x+rad),
					  max(Pmax.y(), y+rad),
					  max(Pmax.z(), z+rad));
	mean_radius += rad;

	if (Tes->move(x,y,z,rad,id)!=NULL)
		return true;
	else {
		cerr << "Tes->move(x,y,z,rad,id)==NULL" << endl; return false;
	}
}


void TesselationWrapper::ComputeTesselation(void)
{
	if (!rad_divided) {
		mean_radius /= n_spheres;
		rad_divided = true;
	}
	Tes->Compute();
}
void TesselationWrapper::ComputeTesselation(double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz, double dt)
{
	AddBoundingPlanes(pminx, pmaxx,  pminy,  pmaxy, pminz, pmaxz, dt);
	ComputeTesselation();
}

void TesselationWrapper::ComputeVolumes(void)
{
	ComputeTesselation();
	Tes->ComputeVolumes();
}
unsigned int TesselationWrapper::NumberOfFacets(bool initIters)
{
	if (initIters) InitIter();
	return Tes->Triangulation().number_of_finite_edges();
}

void TesselationWrapper::InitIter(void)
{
	facet_begin = Tes->Triangulation().finite_edges_begin();
	facet_end = Tes->Triangulation().finite_edges_end();
	facet_it = facet_begin;
}

bool TesselationWrapper::nextFacet(std::pair<unsigned int,unsigned int>& facet)
{
	if (facet_end==facet_it) return false;
	facet.first = facet_it->first->vertex(facet_it->second)->info().id();
	facet.second = facet_it->first->vertex((facet_it)->third)->info().id();
	++facet_it;
	return true;
}



void TesselationWrapper::AddBoundingPlanes(double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz,double dt)
{
//  cout << " pminx                                                    " << pminx << endl;
//  cout << " pminy                                                    " << pminy << endl;
//  cout << " pminz                                                    " << pminz << endl;
//  cout << " pmaxx                                                    " << pmaxx << endl;
//  cout << " pmaxy                                                    " << pmaxy << endl;
//  cout << " pmaxz                                                    " << pmaxz << endl;
	if (dt == 0) {
//  cout << "thickness               =               " << dt << endl;
//  thickness = -1*pminx;
		thickness = -1*pminx;
	}
//  cout << "thickness               =               " << thickness << endl;
	if (!bounded) {
		if (!rad_divided) {
			mean_radius /= n_spheres;
			rad_divided = true;
		}
		double FAR = 10000;

		Tes->redirect();
		//Add big bounding spheres with isFictious=true

		Tes->vertexHandles[0]=Tes->insert(0.5*(pminx+pmaxx), pminy-FAR*(pmaxx-pminx), 0.5*(pmaxz-pminz), FAR*(pmaxx-pminx)+thickness, 0, true);
		Tes->vertexHandles[1]=Tes->insert(0.5*(pminx+pmaxx), pmaxy+FAR*(pmaxx-pminx), 0.5*(pmaxz-pminz), FAR*(pmaxx-pminx)+thickness, 1, true);
		Tes->vertexHandles[2]=Tes->insert(pminx-FAR*(pmaxy-pminy), 0.5*(pmaxy-pminy), 0.5*(pmaxz-pminz), FAR*(pmaxy-pminy)+thickness, 2, true);
		Tes->vertexHandles[3]=Tes->insert(pmaxx+FAR*(pmaxx-pminy), 0.5*(pmaxy-pminy), 0.5*(pmaxz-pminz), FAR*(pmaxy-pminy)+thickness, 3, true);
		Tes->vertexHandles[4]=Tes->insert(0.5*(pminx+pmaxx), 0.5*(pmaxy-pminy), pminz-FAR*(pmaxy-pminy), FAR*(pmaxy-pminy)+thickness, 4, true);
		Tes->vertexHandles[5]=Tes->insert(0.5*(pminx+pmaxx), 0.5*(pmaxy-pminy), pmaxz+FAR*(pmaxy-pminy), FAR*(pmaxy-pminy)+thickness, 5, true);
		bounded = true;
	}
}

void  TesselationWrapper::AddBoundingPlanes(void)
{

	if (!bounded) {
		if (!rad_divided) {
			mean_radius /= n_spheres;
			rad_divided = true;
		}
		double FAR = 10000;

		Tes->redirect();
		//Add big bounding spheres with isFictious=true

		Tes->vertexHandles[0]=Tes->insert(0.5*(Pmin.x()+Pmax.x()), Pmin.y()-FAR*(Pmax.x()-Pmin.x()), 0.5*(Pmax.z()-Pmin.z()), FAR*(Pmax.x()-Pmin.x()), 0, true);
		Tes->vertexHandles[1]=Tes->insert(0.5*(Pmin.x()+Pmax.x()), Pmax.y()+FAR*(Pmax.x()-Pmin.x()), 0.5*(Pmax.z()-Pmin.z()), FAR*(Pmax.x()-Pmin.x()), 1, true);
		Tes->vertexHandles[2]=Tes->insert(Pmin.x()-FAR*(Pmax.y()-Pmin.y()), 0.5*(Pmax.y()-Pmin.y()), 0.5*(Pmax.z()-Pmin.z()), FAR*(Pmax.y()-Pmin.y()), 2, true);
		Tes->vertexHandles[3]=Tes->insert(Pmax.x()+FAR*(Pmax.y()-Pmin.y()), 0.5*(Pmax.y()-Pmin.y()), 0.5*(Pmax.z()-Pmin.z()), FAR*(Pmax.y()-Pmin.y()), 3, true);
		Tes->vertexHandles[4]=Tes->insert(0.5*(Pmin.x()+Pmax.x()), 0.5*(Pmax.y()-Pmin.y()), Pmin.z()-FAR*(Pmax.y()-Pmin.y()), FAR*(Pmax.y()-Pmin.y()), 4, true);
		Tes->vertexHandles[5]=Tes->insert(0.5*(Pmin.x()+Pmax.x()), 0.5*(Pmax.y()-Pmin.y()), Pmax.z()+FAR*(Pmax.y()-Pmin.y()), FAR*(Pmax.y()-Pmin.y()), 5, true);
		bounded = true;
	}



}


void  TesselationWrapper::RemoveBoundingPlanes(void)
{

	cerr << "start redirection";
	Tes->redirect();
	cerr << " | start redirection" << endl;
	Tes->remove(0);
	Tes->remove(1);
	Tes->remove(2);
	Tes->remove(3);
	Tes->remove(4);
	Tes->remove(5);
	Pmin = CGT::Point(inf, inf, inf);
	Pmax = CGT::Point(-inf, -inf, -inf);
	mean_radius = 0;
	//n_spheres = 0;
	rad_divided = false;
	bounded = false;
	cerr << " end remove bounding planes " << endl;
}

//} //namespace CGT





// int main()
// {
//  std::list<Point> input;
//
//  input.push_back ( Point ( 0,0,0 ) );
//  input.push_back ( Point ( 1,0,0 ) );
//  input.push_back ( Point ( 0,1,0 ) );
//  input.push_back ( Point ( 0,0,1 ) );
//  input.push_back ( Point ( 2,2,2 ) );
//  input.push_back ( Point ( -1,0,1 ) );
//
//  Delaunay T;
//
//  build_triangulation_with_indices ( input.begin(),input.end(),T );
//
//  Delaunay::Finite_vertices_iterator vit;
//  for ( vit = T.finite_vertices_begin(); vit != T.finite_vertices_end(); ++vit )
//   std::cout << vit->info() << "\n"; //prints the position in input
//
//  return 0;
// }


