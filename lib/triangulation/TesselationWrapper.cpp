

//
// C++ Implementation: TesselationWrapper
//
// Description: 
//
//
// Author: chareyre <bruno.chareyre@hmg.inpg.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "TesselationWrapper.h"
#include "Tesselation.h"

using namespace std;

namespace CGT {

static Point Pmin;
static Point Pmax;
static double inf = 1e10;
 double pminx=0;
 double pminy=0;
 double pminz=0;
 double pmaxx=0;
 double pmaxy=0;
 double pmaxz=0;
 double thickness = 0;

static Finite_edges_iterator facet_it;	//an edge in a triangulation is a facet in corresponding tesselation, remember...
					//That explain the name.

TesselationWrapper::TesselationWrapper()
{
	Tes = new Tesselation;
	inf = 1e10;
	clear();
	
}


TesselationWrapper::~TesselationWrapper()
{
	delete Tes;
}
  
 void TesselationWrapper::clear(void)
 {
 	Tes->Clear();
 	Pmin = Point(inf, inf, inf);
	Pmax = Point(-inf, -inf, -inf);
	mean_radius = 0;
	n_spheres = 0;
	rad_divided = false;
	bounded = false;
	facet_it = Tes->Triangulation().finite_edges_end ();
	
 }

 
 
 void TesselationWrapper::clear2(void) //for testing purpose
 {
 	Tes->Clear();
 	
//  	Pmin = Point(inf, inf, inf);
// 	Pmax = Point(-inf, -inf, -inf);
// 	mean_radius = 0;
// 	n_spheres = 0;
// 	rad_divided = false;
//	bounded = false;
// 	facet_it = Tes->Triangulation().finite_edges_end ();
 }
 


double TesselationWrapper::Volume( unsigned int id )
{
	return Tes->Volume(id);

}

bool TesselationWrapper::insert(double x, double y, double z, double rad, unsigned int id)
{	
	using namespace std;

		Pmin = Point( min(Pmin.x(), x-rad),
							min(Pmin.y(), y-rad),
							min(Pmin.z(), z-rad) );
		Pmax = Point( max(Pmax.x(), x+rad),
							max(Pmax.y(), y+rad),
							max(Pmax.z(), z+rad) );
		mean_radius += rad;
		++n_spheres;
		
		return (Tes->insert(x,y,z,rad,id)!=NULL);
}

void TesselationWrapper::checkMinMax(double x, double y, double z, double rad)
{	
	using namespace std;
		Pmin = Point( min(Pmin.x(), x-rad),
							min(Pmin.y(), y-rad),
							min(Pmin.z(), z-rad) );
		Pmax = Point( max(Pmax.x(), x+rad),
							max(Pmax.y(), y+rad),
							max(Pmax.z(), z+rad) );
		mean_radius += rad;
		++n_spheres;
		
		//return (Tes->insert(x,y,z,rad,id)!=NULL);
}


bool TesselationWrapper::move(double x, double y, double z, double rad, unsigned int id)
{	
	using namespace std;

		Pmin = Point( min(Pmin.x(), x-rad),
							min(Pmin.y(), y-rad),
							min(Pmin.z(), z-rad) );
		Pmax = Point( max(Pmax.x(), x+rad),
							max(Pmax.y(), y+rad),
							max(Pmax.z(), z+rad) );
		mean_radius += rad;
				
		if (Tes->move(x,y,z,rad,id)!=NULL)		
		return true;
		else {
		cerr << "Tes->move(x,y,z,rad,id)==NULL" << endl; return false;}
}


void TesselationWrapper::ComputeTesselation( void )
{
	if (!rad_divided) {
		mean_radius /= n_spheres;
		rad_divided = true;}
	Tes->Compute();	
}
 void TesselationWrapper::ComputeTesselation( double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz, double dt)
{	
 	AddBoundingPlanes( pminx, pmaxx,  pminy,  pmaxy, pminz, pmaxz, dt);
	ComputeTesselation();
}

void TesselationWrapper::ComputeVolumes(void)
{
  	ComputeTesselation();
	Tes->ComputeVolumes();
}

unsigned int TesselationWrapper::NumberOfFacets(void)
{
	facet_it = Tes->Triangulation().finite_edges_begin ();
	return Tes->Triangulation().number_of_finite_edges ();

}

void TesselationWrapper::nextFacet (std::pair<unsigned int,unsigned int>& facet)
{	

	facet.first = facet_it->first->vertex ( facet_it->second )->info().id();
	facet.second = facet_it->first->vertex ( (facet_it)->third )->info().id();
	++facet_it;

}


 
void TesselationWrapper::AddBoundingPlanes(double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz,double dt)
{	
// 	cout << " pminx                                                    " << pminx << endl;
// 	cout << " pminy                                                    " << pminy << endl;
// 	cout << " pminz                                                    " << pminz << endl;
// 	cout << " pmaxx                                                    " << pmaxx << endl;
// 	cout << " pmaxy                                                    " << pmaxy << endl;
// 	cout << " pmaxz                                                    " << pmaxz << endl;
	if (dt == 0)
	{
// 	cout << "thickness               =               " << dt << endl;
// 	thickness = -1*pminx;
		thickness = -1*pminx;
	}
// 	cout << "thickness               =               " << thickness << endl;
	if (!bounded) {
	if (!rad_divided) {
		mean_radius /= n_spheres;
		rad_divided = true;}
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


 


void 	TesselationWrapper::AddBoundingPlanes (void)
{	

	if (!bounded) {
	if (!rad_divided) {
		mean_radius /= n_spheres;
		rad_divided = true;}
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


void 	TesselationWrapper::RemoveBoundingPlanes (void)
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
	Pmin = Point(inf, inf, inf);
	Pmax = Point(-inf, -inf, -inf);
	mean_radius = 0;
	//n_spheres = 0;
	rad_divided = false;
	bounded = false;
	cerr << " end remove bounding planes " << endl;
}

} //namespace CGT
