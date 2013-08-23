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

CREATE_LOGGER ( UnsaturatedEngine );

CGT::Vecteur makeCgVect2 ( const Vector3r& yv ) {return CGT::Vecteur ( yv[0],yv[1],yv[2] );}
CGT::Point makeCgPoint2 ( const Vector3r& yv ) {return CGT::Point ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r2 ( const CGT::Vecteur& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}


UnsaturatedEngine::~UnsaturatedEngine()
{
}

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

Real UnsaturatedEngine::testFunction()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& triangulation = solver->T[solver->currentTes].Triangulation();
	if (triangulation.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		Build_Triangulation(P_zero,solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initializeCellIndex(solver);//initialize cell index
		getPoreRadius(solver);//save all pore radii before invade
	}		
	solver->noCache = false;
}

void UnsaturatedEngine::action()
{    
  //This will be used later
  /*
   Build_Triangulation();initializeCellIndex();Initialize_volumes(solver);getPoreRadius();
   */
}

template<class Solver>
void UnsaturatedEngine::invadeSingleCell(Cell_handle cell, double pressure, Solver& flow)
{
    double surface_tension = surfaceTension ; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() >= pressure) continue;
        double n_cell_pe = surface_tension/cell->info().poreRadius[facet];
        if (pressure > n_cell_pe) {
            Cell_handle n_cell = cell->neighbor(facet);
            n_cell->info().p() = pressure;
            invadeSingleCell(n_cell, pressure, flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::invade (Solver& flow )
{
    BoundaryConditions(flow);
    flow->pressureChanged=true;
    flow->reApplyBoundaryConditions();

    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().p()!=0) {
            invadeSingleCell(cell,cell->info().p(),flow);
// 	    cerr << "cell pressure: " << cell->info().p(); //test whether the cell's pressure has been changed
        }
    }
}

template<class Solver>
Real UnsaturatedEngine::getMinEntryValue (Solver& flow )
{
    Real nextEntry = 1e50;
    double surface_tension = surfaceTension; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().p()!=0) {
            for (int facet=0; facet<4; facet ++) {
                if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
//              if (cell->info().Pcondition) continue;  //FIXME Add this, the boundary cell pressure will not work; Remove this the initial pressure and initial invade will be chaos.
                if (cell->neighbor(facet)->info().p()!=0) continue;
                if (cell->neighbor(facet)->info().p()==0) {
                    double n_cell_pe = surface_tension/cell->info().poreRadius[facet];
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

//invade version2. updateAirReservoir and updateWaterReservoir before invade. And invade gradually, not Suddenly jump from 0 to .p()
template<class Solver>
void UnsaturatedEngine::invadeSingleCell2(Cell_handle cell, double pressure, Solver& flow)
{
    double surface_tension = surfaceTension ;
    for (int facet = 0; facet < 4; facet ++) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() != 0) continue;
        if (cell->neighbor(facet)->info().isWaterReservoir == false) continue;
        double n_cell_pe = surface_tension/cell->info().poreRadius[facet];
        if (pressure > n_cell_pe) {
            Cell_handle n_cell = cell->neighbor(facet);
            n_cell->info().p() = pressure;
            invadeSingleCell2(n_cell, pressure, flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::invade2 (Solver& flow)
{
    updateAirReservoir(flow);
    updateWaterReservoir(flow);
    BoundaryConditions(flow);
    flow->pressureChanged=true;
    flow->reApplyBoundaryConditions();
//here, we update the pressure of cells inside (.isAirReservoir=true) equal to Pressure_BOTTOM_Boundary
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().isAirReservoir == true)
//             cell->info().p() = Pressure_BOTTOM_Boundary;//FIXME:how to change cell inside pressure to boundary condition.?
            cell->info().p() = bndCondValue[2];//FIXME: x_min_id=wallLeftId=0, x_max_id =wallRightId=1, y_min_id=wallBottomId=2, y_max_id=wallTopId=3, z_min_id=wallBackId=4,z_max_id=wallFrontId=5           
//             cerr<<"cell index: "<<cell->info().index <<" "<< "pressure: " << cell->info().p()<<endl;
    }

    Finite_cells_iterator _cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator _cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); _cell != _cell_end; _cell++ ) {
        if(_cell->info().p() != 0)
            invadeSingleCell2(_cell,_cell->info().p(),flow);
    }
}

template<class Solver>
Real UnsaturatedEngine::getMinEntryValue2 (Solver& flow )
{
    updateAirReservoir(flow);
    updateWaterReservoir(flow);
    Real nextEntry = 1e50;
    double surface_tension = surfaceTension; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (cell->info().p()!=0) {
            for (int facet=0; facet<4; facet ++) {
                if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
                if (cell->neighbor(facet)->info().p()!=0) continue;
                if (cell->neighbor(facet)->info().isWaterReservoir == false) continue;
                double n_cell_pe = surface_tension/cell->info().poreRadius[facet];
                nextEntry = min(nextEntry,n_cell_pe);
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

template<class Cellhandle>
double UnsaturatedEngine::computeEffPoreRadius(Cellhandle cell, int j)
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
//     double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
//     double Area_SolidA = 0.5*A*pow(rA,2);
//     double Area_SolidB = 0.5*B*pow(rB,2);
//     double Area_SolidC = 0.5*C*pow(rC,2);

    double rmin = max(rAB,max(rBC,rAC));
    if (rmin==0) { rmin= 1.0e-10; }
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));
    
    if((computeDeltaForce(cell,j,rmax)<0)||(rmin>rmax)) {
      double EffPoreRadius = rmax;
//       cerr<<"0";
      return EffPoreRadius;
    }
    else if((computeDeltaForce(cell,j,rmin)<0)&&(computeDeltaForce(cell,j,rmax)>0)) {
      double effPoreRadius = bisection(cell,j,rmin,rmax);
//       cerr<<"1";
      return effPoreRadius;
    }
    else if((computeDeltaForce(cell,j,rmin)>0)&&(computeDeltaForce(cell,j,rmin)<computeDeltaForce(cell,j,rmax))) {
      double EffPoreRadius = rmin;
//       cerr<<"2";
      return EffPoreRadius;
    }
    else if ((computeDeltaForce(cell,j,rmin)>computeDeltaForce(cell,j,rmax))&&(computeDeltaForce(cell,j,rmax)>0)) {
      double EffPoreRadius = rmax;
//       cerr<<"3";
      return EffPoreRadius;
    }
//     if (rmin>rmax) {cerr<<"rmin: "<< rmin << " " << "rmax" << rmax << " rA="<<rA<<" rB="<<rB <<" rC="<<rC<<endl;}
    
//     if (((Area_ABC-Area_SolidA-Area_SolidB-Area_SolidC)<0)||(rmin>rmax)) {
//         double EffPoreRadius = rmax;//for cells close to boundary spheres, the effPoreRadius set to inscribe radius.
//         cerr<<"1";
//         return EffPoreRadius;
//     }
//     if( ( computeDeltaForce(cell,j,rmin)>0 ) && ( computeDeltaForce(cell,j,rmin)<computeDeltaForce(cell,j,rmax)) ) {
//         double EffPoreRadius = rmin;
// //         cerr<<"2 ";
// 	return EffPoreRadius;
//     }
//     else if( ( computeDeltaForce(cell,j,rmin)<0 ) && ( computeDeltaForce(cell,j,rmax)>0) ) {
//         double effPoreRadius = bisection(cell,j,rmin,rmax);
// //         cerr<<"3 ";
// 	return effPoreRadius;
//     }
//     else if( ( computeDeltaForce(cell,j,rmin)<computeDeltaForce(cell,j,rmax) ) && ( computeDeltaForce(cell,j,rmax)<0) ) {
//         double EffPoreRadius = rmax;
// //         cerr<<"4 ";
// 	return EffPoreRadius;
//     }
//     else if( ( computeDeltaForce(cell,j,rmin)>computeDeltaForce(cell,j,rmax) ) && ( computeDeltaForce(cell,j,rmax)>0) ) {
//         double EffPoreRadius = rmax;
// //         cerr<<"5 ";
// 	return EffPoreRadius;
//     }
//     else if( ( computeDeltaForce(cell,j,rmin)>0 ) && ( computeDeltaForce(cell,j,rmax)<0) ) {
//         double effPoreRadius = bisection(cell,j,rmin,rmax);
// //         cerr<<"6 ";
// 	return effPoreRadius;
//     }
//     else if( ( computeDeltaForce(cell,j,rmin)> computeDeltaForce(cell,j,rmax) ) && (computeDeltaForce(cell,j,rmin)<0) ) {
//         double EffPoreRadius = rmin;
// //         cerr<<"7 ";
// 	return EffPoreRadius;
//     }
//     else {
//         double EffPoreRadius = rmax;
// //         cerr<<"8";
// 	return EffPoreRadius;
//     }
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
    double surface_tension = surfaceTension; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
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
//     double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
//     double Area_SolidA = 0.5*A*pow(rA,2);//??Area for boundary ERROR ?? FIXME
//     double Area_SolidB = 0.5*B*pow(rB,2);
//     double Area_SolidC = 0.5*C*pow(rC,2);

    //for triangulation ArB,rcap is the radius of sphere r; Note: (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB) maybe >1, bug here!
    double _AB = (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB); if(_AB>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_AB>1.0  _AB="<<_AB<<endl;*/ _AB=1.0;} if(_AB<-1.0) {cerr<<"_AB<-1.0  _AB="<<_AB<<endl; _AB=-1.0;}
    double alphaAB = acos(_AB);
    double _BA = (pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2))/(2*(rB+rcap)*AB); if(_BA>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_BA>1.0  _BA="<<_BA<<endl;*/ _BA=1.0;} if(_BA<-1.0) {cerr<<"_BA<-1.0  _BA="<<_BA<<endl; _BA=-1.0;}
    double alphaBA = acos(_BA);
    double _ArB = (pow((rA+rcap),2)+pow((rB+rcap),2)-pow(AB,2))/(2*(rA+rcap)*(rB+rcap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
    double alphaArB = acos(_ArB);
//     double D=alphaAB + alphaBA + alphaArB; cerr<<D<<" ";
    double length_liquidAB = alphaArB*rcap;
    double AreaArB = 0.5*(rA+rcap)*(rB+rcap)*sin(alphaArB);
    double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rcap,2);

    //for triangulation ArC, rcap is the radius of sphere r;
    double _AC = (pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2))/(2*(rA+rcap)*AC); if(_AC>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_AC>1.0  _AC="<<_AC<<endl;*/ _AC=1.0;} if(_AC<-1.0) {cerr<<"_AC<-1.0  _AC="<<_AC<<endl; _AC=-1.0;}
    double alphaAC = acos(_AC);
    double _CA = (pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2))/(2*(rC+rcap)*AC); if(_CA>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_CA>1.0  _CA="<<_CA<<endl;*/ _CA=1.0;} if(_CA<-1.0) {cerr<<"_CA<-1.0  _CA="<<_CA<<endl; _CA=-1.0;}
    double alphaCA = acos(_CA);
    double _ArC = (pow((rA+rcap),2)+pow((rC+rcap),2)-pow(AC,2))/(2*(rA+rcap)*(rC+rcap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
    double alphaArC = acos(_ArC);
//     double D=alphaAC + alphaCA + alphaArC; cerr<<D<<" ";
    double length_liquidAC = alphaArC*rcap;
    double AreaArC = 0.5*(rA+rcap)*(rC+rcap)*sin(alphaArC);
    double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rcap,2);

    //for triangulation BrC, rcap is the radius of sphere r;
    double _BC = (pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2))/(2*(rB+rcap)*BC); if(_BC>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_BC>1.0  _BC="<<_BC<<endl;*/ _BC=1.0;} if(_BC<-1.0) {cerr<<"_BC<-1.0  _BC="<<_BC<<endl; _BC=-1.0;}
    double alphaBC = acos(_BC);
    double _CB = (pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2))/(2*(rC+rcap)*BC); if(_CB>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_CB>1.0  _CB="<<_CB<<endl;*/ _CB=1.0;} if(_CB<-1.0) {cerr<<"_CB<-1.0  _CB="<<_CB<<endl; _CB=-1.0;}
    double alphaCB = acos(_CB);
    double _BrC = (pow((rB+rcap),2)+pow((rC+rcap),2)-pow(BC,2))/(2*(rB+rcap)*(rC+rcap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
    double alphaBrC = acos(_BrC);
//     double D=alphaBC + alphaCB + alphaBrC; cerr<<D<<" ";    
    double length_liquidBC = alphaBrC*rcap;
    double AreaBrC = 0.5*(rB+rcap)*(rC+rcap)*sin(alphaBrC);
    double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rcap,2);

    double areaCap = sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double areaPore = areaCap - Area_liquidAB - Area_liquidAC - Area_liquidBC;
//     double Area_pore = Area_ABC - Area_liquidAB - Area_liquidAC - Area_liquidBC - Area_SolidA - Area_SolidB - Area_SolidC;
//     if(areaCap<0) {cerr<<"areaCap="<<areaCap<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;}
    if(areaPore<0) {cerr<<"areaPore="<<areaPore<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;}
//     double Area_pore1= Area_ABC - Area_SolidA - Area_SolidB - Area_SolidC;
//         if(Area_pore1<0) {cerr<<"Area_pore1="<<Area_pore1<<" rAB="<<rAB<<" rAC="<<rAC<<" rBC="<<rBC<<" Area_ABC="<<Area_ABC<<" Area_SolidA="<<Area_SolidA<<" Area_SolidB="<<Area_SolidB<<" Area_SolidC="<<Area_SolidC<<endl;} //here because big boundary spheres 
//     if((Area_pore1>0)&&(Area_pore<0))
//     {   double rAB = 0.5*(AB-rA-rB);
//         double rBC = 0.5*(BC-rB-rC);
//         double rAC = 0.5*(AC-rA-rC);
//         double rInscr=abs(solver->Compute_EffectiveRadius(cell, j));
//         if((rAB>0)&&(rBC>0)&&(rAC>0))
//         {	cerr<<"rA= "<<rA<<" rB="<<rB<<" rC="<<rC<<" rInscr="<<rInscr<<" Area_pore="<<Area_pore<<endl;
//         }
//     } //here also because big boundary spheres
    double Perimeter_pore = length_liquidAB + length_liquidAC + length_liquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC; if(Perimeter_pore<0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*double t1=(A - alphaAB - alphaAC)*rA; double t2= (B - alphaBA - alphaBC)*rB; double t3=(C - alphaCA - alphaCB)*rC; cerr<<"cellID="<<cell->info().index<<"  Perimeter_pore<0  Perimeter_pore="<<Perimeter_pore<<" liquidAB="<<length_liquidAB<<" liquidAC="<<length_liquidAC<<" liquidBC="<<length_liquidBC<<" t1="<<t1<<" t2="<<t2<<" t3="<<t3<<endl;*/}
    if(Perimeter_pore<0) {cerr<<"Perimeter_pore="<<Perimeter_pore<<" rA= "<<rA<<" rB="<<rB<<" rC="<<rC<<endl;}
    double deltaForce = surface_tension*Perimeter_pore - areaPore*(surface_tension/rcap);
//     if((Area_pore<0)&&(deltaForce>0)) {cerr<<"rA= "<<rA<<" rB="<<rB<<" rC="<<rC<<endl;}
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

// template<class Solver>
// void UnsaturatedEngine::BoundaryConditions ( Solver& flow )
// {
//         if ( flow->y_min_id>=0 ) {
//                 flow->boundary ( flow->y_min_id ).flowCondition=Flow_imposed_BOTTOM_Boundary;
//                 flow->boundary ( flow->y_min_id ).value=Pressure_BOTTOM_Boundary;
//         }
//         if ( flow->y_max_id>=0 ) {
//                 flow->boundary ( flow->y_max_id ).flowCondition=Flow_imposed_TOP_Boundary;
//                 flow->boundary ( flow->y_max_id ).value=Pressure_TOP_Boundary;
//         }
//         if ( flow->x_max_id>=0 ) {
//                 flow->boundary ( flow->x_max_id ).flowCondition=Flow_imposed_RIGHT_Boundary;
//                 flow->boundary ( flow->x_max_id ).value=Pressure_RIGHT_Boundary;
//         }
//         if ( flow->x_min_id>=0 ) {
//                 flow->boundary ( flow->x_min_id ).flowCondition=Flow_imposed_LEFT_Boundary;
//                 flow->boundary ( flow->x_min_id ).value=Pressure_LEFT_Boundary;
//         }
//         if ( flow->z_max_id>=0 ) {
//                 flow->boundary ( flow->z_max_id ).flowCondition=Flow_imposed_FRONT_Boundary;
//                 flow->boundary ( flow->z_max_id ).value=Pressure_FRONT_Boundary;
//         }
//         if ( flow->z_min_id>=0 ) {
//                 flow->boundary ( flow->z_min_id ).flowCondition=Flow_imposed_BACK_Boundary;
//                 flow->boundary ( flow->z_min_id ).value=Pressure_BACK_Boundary;
//         }
// }

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

//         if ( flow->y_min_id>=0 ) flow->boundary ( flow->y_min_id ).useMaxMin = boundaryUseMaxMin[ymin];
//         if ( flow->y_max_id>=0 ) flow->boundary ( flow->y_max_id ).useMaxMin = boundaryUseMaxMin[ymax];
//         if ( flow->x_max_id>=0 ) flow->boundary ( flow->x_max_id ).useMaxMin = boundaryUseMaxMin[xmax];
//         if ( flow->x_min_id>=0 ) flow->boundary ( flow->x_min_id ).useMaxMin = boundaryUseMaxMin[xmin];
//         if ( flow->z_max_id>=0 ) flow->boundary ( flow->z_max_id ).useMaxMin = boundaryUseMaxMin[zmax];
//         if ( flow->z_min_id>=0 ) flow->boundary ( flow->z_min_id ).useMaxMin = boundaryUseMaxMin[zmin];

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

// template<class Solver>
// void UnsaturatedEngine::AddBoundary ( Solver& flow )
// {
// 	vector<posData>& buffer = positionBufferCurrent;
//         solver->x_min = Mathr::MAX_REAL, solver->x_max = -Mathr::MAX_REAL, solver->y_min = Mathr::MAX_REAL, solver->y_max = -Mathr::MAX_REAL, solver->z_min = Mathr::MAX_REAL, solver->z_max = -Mathr::MAX_REAL;
//         FOREACH ( const posData& b, buffer ) {
//                 if ( !b.exists ) continue;
//                 if ( b.isSphere ) {
//                         const Real& rad = b.radius;
//                         const Real& x = b.pos[0];
//                         const Real& y = b.pos[1];
//                         const Real& z = b.pos[2];
//                         flow->x_min = min ( flow->x_min, x-rad );
//                         flow->x_max = max ( flow->x_max, x+rad );
//                         flow->y_min = min ( flow->y_min, y-rad );
//                         flow->y_max = max ( flow->y_max, y+rad );
//                         flow->z_min = min ( flow->z_min, z-rad );
//                         flow->z_max = max ( flow->z_max, z+rad );
//                 }
//         }
// 	//FIXME id_offset must be set correctly, not the case here (always 0), then we need walls first or it will fail
//         id_offset = flow->T[flow->currentTes].max_id+1;
//         flow->id_offset = id_offset;
//         flow->SectionArea = ( flow->x_max - flow->x_min ) * ( flow->z_max-flow->z_min );
//         flow->Vtotale = ( flow->x_max-flow->x_min ) * ( flow->y_max-flow->y_min ) * ( flow->z_max-flow->z_min );
//         flow->y_min_id=wallBottomId;
//         flow->y_max_id=wallTopId;
//         flow->x_max_id=wallRightId;
//         flow->x_min_id=wallLeftId;
//         flow->z_min_id=wallBackId;
//         flow->z_max_id=wallFrontId;
// 
//         if ( flow->y_min_id>=0 ) flow->boundary ( flow->y_min_id ).useMaxMin = BOTTOM_Boundary_MaxMin;
//         if ( flow->y_max_id>=0 ) flow->boundary ( flow->y_max_id ).useMaxMin = TOP_Boundary_MaxMin;
//         if ( flow->x_max_id>=0 ) flow->boundary ( flow->x_max_id ).useMaxMin = RIGHT_Boundary_MaxMin;
//         if ( flow->x_min_id>=0 ) flow->boundary ( flow->x_min_id ).useMaxMin = LEFT_Boundary_MaxMin;
//         if ( flow->z_max_id>=0 ) flow->boundary ( flow->z_max_id ).useMaxMin = FRONT_Boundary_MaxMin;
//         if ( flow->z_min_id>=0 ) flow->boundary ( flow->z_min_id ).useMaxMin = BACK_Boundary_MaxMin;
// 
//         //FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
//         flow->boundsIds[0]= &flow->x_min_id;
//         flow->boundsIds[1]= &flow->x_max_id;
//         flow->boundsIds[2]= &flow->y_min_id;
//         flow->boundsIds[3]= &flow->y_max_id;
//         flow->boundsIds[4]= &flow->z_min_id;
//         flow->boundsIds[5]= &flow->z_max_id;
// 
//         flow->Corner_min = CGT::Point ( flow->x_min, flow->y_min, flow->z_min );
//         flow->Corner_max = CGT::Point ( flow->x_max, flow->y_max, flow->z_max );
// 
//         if ( Debug ) {
//                 cout << "Section area = " << flow->SectionArea << endl;
//                 cout << "Vtotale = " << flow->Vtotale << endl;
//                 cout << "x_min = " << flow->x_min << endl;
//                 cout << "x_max = " << flow->x_max << endl;
//                 cout << "y_max = " << flow->y_max << endl;
//                 cout << "y_min = " << flow->y_min << endl;
//                 cout << "z_min = " << flow->z_min << endl;
//                 cout << "z_max = " << flow->z_max << endl;
//                 cout << endl << "Adding Boundary------" << endl;
//         }
//         //assign BCs types and values
//         BoundaryConditions ( flow );
// 
//         double center[3];
//         for ( int i=0; i<6; i++ ) {
//                 if ( *flow->boundsIds[i]<0 ) continue;
//                 CGT::Vecteur Normal ( normal[i].x(), normal[i].y(), normal[i].z() );
//                 if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->AddBoundingPlane ( true, Normal, *flow->boundsIds[i],5000.0 );
//                 else {
// 			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
//                         flow->AddBoundingPlane ( center, wall_thickness, Normal,*flow->boundsIds[i],5000.0 );
//                 }
//         }
// }

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
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        cell->info().index=k++;
    }
}

template<class Solver>
void UnsaturatedEngine::getPoreRadius(Solver& flow)
{
    RTriangulation& Tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = Tri.finite_cells_end();
    Cell_handle neighbour_cell;
    for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
        for (int j=0; j<4; j++) {
            neighbour_cell = cell->neighbor(j);
            if (!Tri.is_infinite(neighbour_cell)) {
                cell->info().poreRadius[j]=computeEffPoreRadius(cell, j);
                neighbour_cell->info().poreRadius[Tri.mirror_index(cell, j)]= cell->info().poreRadius[j];
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

template<class Cellhandle>
Real UnsaturatedEngine::volumeCapillaryCell ( Cellhandle cell )
{
    Initialize_volumes(solver);  
    Real volume = abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
    if (cell->info().volume()<0) { cerr << "cell ID: " << cell->info().index << "  cell volume: " << cell->info().volume() << endl; }
    if (solver->volumeSolidPore(cell)<0) { cerr << "cell ID: " << cell->info().index << "  volumeSolidPore: " << solver->volumeSolidPore(cell) << endl; }
    if (volume<0) { cerr<<"cell ID: " << cell->info().index << "cell volume: " << cell->info().volume() << "  volumeSolidPore: " << solver->volumeSolidPore(cell) << endl; }
    return volume;
}

template<class Solver>
Real UnsaturatedEngine::getSaturation (Solver& flow )
{
    RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
    Real capillary_volume = 0.0; //total volume
    Real air_volume = 0.0; 	//air volume
    Finite_cells_iterator cell_end = tri.finite_cells_end();
    for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().Pcondition) continue;
// 	    if (cell.has_vertex() )
        capillary_volume = capillary_volume + volumeCapillaryCell ( cell );
        if (cell->info().p()!=0) {
            air_volume = air_volume + volumeCapillaryCell (cell);
        }
    }
    Real saturation = 1 - air_volume/capillary_volume;
    return saturation;
}

template<class Solver>
void UnsaturatedEngine::saveListOfNodes(Solver& flow)
{
    ofstream file;
    file.open("ListOfNodes.txt");
    file << "#List Of Nodes. For one cell,there are four neighbour cells \n";
    file << "Cell_ID" << " " << "NeighborCell_ID" << endl;
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << cell->neighbor(1)->info().index << " " << cell->neighbor(2)->info().index << " " << cell->neighbor(3)->info().index << endl;
    }
    file.close();
}

template<class Solver>
void UnsaturatedEngine::saveListOfConnections(Solver& flow)
{
    ofstream file;
    file.open("ListOfConnections.txt");
    file << "#List of Connections \n";
    file << "Cell_ID" << " " << "NeighborCell_ID" << " " << "EntryValue" << " " << "poreRadius" << " " << "poreArea" << " " << "porePerimeter" << endl;
    double surface_tension = surfaceTension ; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell)) continue;
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
    And in python script, don't forget set: Flow_imposed_TOP_Boundary=False,Flow_imposed_BOTTOM_Boundary=False, */
    if(flow->boundingCells[3].size()==0) {
        cerr << "please set Flow_imposed_TOP_Boundary=False" << endl;
    }
    else {
        vector<Cell_handle>::iterator it = flow->boundingCells[3].begin();
        ofstream file;
        file.open("ListAdjacentCellsTopBoundary.txt");
        file << "#List of Cells IDs adjacent top boundary \n";
        for ( it ; it != flow->boundingCells[3].end(); it++) {
            if ((*it)->info().index == 0) continue;
// 	  cerr << (*it)->info().index << " ";
            file << (*it)->info().index << endl;
        }
        file.close();
    }
}

template<class Solver>
void UnsaturatedEngine::saveListAdjCellsBottomBound(Solver& flow)
{
    if(flow->boundingCells[2].size()==0) {
        cerr << "please set Flow_imposed_BOTTOM_Boundary=False"<< endl;
    }
    else {
        vector<Cell_handle>::iterator it = flow->boundingCells[2].begin();
        ofstream file;
        file.open("ListAdjacentCellsBottomBoundary.txt");
        file << "#List of Cells IDs adjacent bottom boundary \n";
        for ( it ; it != flow->boundingCells[2].end(); it++) {
            if ((*it)->info().index == 0) continue;
            file << (*it)->info().index << endl;
        }
        file.close();
    }
}

//initialize the boundingCells info().isWaterReservoir=true,  on condition that those cells meet (flow->boundingCells[bound].size()!=0 && cell->info().p()==0) 
template<class Solver>
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
            if((*it)->info().p()!=0) continue;//FIXME: the default pressure for water is 0
            (*it)->info().isWaterReservoir = true;
        }
    }
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
            if((*it)->info().isWaterReservoir == false) continue;
            waterReservoirRecursion((*it),flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::waterReservoirRecursion(Cell_handle cell, Solver& flow)
{
    if(cell->info().isWaterReservoir == true) {
        for (int facet = 0; facet < 4; facet ++) {
            if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().p()!=0) continue;
            if (cell->neighbor(facet)->info().isWaterReservoir==true) continue;
            Cell_handle n_cell = cell->neighbor(facet);
            n_cell->info().isWaterReservoir = true;
            waterReservoirRecursion(n_cell,flow);
        }
    }
}

//initialize the boundingCells info().isAirReservoir=true, on condition that those cells meet (flow->boundingCells[bound].size()!=0 && cell->info().p()!=0) 
template<class Solver>
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
            if ((*it)->info().index == 0) continue;
            if((*it)->info().p() == 0) continue;//FIXME: the default pressure for water is 0
            (*it)->info().isAirReservoir = true;
        }
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
            if((*it)->info().isAirReservoir == false) continue;
            airReservoirRecursion((*it),flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::airReservoirRecursion(Cell_handle cell, Solver& flow)
{
    if(cell->info().isAirReservoir == true) {
        for (int facet = 0; facet < 4; facet ++) {
            if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().p() == 0) continue;
            if (cell->neighbor(facet)->info().isAirReservoir == true) continue;
            Cell_handle n_cell = cell->neighbor(facet);
            n_cell->info().isAirReservoir = true;
            airReservoirRecursion(n_cell,flow);
        }
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
template<class Cellhandle>//ERROR! check later
Real UnsaturatedEngine::computePoreArea(Cellhandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;
    
    Real rcap = cell->info().poreRadius[j];
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
    double rAB = 0.5*(AB-rA-rB);    if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC);    if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC);    if (rAC<0) { rAC=0; }
    double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);

    double rmin = max(rAB,max(rBC,rAC));
    if (rmin==0) { rmin= 1.0e-10; }
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));

    Real poreArea = 0;
    if (abs(rcap-rmax)<1.0e-6) {    
        poreArea = Mathr::PI*pow(rmax,2);
        return poreArea;
    }
