/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*  Copyright (C) 2012 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
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
#include <CGAL/Plane_3.h>
#include <CGAL/Plane_3.h>

CREATE_LOGGER ( UnsaturatedEngine );

CGT::Vecteur makeCgVect2 ( const Vector3r& yv ) {return CGT::Vecteur ( yv[0],yv[1],yv[2] );}
CGT::Point makeCgPoint2 ( const Vector3r& yv ) {return CGT::Point ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Vecteur& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}


UnsaturatedEngine::~UnsaturatedEngine()
{
}

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

void UnsaturatedEngine::testFunction()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	if (tri.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		Build_Triangulation(P_zero,solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initializeCellIndex(solver);//initialize cell index
		initializePoreRadius(solver);//save all pore radii before invade
		updateVolumeCapillaryCell(solver);//save capillary volume of all cells, for calculating saturation
		computeSolidLine(solver);//save cell->info().solidLine[j][y]
	}
	solver->noCache = true;
}

void UnsaturatedEngine::action()
{
    if ( !isActivated ) return;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if ( (tri.number_of_vertices()==0) || (updateTriangulation) ) {
        cout<< "triangulation is empty: building a new one" << endl;
        scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
        setPositionsBuffer(true);//copy sphere positions in a buffer...
        Build_Triangulation(P_zero,solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
        initializeCellIndex(solver);//initialize cell index
        initializePoreRadius(solver);//save all pore radii before invade
        updateVolumeCapillaryCell(solver);//save capillary volume of all cells, for calculating saturation
        computeSolidLine(solver);//save cell->info().solidLine[j][y]
        solver->noCache = true;
    }
    ///compute invade
    if (pressureForce) {
        invade(solver);
    }
    ///compute force
    computeFacetPoreForcesWithCache(solver);
    Vector3r force;
    Finite_vertices_iterator vertices_end = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
    for ( Finite_vertices_iterator V_it = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++ ) {
        force = pressureForce ? Vector3r ( V_it->info().forces[0],V_it->info().forces[1],V_it->info().forces[2] ): Vector3r(0,0,0);
        scene->forces.addForce ( V_it->info().id(), force);
    }
}

template<class Solver>
void UnsaturatedEngine::invade(Solver& flow)
{
    if (isPhaseTrapped) {
        invade1(solver);
    }
    else {
        invade2(solver);
    }
}
template<class Solver>
Real UnsaturatedEngine::getMinEntryValue(Solver& flow )
{
    if (isPhaseTrapped) {
        return getMinEntryValue1(solver);
    }
    else {
        return getMinEntryValue2(solver);
    }
}
template<class Solver>
Real UnsaturatedEngine::getSaturation(Solver& flow )
{
    if (isPhaseTrapped) {
        return getSaturation1(solver);
    }
    else {
        return getSaturation2(solver);
    }
}

///invade mode 1. update phase reservoir before invasion. Consider no viscous effects, and invade gradually.
template<class Solver>
void UnsaturatedEngine::invadeSingleCell1(Cell_handle cell, double pressure, Solver& flow)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().isAirReservoir == true) continue;
        if (cell->neighbor(facet)->info().isWaterReservoir == false) continue;
        if (cell->neighbor(facet)->info().Pcondition) continue;
        double nCellP = surfaceTension/cell->info().poreRadius[facet];
        if (pressure > nCellP) {
            Cell_handle nCell = cell->neighbor(facet);
            nCell->info().p() = pressure;
            nCell->info().isAirReservoir=true;
            nCell->info().isWaterReservoir=false;
            invadeSingleCell1(nCell, pressure, flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::invade1(Solver& flow)
{
    updateReservoir(flow);
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if(cell->info().isAirReservoir == true)
            invadeSingleCell1(cell,cell->info().p(),flow);
    }
    checkTrap(flow,bndCondValue[2]);
    Finite_cells_iterator _cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator _cell = tri.finite_cells_begin(); _cell != _cell_end; _cell++ ) {
        if( (_cell->info().isWaterReservoir) || (_cell->info().isAirReservoir) ) continue;
        _cell->info().p() = bndCondValue[2] - _cell->info().trapCapP;
    }
}

template<class Solver>//check trapped phase, define trapCapP.
void UnsaturatedEngine::checkTrap(Solver& flow, double pressure)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
      if( (cell->info().isWaterReservoir) || (cell->info().isAirReservoir) ) continue;
      if(cell->info().trapCapP!=0) continue;
      cell->info().trapCapP=pressure;
    }
}

template<class Solver>
Real UnsaturatedEngine::getMinEntryValue1(Solver& flow )
{
    updateReservoir(flow);
    Real nextEntry = 1e50;
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().isAirReservoir == true) {
            for (int facet=0; facet<4; facet ++) {
                if (tri.is_infinite(cell->neighbor(facet))) continue;
                if ( (cell->neighbor(facet)->info().isAirReservoir == true) || (cell->neighbor(facet)->info().isWaterReservoir == false) ) continue;
                if (cell->neighbor(facet)->info().Pcondition) continue;
                double n_cell_pe = surfaceTension/cell->info().poreRadius[facet];
                nextEntry = min(nextEntry,n_cell_pe);
            }
        }
    }
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;
    }
    else {
        return nextEntry;
    }
}

template<class Solver>
void UnsaturatedEngine::updatePressure(Solver& flow)
{
    BoundaryConditions(flow);
    flow->pressureChanged=true;
    flow->reApplyBoundaryConditions();
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
      if (cell->info().isAirReservoir==true) cell->info().p()=bndCondValue[2];
      if (cell->info().isWaterReservoir==true) cell->info().p()=bndCondValue[3];
    } 
}

template<class Solver>//update reservoir attr and pressure
void UnsaturatedEngine::updateReservoir(Solver& flow)
{
    updatePressure(flow);
    updateAirReservoir(flow);
    updateWaterReservoir(flow);  
}

template<class Solver>
void UnsaturatedEngine::updateWaterReservoir(Solver& flow)
{
    initWaterReservoirBound(flow);
    for (int bound=0; bound<6; bound++) {
        if (flow->boundingCells[bound].size()==0) continue;
        vector<Cell_handle>::iterator it = flow->boundingCells[bound].begin();
        for ( it ; it != flow->boundingCells[bound].end(); it++) {
            if ((*it)->info().index == 0) continue;
            waterReservoirRecursion((*it),flow);
        }
    }
}
template<class Solver>//the boundingCells[3] should always connect water reservoir && isWaterReservoir=true
void UnsaturatedEngine::initWaterReservoirBound(Solver& flow/*, int boundN*/)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        cell->info().isWaterReservoir = false;
    }
    for (int bound=0; bound<6; bound++) {
        if (flow->boundingCells[bound].size()==0) continue;
        vector<Cell_handle>::iterator it = flow->boundingCells[bound].begin();
        for ( it ; it != flow->boundingCells[bound].end(); it++) {
            if ((*it)->info().index == 0) continue;
            if((*it)->info().p() == bndCondValue[3])
                (*it)->info().isWaterReservoir = true;
        }
    }
}
template<class Solver>
void UnsaturatedEngine::waterReservoirRecursion(Cell_handle cell, Solver& flow)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() != bndCondValue[3]) continue;
        if (cell->neighbor(facet)->info().isWaterReservoir==true) continue;
        Cell_handle n_cell = cell->neighbor(facet);
        n_cell->info().isWaterReservoir = true;
        waterReservoirRecursion(n_cell,flow);
    }
}

template<class Solver>
void UnsaturatedEngine::updateAirReservoir(Solver& flow)
{
    initAirReservoirBound(flow);
    for (int bound=0; bound<6; bound++) {
        if (flow->boundingCells[bound].size()==0) continue;
        vector<Cell_handle>::iterator it = flow->boundingCells[bound].begin();
        for ( it ; it != flow->boundingCells[bound].end(); it++) {
            if ((*it)->info().index == 0) continue;
            airReservoirRecursion((*it),flow);
        }
    }
}
template<class Solver>//the boundingCells[2] should always connect air reservoir && isAirReservoir=true
void UnsaturatedEngine::initAirReservoirBound(Solver& flow)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        cell->info().isAirReservoir = false;
    }
    for (int bound=0; bound<6; bound++) {
        if (flow->boundingCells[bound].size()==0) continue;
        vector<Cell_handle>::iterator it = flow->boundingCells[bound].begin();
        for ( it ; it != flow->boundingCells[bound].end(); it++) {
            if((*it)->info().index == 0) continue;
            if((*it)->info().p() == bndCondValue[2])
                (*it)->info().isAirReservoir = true;
        }
    }
}
template<class Solver>
void UnsaturatedEngine::airReservoirRecursion(Cell_handle cell, Solver& flow)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() != bndCondValue[2]) continue;
        if (cell->neighbor(facet)->info().isAirReservoir == true) continue;
        Cell_handle n_cell = cell->neighbor(facet);
        n_cell->info().isAirReservoir = true;
        airReservoirRecursion(n_cell,flow);
    }
}

template<class Solver>
Real UnsaturatedEngine::getSaturation1 (Solver& flow )
{
    updateReservoir(flow);
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Real capillary_volume = 0.0; //total capillary volume
    Real air_volume = 0.0; 	//air volume
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().Pcondition) continue;//when calculating saturation, exclude boundary cells?(chao)
// 	    if (cell.has_vertex() )
        capillary_volume = capillary_volume + cell->info().capillaryCellVolume;
        if (cell->info().isAirReservoir==true) {
            air_volume = air_volume + cell->info().capillaryCellVolume;
        }
    }/*cerr<<"air_volume:"<<air_volume<<"  capillary_volume:"<<capillary_volume<<endl;*/
    Real saturation = 1 - air_volume/capillary_volume;
    return saturation;
}

///invade mode 2. Consider no trapped phase.
template<class Solver>
void UnsaturatedEngine::invadeSingleCell2(Cell_handle cell, double pressure, Solver& flow)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().Pcondition) continue;
        if (cell->neighbor(facet)->info().p()!=0) continue;
        double nCellP = surfaceTension/cell->info().poreRadius[facet];
        if (pressure > nCellP) {
            Cell_handle nCell = cell->neighbor(facet);
            nCell->info().p() = pressure;
            invadeSingleCell2(nCell, pressure, flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::invade2(Solver& flow )
{
    updatePressure2(flow);
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().p()!=0) {
            invadeSingleCell2(cell,cell->info().p(),flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::updatePressure2(Solver& flow)
{
    BoundaryConditions(flow);
    flow->pressureChanged=true;
    flow->reApplyBoundaryConditions();
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
      if (cell->info().p()!=0) cell->info().p()=bndCondValue[2];
    }   
}

template<class Solver>
Real UnsaturatedEngine::getMinEntryValue2(Solver& flow )
{
    Real nextEntry = 1e50;
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().p()!=0) {
            for (int facet=0; facet<4; facet ++) {
                if (tri.is_infinite(cell->neighbor(facet))) continue;
                if (cell->neighbor(facet)->info().Pcondition) continue;
                if (cell->neighbor(facet)->info().p()!=0) continue;
                if (cell->neighbor(facet)->info().p()==0) {
                    double n_cell_pe = surfaceTension/cell->info().poreRadius[facet];
                    nextEntry = min(nextEntry,n_cell_pe);
                }
            }
        }
    }
    if (nextEntry==1e50) {
        cout << "please set initial air pressure for the cell !" << endl;
    }
    else {
        return nextEntry;
    }
}

template<class Solver>
Real UnsaturatedEngine::getSaturation2(Solver& flow )
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Real capillary_volume = 0.0;
    Real water_volume = 0.0;
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().Pcondition) continue;//when calculating saturation, exclude boundary cells?(chao)
// 	    if (cell.has_vertex() )
        capillary_volume = capillary_volume + cell->info().capillaryCellVolume;
        if (cell->info().p()==0) {
            water_volume = water_volume + cell->info().capillaryCellVolume;
        }
    }
    Real saturation = water_volume/capillary_volume;
    return saturation;
}

template<class Cellhandle>
double UnsaturatedEngine::computeEffPoreRadius(Cellhandle cell, int j)
{
    double rInscribe = abs(solver->Compute_EffectiveRadius(cell, j));  
    Cell_handle cellh = Cell_handle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
  switch (facetNFictious) {
    case (0) : { return computeEffPoreRadiusNormal(cell,j); }; break;
    case (1) : { return rInscribe; }; break;
    case (2) : { return rInscribe; }; break;    
  }   
}
template<class Cellhandle>//seperate rmin=getMinPoreRadius(cell,j) later;
double UnsaturatedEngine::computeEffPoreRadiusNormal(Cellhandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r2(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r2(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r2(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }

    double rmin = max(rAB,max(rBC,rAC)); if (rmin==0) { rmin= 1.0e-10; }
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));//rmin>rmax ?
    if(rmin>rmax) { cerr<<"rmin>rmax. rmin="<<rmin<<" ,rmax="<<rmax<<endl; }
    
    double deltaForce_rMin = computeDeltaForce(cell,j,rmin);
    double deltaForce_rMax = computeDeltaForce(cell,j,rmax);
    if(deltaForce_rMax<0) {
        double EffPoreRadius = rmax;
        cerr<<"deltaForce_rMax Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;
        return EffPoreRadius;
    }
    else if(deltaForce_rMin<0) {
        double effPoreRadius = bisection(cell,j,rmin,rmax);// cerr<<"1";//we suppose most cases should be this.
        return effPoreRadius;
    }
    else if( (deltaForce_rMin>0) && (deltaForce_rMin<deltaForce_rMax) ) {
        double EffPoreRadius = rmin;// cerr<<"2";
        return EffPoreRadius;
    }
    else if(deltaForce_rMin>deltaForce_rMax) {
        double EffPoreRadius = rmax;
        cerr<<"deltaForce_rMin>deltaForce_rMax. cellID: "<<cell->info().index<<"; deltaForce_rMin="<<deltaForce_rMin<<"; deltaForce_rMax="<<deltaForce_rMax<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;
        return EffPoreRadius;
    }
}

template<class Cellhandle>
double UnsaturatedEngine::bisection(Cellhandle cell, int j, double a, double b)
{
    double m = 0.5*(a+b);
    if (abs(b-a)>abs((solver->Compute_EffectiveRadius(cell, j)*1.0e-6))) {
        if ( computeDeltaForce(cell,j,m) * computeDeltaForce(cell,j,a) < 0 ) {
            b = m;
            return bisection(cell,j,a,b);
        }
        else {
            a = m;
            return bisection(cell,j,a,b);
        }
    }
    else return m;
}

template<class Cellhandle>
double UnsaturatedEngine::computeDeltaForce(Cellhandle cell,int j, double rcap)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r2(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r2(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r2(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();    
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }

    //In triangulation ArB,rcap is the radius of sphere r; 
    double _AB = (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB); if(_AB>1.0) {_AB=1.0;} if(_AB<-1.0) {_AB=-1.0;}
    double alphaAB = acos(_AB);
    double _BA = (pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2))/(2*(rB+rcap)*AB); if(_BA>1.0) {_BA=1.0;} if(_BA<-1.0) {_BA=-1.0;}
    double alphaBA = acos(_BA);
    double _ArB = (pow((rA+rcap),2)+pow((rB+rcap),2)-pow(AB,2))/(2*(rA+rcap)*(rB+rcap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
    double alphaArB = acos(_ArB);
    
    double length_liquidAB = alphaArB*rcap;
    double AreaArB = 0.5*(rA+rcap)*(rB+rcap)*sin(alphaArB);
    double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rcap,2);

    double _AC = (pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2))/(2*(rA+rcap)*AC); if(_AC>1.0) {_AC=1.0;} if(_AC<-1.0) {_AC=-1.0;}
    double alphaAC = acos(_AC);
    double _CA = (pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2))/(2*(rC+rcap)*AC); if(_CA>1.0) {_CA=1.0;} if(_CA<-1.0) {_CA=-1.0;}
    double alphaCA = acos(_CA);
    double _ArC = (pow((rA+rcap),2)+pow((rC+rcap),2)-pow(AC,2))/(2*(rA+rcap)*(rC+rcap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
    double alphaArC = acos(_ArC);

    double length_liquidAC = alphaArC*rcap;
    double AreaArC = 0.5*(rA+rcap)*(rC+rcap)*sin(alphaArC);
    double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rcap,2);

    double _BC = (pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2))/(2*(rB+rcap)*BC); if(_BC>1.0) {_BC=1.0;} if(_BC<-1.0) {_BC=-1.0;}
    double alphaBC = acos(_BC);
    double _CB = (pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2))/(2*(rC+rcap)*BC); if(_CB>1.0) {_CB=1.0;} if(_CB<-1.0) {_CB=-1.0;}
    double alphaCB = acos(_CB);
    double _BrC = (pow((rB+rcap),2)+pow((rC+rcap),2)-pow(BC,2))/(2*(rB+rcap)*(rC+rcap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
    double alphaBrC = acos(_BrC);

    double length_liquidBC = alphaBrC*rcap;
    double AreaBrC = 0.5*(rB+rcap)*(rC+rcap)*sin(alphaBrC);
    double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rcap,2);

    double areaCap = sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double areaPore = areaCap - Area_liquidAB - Area_liquidAC - Area_liquidBC;
    
    //FIXME:rethink here,areaPore Negative, Flat facets, do nothing ?
    if(areaPore<0) {cerr<<"areaPore Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;}
    double perimeterPore = length_liquidAB + length_liquidAC + length_liquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC;
    if(perimeterPore<0) {cerr<<"perimeterPore Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;}

    double deltaForce = perimeterPore - areaPore/rcap;//deltaForce=surfaceTension*(perimeterPore - areaPore/rcap)
    return deltaForce;
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

	for (int k=0;k<6;k++)	{
		flow->boundary (wallIds[k]).flowCondition=!bndCondIsPressure[k];
                flow->boundary (wallIds[k]).value=bndCondValue[k];
                flow->boundary (wallIds[k]).velocity = boundaryVelocity[k];//FIXME: needs correct implementation, maybe update the cached pos/vel?
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
        flow->y_min_id=wallIds[ymin];
        flow->y_max_id=wallIds[ymax];
        flow->x_max_id=wallIds[xmax];
        flow->x_min_id=wallIds[xmin];
        flow->z_min_id=wallIds[zmin];
        flow->z_max_id=wallIds[zmax];

        //FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
        flow->boundsIds[0]= &flow->x_min_id;
        flow->boundsIds[1]= &flow->x_max_id;
        flow->boundsIds[2]= &flow->y_min_id;
        flow->boundsIds[3]= &flow->y_max_id;
        flow->boundsIds[4]= &flow->z_min_id;
        flow->boundsIds[5]= &flow->z_max_id;

	for (int k=0;k<6;k++) flow->boundary ( *flow->boundsIds[k] ).useMaxMin = boundaryUseMaxMin[k];

        flow->Corner_min = CGT::Point ( flow->x_min, flow->y_min, flow->z_min );
        flow->Corner_max = CGT::Point ( flow->x_max, flow->y_max, flow->z_max );
 
        //assign BCs types and values
        BoundaryConditions ( flow );

        double center[3];
        for ( int i=0; i<6; i++ ) {
                if ( *flow->boundsIds[i]<0 ) continue;
                CGT::Vecteur Normal ( normal[i].x(), normal[i].y(), normal[i].z() );
                if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->AddBoundingPlane(Normal, *flow->boundsIds[i] );
                else {
			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
// 			cerr << "id="<<*flow->boundsIds[i] <<" center="<<center[0]<<","<<center[1]<<","<<center[2]<<endl;
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

template<class Solver>
void UnsaturatedEngine::initializeCellIndex(Solver& flow)
{
    int k=0;
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        cell->info().index=k++;
    }
}

template<class Solver>
void UnsaturatedEngine::initializePoreRadius(Solver& flow)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    Cell_handle neighbour_cell;
    for (Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++) {
        for (int j=0; j<4; j++) {
            neighbour_cell = cell->neighbor(j);
            if (!tri.is_infinite(neighbour_cell)) {
                cell->info().poreRadius[j]=computeEffPoreRadius(cell, j);
                neighbour_cell->info().poreRadius[tri.mirror_index(cell, j)]= cell->info().poreRadius[j];
            }
        }
    }
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
	Real volume = inv6 * ((p1-p0).cross(p2-p0)).dot(p3-p0);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}

template<class Solver>
void UnsaturatedEngine::updateVolumeCapillaryCell ( Solver& flow)
{
    Initialize_volumes(flow);
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    Cell_handle neighbour_cell;
    for (Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++) {
        cell->info().capillaryCellVolume = abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
//         if (cell->info().capillaryCellVolume<0) {cerr<<"volumeCapillaryCell Negative. cell ID: " << cell->info().index << "cell volume: " << cell->info().volume() << "  volumeSolidPore: " << solver->volumeSolidPore(cell) << endl;        }
    }
}

template<class Solver>
void UnsaturatedEngine::saveListNodes(Solver& flow)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    ofstream file;
    file.open("ListOfNodes.txt");
    file << "#List Of Nodes. For one cell,there are four neighbour cells \n";
    file << "Cell_ID" << " " << "NeighborCell_ID" << endl;
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << cell->neighbor(1)->info().index << " " << cell->neighbor(2)->info().index << " " << cell->neighbor(3)->info().index << endl;
    }
    file.close();
}

template<class Solver>
void UnsaturatedEngine::saveListConnection(Solver& flow)
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    ofstream file;
    file.open("ListConnection.txt");
    file << "#List of Connections \n";
    file << "Cell_ID" << " " << "NeighborCell_ID" << " " << "EntryValue" << " " << "poreRadius" << " " << "poreArea" << " " << "porePerimeter" << endl;
    double surface_tension = surfaceTension ; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << surface_tension/cell->info().poreRadius[0] << " " << cell->info().poreRadius[0] << " " << computePoreArea(cell,0) << " " << computePorePerimeter(cell,0) << endl;
        file << cell->info().index << " " <<cell->neighbor(1)->info().index << " " << surface_tension/cell->info().poreRadius[1] << " " << cell->info().poreRadius[1] << " " << computePoreArea(cell,1) << " " << computePorePerimeter(cell,1) << endl;
        file << cell->info().index << " " <<cell->neighbor(2)->info().index << " " << surface_tension/cell->info().poreRadius[2] << " " << cell->info().poreRadius[2] << " " << computePoreArea(cell,2) << " " << computePorePerimeter(cell,2) << endl;
        file << cell->info().index << " " <<cell->neighbor(3)->info().index << " " << surface_tension/cell->info().poreRadius[3] << " " << cell->info().poreRadius[3] << " " << computePoreArea(cell,3) << " " << computePorePerimeter(cell,3) << endl;
    }
    file.close();
}

template<class Solver>
void UnsaturatedEngine::saveLatticeNodeX(Solver& flow, double x)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    if((x<flow->x_min)||(x>flow->x_max)) {
        cerr<<"x is out of range! "<<"pleas set x between "<<flow->x_min<<" and "<<flow->x_max<<endl;
    }
    else {
        int N=100;// the default Node number for each slice is 100X100
        ofstream file;
        std::ostringstream fileNameStream(".txt");
        fileNameStream << "LatticeNodeX_"<< x;
        std::string fileName = fileNameStream.str();
        file.open(fileName.c_str());
//     file << "#Slice Of LatticeNodes: 0: out of sphere; 1: inside of sphere  \n";
        Real delta_y = (flow->y_max-flow->y_min)/N;
        Real delta_z = (flow->z_max-flow->z_min)/N;
        for (int j=0; j<N+1; j++) {
            for (int k=0; k<N+1; k++) {
                double y=flow->y_min+j*delta_y;
                double z=flow->z_min+k*delta_z;
                int M=0;
                Vector3r LatticeNode = Vector3r(x,y,z);
                for (Finite_vertices_iterator V_it = tri.finite_vertices_begin(); V_it != tri.finite_vertices_end(); V_it++) {
                    if(V_it->info().isFictious) continue;
                    Vector3r SphereCenter = makeVector3r2(V_it->point().point());
                    if ((LatticeNode-SphereCenter).squaredNorm() < V_it->point().weight()) {
                        M=1;
                        break;
                    }
                }
                file << M;
            }
            file << "\n";
        }
        file.close();
    }
}

template<class Solver>
void UnsaturatedEngine::saveLatticeNodeY(Solver& flow, double y)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    if((y<flow->y_min)||(y>flow->y_max)) {
        cerr<<"y is out of range! "<<"pleas set y between "<<flow->y_min<<" and "<<flow->y_max<<endl;
    }
    else {
        int N=100;// the default Node number for each slice is 100X100
        ofstream file;
        std::ostringstream fileNameStream(".txt");
        fileNameStream << "LatticeNodeY_"<< y;
        std::string fileName = fileNameStream.str();
        file.open(fileName.c_str());
//     file << "#Slice Of LatticeNodes: 0: out of sphere; 1: inside of sphere  \n";
        Real delta_x = (flow->x_max-flow->x_min)/N;
        Real delta_z = (flow->z_max-flow->z_min)/N;
        for (int j=0; j<N+1; j++) {
            for (int k=0; k<N+1; k++) {
                double x=flow->x_min+j*delta_x;
                double z=flow->z_min+k*delta_z;
                int M=0;
                Vector3r LatticeNode = Vector3r(x,y,z);
                for (Finite_vertices_iterator V_it = tri.finite_vertices_begin(); V_it != tri.finite_vertices_end(); V_it++) {
                    if(V_it->info().isFictious) continue;
                    Vector3r SphereCenter = makeVector3r2(V_it->point().point());
                    if ((LatticeNode-SphereCenter).squaredNorm() < V_it->point().weight()) {
                        M=1;
                        break;
                    }
                }
                file << M;
            }
            file << "\n";
        }
        file.close();
    }
}

template<class Solver>
void UnsaturatedEngine::saveLatticeNodeZ(Solver& flow, double z)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    if((z<flow->z_min)||(z>flow->z_max)) {
        cerr<<"z is out of range! "<<"pleas set z between "<<flow->z_min<<" and "<<flow->z_max<<endl;
    }
    else {
        int N=100;// the default Node number for each slice is 100X100
        ofstream file;
        std::ostringstream fileNameStream(".txt");
        fileNameStream << "LatticeNodeZ_"<< z;
        std::string fileName = fileNameStream.str();
        file.open(fileName.c_str());
//     file << "#Slice Of LatticeNodes: 0: out of sphere; 1: inside of sphere  \n";
        Real delta_x = (flow->x_max-flow->x_min)/N;
        Real delta_y = (flow->y_max-flow->y_min)/N;
        for (int j=0; j<N+1; j++) {
            for (int k=0; k<N+1; k++) {
                double x=flow->x_min+j*delta_x;
                double y=flow->z_min+k*delta_y;
                int M=0;
                Vector3r LatticeNode = Vector3r(x,y,z);
                for (Finite_vertices_iterator V_it = tri.finite_vertices_begin(); V_it != tri.finite_vertices_end(); V_it++) {
                    if(V_it->info().isFictious) continue;
                    Vector3r SphereCenter = makeVector3r2(V_it->point().point());
                    if ((LatticeNode-SphereCenter).squaredNorm() < V_it->point().weight()) {
                        M=1;
                        break;
                    }
                }
                file << M;
            }
            file << "\n";
        }
        file.close();
    }
}

template<class Solver>
void UnsaturatedEngine::saveListAdjCellsTopBound(Solver& flow)
{
    /*Here,boundsIds according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
    For drainage and imbibition situations, we only care about top(3) and bottom(2) boundaries.(0-xmin,1-xmax,2-ymin,3-ymax,4-zmin,5-zmax)
    And in python script, don't forget set: bndCondIsPressure=[0,0,1,1,0,0], */
    if(flow->boundingCells[3].size()==0) {
        cerr << "please set bndCondIsPressure=[0,0,1,1,0,0]." << endl;
    }
    else {
        vector<Cell_handle>::iterator it = flow->boundingCells[3].begin();
        ofstream file;
        file.open("ListAdjacentCellsTopBoundary.txt");
        file << "#Check the boundingCells[3] statement of last step. boundingCells[3] connecting water reservoir(ymax), shoule be isWaterReservoir=0 in the last step. \n";
	file << "Cell_ID"<<"	Cell_Pressure"<<"	isWaterReservoir"<<endl;
        for ( it ; it != flow->boundingCells[3].end(); it++) {
            if ((*it)->info().index == 0) continue;
            file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isWaterReservoir<< endl;
        }
        file.close();
    }
}

template<class Solver>
void UnsaturatedEngine::saveListAdjCellsBottomBound(Solver& flow)
{
    if(flow->boundingCells[2].size()==0) {
        cerr << "please set bndCondIsPressure=[0,0,1,1,0,0]."<< endl;
    }
    else {
        vector<Cell_handle>::iterator it = flow->boundingCells[2].begin();
        ofstream file;
        file.open("ListAdjacentCellsBottomBoundary.txt");
        file << "#Checking the boundingCells[2] statement of last step. boundingCells[2] connecting air reservoir(ymin), should be isAirReservoir=1 in the last step.\n";
	file << "Cell_ID"<<"	Cell_Pressure"<<"	isAirReservoir"<<endl;
        for ( it ; it != flow->boundingCells[2].end(); it++) {
            if ((*it)->info().index == 0) continue;
            file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isAirReservoir<<endl;
        }
        file.close();
    }
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

//----tempt function for Vahid Joekar-Niasar's data----
template<class Cellhandle >
double UnsaturatedEngine::getRadiusMin(Cellhandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r2(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r2(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r2(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }  

    double rmin = max(rAB,max(rBC,rAC)); if (rmin==0) { rmin= 1.0e-10; }
    return rmin;
}
template<class Solver>
void UnsaturatedEngine::debugTemp(Solver& flow)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("bugTemp.txt");
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
	file << cell->info().index << "  " <<cell->info().poreRadius[0]<<" "<<getRadiusMin(cell,0)<<" "<<abs(flow->Compute_EffectiveRadius(cell, 0))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[1]<<" "<<getRadiusMin(cell,1)<<" "<<abs(flow->Compute_EffectiveRadius(cell, 1))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[2]<<" "<<getRadiusMin(cell,2)<<" "<<abs(flow->Compute_EffectiveRadius(cell, 2))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[3]<<" "<<getRadiusMin(cell,3)<<" "<<abs(flow->Compute_EffectiveRadius(cell, 3))<<endl;
    }
    file.close();
}

template<class Cellhandle>
Real UnsaturatedEngine::computePoreArea(Cellhandle cell, int j)
{
    double rInscribe = abs(solver->Compute_EffectiveRadius(cell, j));  
    Cell_handle cellh = Cell_handle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
  switch (facetNFictious) {
    case (0) : {
        RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
        if (tri.is_infinite(cell->neighbor(j))) return 0;

        Vector3r posA = makeVector3r2(cell->vertex(facetVertices[j][0])->point().point());
        Vector3r posB = makeVector3r2(cell->vertex(facetVertices[j][1])->point().point());
        Vector3r posC = makeVector3r2(cell->vertex(facetVertices[j][2])->point().point());
        double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
        double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
        double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
        double AB = (posA-posB).norm();
        double AC = (posA-posC).norm();
        double BC = (posB-posC).norm();
        double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
        double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
        double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
        double rAB = 0.5*(AB-rA-rB); if (rAB<0) {rAB=0;}
        double rBC = 0.5*(BC-rB-rC); if (rBC<0) {rBC=0;}
        double rAC = 0.5*(AC-rA-rC); if (rAC<0) {rAC=0;}
        
	double rcap = cell->info().poreRadius[j];
        //In triangulation ArB,rcap is the radius of sphere r;
        double _AB = (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB); if(_AB>1.0) {_AB=1.0;} if(_AB<-1.0) {_AB=-1.0;}
        double alphaAB = acos(_AB);
        double _BA = (pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2))/(2*(rB+rcap)*AB); if(_BA>1.0) {_BA=1.0;} if(_BA<-1.0) {_BA=-1.0;}
        double alphaBA = acos(_BA);
        double _ArB = (pow((rA+rcap),2)+pow((rB+rcap),2)-pow(AB,2))/(2*(rA+rcap)*(rB+rcap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
        double alphaArB = acos(_ArB);
//     double D1=alphaAB + alphaBA + alphaArB; cerr<<D<<" ";
        double length_liquidAB = alphaArB*rcap;
        double AreaArB = 0.5*(rA+rcap)*(rB+rcap)*sin(alphaArB);
        double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rcap,2);

        double _AC = (pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2))/(2*(rA+rcap)*AC); if(_AC>1.0) {_AC=1.0;} if(_AC<-1.0) {_AC=-1.0;}
        double alphaAC = acos(_AC);
        double _CA = (pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2))/(2*(rC+rcap)*AC); if(_CA>1.0) {_CA=1.0;} if(_CA<-1.0) {_CA=-1.0;}
        double alphaCA = acos(_CA);
        double _ArC = (pow((rA+rcap),2)+pow((rC+rcap),2)-pow(AC,2))/(2*(rA+rcap)*(rC+rcap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
        double alphaArC = acos(_ArC);
//     double D2=alphaAC + alphaCA + alphaArC; cerr<<D<<" ";
        double length_liquidAC = alphaArC*rcap;
        double AreaArC = 0.5*(rA+rcap)*(rC+rcap)*sin(alphaArC);
        double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rcap,2);

        double _BC = (pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2))/(2*(rB+rcap)*BC); if(_BC>1.0) {_BC=1.0;} if(_BC<-1.0) {_BC=-1.0;}
        double alphaBC = acos(_BC);
        double _CB = (pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2))/(2*(rC+rcap)*BC); if(_CB>1.0) {_CB=1.0;} if(_CB<-1.0) {_CB=-1.0;}
        double alphaCB = acos(_CB);
        double _BrC = (pow((rB+rcap),2)+pow((rC+rcap),2)-pow(BC,2))/(2*(rB+rcap)*(rC+rcap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
        double alphaBrC = acos(_BrC);
//     double D3=alphaBC + alphaCB + alphaBrC; cerr<<D<<" ";
        double length_liquidBC = alphaBrC*rcap;
        double AreaBrC = 0.5*(rB+rcap)*(rC+rcap)*sin(alphaBrC);
        double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rcap,2);

        double areaCap = sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
        double areaPore = areaCap - Area_liquidAB - Area_liquidAC - Area_liquidBC;
	if(areaPore<0) {cerr<<"areaPore Negative.  cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;
	areaPore=Mathr::PI*pow(rInscribe,2);}
	return areaPore;
    }; break;
    case (1) : { return Mathr::PI*pow(rInscribe,2); }; break;
    case (2) : { return Mathr::PI*pow(rInscribe,2); }; break;    
  }   
}

template<class Cellhandle>
Real UnsaturatedEngine::computePorePerimeter(Cellhandle cell, int j)
{
    double rInscribe = abs(solver->Compute_EffectiveRadius(cell, j));  
    Cell_handle cellh = Cell_handle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
  switch (facetNFictious) {
    case (0) : {
        RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
        if (tri.is_infinite(cell->neighbor(j))) return 0;

        Vector3r posA = makeVector3r2(cell->vertex(facetVertices[j][0])->point().point());
        Vector3r posB = makeVector3r2(cell->vertex(facetVertices[j][1])->point().point());
        Vector3r posC = makeVector3r2(cell->vertex(facetVertices[j][2])->point().point());
        double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
        double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
        double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
        double AB = (posA-posB).norm();
        double AC = (posA-posC).norm();
        double BC = (posB-posC).norm();
        double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
        double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
        double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
        double rAB = 0.5*(AB-rA-rB); if (rAB<0) {rAB=0;}
        double rBC = 0.5*(BC-rB-rC); if (rBC<0) {rBC=0;}
        double rAC = 0.5*(AC-rA-rC); if (rAC<0) {rAC=0;}

        double rcap = cell->info().poreRadius[j];
        //In triangulation ArB,rcap is the radius of sphere r;
        double _AB = (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB); if(_AB>1.0) {_AB=1.0;} if(_AB<-1.0) {_AB=-1.0;}
        double alphaAB = acos(_AB);
        double _BA = (pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2))/(2*(rB+rcap)*AB); if(_BA>1.0) {_BA=1.0;} if(_BA<-1.0) {_BA=-1.0;}
        double alphaBA = acos(_BA);
        double _ArB = (pow((rA+rcap),2)+pow((rB+rcap),2)-pow(AB,2))/(2*(rA+rcap)*(rB+rcap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
        double alphaArB = acos(_ArB);
//     double D1=alphaAB + alphaBA + alphaArB; cerr<<D<<" ";
        double length_liquidAB = alphaArB*rcap;
        double AreaArB = 0.5*(rA+rcap)*(rB+rcap)*sin(alphaArB);
        double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rcap,2);

        double _AC = (pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2))/(2*(rA+rcap)*AC); if(_AC>1.0) {_AC=1.0;} if(_AC<-1.0) {_AC=-1.0;}
        double alphaAC = acos(_AC);
        double _CA = (pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2))/(2*(rC+rcap)*AC); if(_CA>1.0) {_CA=1.0;} if(_CA<-1.0) {_CA=-1.0;}
        double alphaCA = acos(_CA);
        double _ArC = (pow((rA+rcap),2)+pow((rC+rcap),2)-pow(AC,2))/(2*(rA+rcap)*(rC+rcap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
        double alphaArC = acos(_ArC);
//     double D2=alphaAC + alphaCA + alphaArC; cerr<<D<<" ";
        double length_liquidAC = alphaArC*rcap;
        double AreaArC = 0.5*(rA+rcap)*(rC+rcap)*sin(alphaArC);
        double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rcap,2);

        double _BC = (pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2))/(2*(rB+rcap)*BC); if(_BC>1.0) {_BC=1.0;} if(_BC<-1.0) {_BC=-1.0;}
        double alphaBC = acos(_BC);
        double _CB = (pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2))/(2*(rC+rcap)*BC); if(_CB>1.0) {_CB=1.0;} if(_CB<-1.0) {_CB=-1.0;}
        double alphaCB = acos(_CB);
        double _BrC = (pow((rB+rcap),2)+pow((rC+rcap),2)-pow(BC,2))/(2*(rB+rcap)*(rC+rcap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
        double alphaBrC = acos(_BrC);
//     double D3=alphaBC + alphaCB + alphaBrC; cerr<<D<<" ";
        double length_liquidBC = alphaBrC*rcap;
        double AreaBrC = 0.5*(rB+rcap)*(rC+rcap)*sin(alphaBrC);
        double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rcap,2);
	double perimeterPore = length_liquidAB + length_liquidAC + length_liquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC;
	if(perimeterPore<0) {cerr<<"perimeterPore Negative. perimeterPore="<<perimeterPore<<endl;perimeterPore=2.0*Mathr::PI*rInscribe;}
	return perimeterPore;
    }; break;
    case (1) : { return 2.0*Mathr::PI*rInscribe; }; break;
    case (2) : { return 2.0*Mathr::PI*rInscribe; }; break;        
  }   
}

template<class Solver>
void UnsaturatedEngine::savePoreBodyInfo(Solver& flow)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("PoreBodyInfo.txt");
    file << "#pore bodies position (or Voronoi centers) and size (volume) \n";
    file << "Cell_ID " << " x " << " y " << " z " << " volume "<< endl;
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        file << cell->info().index << " " << cell->info() << " " << cell->info().capillaryCellVolume << endl;
    }
    file.close();
}

template<class Solver>
void UnsaturatedEngine::savePoreThroatInfo(Solver& flow)
{
    RTriangulation& tri = flow->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("PoreThroatInfo.txt");
    file << "#pore throat area, inscribeRadius and perimeter. \n";
    file << "Cell_ID " << " NeighborCell_ID "<<" area " << " inscribeRadius " << " perimeter " << endl;
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
	file << cell->info().index <<" "<< cell->neighbor(0)->info().index <<" "<< computePoreArea(cell, 0) <<" "<< abs(flow->Compute_EffectiveRadius(cell, 0)) <<" "<< computePorePerimeter(cell,0) <<endl;  
	file << cell->info().index <<" "<< cell->neighbor(1)->info().index <<" "<< computePoreArea(cell, 1) <<" "<< abs(flow->Compute_EffectiveRadius(cell, 1)) <<" "<< computePorePerimeter(cell,1) <<endl;  
	file << cell->info().index <<" "<< cell->neighbor(2)->info().index <<" "<< computePoreArea(cell, 2) <<" "<< abs(flow->Compute_EffectiveRadius(cell, 2)) <<" "<< computePorePerimeter(cell,2) <<endl;  
	file << cell->info().index <<" "<< cell->neighbor(3)->info().index <<" "<< computePoreArea(cell, 3) <<" "<< abs(flow->Compute_EffectiveRadius(cell, 3)) <<" "<< computePorePerimeter(cell,3) <<endl;  
    }
    file.close();  
}
//----------end tempt function for Vahid Joekar-Niasar's data (clear later)---------------------

template <class Solver> 
void UnsaturatedEngine::computeFacetPoreForcesWithCache(Solver& flow, bool onlyCache)
{
	RTriangulation& Tri = flow->T[solver->currentTes].Triangulation();
	Finite_cells_iterator cell_end = Tri.finite_cells_end();
	CGT::Vecteur nullVect(0,0,0);
	//reset forces
	if (!onlyCache) for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) v->info().forces=nullVect;
	
	#ifdef parallel_forces
	if (solver->noCache) {
		solver->perVertexUnitForce.clear(); solver->perVertexPressure.clear();
// 		vector<const Vecteur*> exf; exf.reserve(20);
// 		vector<const Real*> exp; exp.reserve(20);
		solver->perVertexUnitForce.resize(Tri.number_of_vertices());
		solver->perVertexPressure.resize(Tri.number_of_vertices());}
	#endif

	Cell_handle neighbour_cell;
	Vertex_handle mirror_vertex;
	CGT::Vecteur tempVect;
	//FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
	if (solver->noCache) {for (VCell_iterator cell_it=flow->T[currentTes].cellHandles.begin(); cell_it!=flow->T[currentTes].cellHandles.end(); cell_it++){
// 	if (noCache) for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			Cell_handle& cell = *cell_it;
			//reset cache
			for (int k=0;k<4;k++) cell->info().unitForceVectors[k]=nullVect;
			for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
					neighbour_cell = cell->neighbor(j);
					const CGT::Vecteur& Surfk = cell->info().facetSurfaces[j];
					//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
					//The ratio void surface / facet surface
					Real area = sqrt(Surfk.squared_length()); if (area<=0) cerr <<"AREA <= 0!!"<<endl;
					CGT::Vecteur facetNormal = Surfk/area;
					const std::vector<CGT::Vecteur>& crossSections = cell->info().facetSphereCrossSections;
					CGT::Vecteur fluidSurfk = cell->info().facetSurfaces[j]*cell->info().facetFluidSurfacesRatio[j];
					/// handle fictious vertex since we can get the projected surface easily here
					if (cell->vertex(j)->info().isFictious) {
						Real projSurf=abs(Surfk[flow->boundary(cell->vertex(j)->info().id()).coordinate]);
						tempVect=-projSurf*flow->boundary(cell->vertex(j)->info().id()).normal;
						cell->vertex(j)->info().forces = cell->vertex(j)->info().forces+tempVect*cell->info().p();
						//define the cached value for later use with cache*p
						cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
					}
					/// Apply weighted forces f_k=sqRad_k/sumSqRad*f
					CGT::Vecteur Facet_Unit_Force = -fluidSurfk*cell->info().solidLine[j][3];
					CGT::Vecteur Facet_Force = cell->info().p()*Facet_Unit_Force;
										
					for (int y=0; y<3;y++) {
						cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + Facet_Force*cell->info().solidLine[j][y];
						//add to cached value
						cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]+Facet_Unit_Force*cell->info().solidLine[j][y];
						//uncomment to get total force / comment to get only pore tension forces
						if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
							cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces -facetNormal*cell->info().p()*crossSections[j][y];
							//add to cached value
							cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]-facetNormal*crossSections[j][y];
						}
					}
					#ifdef parallel_forces
					solver->perVertexUnitForce[cell->vertex(j)->info().id()].push_back(&(cell->info().unitForceVectors[j]));
					solver->perVertexPressure[cell->vertex(j)->info().id()].push_back(&(cell->info().p()));
					#endif
			}
		}
		solver->noCache=false;//cache should always be defined after execution of this function
		if (onlyCache) return;
	} else {//use cached values when triangulation doesn't change
		#ifndef parallel_forces
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			for (int yy=0;yy<4;yy++) cell->vertex(yy)->info().forces = cell->vertex(yy)->info().forces + cell->info().unitForceVectors[yy]*cell->info().p();}
			
		#else
		#pragma omp parallel for num_threads(ompThreads)
		for (int vn=0; vn<= flow->T[currentTes].max_id; vn++) {
			Vertex_handle& v = flow->T[currentTes].vertexHandles[vn];
// 		for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v){
			const int& id =  v->info().id();
			CGT::Vecteur tf (0,0,0);
			int k=0;
			for (vector<const Real*>::iterator c = solver->perVertexPressure[id].begin(); c != solver->perVertexPressure[id].end(); c++)
				tf = tf + (*(solver->perVertexUnitForce[id][k++]))*(**c);
			v->info().forces = tf;
		}
		#endif
	}
	if (flow->DEBUG_OUT) {
		CGT::Vecteur TotalForce = nullVect;
		for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)	{
			if (!v->info().isFictious) TotalForce = TotalForce + v->info().forces;
			else if (flow->boundary(v->info().id()).flowCondition==1) TotalForce = TotalForce + v->info().forces;	}
		cout << "TotalForce = "<< TotalForce << endl;}
}

template<class Solver>
void UnsaturatedEngine::computeSolidLine(Solver& flow)
{
    RTriangulation& Tri = flow->T[solver->currentTes].Triangulation();
    Finite_cells_iterator cell_end = Tri.finite_cells_end();
    for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
        for(int j=0; j<4; j++) {
            solver->Line_Solid_Pore(cell, j);
        }
    }
}

YADE_PLUGIN ( ( UnsaturatedEngine ) );

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */
