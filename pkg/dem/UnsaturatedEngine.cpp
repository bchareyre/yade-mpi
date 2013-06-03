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

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

Real UnsaturatedEngine::testFunction()
{
	cout<<"This is Chao's test program"<<endl;

// 	UnsaturatedEngine inherits from Emanuele's flow engine, so it contains many things. However, we will ignore what's in it for the moment.
// 	The only thing interesting for us is that UnsaturatedEngine contains an object "triangulation" from CGAL library.
//	Let us define an alias for this triangulation:
	RTriangulation& triangulation = solver->T[solver->currentTes].Triangulation();

	if (triangulation.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		//here we define the pointer to Yade's scene
		scene = Omega::instance().getScene().get();
		//copy sphere positions in a buffer...
		setPositionsBuffer(true);
		//then create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		Build_Triangulation(P_zero,solver);
		initializeCellIndex(solver);
		get_pore_radius(solver);
	}
	
	//Now, you can use "triangulation", with all the functions listed in CGAL documentation
	//We can insert spheres (here I'm in fact stealing the code from Tesselation::insert() (see Tesselation.ipp)	
	cout << "triangulation.number_of_vertices()" << triangulation.number_of_vertices() << endl;
	cout <<"triangulation.number_of_cells()" << triangulation.number_of_cells() << endl;
	
	//now we can start playing with pressure (=1 for dry pore, =0 for saturated pore)
	//they all have 0 by default, we find one cell and set pressure to 1
// 	Cell_handle cell = triangulation.locate(Point(0.5,0.5,0.5));
// 	cell->info().p()= gasPressure; //initialised air entry pressure	
// 	show number_of_cells with air	
	unsigned int m=0;
	Finite_cells_iterator cell_end = triangulation.finite_cells_end();
	for ( Finite_cells_iterator cell = triangulation.finite_cells_begin(); cell != cell_end; cell++ )
	{
	  if (cell->info().p()!=0)
	    m++;
	}
	cout << "number_of_cells with air: "<< m <<endl;
	
// 	cout<<"xmin: "<<solver->x_min<<endl;
// 	cout<<"xmax: "<<solver->x_max<<endl;
// 	cout<<"ymin: "<<solver->y_min<<endl;
// 	cout<<"ymax: "<<solver->y_max<<endl;
// 	cout<<"zmin: "<<solver->z_min<<endl;
// 	cout<<"zmax: "<<solver->z_max<<endl;
	
	/*	FlowSolver FS;
	double surface_tension = 1; //hypothesis that's surface tension
	
	for(int facet = 0; facet < 4; facet ++)
	{
	  //NOTE: this test is important, the infinite cells are outside the problem, we skip them
	  if (triangulation.is_infinite(cell->neighbor(facet))) continue;
	  double pe = 2*surface_tension/FS.Compute_EffectiveRadius(cell, facet);		
	  //pe is air entry pressure, related to facet(inscribe circle r), vertices and surface_tension, pe = (Lnw+Lns*cosθ)*σnw/An = 2*σnw/r
	  cout << "facet: " << facet << ", air entry pressure pe =  " << pe << endl;
	  if (cell->info().p() > pe)
	    cell->neighbor(facet)->info().p() = cell->info().p();
	}
	
	double cell_pressure;
	//FIXME CHao, the coordinates of measurments must be correct with respect to the simulation sizes,needs to be adapted here
	//cell_pressure = FS.MeasurePorePressure (6, 7, 6);
	cout << "the pressure in cell(6,7,6) is: " << cell_pressure << endl;*/
	
	solver->noCache = false;
}

template<class Solver>
void UnsaturatedEngine::invadeSingleCell(Cell_handle cell, double pressure, Solver& flow)
{
    double surface_tension = surfaceTension ; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    for (int facet = 0; facet < 4; facet ++)
    {
        if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
	if (cell->neighbor(facet)->info().p() >= pressure) continue;
        double n_cell_pe = surface_tension/cell->info().pore_radius[facet];
        if (pressure > n_cell_pe)
        {   Cell_handle n_cell = cell->neighbor(facet);
            n_cell->info().p() = pressure;
            invadeSingleCell(n_cell, pressure, flow);
        }
    }
}

template<class Solver>
void UnsaturatedEngine::invade (Solver& flow )
{
    BoundaryConditions(flow);
    flow->pressureChanged=true; flow->reApplyBoundaryConditions();
    
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
    {
        if (cell->info().p()!=0)
        {        
            invadeSingleCell(cell,cell->info().p(),flow);
// 	    cout << "cell pressure: " << cell->info().p(); //test whether the cell's pressure has been changed
        }
    }
}

template<class Solver>
Real UnsaturatedEngine::get_min_EntryValue (Solver& flow )
{
    Real nextEntry = 1e50;
    double surface_tension = surfaceTension; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
    {
        if (cell->info().p()!=0)
        {
            for (int facet=0; facet<4; facet ++)
            {
                if (flow->T[flow->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
//              if (cell->info().Pcondition) continue;  //FIXME Add this, the boundary cell pressure will not work; Remove this the initial pressure and initial invade will be chaos.
                if (cell->neighbor(facet)->info().p()!=0) continue;
                if (cell->neighbor(facet)->info().p()==0)
                {
                    double n_cell_pe = surface_tension/cell->info().pore_radius[facet];
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

template<class Cellhandle>
double UnsaturatedEngine::compute_EffPoreRadius(Cellhandle cell, int j)
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

    double rAB = 0.5*(AB-rA-rB);
    if (rAB<0) {
        rAB=0;
    }
    double rBC = 0.5*(BC-rB-rC);
    if (rBC<0) {
        rBC=0;
    }
    double rAC = 0.5*(AC-rA-rC);
    if (rAC<0) {
        rAC=0;
    }

    double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);

    double rmin = max(rAB,max(rBC,rAC));
    if (rmin==0) {
        rmin= 1.0e-10;
    }
    double rmax = abs(solver->Compute_EffectiveRadius(cell, j));

    if ((Area_ABC-Area_SolidA-Area_SolidB-Area_SolidC)<0) {
//         cerr<<"(Area_ABC-Area_SolidA-Area_SolidB-Area_SolidC)="<<Area_ABC-Area_SolidA-Area_SolidB-Area_SolidC <<endl<<cell->vertex(facetVertices[j][0])->point()<<endl;
//         cerr<<cell->vertex(facetVertices[j][1])->point()<<endl;
//         cerr<<cell->vertex(facetVertices[j][2])->point()<<endl;
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double EffPoreRadius = rmax;//for cells close to boundary spheres, the effPoreRadius set to inscribe radius.
        return EffPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)>0 ) && ( computeDeltaPressure(cell,j,rmin)<computeDeltaPressure(cell,j,rmax)) ) {
//         cerr<<"1";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double EffPoreRadius = rmin;
        return EffPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)<0 ) && ( computeDeltaPressure(cell,j,rmax)>0) ) {
//         cerr<<"2";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double effPoreRadius = bisection(cell,j,rmin,rmax);
        return effPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)<computeDeltaPressure(cell,j,rmax) ) && ( computeDeltaPressure(cell,j,rmax)<0) ) {
//         cerr<<"3";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double EffPoreRadius = rmax;
        return EffPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)>computeDeltaPressure(cell,j,rmax) ) && ( computeDeltaPressure(cell,j,rmax)>0) ) {
//         cerr<<"4";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double EffPoreRadius = rmax;
        return EffPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)>0 ) && ( computeDeltaPressure(cell,j,rmax)<0) ) {
//         cerr<<"5";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double effPoreRadius = bisection(cell,j,rmin,rmax);
        return effPoreRadius;
    }
    else if( ( computeDeltaPressure(cell,j,rmin)> computeDeltaPressure(cell,j,rmax) ) && (computeDeltaPressure(cell,j,rmin)<0) ) {
//         cerr<<"6";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
        double EffPoreRadius = rmin;
        return EffPoreRadius;
    }
    else {
//         cerr<<"7";
//         cerr<<"rmin: "<<rmin<<" "<<"rmax: "<<rmax<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmin): "<<computeDeltaPressure(cell,j,rmin)<<endl;
//         cerr<<"computeDeltaPressure(cell,j,rmax): "<<computeDeltaPressure(cell,j,rmax)<<endl;
//         cerr<<"AB="<<AB<<" "<<"BC="<<BC<<" "<<"AC"<<AC<<endl;
//         cerr<<"rA="<<rA<<" "<<"rB="<<rB<<" "<<"rC"<<rC<<endl;
//         cerr<<cell->vertex(facetVertices[j][0])->point()<<endl;
//         cerr<<cell->vertex(facetVertices[j][1])->point()<<endl;
//         cerr<<cell->vertex(facetVertices[j][2])->point()<<endl;
        double EffPoreRadius = rmax;
        return EffPoreRadius;
    }
}

template<class Cellhandle>
double UnsaturatedEngine::bisection(Cellhandle cell, int j, double a, double b)
{
    double m = 0.5*(a+b);
    if (abs(b-a)>abs((solver->Compute_EffectiveRadius(cell, j)*1.0e-6))) {
        if ( computeDeltaPressure(cell,j,m) * computeDeltaPressure(cell,j,a) < 0 ) {
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
double UnsaturatedEngine::computeDeltaPressure(Cellhandle cell,int j, double rcap)
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

    double Area_ABC = 0.5 * ((posB-posA).cross(posC-posA)).norm();
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);
    
    //for triangulation ArB,rcap is the radius of sphere r; Note: (pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2))/(2*(rA+rcap)*AB) maybe >1, bug here!
    double _AB = pow((rA+rcap),2)+pow(AB,2)-pow((rB+rcap),2)/(2*(rA+rcap)*AB);	if (_AB>1) {_AB=1.0;}
    double alphaAB = acos(_AB);
    double _BA = pow((rB+rcap),2)+pow(AB,2)-pow((rA+rcap),2)/(2*(rB+rcap)*AB);	if (_BA>1) {_BA=1.0;}
    double alphaBA = acos(_BA);
    double betaAB = 0.5*Mathr::PI - alphaAB; 
    double betaBA = 0.5*Mathr::PI - alphaBA;
    double length_liquidAB = (betaAB+betaBA)*rcap;
    double AreaArB = 0.5*AB*(rA+rcap)*sin(alphaAB);
    double Area_liquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*(betaAB+betaBA)*pow(rcap,2);    
   
    //for triangulation ArC, rcap is the radius of sphere r;
    double _AC = pow((rA+rcap),2)+pow(AC,2)-pow((rC+rcap),2)/(2*(rA+rcap)*AC);	if (_AC>1) {_AC=1.0;}
    double alphaAC = acos(_AC);
    double _CA = pow((rC+rcap),2)+pow(AC,2)-pow((rA+rcap),2)/(2*(rC+rcap)*AC);	if (_CA>1) {_CA=1.0;}
    double alphaCA = acos(_CA);
    double betaAC = 0.5*Mathr::PI - alphaAC; 
    double betaCA = 0.5*Mathr::PI - alphaCA;
    double length_liquidAC = (betaAC+betaCA)*rcap;
    double AreaArC = 0.5*AC*(rA+rcap)*sin(alphaAC);
    double Area_liquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*(betaAC+betaCA)*pow(rcap,2);    
  
    //for triangulation BrC, rcap is the radius of sphere r;
    double _BC = pow((rB+rcap),2)+pow(BC,2)-pow((rC+rcap),2)/(2*(rB+rcap)*BC);	if (_BC>1) {_BC=1.0;}
    double alphaBC = acos(_BC);
    double _CB = pow((rC+rcap),2)+pow(BC,2)-pow((rB+rcap),2)/(2*(rC+rcap)*BC);	if (_CB>1) {_CB=1.0;}
    double alphaCB = acos(_CB);
    double betaBC = 0.5*Mathr::PI - alphaBC; 
    double betaCB = 0.5*Mathr::PI - alphaCB;
    double length_liquidBC = (betaBC+betaCB)*rcap;
    double AreaBrC = 0.5*BC*(rB+rcap)*sin(alphaBC);
    double Area_liquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*(betaBC+betaCB)*pow(rcap,2);    
    
    double Area_pore = Area_ABC - Area_liquidAB - Area_liquidAC - Area_liquidBC - Area_SolidA - Area_SolidB - Area_SolidC;
    double Perimeter_pore = length_liquidAB + length_liquidAC + length_liquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC;	
    double deltaPressure = surface_tension*Perimeter_pore - Area_pore*(surface_tension/rcap);
    return deltaPressure;
}

/*//suppose fluid-air interface tangent with two spheres A,B and line AB. The results proved that's not the maximum EffPoreRadius.
template<class Cellhandle>
double UnsaturatedEngine::compute_EffPoreRadius(Cellhandle cell, int j)
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
    double r12=0; double r13=0; double r23=0;
    
    //for particle A,B;circle r12 is tangent with A,B and line AB;
    double a1 = 4*pow((rA-rB),2);
    double b1 = 4*pow((rA-rB),2)*(rA+rB)-4*pow(AB,2)*(rA-rB)-8*pow(AB,2)*rB;
    double c1 = pow((pow(rA,2)-pow(rB,2)),2)-2*pow(AB,2)*(pow(rA,2)-pow(rB,2))+pow(AB,4)-4*pow(AB,2)*pow(rB,2);
    
    if ((pow(b1,2)-4*a1*c1)<0) {
        cout << "NEGATIVE DETERMINANT" << endl;
    }
    if (rA==rB) {
        r12 = (0.25*pow(AB,2)-pow(rA,2))/(2*rA);
    }
    else {
        r12 = (-b1-sqrt(pow(b1,2)-4*a1*c1))/(2*a1);
    }
    if (r12<0) {
//         cout << "r12 = " << r12 << endl;
        r12 = (-b1+sqrt(pow(b1,2)-4*a1*c1))/(2*a1);
//         cout << endl << "r12 = " << r12 << endl;
    }
    if (rA+rB>=AB) {
//         cout << "r12 = " << r12 << endl;  
	r12 = 0;
//         cout << endl << "r12 = " << r12 << endl;
    }

    double beta12 = acos(r12/(r12+rA));
    double beta21 = acos(r12/(r12+rB));
    double Area_LiquidBridge12 = 0.5*AB*r12 - 0.5*pow(rA,2)*(0.5*Mathr::PI-beta12) - 0.5*pow(rB,2)*(0.5*Mathr::PI-beta21) - 0.5*pow(r12,2)*(beta12+beta21);
    double Length_LiquidBridge12 = (beta12+beta21)*r12;

    //for particle A,C;circle r13 is tangent with A,C and line AC;
    double a2 = 4*pow((rA-rC),2);
    double b2 = 4*pow((rA-rC),2)*(rA+rC)-4*pow(AC,2)*(rA-rC)-8*pow(AC,2)*rC;
    double c2 = pow((pow(rA,2)-pow(rC,2)),2)-2*pow(AC,2)*(pow(rA,2)-pow(rC,2))+pow(AC,4)-4*pow(AC,2)*pow(rC,2);

    if ((pow(b2,2)-4*a2*c2)<0) {
        cout << "NEGATIVE DETERMINANT" << endl;
    }
    if (rA==rC) {
        r13 = (0.25*pow(AC,2)-pow(rA,2))/(2*rA);
    }
    else {
        r13 = (-b2-sqrt(pow(b2,2)-4*a2*c2))/(2*a2);
    }
    if (r13<0) {
//         cout << "r13 = " << r13 << endl;
        r13 = (-b2+sqrt(pow(b2,2)-4*a2*c2))/(2*a2);
//         cout << endl << "r13 = " << r13 << endl;
    }
    if (rA+rC>=AC) {
//         cout << "r13 = " << r13 << endl;  
	r13 = 0;
// 	cout << endl << "r13 = " << r13 << endl;
    }

    double beta13 = acos(r13/(r13+rA));
    double beta31 = acos(r13/(r13+rC));
    double Area_LiquidBridge13 = 0.5*AC*r13 - 0.5*pow(rA,2)*(0.5*Mathr::PI-beta13) - 0.5*pow(rC,2)*(0.5*Mathr::PI-beta31) - 0.5*pow(r13,2)*(beta13+beta31);
    double Length_LiquidBridge13 = (beta13+beta31)*r13;

    //for particle B,C;circle r23 is tangent with B,C and line BC;
    double a3 = 4*pow((rB-rC),2);
    double b3 = 4*pow((rB-rC),2)*(rB+rC)-4*pow(BC,2)*(rB-rC)-8*pow(BC,2)*rC;
    double c3 = pow((pow(rB,2)-pow(rC,2)),2)-2*pow(BC,2)*(pow(rB,2)-pow(rC,2))+pow(BC,4)-4*pow(BC,2)*pow(rC,2);

    if ((pow(b3,2)-4*a3*c3)<0) {
        cout << "NEGATIVE DETERMINANT" << endl;
    }
    if (rB==rC) {
        r23 = (0.25*pow(BC,2)-pow(rB,2))/(2*rB);
    }
    else {
        r23 = (-b3-sqrt(pow(b3,2)-4*a3*c3))/(2*a3);
    }
    if (r23<0) {
//         cout << "r23 = " << r23 << endl;
        r23 = (-b3+sqrt(pow(b3,2)-4*a3*c3))/(2*a3);
//         cout << endl << "r23 = " << r23 << endl;
    }
    if (rB+rC>=BC) {
//         cout << "r23 = " << r23 << endl;  
	r23 = 0;
// 	cout << endl << "r23 = " << r23 << endl;
    }

    double beta23 = acos(r23/(r23+rB));
    double beta32 = acos(r23/(r23+rC));
    double Area_LiquidBridge23 = 0.5*BC*r23 - 0.5*pow(rB,2)*(0.5*Mathr::PI-beta23) - 0.5*pow(rC,2)*(0.5*Mathr::PI-beta32) - 0.5*pow(r23,2)*(beta23+beta32);
    double Length_LiquidBridge23 = (beta23+beta32)*r23;

    //effective radius for pore throat: radius = Area/Perimeter
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);
    double Area_Pore = 0.5 * ((posB-posA).cross(posC-posA)).norm() - Area_SolidA - Area_SolidB-Area_SolidC-Area_LiquidBridge12-Area_LiquidBridge13-Area_LiquidBridge23;
    double Perimeter_Pore = (A-(0.5*Mathr::PI-beta12)-(0.5*Mathr::PI-beta13))*rA + (B-(0.5*Mathr::PI-beta21)-(0.5*Mathr::PI-beta23))*rB + (C-(0.5*Mathr::PI-beta31)-(0.5*Mathr::PI-beta32))*rC + Length_LiquidBridge12 + Length_LiquidBridge13 + Length_LiquidBridge23;
    
    double Effective_PoreRadius = Area_Pore/Perimeter_Pore; 
    //Note:here Effective_PoreRadius is different from eff_radius = 2*Area/Perimeter; eff_radius is for calculating 2D plan effective radius. Here EntryValue*Area_Pore = tension*Perimeter_Pore, EntryValue = tension/Effective_PoreRadius = tension/(Area_Pore/Perimeter_Pore);
    
//     cout << "AB: " << AB <<endl; cout<< "AC: " << AC <<endl; cout << "BC: " << BC << endl;
//     cout << "rA: " << rA <<endl; cout<< "rB: " << rB <<endl; cout << "rC: " << rC << endl; 
//     cout << "r12: " << r12 <<endl; cout << "r13:" << r13 <<endl; cout <<"r23: "<<r23<<endl;
//     cout << "0.5 * ((posB-posA).cross(posC-posA)).norm():  " << 0.5 * ((posB-posA).cross(posC-posA)).norm() <<endl;
//     cout << "Area_SolidA: " << Area_SolidA << endl;
//     cout << "Area_SolidB: " << Area_SolidB << endl;
//     cout << "Area_SolidC: " << Area_SolidC << endl;
//     cout << "Area_LiquidBridge12: " << Area_LiquidBridge12 << endl;
//     cout << "Area_LiquidBridge13: " << Area_LiquidBridge13 << endl;
//     cout << "Area_LiquidBridge23: " << Area_LiquidBridge23 << endl;
//     cout << "Area_Pore: " << Area_Pore <<endl;
//     cout << "Perimeter_Pore: " << Perimeter_Pore <<endl;  
//       if (Area_Pore<0) {cout << "0" << endl;}
//       if (Perimeter_Pore<0) {cout << "1" << endl;}
      if (Effective_PoreRadius<0) {Effective_PoreRadius=1.0e-5; return Effective_PoreRadius;} 
      //if Effective_PoreRadius<0, it means three particle are close to each other no pore generated.
      else return Effective_PoreRadius;
}
*/
/*//suppose there is not liquid bridge between two spheres A,B. But that's also what we want.
template<class Cellhandle>//waste
double UnsaturatedEngine::compute_EffPoreRadius(Cellhandle cell, int j)
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
    
    double gapAB=0; double gapAC=0; double gapBC=0;
    //for particle A,B;
    if (rA+rB>=AB) {gapAB = 0;}
    else {gapAB = AB - rA -rB;}
    //for particle A,C;
    if (rA+rC>=AC) {gapAC = 0;}
    else {gapAC = AC - rA -rC;}
    //for particle B,C;
    if (rB+rC>=BC) {gapBC = 0;}
    else {gapBC = BC - rB -rC;}
    
    //effective radius for pore throat: radius = Area/Perimeter
    double Area_SolidA = 0.5*A*pow(rA,2);
    double Area_SolidB = 0.5*B*pow(rB,2);
    double Area_SolidC = 0.5*C*pow(rC,2);
    double Area_Pore = 0.5 * ((posB-posA).cross(posC-posA)).norm() - Area_SolidA - Area_SolidB - Area_SolidC;
    double Perimeter_Pore = A*rA + B*rB + C*rC + gapAB + gapAC + gapBC;
    double Effective_PoreRadius = Area_Pore/Perimeter_Pore; 
    //Note:here Effective_PoreRadius is different from eff_radius = 2*Area/Perimeter; eff_radius is for calculating 2D plan effective radius. Here EntryValue*Area_Pore = tension*Perimeter_Pore, EntryValue = tension/Effective_PoreRadius = tension/(Area_Pore/Perimeter_Pore);
    
//     cout << "0.5 * ((posB-posA).cross(posC-posA)).norm():  " << 0.5 * ((posB-posA).cross(posC-posA)).norm() <<endl;    
//     cout << "Area_SolidA: " << Area_SolidA << endl;
//     cout << "Area_SolidB: " << Area_SolidB << endl;
//     cout << "Area_SolidC: " << Area_SolidC << endl;
//     cout << "Area_Pore: " << Area_Pore <<endl;    
//     cout << "Perimeter_Pore: " << Perimeter_Pore <<endl;
//       if (Area_Pore<0) {cout << "0" << endl;}
//       if (Perimeter_Pore<0) {cout << "1" << endl;}    
    if (Effective_PoreRadius<0) {Effective_PoreRadius=1.0e-5; return Effective_PoreRadius;}
      //if Effective_PoreRadius<0, it means three particle are close to each other no pore generated.    
    else return Effective_PoreRadius;
}
*/
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
                if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->AddBoundingPlane ( true, Normal, *flow->boundsIds[i],5000.0 );
                else {
			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
                        flow->AddBoundingPlane ( center, wall_thickness, Normal,*flow->boundsIds[i],5000.0 );
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
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
    {
        cell->info().index=k++;
    }
}
template<class Solver>
void UnsaturatedEngine::get_pore_radius(Solver& flow)
{
    RTriangulation& Tri = flow->T[flow->currentTes].Triangulation();
    Finite_cells_iterator cell_end = Tri.finite_cells_end();
    Cell_handle neighbour_cell;
    for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
        for (int j=0; j<4; j++) {
            neighbour_cell = cell->neighbor(j);
            if (!Tri.is_infinite(neighbour_cell)) {
                    cell->info().pore_radius[j]=compute_EffPoreRadius(cell, j);
                    neighbour_cell->info().pore_radius[Tri.mirror_index(cell, j)]= cell->info().pore_radius[j];
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
Real UnsaturatedEngine::capillary_Volume_cell ( Cellhandle cell )
{
  Real volume = abs( Volume_cell(cell) ) - solver->volumeSolidPore(cell);
  return volume;
}

template<class Solver>
Real UnsaturatedEngine::getSaturation (Solver& flow )
{
	RTriangulation& tri = flow->T[flow->currentTes].Triangulation();
	Real capillary_volume = 0.0; //total volume
	Real air_volume = 0.0; 	//air volume
	Finite_cells_iterator cell_end = tri.finite_cells_end();
	for ( Finite_cells_iterator cell = tri.finite_cells_begin(); cell != cell_end; cell++ ) 
	{
	    if (tri.is_infinite(cell)) continue;
	    if (cell->info().Pcondition) continue;
// 	    if (cell.has_vertex() ) 
	    capillary_volume = capillary_volume + capillary_Volume_cell ( cell );
	    if (cell->info().p()!=0)
	    {
		air_volume = air_volume + capillary_Volume_cell (cell);
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
    file << "#List Of Nodes \n";
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
    {
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
    file << "Cell_ID" << " " << "NeighborCell_ID" << " " << "EntryValue" << " " << "Inscribed_Radius" <<endl;
    double surface_tension = surfaceTension ; //Surface Tension in contact with air at 20 Degrees Celsius is:0.0728(N/m)
    Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
    {
	if (flow->T[flow->currentTes].Triangulation().is_infinite(cell)) continue;
	file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << surface_tension/cell->info().pore_radius[0] << " " << cell->info().pore_radius[0] << endl;
        file << cell->info().index << " " <<cell->neighbor(1)->info().index << " " << surface_tension/cell->info().pore_radius[1] << " " << cell->info().pore_radius[1] << endl;
        file << cell->info().index << " " <<cell->neighbor(2)->info().index << " " << surface_tension/cell->info().pore_radius[2] << " " << cell->info().pore_radius[2] << endl;
        file << cell->info().index << " " <<cell->neighbor(3)->info().index << " " << surface_tension/cell->info().pore_radius[3] << " " << cell->info().pore_radius[3] << endl;
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
// 		    cerr<<"dfdsf";
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
// 		    cerr<<"dfdsf";
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
// 		    cerr<<"dfdsf";
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