//     else if (cell->info().poreRadius[j] > rmax) {cerr<<"poreRadius Error: "<<cell->info().poreRadius[j];}
    else {
    //for triangulation ArB,rcap is the radius of sphere r; Note: (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB) maybe >1, bug here!
    double _AB = (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB); if(_AB>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_AB>1.0  _AB="<<_AB<<endl;*/ _AB=1.0;} if(_AB<-1.0) {cerr<<"_AB<-1.0  _AB="<<_AB<<endl; _AB=-1.0;}
    double alphaAB = acos(_AB);
    double _BA = (pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2))/(2*(rB+rcap)*AB); if(_BA>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_BA>1.0  _BA="<<_BA<<endl;*/ _BA=1.0;} if(_BA<-1.0) {cerr<<"_BA<-1.0  _BA="<<_BA<<endl; _BA=-1.0;}
    double alphaBA = acos(_BA);
    double _ArB = (pow((rA+rcap),2)+pow((rB+rcap),2)-pow(AB,2))/(2*(rA+rcap)*(rB+rcap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
    double alphaArB = acos(_ArB);
//     double D=alphaAB + alphaBA + alphaArB; cerr<<D<<" ";
    double length_liquidAB = alphaArB*rcap;
    double AreaArB = 0.5*(rA+rcap)*(rB+rcap)*sin(alphaArB);
    double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rcap,2);

    //for triangulation ArC, rcap is the radius of sphere r;
    double _AC = (pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2))/(2*(rA+rcap)*AC); if(_AC>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_AC>1.0  _AC="<<_AC<<endl;*/ _AC=1.0;} if(_AC<-1.0) {cerr<<"_AC<-1.0  _AC="<<_AC<<endl; _AC=-1.0;}
    double alphaAC = acos(_AC);
    double _CA = (pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2))/(2*(rC+rcap)*AC); if(_CA>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_CA>1.0  _CA="<<_CA<<endl;*/ _CA=1.0;} if(_CA<-1.0) {cerr<<"_CA<-1.0  _CA="<<_CA<<endl; _CA=-1.0;}
    double alphaCA = acos(_CA);
    double _ArC = (pow((rA+rcap),2)+pow((rC+rcap),2)-pow(AC,2))/(2*(rA+rcap)*(rC+rcap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
    double alphaArC = acos(_ArC);
//     double D=alphaAC + alphaCA + alphaArC; cerr<<D<<" ";
    double length_liquidAC = alphaArC*rcap;
    double AreaArC = 0.5*(rA+rcap)*(rC+rcap)*sin(alphaArC);
    double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rcap,2);

    //for triangulation BrC, rcap is the radius of sphere r;
    double _BC = (pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2))/(2*(rB+rcap)*BC); if(_BC>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_BC>1.0  _BC="<<_BC<<endl;*/ _BC=1.0;} if(_BC<-1.0) {cerr<<"_BC<-1.0  _BC="<<_BC<<endl; _BC=-1.0;}
    double alphaBC = acos(_BC);
    double _CB = (pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2))/(2*(rC+rcap)*BC); if(_CB>1.0) {/*cerr<<"cellID="<<cell->info().index<<" rA="<<rA<<" rB="<<rB<<" rC="<<rC<<endl;*//*cerr<<"_CB>1.0  _CB="<<_CB<<endl;*/ _CB=1.0;} if(_CB<-1.0) {cerr<<"_CB<-1.0  _CB="<<_CB<<endl; _CB=-1.0;}
    double alphaCB = acos(_CB);
    double _BrC = (pow((rB+rcap),2)+pow((rC+rcap),2)-pow(BC,2))/(2*(rB+rcap)*(rC+rcap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
    double alphaBrC = acos(_BrC);
//     double D=alphaBC + alphaCB + alphaBrC; cerr<<D<<" ";    
    double length_liquidBC = alphaBrC*rcap;
    double AreaBrC = 0.5*(rB+rcap)*(rC+rcap)*sin(alphaBrC);
    double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rcap,2);

    double areaCap = sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double areaPore = areaCap - Area_liquidAB - Area_liquidAC - Area_liquidBC;
    return areaPore;
    }
}
template<class Cellhandle>
Real UnsaturatedEngine::computePorePerimeter(Cellhandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Real rcap = cell->info().poreRadius[j];
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
    double rAB = 0.5*(AB-rA-rB);    if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC);    if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC);    if (rAC<0) { rAC=0; }
    double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);

    double rmin = max(rAB,max(rBC,rAC));
    if (rmin==0) { rmin= 1.0e-10; }
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));

    Real porePerimeter = 0;
    if (abs(rcap-rmax)<1.0e-6) {
// 	cerr<<"rcap: "<<rcap <<" "<<"rmax: "<<rmax<<endl;
        porePerimeter = 2*Mathr::PI*rmax;
        return porePerimeter;
    }
    else {
        //for triangulation ArB,rcap is the radius of sphere r; Note: (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB) maybe >1, bug here!
        double _AB = pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2)/(2*(rA+rcap)*AB);
        if (_AB>1) { _AB=1.0; }
        double alphaAB = acos(_AB);
        double _BA = pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2)/(2*(rB+rcap)*AB);
        if (_BA>1) { _BA=1.0; }
        double alphaBA = acos(_BA);
        double betaAB = 0.5*Mathr::PI - alphaAB;
        double betaBA = 0.5*Mathr::PI - alphaBA;
        double length_liquidAB = (betaAB+betaBA)*rcap;
        double AreaArB = 0.5*AB*(rA+rcap)*sin(alphaAB);
        double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*(betaAB+betaBA)*pow(rcap,2);

        //for triangulation ArC, rcap is the radius of sphere r;
        double _AC = pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2)/(2*(rA+rcap)*AC);
        if (_AC>1) { _AC=1.0; }
        double alphaAC = acos(_AC);
        double _CA = pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2)/(2*(rC+rcap)*AC);
        if (_CA>1) { _CA=1.0; }
        double alphaCA = acos(_CA);
        double betaAC = 0.5*Mathr::PI - alphaAC;
        double betaCA = 0.5*Mathr::PI - alphaCA;
        double length_liquidAC = (betaAC+betaCA)*rcap;
        double AreaArC = 0.5*AC*(rA+rcap)*sin(alphaAC);
        double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*(betaAC+betaCA)*pow(rcap,2);

        //for triangulation BrC, rcap is the radius of sphere r;
        double _BC = pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2)/(2*(rB+rcap)*BC);
        if (_BC>1) { _BC=1.0; }
        double alphaBC = acos(_BC);
        double _CB = pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2)/(2*(rC+rcap)*BC);
        if (_CB>1) { _CB=1.0; }
        double alphaCB = acos(_CB);
        double betaBC = 0.5*Mathr::PI - alphaBC;
        double betaCB = 0.5*Mathr::PI - alphaCB;
        double length_liquidBC = (betaBC+betaCB)*rcap;
        double AreaBrC = 0.5*BC*(rB+rcap)*sin(alphaBC);
        double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*(betaBC+betaCB)*pow(rcap,2);

	double porePerimeter = length_liquidAB + length_liquidAC + length_liquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC;
	if (porePerimeter<0) { porePerimeter = 2*Mathr::PI*rmax; }
        return porePerimeter;
    }
}
template<class Solver>
void UnsaturatedEngine::savePoreBodyInfo(Solver& flow)
{
    ofstream file;
    file.open("PoreBodyInfo.txt");
    file << "#List of pore bodies position (or Voronoi centers) and size (volume) \n";
    file << "Cell_ID " << " x " << " y " << " z " << " volume "<< endl;
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell)) continue;
        file << cell->info().index << " " << cell->info() << " " << volumeCapillaryCell(cell)<< endl;
    }
    file.close();
}
template<class Solver>
void UnsaturatedEngine::debugTemp(Solver& flow)
{
    double surface_tension = surfaceTension; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    ofstream file;
    file.open("bug.txt");
//     file<<"cellID   "<<"deltaForce  "<<"inscribeRadius*PI*tension  "<<endl;
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell)) continue;
	double rmax0= abs(solver->Compute_EffectiveRadius(cell, 0));double D0=surface_tension*rmax0*Mathr::PI ;
	double rmax1= abs(solver->Compute_EffectiveRadius(cell, 1));double D1=surface_tension*rmax1*Mathr::PI ;
	double rmax2= abs(solver->Compute_EffectiveRadius(cell, 2));double D2=surface_tension*rmax2*Mathr::PI ;
	double rmax3= abs(solver->Compute_EffectiveRadius(cell, 3));double D3=surface_tension*rmax3*Mathr::PI ;
	file << cell->info().index << "  " <<computeDeltaForce(cell,0,rmax0)<<"  "<<computeDeltaMin(cell, 0)<<endl;
	file << cell->info().index << "  " <<computeDeltaForce(cell,1,rmax1)<<"  "<<computeDeltaMin(cell, 1)<<endl;
	file << cell->info().index << "  " <<computeDeltaForce(cell,2,rmax2)<<"  "<<computeDeltaMin(cell, 2)<<endl;
	file << cell->info().index << "  " <<computeDeltaForce(cell,3,rmax3)<<"  "<<computeDeltaMin(cell, 3)<<endl;
    }
    file.close();
}
template<class Cellhandle>
double UnsaturatedEngine::computeDeltaMin(Cellhandle cell, int j)
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
//     double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
//     double Area_SolidA = 0.5*A*pow(rA,2);
//     double Area_SolidB = 0.5*B*pow(rB,2);
//     double Area_SolidC = 0.5*C*pow(rC,2);
    
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));
    double rmin = max(rAB,max(rBC,rAC));
    if (rmin==0) {/*cerr<<"1";*/
        rmin= 1.0e-10;
    }
    double deltaMin = computeDeltaForce(cell,j,rmin);
    if(deltaMin>0) {
        if((rAB!=0)&&(rBC!=0)&&(rAC!=0)) {
         /*   cerr<<"rAB= "<<rAB<<" rBC="<<rBC<<" rAC="<<rAC<<" rmin="<<rmin<<endl;
        */double deltaMax = computeDeltaForce(cell,j,rmax);
// 	     cerr<<"deltaMax="<<deltaMax<<" deltaMin="<<deltaMin<<" rmax="<<rmax<<" rmin="<<rmin<<" rA= "<<rA<<" rB="<<rB<<" rC="<<rC<<endl;       
	}
    }
    return deltaMin;
}
//----------end tempt function for Vahid Joekar-Niasar's data (clear later)---------------------
YADE_PLUGIN ( ( UnsaturatedEngine ) );

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */
