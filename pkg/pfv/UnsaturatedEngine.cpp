/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*  Copyright (C) 2012 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE
#include "TwoPhaseFlowEngine.hpp"

//keep this #ifdef for commited versions unless you really have stable version that should be compiled by default
//it will save compilation time for everyone else
//when you want it compiled, you can pass -DTWOPHASEFLOW to cmake, or just uncomment the following line
#ifdef TWOPHASEFLOW




class UnsaturatedEngine : public TwoPhaseFlowEngine
{
	public :
		double totalCellVolume;
		double computeCellInterfacialArea(CellHandle cell, int j, double rC);

// 		void computeSolidLine();

		//record and test functions
		void checkLatticeNodeY(double y);
		//temporary functions
		void initializeCellWindowsID();
		double getWindowsSaturation(int i, bool isSideBoundaryIncluded=false);
		bool checknoCache() {return solver->noCache;}
		double getInvadeDepth();
		double getSphericalSubdomainSaturation(Vector3r pos, double radius);
		double getCuboidSubdomainSaturation(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded);
		double getCuboidSubdomainPorosity(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded);
		double getSpecificInterfacialArea();

		void printSomething();

		boost::python::list getPotentialPendularSpheresPair() {
			RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
			boost::python::list bridgeIds;
			FiniteEdgesIterator ed_it = Tri.finite_edges_begin();
			for ( ; ed_it!=Tri.finite_edges_end(); ed_it++ ) {
			  if (detectBridge(ed_it)==true) {
			    const VertexInfo& vi1=(ed_it->first)->vertex(ed_it->second)->info();
			    const VertexInfo& vi2=(ed_it->first)->vertex(ed_it->third)->info();
			    const int& id1 = vi1.id();
			    const int& id2 = vi2.id();
			    bridgeIds.append(boost::python::make_tuple(id1,id2));}}
			    return bridgeIds;}
  		bool detectBridge(RTriangulation::Finite_edges_iterator& edge);
		
		boost::python::list pyClusters() { boost::python::list ret;
			for(vector<shared_ptr<PhaseCluster> >::iterator it=clusters.begin(); it!=clusters.end(); ++it) ret.append(*it);
			return ret;}
				
		virtual ~UnsaturatedEngine();

		virtual void action();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(UnsaturatedEngine,TwoPhaseFlowEngine,"Preliminary version engine of a drainage model for unsaturated soils. Note:Air reservoir is on the top; water reservoir is on the bottom.(deprecated engine, use TwoPhaseFlowEngine instead)",

		((int, windowsNo, 10,, "Number of genrated windows(or zoomed samples)."))
					,,,
		.def("getSpecificInterfacialArea",&UnsaturatedEngine::getSpecificInterfacialArea,"get specific interfacial area (defined as the amount of fluid-fluid interfacial area per unit volume pf the porous medium).")
		.def("checkLatticeNodeY",&UnsaturatedEngine::checkLatticeNodeY,(boost::python::arg("y")),"Check the slice of lattice nodes for yNormal(y). 0: out of sphere; 1: inside of sphere.")
		.def("getInvadeDepth",&UnsaturatedEngine::getInvadeDepth,"Get NW-phase invasion depth. (the distance from NW-reservoir to front of NW-W interface.)")
		.def("getSphericalSubdomainSaturation",&UnsaturatedEngine::getSphericalSubdomainSaturation,(boost::python::arg("pos"),boost::python::arg("radius")),"Get saturation of spherical subdomain defined by (pos, radius). The subdomain exclude boundary pores.")
		.def("getCuboidSubdomainSaturation",&UnsaturatedEngine::getCuboidSubdomainSaturation,(boost::python::arg("pos1"),boost::python::arg("pos2"),boost::python::arg("isSideBoundaryIncluded")),"Get saturation of cuboid subdomain defined by (pos1,pos2). If isSideBoundaryIncluded=false, the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
		.def("getCuboidSubdomainPorosity",&UnsaturatedEngine::getCuboidSubdomainPorosity,(boost::python::arg("pos1"),boost::python::arg("pos2"),boost::python::arg("isSideBoundaryIncluded")),"Get the porosity of cuboid subdomain defined by (pos1,pos2). If isSideBoundaryIncluded=false, the pores of side boundary are excluded in porosity calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in porosity calculating.")
		.def("checknoCache",&UnsaturatedEngine::checknoCache,"check noCache. (temporary function.)")
		.def("getWindowsSaturation",&UnsaturatedEngine::getWindowsSaturation,(boost::python::arg("windowsID"),boost::python::arg("isSideBoundaryIncluded")), "get saturation of subdomain with windowsID. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
		.def("initializeCellWindowsID",&UnsaturatedEngine::initializeCellWindowsID,"Initialize cell windows index. A temporary function for comparison with experiments, will delete soon")
		.def("printSomething",&UnsaturatedEngine::printSomething,"print debug.")
		.def("getPotentialPendularSpheresPair",&UnsaturatedEngine::getPotentialPendularSpheresPair,"Get the list of sphere ID pairs of potential pendular liquid bridge.")
		.def("getClusters",&UnsaturatedEngine::pyClusters/*,(boost::python::arg("folder")="./VTK")*/,"Get the list of clusters.")
		)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
YADE_PLUGIN((UnsaturatedEngine));

UnsaturatedEngine::~UnsaturatedEngine(){}

/*void UnsaturatedEngine::initialDrainage()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	if (tri.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initialReservoirs();
		initializeCellIndex();//initialize cell index
		computePoreThroatRadius();//save all pore radii before drainage
		computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		computePoreBodyVolume();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
	}
	solver->noCache = true;
}*/

void UnsaturatedEngine::action()
{
/*
  //the drainage is in quasi-static regime, so it can work outside Omega.  
    if ( !isActivated ) return;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if ( (tri.number_of_vertices()==0) || (updateTriangulation) ) {
        cout<< "triangulation is empty: building a new one" << endl;
        scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
        setPositionsBuffer(true);//copy sphere positions in a buffer...
        buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
	initialReservoirs();
        initializeCellIndex();//initialize cell index
        computePoreThroatRadius();//save all pore radii before drainage
        computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
        computePoreBodyVolume();//save capillary volume of all cells, for calculating saturation
        computeSolidLine();//save cell->info().solidLine[j][y]
        solver->noCache = true;
    }
    ///compute drainage
    if (pressureForce) { drainage();}
    
    ///compute force
    if(computeForceActivated){
    computeCapillaryForce();
    Vector3r force;
    FiniteVerticesIterator vertices_end = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
    for ( FiniteVerticesIterator V_it = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++ ) {
        force = pressureForce ? Vector3r ( V_it->info().forces[0],V_it->info().forces[1],V_it->info().forces[2] ): Vector3r(0,0,0);
        scene->forces.addForce ( V_it->info().id(), force); }}
*/}





double UnsaturatedEngine::getSpecificInterfacialArea()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    double interfacialArea=0;

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//             if (cell->info().Pcondition==true) continue;//NOTE:reservoirs cells interfacialArea should not be included.
            if(cell->info().isFictious) continue;
            if (cell->info().isNWRes==true) {
                for (int facet = 0; facet < 4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition==true) continue;
                    if ( (cell->neighbor(facet)->info().isFictious) && (!isInvadeBoundary) ) continue;
                    if (cell->neighbor(facet)->info().isNWRes==false)
                        interfacialArea = interfacialArea + computeCellInterfacialArea(cell, facet, cell->info().poreThroatRadius[facet]);}}}
//     cerr<<"InterArea:"<<interfacialArea<<"  totalCellVolume:"<<totalCellVolume<<endl;
    return interfacialArea/totalCellVolume;
}

double UnsaturatedEngine::computeCellInterfacialArea(CellHandle cell, int j, double rC)
{
    double rInscribe = std::abs(solver->computeEffectiveRadius(cell, j));  
    CellHandle cellh = CellHandle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
    
    if(facetNFictious==0) {
        RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
        if (tri.is_infinite(cell->neighbor(j))) return 0;

        Vector3r pos[3]; //solid pos
        double r[3]; //solid radius
        double rRc[3]; //r[i] + rC (rC: capillary radius)
        double e[3]; //edges of triangulation
        double rad[4][3]; //angle in radian

        for (int i=0; i<3; i++) {
            pos[i] = makeVector3r(cell->vertex(facetVertices[j][i])->point().point());
            r[i] = sqrt(cell->vertex(facetVertices[j][i])->point().weight());
            rRc[i] = r[i]+rC;
        }

        e[0] = (pos[1]-pos[2]).norm();
        e[1] = (pos[2]-pos[0]).norm();
        e[2] = (pos[1]-pos[0]).norm();

        rad[3][0]=acos(((pos[1]-pos[0]).dot(pos[2]-pos[0]))/(e[2]*e[1]));
        rad[3][1]=acos(((pos[2]-pos[1]).dot(pos[0]-pos[1]))/(e[0]*e[2]));
        rad[3][2]=acos(((pos[0]-pos[2]).dot(pos[1]-pos[2]))/(e[1]*e[0]));

        rad[0][0]=computeTriRadian(e[0],rRc[1],rRc[2]);
        rad[0][1]=computeTriRadian(rRc[2],e[0],rRc[1]);
        rad[0][2]=computeTriRadian(rRc[1],rRc[2],e[0]);

        rad[1][0]=computeTriRadian(rRc[2],e[1],rRc[0]);
        rad[1][1]=computeTriRadian(e[1],rRc[0],rRc[2]);
        rad[1][2]=computeTriRadian(rRc[0],rRc[2],e[1]);

        rad[2][0]=computeTriRadian(rRc[1],e[2],rRc[0]);
        rad[2][1]=computeTriRadian(rRc[0],rRc[1],e[2]);
        rad[2][2]=computeTriRadian(e[2],rRc[0],rRc[1]);

        double sW0=0.5*rRc[1]*rRc[2]*sin(rad[0][0])-0.5*rad[0][0]*pow(rC,2)-0.5*rad[0][1]*pow(r[1],2)-0.5*rad[0][2]*pow(r[2],2) ;
        double sW1=0.5*rRc[2]*rRc[0]*sin(rad[1][1])-0.5*rad[1][1]*pow(rC,2)-0.5*rad[1][2]*pow(r[2],2)-0.5*rad[1][0]*pow(r[0],2) ;
        double sW2=0.5*rRc[0]*rRc[1]*sin(rad[2][2])-0.5*rad[2][2]*pow(rC,2)-0.5*rad[2][0]*pow(r[0],2)-0.5*rad[2][1]*pow(r[1],2) ;
        double sW=sW0+sW1+sW2;
        double sVoid=sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
        double sInterface=sVoid-sW;
	return sInterface;
    }
    else {
        return Mathr::PI*pow(rInscribe,2);
    }
}

// ------------------for checking----------
void UnsaturatedEngine::checkLatticeNodeY(double y)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if((y<solver->yMin)||(y>solver->yMax)) {
        cerr<<"y is out of range! "<<"pleas set y between "<<solver->yMin<<" and "<<solver->yMax<<endl;}
    else {
        int N=100;// the default Node number for each slice is 100X100
        ofstream file;
        std::ostringstream fileNameStream(".txt");
        fileNameStream << "LatticeNodeY_"<< y;
        std::string fileName = fileNameStream.str();
        file.open(fileName.c_str());
//     file << "#Slice Of LatticeNodes: 0: out of sphere; 1: inside of sphere  \n";
        double deltaX = (solver->xMax-solver->xMin)/N;
        double deltaZ = (solver->zMax-solver->zMin)/N;
        for (int j=0; j<N+1; j++) {
            for (int k=0; k<N+1; k++) {
                double x=solver->xMin+j*deltaX;
                double z=solver->zMin+k*deltaZ;
                int M=0;
                Vector3r LatticeNode = Vector3r(x,y,z);
                for (FiniteVerticesIterator V_it = tri.finite_vertices_begin(); V_it != tri.finite_vertices_end(); V_it++) {
                    if(V_it->info().isFictious) continue;
                    Vector3r SphereCenter = makeVector3r(V_it->point().point());
                    if ((LatticeNode-SphereCenter).squaredNorm() < V_it->point().weight()) {
                        M=1;
                        break;}}
                file << M;}
            file << "\n";}
        file.close();}
}

void UnsaturatedEngine::printSomething()
{

    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteEdgesIterator ed_it;
    for ( FiniteEdgesIterator ed_it = Tri.finite_edges_begin(); ed_it!=Tri.finite_edges_end();ed_it++ )
    {
      const VertexInfo& vi1=(ed_it->first)->vertex(ed_it->second)->info();
      const VertexInfo& vi2=(ed_it->first)->vertex(ed_it->third)->info();
      const int& id1 = vi1.id();
      const int& id2 = vi2.id();
      cerr<<id1<<" "<<id2<<endl;}
}


//----------temporary functions for comparison with experiment-----------------------
void UnsaturatedEngine::initializeCellWindowsID()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        for (int i=1; i<(windowsNo+1); i++) {
            if ( (cell->info()[1]>(solver->yMin+(i-1)*(solver->yMax-solver->yMin)/windowsNo) ) && (cell->info()[1] < (solver->yMin+i*(solver->yMax-solver->yMin)/windowsNo)) )
            {cell->info().windowsID=i; break;}
        }
    }
}

double UnsaturatedEngine::getWindowsSaturation(int i, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//W-phase volume
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
        if (cell->info().windowsID != i) continue;
        poresVolume = poresVolume + cell->info().poreBodyVolume;
        if (cell->info().saturation>0.0) {
            wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

double UnsaturatedEngine::getCuboidSubdomainSaturation(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//W-phase volume
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
	if ( ((pos1[0]-cell->info()[0])*(pos2[0]-cell->info()[0])<0) && ((pos1[1]-cell->info()[1])*(pos2[1]-cell->info()[1])<0) && ((pos1[2]-cell->info()[2])*(pos2[2]-cell->info()[2])<0) ) {
	  poresVolume = poresVolume + cell->info().poreBodyVolume;
	  if (cell->info().saturation>0.0) {
	    wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
	  }
	}
    }
    return wVolume/poresVolume;
}

double UnsaturatedEngine::getCuboidSubdomainPorosity(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double totalCellVolume = 0.0; 
    double totalVoidVolume = 0.0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
	if ( ((pos1[0]-cell->info()[0])*(pos2[0]-cell->info()[0])<0) && ((pos1[1]-cell->info()[1])*(pos2[1]-cell->info()[1])<0) && ((pos1[2]-cell->info()[2])*(pos2[2]-cell->info()[2])<0) ) {
	  totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );
	  totalVoidVolume = totalVoidVolume + cell->info().poreBodyVolume;
	}
    }
    if(totalVoidVolume==0 || totalCellVolume==0) cerr<<"subdomain too small!"<<endl;
    return totalVoidVolume/totalCellVolume;
}

double UnsaturatedEngine::getInvadeDepth()
{
    double yPosMax=-1e50;
    double yPosMin=1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isNWRes) {
	  yPosMax=std::max(yPosMax,cell->info()[1]);
	  yPosMin=std::min(yPosMin,cell->info()[1]);
        }
    }
    return std::abs(yPosMax-yPosMin);
}

double UnsaturatedEngine::getSphericalSubdomainSaturation(Vector3r pos, double radius)
{
    double poresVolume=0.0;
    double wVolume=0.0;
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = Tri.finite_cells_end();
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
        Vector3r cellPos = makeVector3r(cell->info());
        double dist=(pos-cellPos).norm();
        if(dist>radius) continue;
        if(cell->info().isFictious) {
	  cerr<<"The radius of subdomain is too large, or the center of subdomain is out of packing. Please reset subdomain again."<<endl;
	  return -1;
        }
        poresVolume=poresVolume+cell->info().poreBodyVolume;
        if(cell->info().saturation>0.0) {
            wVolume=wVolume+cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

//--------------end of comparison with experiment----------------------------

#endif //TWOPHASEFLOW
#endif //FLOW_ENGINE
