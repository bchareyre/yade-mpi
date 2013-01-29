/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalano@hmg.inpg.fr                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL

#ifdef FLOW_ENGINE
#include<yade/core/Scene.hpp>
#include<yade/lib/base/Math.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Wall.hpp>
#include<yade/pkg/common/Box.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "UnsaturatedEngine.hpp"

CREATE_LOGGER ( UnsaturatedEngine );

CGT::Vecteur makeCgVect2 ( const Vector3r& yv ) {return CGT::Vecteur ( yv[0],yv[1],yv[2] );}
CGT::Point makeCgPoint2 ( const Vector3r& yv ) {return CGT::Point ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Vecteur& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}


UnsaturatedEngine::~UnsaturatedEngine()
{
}

void UnsaturatedEngine::testFunction()
{
	cout<<"This is Chao's test program"<<endl;

// 	UnsaturatedEngine inherits from Emanuele's flow engine, so it contains many things. However, we will ignore what's in it for the moment.
// 	The only thing interesting for us is that UnsaturatedEngine contains an object "triangulation" from CGAL library.
//	Let us define an alias for this triangulation:

	RTriangulation& triangulation = solver->T[solver->currentTes].Triangulation();
	//Now, you can use "triangulation", with all the functions listed in CGAL documentation
	//We can insert spheres (here I'm in fact stealing the code from Tesselation::insert() (see Tesselation.ipp)

/*	//test Compute_EffectiveRadius function
	unsigned int k=0;
	Real x=0.0, y=0.0,z=0.0, rad=1.0;
	Vertex_handle Vh;
	Vh = triangulation.insert(CGALSphere(Point(x,y,z),pow(rad,2)));
	//The vertex base includes integers, so we can assign indices to the vertex/spheres 
	Vh->info() = k;
	k = k+1;
	Vh = triangulation.insert(CGALSphere(Point(3.464101616,0.0,0.0),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(1.732050808,3.0,0.0),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(1.732050808,1.0,3),pow(rad,2)));
	Vh->info() = k++;		
	cout << "triangulation.number_of_vertices()" << triangulation.number_of_vertices() << endl;
	cout <<"triangulation.number_of_cells()" << triangulation.number_of_cells() << endl;
	
	//now we can start playing with pressure (=1 for dry pore, =0 for saturated pore)
	//they all have 0 by default, we find one cell and set pressure to 1
	Cell_handle cell = triangulation.locate(Point(1.732,1.732,1.0));
	cell->info().p()=1;
	FlowSolver FS;
	double inscribe_r0 = FS.Compute_EffectiveRadius(cell, 0);
	double inscribe_r1 = FS.Compute_EffectiveRadius(cell, 1);
	double inscribe_r2 = FS.Compute_EffectiveRadius(cell, 2);
	double inscribe_r3 = FS.Compute_EffectiveRadius(cell, 3);	
	cout << "the radius of inscribe circle for facet 0: inscribe_r0 = " << inscribe_r0 << endl;
	cout << "the radius of inscribe circle for facet 1: inscribe_r1 = " << inscribe_r1 << endl;
	cout << "the radius of inscribe circle for facet 2: inscribe_r2 = " << inscribe_r2 << endl;
	cout << "the radius of inscribe circle for facet 3: inscribe_r3 = " << inscribe_r3 << endl;

	//pass. triangulation.number_of_cells()5
	//the radius of inscribe circle for facet 0: inscribe_r0 = 1.05548
	//the radius of inscribe circle for facet 1: inscribe_r1 = 1.05548
	//the radius of inscribe circle for facet 2: inscribe_r2 = 1.05548
	//the radius of inscribe circle for facet 3: inscribe_r3 = 1
*/	
	//test 8 vertices  	
	unsigned int k=0, m=0;
	Real x=5.0, y=5.0,z=5.0, rad=1.0;
	Vertex_handle Vh;
	Vh = triangulation.insert(CGALSphere(Point(x,y,z),pow(rad,2)));
	//The vertex base includes integers, so we can assign indices to the vertex/spheres 
	Vh->info() = k;
	k = k+1;
	Vh = triangulation.insert(CGALSphere(Point(7.5,6.0,5.5),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(6.0,6.0,7.5),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(5.4,7.3,5.8),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(7.0,4.0,6.5),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(6.5,7.2,3.8),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(7.6,8.2,6.6),pow(rad,2)));
	Vh->info() = k++;
	Vh = triangulation.insert(CGALSphere(Point(3.5,6.0,7.0),pow(rad,2)));
	Vh->info() = k++;
	
	cout << "triangulation.number_of_vertices()" << triangulation.number_of_vertices() << endl;
	cout <<"triangulation.number_of_cells()" << triangulation.number_of_cells() << endl;
	
	//now we can start playing with pressure (=1 for dry pore, =0 for saturated pore)
	//they all have 0 by default, we find one cell and set pressure to 1
	Cell_handle cell = triangulation.locate(Point(6.0,6.0,6.0));
	cell->info() = m;
	cell->info().p()= 5; //initialised air entry pressure
	m = m+1;	
	
	FlowSolver FS;
	double surface_tension = 1; //hypothesis that's surface tension
	
	for(int facet = 0; facet < 4; facet ++)
	{
	  double pe = 2*surface_tension/FS.Compute_EffectiveRadius(cell, facet);		
	  //pe is air entry pressure, related to facet(inscribe circle r), vertices and surface_tension, pe = (Lnw+Lns*cosθ)*σnw/An = 2*σnw/r
	  cout << "facet: " << facet << ", air entry pressure pe =  " << pe << endl;
	  if (cell->info().p() > pe)
	    cell->neighbor(facet)->info().p() = cell->info().p();
	}
	
	double cell_pressure;
	cell_pressure = FS.MeasurePorePressure (6, 7, 6);
	cout << "the pressure in cell(6,7,6) is: " << cell_pressure << endl;
	


	solver->noCache = false;

	/*
	//This is how we could input spheres from the simulation into a triangulation, we will use it latter as fow now we only define a few spheres manually (below)
	//here we define the pointer to Yade's scene
	scene = Omega::instance().getScene().get();
	//copy sphere positions in a buffer...
	setPositionsBuffer(true);
	//then create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
	Build_Triangulation(P_zero,solver);
	*/

	
}


void UnsaturatedEngine::action()
{
	//This will be used later
}

template<class Solver>
unsigned int UnsaturatedEngine::imposePressure(Vector3r pos, Real p,Solver& flow)
{
	if (!flow) LOG_ERROR("no flow defined yet, run at least one iter");
	flow->imposedP.push_back( pair<CGT::Point,Real>(CGT::Point(pos[0],pos[1],pos[2]),p) );
	return flow->imposedP.size()-1;
}


template<class Solver>
void UnsaturatedEngine::BoundaryConditions ( Solver& flow )
{
        if ( flow->y_min_id>=0 ) {
                flow->boundary ( flow->y_min_id ).flowCondition=Flow_imposed_BOTTOM_Boundary;
                flow->boundary ( flow->y_min_id ).value=Pressure_BOTTOM_Boundary;
        }
        if ( flow->y_max_id>=0 ) {
                flow->boundary ( flow->y_max_id ).flowCondition=Flow_imposed_TOP_Boundary;
                flow->boundary ( flow->y_max_id ).value=Pressure_TOP_Boundary;
        }
        if ( flow->x_max_id>=0 ) {
                flow->boundary ( flow->x_max_id ).flowCondition=Flow_imposed_RIGHT_Boundary;
                flow->boundary ( flow->x_max_id ).value=Pressure_RIGHT_Boundary;
        }
        if ( flow->x_min_id>=0 ) {
                flow->boundary ( flow->x_min_id ).flowCondition=Flow_imposed_LEFT_Boundary;
                flow->boundary ( flow->x_min_id ).value=Pressure_LEFT_Boundary;
        }
        if ( flow->z_max_id>=0 ) {
                flow->boundary ( flow->z_max_id ).flowCondition=Flow_imposed_FRONT_Boundary;
                flow->boundary ( flow->z_max_id ).value=Pressure_FRONT_Boundary;
        }
        if ( flow->z_min_id>=0 ) {
                flow->boundary ( flow->z_min_id ).flowCondition=Flow_imposed_BACK_Boundary;
                flow->boundary ( flow->z_min_id ).value=Pressure_BACK_Boundary;
        }
}

template<class Solver>
void UnsaturatedEngine::initSolver ( Solver& flow )
{
       	flow->Vtotalissimo=0; flow->Vsolid_tot=0; flow->Vporale=0; flow->Ssolid_tot=0;
        flow->T[flow->currentTes].Clear();
        flow->T[flow->currentTes].max_id=-1;
        flow->x_min = 1000.0, flow->x_max = -10000.0, flow->y_min = 1000.0, flow->y_max = -10000.0, flow->z_min = 1000.0, flow->z_max = -10000.0;
}

template<class Solver>
void UnsaturatedEngine::Build_Triangulation ( Solver& flow )
{
        Build_Triangulation ( 0.f,flow );
}

template<class Solver>
void UnsaturatedEngine::Build_Triangulation ( double P_zero, Solver& flow )
{
        flow->ResetNetwork();
	if (first) flow->currentTes=0;
        else {
                flow->currentTes=!flow->currentTes;
                if (Debug) cout << "--------RETRIANGULATION-----------" << endl;
        }

	initSolver(flow);

        AddBoundary ( flow );
        Triangulate ( flow );
        if ( Debug ) cout << endl << "Tesselating------" << endl << endl;
        flow->T[flow->currentTes].Compute();

        flow->Define_fictious_cells();
	// For faster loops on cells define this vector
	flow->T[flow->currentTes].cellHandles.clear();
	flow->T[flow->currentTes].cellHandles.reserve(flow->T[flow->currentTes].Triangulation().number_of_finite_cells());
	Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
		flow->T[flow->currentTes].cellHandles.push_back(cell);
        flow->DisplayStatistics ();
        flow->Compute_Permeability ( );

        porosity = flow->V_porale_porosity/flow->V_totale_porosity;

        BoundaryConditions ( flow );
        flow->Initialize_pressures ( P_zero );
}

void UnsaturatedEngine::setPositionsBuffer(bool current)
{
	vector<posData>& buffer = current? positionBufferCurrent : positionBufferParallel;
	buffer.clear();
	buffer.resize(scene->bodies->size());
	shared_ptr<Sphere> sph ( new Sphere );
        const int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies ) {
                posData& dat = buffer[b->getId()];
		dat.id=b->getId();
		dat.pos=b->state->pos;
		dat.isSphere= (b->shape->getClassIndex() ==  Sph_Index);
		if (dat.isSphere) dat.radius = YADE_CAST<Sphere*>(b->shape.get())->radius;
		dat.exists=true;
	}
}

template<class Solver>
void UnsaturatedEngine::AddBoundary ( Solver& flow )
{
	vector<posData>& buffer = positionBufferCurrent;
        solver->x_min = Mathr::MAX_REAL, solver->x_max = -Mathr::MAX_REAL, solver->y_min = Mathr::MAX_REAL, solver->y_max = -Mathr::MAX_REAL, solver->z_min = Mathr::MAX_REAL, solver->z_max = -Mathr::MAX_REAL;
        FOREACH ( const posData& b, buffer ) {
                if ( !b.exists ) continue;
                if ( b.isSphere ) {
                        const Real& rad = b.radius;
                        const Real& x = b.pos[0];
                        const Real& y = b.pos[1];
                        const Real& z = b.pos[2];
                        flow->x_min = min ( flow->x_min, x-rad );
                        flow->x_max = max ( flow->x_max, x+rad );
                        flow->y_min = min ( flow->y_min, y-rad );
                        flow->y_max = max ( flow->y_max, y+rad );
                        flow->z_min = min ( flow->z_min, z-rad );
                        flow->z_max = max ( flow->z_max, z+rad );
                }
        }
	//FIXME id_offset must be set correctly, not the case here (always 0), then we need walls first or it will fail
        id_offset = flow->T[flow->currentTes].max_id+1;
        flow->id_offset = id_offset;
        flow->SectionArea = ( flow->x_max - flow->x_min ) * ( flow->z_max-flow->z_min );
        flow->Vtotale = ( flow->x_max-flow->x_min ) * ( flow->y_max-flow->y_min ) * ( flow->z_max-flow->z_min );
        flow->y_min_id=wallBottomId;
        flow->y_max_id=wallTopId;
        flow->x_max_id=wallRightId;
        flow->x_min_id=wallLeftId;
        flow->z_min_id=wallBackId;
        flow->z_max_id=wallFrontId;

        if ( flow->y_min_id>=0 ) flow->boundary ( flow->y_min_id ).useMaxMin = BOTTOM_Boundary_MaxMin;
        if ( flow->y_max_id>=0 ) flow->boundary ( flow->y_max_id ).useMaxMin = TOP_Boundary_MaxMin;
        if ( flow->x_max_id>=0 ) flow->boundary ( flow->x_max_id ).useMaxMin = RIGHT_Boundary_MaxMin;
        if ( flow->x_min_id>=0 ) flow->boundary ( flow->x_min_id ).useMaxMin = LEFT_Boundary_MaxMin;
        if ( flow->z_max_id>=0 ) flow->boundary ( flow->z_max_id ).useMaxMin = FRONT_Boundary_MaxMin;
        if ( flow->z_min_id>=0 ) flow->boundary ( flow->z_min_id ).useMaxMin = BACK_Boundary_MaxMin;

        //FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
        flow->boundsIds[0]= &flow->x_min_id;
        flow->boundsIds[1]= &flow->x_max_id;
        flow->boundsIds[2]= &flow->y_min_id;
        flow->boundsIds[3]= &flow->y_max_id;
        flow->boundsIds[4]= &flow->z_min_id;
        flow->boundsIds[5]= &flow->z_max_id;

        flow->Corner_min = CGT::Point ( flow->x_min, flow->y_min, flow->z_min );
        flow->Corner_max = CGT::Point ( flow->x_max, flow->y_max, flow->z_max );

        if ( Debug ) {
                cout << "Section area = " << flow->SectionArea << endl;
                cout << "Vtotale = " << flow->Vtotale << endl;
                cout << "x_min = " << flow->x_min << endl;
                cout << "x_max = " << flow->x_max << endl;
                cout << "y_max = " << flow->y_max << endl;
                cout << "y_min = " << flow->y_min << endl;
                cout << "z_min = " << flow->z_min << endl;
                cout << "z_max = " << flow->z_max << endl;
                cout << endl << "Adding Boundary------" << endl;
        }
        //assign BCs types and values
        BoundaryConditions ( flow );

        double center[3];
        for ( int i=0; i<6; i++ ) {
                if ( *flow->boundsIds[i]<0 ) continue;
                CGT::Vecteur Normal ( normal[i].x(), normal[i].y(), normal[i].z() );
                if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->AddBoundingPlane ( true, Normal, *flow->boundsIds[i] );
                else {
			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
                        flow->AddBoundingPlane ( center, wall_thickness, Normal,*flow->boundsIds[i] );
                }
        }
}

template<class Solver>
void UnsaturatedEngine::Triangulate ( Solver& flow )
{
///Using Tesselation wrapper (faster)
// 	TesselationWrapper TW;
// 	if (TW.Tes) delete TW.Tes;
// 	TW.Tes = &(flow->T[flow->currentTes]);//point to the current Tes we have in Flowengine
// 	TW.insertSceneSpheres();//TW is now really inserting in UnsaturatedEngine, using the faster insert(begin,end)
// 	TW.Tes = NULL;//otherwise, Tes would be deleted by ~TesselationWrapper() at the end of the function.
///Using one-by-one insertion
	vector<posData>& buffer = positionBufferCurrent;
	FOREACH ( const posData& b, buffer ) {
                if ( !b.exists ) continue;
                if ( b.isSphere ) {
// 			if (b.id==ignoredBody) continue;
                        flow->T[flow->currentTes].insert ( b.pos[0], b.pos[1], b.pos[2], b.radius, b.id );}
        }
        flow->T[flow->currentTes].redirected=true;//By inserting one-by-one, we already redirected
}

template<class Solver>
void UnsaturatedEngine::Initialize_volumes ( Solver& flow )
{
	typedef typename Solver::element_type Flow;
	typedef typename Flow::Finite_vertices_iterator Finite_vertices_iterator;
	typedef typename Solver::element_type Flow;
	typedef typename Flow::Finite_cells_iterator Finite_cells_iterator;
	
	Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
	CGT::Vecteur Zero(0,0,0);
	for (Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it!= vertices_end; V_it++) V_it->info().forces=Zero;

	FOREACH(Cell_handle& cell, flow->T[flow->currentTes].cellHandles)
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
			default: break; 
		}

		if (flow->fluidBulkModulus>0) { cell->info().invVoidVolume() = 1 / ( abs(cell->info().volume()) - flow->volumeSolidPore(cell) ); }
	}
	if (Debug) cout << "Volumes initialised." << endl;
}

template<class Cellhandle>
Real UnsaturatedEngine::Volume_cell_single_fictious ( Cellhandle cell )
{
	//With buffer
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real Wall_coordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
//                         const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( y )->info().id(), scene );
                        V[w]=positionBufferCurrent[cell->vertex ( y )->info().id()].pos;
			w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b , scene );
                        if ( !solver->boundary ( b ).useMaxMin ) Wall_coordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wall_thickness/2;
                        else Wall_coordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real Volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - Wall_coordinate );
        return abs ( Volume );
}
template<class Cellhandle>
Real UnsaturatedEngine::Volume_cell_double_fictious ( Cellhandle cell )
{
        Vector3r A=Vector3r::Zero(), AS=Vector3r::Zero(),B=Vector3r::Zero(), BS=Vector3r::Zero();

        cell->info().volumeSign=1;
        int b[2];
        int coord[2];
        Real Wall_coordinate[2];
        int j=0;
        bool first_sph=true;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
                        if ( !solver->boundary ( b[j] ).useMaxMin ) Wall_coordinate[j] = positionBufferCurrent[b[j]].pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wall_thickness/2;
                        else Wall_coordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else if ( first_sph ) {
                        A=AS=/*AT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;
                        first_sph=false;
                } else {
                        B=BS=/*BT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;;
                }
        }
        AS[coord[0]]=BS[coord[0]] = Wall_coordinate[0];

        //first pyramid with triangular base (A,B,BS)
        Real Vol1=0.5* ( ( A-BS ).cross ( B-BS ) ) [coord[1]]* ( 0.333333333* ( 2*B[coord[1]]+A[coord[1]] )-Wall_coordinate[1] );
        //second pyramid with triangular base (A,AS,BS)
        Real Vol2=0.5* ( ( AS-BS ).cross ( A-BS ) ) [coord[1]]* ( 0.333333333* ( B[coord[1]]+2*A[coord[1]] )-Wall_coordinate[1] );
        return abs ( Vol1+Vol2 );
}
template<class Cellhandle>
Real UnsaturatedEngine::Volume_cell_triple_fictious ( Cellhandle cell )
{
        Vector3r A;

        int b[3];
        int coord[3];
        Real Wall_coordinate[3];
        int j=0;
        cell->info().volumeSign=1;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
                        const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
                        if ( !solver->boundary ( b[j] ).useMaxMin ) Wall_coordinate[j] = wll->state->pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wall_thickness/2;
                        else Wall_coordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
                        A= ( sph->state->pos );
                }
        }
        Real Volume = ( A[coord[0]]-Wall_coordinate[0] ) * ( A[coord[1]]-Wall_coordinate[1] ) * ( A[coord[2]]-Wall_coordinate[2] );
        return abs ( Volume );
}
template<class Cellhandle>
Real UnsaturatedEngine::Volume_cell ( Cellhandle cell )
{
	static const Real inv6 = 1/6.;
	const Vector3r& p0 = positionBufferCurrent[cell->vertex ( 0 )->info().id()].pos;
	const Vector3r& p1 = positionBufferCurrent[cell->vertex ( 1 )->info().id()].pos;
	const Vector3r& p2 = positionBufferCurrent[cell->vertex ( 2 )->info().id()].pos;
	const Vector3r& p3 = positionBufferCurrent[cell->vertex ( 3 )->info().id()].pos;
	Real volume = inv6 * ((p0-p1).cross(p0-p2)).dot(p0-p3);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}

template<class Solver>
void UnsaturatedEngine::setImposedPressure ( unsigned int cond, Real p,Solver& flow )
{
        if ( cond>=flow->imposedP.size() ) LOG_ERROR ( "Setting p with cond higher than imposedP size." );
        flow->imposedP[cond].second=p;
        //force immediate update of boundary conditions
	flow->pressureChanged=true;
}

template<class Solver>
void UnsaturatedEngine::clearImposedPressure ( Solver& flow ) { flow->imposedP.clear(); flow->IPCells.clear();}

YADE_PLUGIN ( ( UnsaturatedEngine ) );

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */

