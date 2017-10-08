/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2013 by T. Sweijen (T.sweijen@uu.nl)                    *
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// This is an example of how to derive a new FlowEngine with additional data and possibly completely new behaviour.
// Every functions of the base engine can be overloaded, and new functions can be added

//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can pass -DTWOPHASEFLOW to cmake, or just uncomment the following line

#include "TwoPhaseFlowEngine.hpp"
#ifdef TWOPHASEFLOW
#include <boost/range/algorithm_ext/erase.hpp>

YADE_PLUGIN((TwoPhaseFlowEngineT));
YADE_PLUGIN((TwoPhaseFlowEngine));
YADE_PLUGIN((PhaseCluster));

PhaseCluster::~PhaseCluster(){}

void TwoPhaseFlowEngine::initialization()
{
// 		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		if(!keepTriangulation){buildTriangulation(0.0,*solver);}//create a triangulation and initialize pressure in the elements (connecting with W-reservoir), everything will be contained in "solver"
// 		initializeCellIndex();//initialize cell index
// 		if(isInvadeBoundary) {computePoreThroatRadius();}
// // 		else {computePoreThroatRadiusTrickyMethod1();}//save pore throat radius before drainage. Thomas, here you can also revert this to computePoreThroatCircleRadius().
		
		// Determine the entry-pressure
		if(entryPressureMethod == 1 && isInvadeBoundary){computePoreThroatRadiusMethod1();} //MS-P method
		else if(entryPressureMethod == 1 && isInvadeBoundary == false){computePoreThroatRadiusTrickyMethod1();} //MS-P method
		else if(entryPressureMethod == 2){computePoreThroatRadiusMethod2();} //Inscribed circle}
		else if(entryPressureMethod == 3){computePoreThroatRadiusMethod3();} //Area equivalent circle}
		else if(entryPressureMethod > 3){cout << endl << "ERROR - Method for determining the entry pressure does not exist";}
		
		computePoreBodyRadius();//save pore body radius before imbibition
		computePoreBodyVolume();//save capillary volume of all cells, for fast calculating saturation. Also save the porosity of each cell.
		computeSolidLine();//save cell->info().solidLine[j][y]
		initializeReservoirs();//initial pressure, reservoir flags and local pore saturation
// 		if(isCellLabelActivated) updateReservoirLabel();
		solver->noCache = true;
}

void TwoPhaseFlowEngine::computePoreBodyVolume()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        cell->info().poreBodyVolume = std::abs( cell->info().volume() ) - std::abs(solver->volumeSolidPore(cell));
	cell->info().porosity = cell->info().poreBodyVolume/std::abs( cell->info().volume() );
    }
}

void TwoPhaseFlowEngine::computePoreThroatRadiusMethod2()
{
  //Calculate the porethroat radii of the inscribed sphere in each pore-body. 
  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
  FiniteCellsIterator cellEnd = tri.finite_cells_end();
  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
    for(unsigned int i = 0; i<4;i++){
    cell->info().poreThroatRadius[i] = std::abs(solver->computeEffectiveRadius(cell,i));
    }
  }
}

void TwoPhaseFlowEngine::computePoreThroatRadiusMethod3()
{
  //Calculate the porethroat radii of the surface equal circle of a throat
  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
  FiniteCellsIterator cellEnd = tri.finite_cells_end();
  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
    for(unsigned int i = 0; i<4;i++){
    cell->info().poreThroatRadius[i] = solver->computeEquivalentRadius(cell,i); 
    }
  }
}

void TwoPhaseFlowEngine::computePoreBodyRadius()
{
  
    // This routine finds the radius of the inscribed sphere within each pore-body
    // Following Mackay et al., 1972. 
      double d01 = 0.0, d02 = 0.0, d03 = 0.0, d12 = 0.0, d13 = 0.0, d23 = 0.0, Rin = 0.0, r0 = 0.0, r1 = 0.0, r2 =0.0, r3 = 0.0;
      bool check = false;
      unsigned int i = 0;
      double dR=0.0, tempR = 0.0;
      bool initialSign = false; //False = negative, true is positive
      bool first = true;
      
      Eigen::MatrixXd M(6,6);
      
      FOREACH(CellHandle& cell, solver->T[solver->currentTes].cellHandles){
	//Distance between multiple particles, can be done more efficient
	d01 = d02 = d03 = d12 = d13 = d23 = r0 = r1 = r2= r3 = 0.0;
	
	d01 = pow((cell->vertex(0)->point().x()-cell->vertex(1)->point().x()),2)+
	pow((cell->vertex(0)->point().y()-cell->vertex(1)->point().y()),2)+
	pow((cell->vertex(0)->point().z()-cell->vertex(1)->point().z()),2);
	
	d02 = pow((cell->vertex(0)->point().x()-cell->vertex(2)->point().x()),2)+
	pow((cell->vertex(0)->point().y()-cell->vertex(2)->point().y()),2)+
	pow((cell->vertex(0)->point().z()-cell->vertex(2)->point().z()),2);
	
	d03 = pow((cell->vertex(0)->point().x()-cell->vertex(3)->point().x()),2)+
	pow((cell->vertex(0)->point().y()-cell->vertex(3)->point().y()),2)+
	pow((cell->vertex(0)->point().z()-cell->vertex(3)->point().z()),2);
	
	d12 =pow((cell->vertex(1)->point().x()-cell->vertex(2)->point().x()),2)+
	pow((cell->vertex(1)->point().y()-cell->vertex(2)->point().y()),2)+
	pow((cell->vertex(1)->point().z()-cell->vertex(2)->point().z()),2);
	
	d13 = pow((cell->vertex(1)->point().x()-cell->vertex(3)->point().x()),2)+
	pow((cell->vertex(1)->point().y()-cell->vertex(3)->point().y()),2)+
	pow((cell->vertex(1)->point().z()-cell->vertex(3)->point().z()),2);
	
	d23 = pow((cell->vertex(2)->point().x()-cell->vertex(3)->point().x()),2)+
	pow((cell->vertex(2)->point().y()-cell->vertex(3)->point().y()),2)+
	pow((cell->vertex(2)->point().z()-cell->vertex(3)->point().z()),2);
	
	//Radii of the particles
	r0 = sqrt(cell -> vertex(0) -> point().weight());
	r1 = sqrt(cell -> vertex(1) -> point().weight());
	r2 = sqrt(cell -> vertex(2) -> point().weight());
	r3 = sqrt(cell -> vertex(3) -> point().weight());	
	
	//Fill coefficient matrix
	M(0,0) = 0.0; M(1,0) = d01; M(2,0) = d02; M(3,0) = d03; M(4,0) = pow((r0+Rin),2); M(5,0) = 1.0;
	M(0,1) = d01; M(1,1) = 0.0; M(2,1) = d12; M(3,1) = d13; M(4,1) = pow((r1+Rin),2); M(5,1) = 1.0;
	M(0,2) = d02; M(1,2) = d12; M(2,2) = 0.0; M(3,2) = d23; M(4,2) = pow((r2+Rin),2); M(5,2) = 1.0;
	M(0,3) = d03; M(1,3) = d13; M(2,3) = d23; M(3,3) = 0.0; M(4,3) = pow((r3+Rin),2); M(5,3) = 1.0;
	M(0,4) = pow((r0+Rin),2); M(1,4) = pow((r1+Rin),2); M(2,4) = pow((r2+Rin),2); M(3,4) = pow((r3+Rin),2); M(4,4) = 0.0; M(5,4) = 1.0;
	M(0,5) = 1.0; M(1,5) = 1.0; M(2,5) = 1.0; M(3,5) = 1.0; M(4,5) = 1.0; M(5,5) = 0.0;

	i = 0;
	check = false;
	dR = Rin = 0.0 + (min(r0,min(r1,min(r2,r3))) / 50.0); //Estimate an initial dR
	first = true;
	//Iterate untill check = true, such that an accurate answer as been found
	while (check == false){
	  i = i + 1;
	  tempR = Rin;
	  Rin = Rin + dR;
	
	  M(4,0) = pow((r0+Rin),2);
	  M(4,1) = pow((r1+Rin),2);
	  M(4,2) = pow((r2+Rin),2);
	  M(4,3) = pow((r3+Rin),2);
	  M(0,4) = pow((r0+Rin),2);
	  M(1,4) = pow((r1+Rin),2);
	  M(2,4) = pow((r2+Rin),2);
	  M(3,4) = pow((r3+Rin),2);	
	
	  if (first){
	    first = false;
	    if(M.determinant() < 0.0){initialSign = false;} //Initial D is negative
	    if(M.determinant() > 0.0){initialSign = true;} // Initial D is positive
	  }
	
	  if(std::abs(M.determinant()) < 1E-30){check = true;}	
	
	  if((initialSign==true) && (check ==false)){
	    if(M.determinant() < 0.0){
	      Rin = Rin -dR;
	      dR = dR / 2.0; 
	    }	  
	  }
	
	  if((initialSign==false) && (check ==false)){
	    if(M.determinant() > 0.0){
	      Rin = Rin -dR;
	      dR = dR / 2.0;  
	    }	  
	  }
	
	  if(solver->debugOut) {cout << endl << i << " "<<Rin << " "<< dR << " "<< M.determinant();}
	  if(i > 4000){
	    cout << endl << "error, finding solution takes too long cell:" << cell->info().id;
	    check = true;
	  }
	  if ( std::abs(tempR - Rin)/Rin < 0.001){check = true;}
	
      }
    cell -> info().poreBodyRadius = Rin;
   }
}

void TwoPhaseFlowEngine::computePoreThroatRadiusMethod1()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        for (int j=0; j<4; j++) {
            neighbourCell = cell->neighbor(j);
            if (!tri.is_infinite(neighbourCell)) {
                cell->info().poreThroatRadius[j]=computeEffPoreThroatRadius(cell, j);
                neighbourCell->info().poreThroatRadius[tri.mirror_index(cell, j)]= cell->info().poreThroatRadius[j];}}}
}
double TwoPhaseFlowEngine::computeEffPoreThroatRadius(CellHandle cell, int j)
{
    double rInscribe = std::abs(solver->computeEffectiveRadius(cell, j));
    CellHandle cellh = CellHandle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
    double r;
    if(facetNFictious==0) {r=computeEffPoreThroatRadiusFine(cell,j);}
    else r=rInscribe;    
    return r;
}
double TwoPhaseFlowEngine::computeEffPoreThroatRadiusFine(CellHandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r pos[3]; //solid pos
    double r[3]; //solid radius
    
    for (int i=0; i<3; i++) {
      pos[i] = makeVector3r(cell->vertex(facetVertices[j][i])->point().point());
      r[i] = sqrt(cell->vertex(facetVertices[j][i])->point().weight());
    }
    return computeMSPRcByPosRadius(pos[0],r[0],pos[1],r[1],pos[2],r[2]);
}   
double TwoPhaseFlowEngine::computeMSPRcByPosRadius(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC)
{
    double e[3]; //edges of triangulation
    double g[3]; //gap radius between solid
        
    e[0] = (posB-posC).norm();
    e[1] = (posC-posA).norm();
    e[2] = (posB-posA).norm();
    g[0] = ((e[0]-rB-rC)>0) ? 0.5*(e[0]-rB-rC):0 ;
    g[1] = ((e[1]-rC-rA)>0) ? 0.5*(e[1]-rC-rA):0 ;
    g[2] = ((e[2]-rA-rB)>0) ? 0.5*(e[2]-rA-rB):0 ;
    
    double rmin= (std::max(g[0],std::max(g[1],g[2]))==0) ? 1.0e-11:std::max(g[0],std::max(g[1],g[2])) ;
    double rmax= computeEffRcByPosRadius(posA, rA, posB, rB, posC, rC);
    if(rmin>rmax) { cerr<<"WARNING! rmin>rmax. rmin="<<rmin<<" ,rmax="<<rmax<<endl; }
    
    double deltaForceRMin = computeDeltaForce(posA, rA, posB, rB, posC, rC, rmin);
    double deltaForceRMax = computeDeltaForce(posA, rA, posB, rB, posC, rC, rmax);
    double effPoreRadius;
    
    if(deltaForceRMin>deltaForceRMax) { effPoreRadius=rmax; }
    else if(deltaForceRMax<0) { effPoreRadius=rmax; }
    else if(deltaForceRMin>0) { effPoreRadius=rmin; }
    else { effPoreRadius=bisection(posA, rA, posB, rB, posC, rC, rmin,rmax); }
    return effPoreRadius;
}
double TwoPhaseFlowEngine::bisection(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC, double a, double b)
{
    double m = 0.5*(a+b);
    if (std::abs(b-a)>computeEffRcByPosRadius(posA, rA, posB, rB, posC, rC)*1.0e-6) {
        if ( computeDeltaForce(posA, rA, posB, rB, posC, rC,m) * computeDeltaForce(posA, rA, posB, rB, posC, rC,a) < 0 ) {
            b = m; return bisection(posA, rA, posB, rB, posC, rC, a,b);}
        else {a = m; return bisection(posA, rA, posB, rB, posC, rC, a,b);}
    }
    else {return m;}
}
double TwoPhaseFlowEngine::computeDeltaForce(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC, double r)
{
    double rRc[3]; //r[i] + r (r: capillary radius)
    double e[3]; //edges of triangulation
    double rad[4][3]; //angle in radian

    rRc[0] = rA+r;
    rRc[1] = rB+r;
    rRc[2] = rC+r;

    e[0] = (posB-posC).norm();
    e[1] = (posC-posA).norm();
    e[2] = (posB-posA).norm();

    rad[3][0]=acos(((posB-posA).dot(posC-posA))/(e[2]*e[1]));
    rad[3][1]=acos(((posC-posB).dot(posA-posB))/(e[0]*e[2]));
    rad[3][2]=acos(((posA-posC).dot(posB-posC))/(e[1]*e[0]));

    rad[0][0]=computeTriRadian(e[0],rRc[1],rRc[2]);
    rad[0][1]=computeTriRadian(rRc[2],e[0],rRc[1]);
    rad[0][2]=computeTriRadian(rRc[1],rRc[2],e[0]);

    rad[1][0]=computeTriRadian(rRc[2],e[1],rRc[0]);
    rad[1][1]=computeTriRadian(e[1],rRc[0],rRc[2]);
    rad[1][2]=computeTriRadian(rRc[0],rRc[2],e[1]);

    rad[2][0]=computeTriRadian(rRc[1],e[2],rRc[0]);
    rad[2][1]=computeTriRadian(rRc[0],rRc[1],e[2]);
    rad[2][2]=computeTriRadian(e[2],rRc[0],rRc[1]);

    double lNW = (rad[0][0]+rad[1][1]+rad[2][2])*r;
    double lNS = (rad[3][0]-rad[1][0]-rad[2][0])*rA + (rad[3][1]-rad[2][1]-rad[0][1])*rB + (rad[3][2]-rad[1][2]-rad[0][2])*rC ;
    double lInterface=lNW+lNS;

    double sW0=0.5*rRc[1]*rRc[2]*sin(rad[0][0])-0.5*rad[0][0]*pow(r,2)-0.5*rad[0][1]*pow(rB,2)-0.5*rad[0][2]*pow(rC,2) ;
    double sW1=0.5*rRc[2]*rRc[0]*sin(rad[1][1])-0.5*rad[1][1]*pow(r,2)-0.5*rad[1][2]*pow(rC,2)-0.5*rad[1][0]*pow(rA,2) ;
    double sW2=0.5*rRc[0]*rRc[1]*sin(rad[2][2])-0.5*rad[2][2]*pow(r,2)-0.5*rad[2][0]*pow(rA,2)-0.5*rad[2][1]*pow(rB,2) ;
    double sW=sW0+sW1+sW2;

    CVector facetSurface = 0.5*CGAL::cross_product(makeCgVect(posA-posC),makeCgVect(posB-posC));
    double sVoid = sqrt(facetSurface.squared_length()) - (0.5*rad[3][0]*pow(rA,2) + 0.5*rad[3][1]*pow(rB,2) + 0.5*rad[3][2]*pow(rC,2));
    double sInterface=sVoid-sW;

    double deltaF = lInterface - sInterface/r;//deltaF=surfaceTension*(perimeterPore - areaPore/rCap)
    return deltaF;
}
//calculate radian with law of cosines. (solve $\alpha$)
double TwoPhaseFlowEngine::computeTriRadian(double a, double b, double c)
{   
  double cosAlpha = (pow(b,2) + pow(c,2) - pow(a,2))/(2*b*c);
  if (cosAlpha>1.0) {cosAlpha=1.0;} if (cosAlpha<-1.0) {cosAlpha=-1.0;}
  double alpha = acos(cosAlpha);
  return alpha;
}

void TwoPhaseFlowEngine::savePhaseVtk(const char* folder)
{
// 	RTriangulation& Tri = T[solver->noCache?(!currentTes):currentTes].Triangulation();
	RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
        static unsigned int number = 0;
        char filename[80];
	mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"%s/out_%d.vtk",folder,number++);
	int firstReal=-1;

	//count fictious vertices and cells
	solver->vtkInfiniteVertices=solver->vtkInfiniteCells=0;
 	FiniteCellsIterator cellEnd = Tri.finite_cells_end();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (!isDrawable) solver->vtkInfiniteCells+=1;
	}
	for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (!v->info().isReal()) solver->vtkInfiniteVertices+=1;
                else if (firstReal==-1) firstReal=solver->vtkInfiniteVertices;}

        basicVTKwritter vtkfile((unsigned int) Tri.number_of_vertices()-solver->vtkInfiniteVertices, (unsigned int) Tri.number_of_finite_cells()-solver->vtkInfiniteCells);

        vtkfile.open(filename,"test");

        vtkfile.begin_vertices();
        double x,y,z;
        for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (v->info().isReal()){
		x = (double)(v->point().point()[0]);
                y = (double)(v->point().point()[1]);
                z = (double)(v->point().point()[2]);
                vtkfile.write_point(x,y,z);}
        }
        vtkfile.end_vertices();

        vtkfile.begin_cells();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
        	if (isDrawable){vtkfile.write_cell(cell->vertex(0)->info().id()-firstReal, cell->vertex(1)->info().id()-firstReal, cell->vertex(2)->info().id()-firstReal, cell->vertex(3)->info().id()-firstReal);}
        }
        vtkfile.end_cells();

	vtkfile.begin_data("Saturation",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().saturation);}
	}
	vtkfile.end_data();
	
	vtkfile.begin_data("HasInterface",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().hasInterface);}
	}
	vtkfile.end_data();
	
	vtkfile.begin_data("Pcondition",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().Pcondition);}
	}
	vtkfile.end_data();

	vtkfile.begin_data("flux",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().flux);}
	}
	vtkfile.end_data();

	vtkfile.begin_data("mergedID",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().mergedID);}
	}
	vtkfile.end_data();

	vtkfile.begin_data("deltaVolume",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().accumulativeDV);}
	}
	vtkfile.end_data();

	vtkfile.begin_data("Porosity",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().porosity);}
	}
	vtkfile.end_data();

	vtkfile.begin_data("Label",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().label);}
	}
	vtkfile.end_data();
}

void TwoPhaseFlowEngine::computePoreThroatRadiusTrickyMethod1()
{
  computePoreThroatRadiusMethod1();
  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
  FiniteCellsIterator cellEnd = tri.finite_cells_end();
  CellHandle neighbourCell;
  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
      for (int j=0; j<4; j++) {
          neighbourCell = cell->neighbor(j);
	  if(cell->info().isFictious && neighbourCell->info().isFictious)
	  {cell->info().poreThroatRadius[j]=-1.0;
	  neighbourCell->info().poreThroatRadius[tri.mirror_index(cell, j)]= cell->info().poreThroatRadius[j];}
      }
  }
}

void TwoPhaseFlowEngine::computeSolidLine()
{
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = Tri.finite_cells_end();
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
        for(int j=0; j<4; j++) {
            solver->lineSolidPore(cell, j);
        }
    }
    if(solver->debugOut) {cout<<"----computeSolidLine-----."<<endl;}
}

void TwoPhaseFlowEngine::initializeReservoirs()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    ///keep boundingCells[2] as W-reservoir.
    for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
        (*it)->info().isWRes = true;
        (*it)->info().isNWRes = false;
        (*it)->info().saturation=1.0;
    }
    ///keep boundingCells[3] as NW-reservoir.
    for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
        (*it)->info().isNWRes = true;
        (*it)->info().isWRes = false;
        (*it)->info().saturation=0.0;
    }

    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    ///if we start from drainage
    if(drainageFirst)
    {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().Pcondition) continue;
	    cell->info().p()=bndCondValue[2];
            cell->info().isWRes = true;
            cell->info().isNWRes= false;
            cell->info().saturation=1.0;
        }
    }
    ///if we start from imbibition
    if(!drainageFirst)
    {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().Pcondition) continue;
	    cell->info().p()=bndCondValue[3];
            cell->info().isWRes = false;
            cell->info().isNWRes= true;
            cell->info().saturation=0.0;
        }
    }
    if(solver->debugOut) {cout<<"----initializeReservoirs----"<<endl;}    
}


void TwoPhaseFlowEngine::savePoreNetwork(const char* folder)
{
    //Open relevant files
    std::ofstream filePoreBodyRadius;
    std::cout << "Opening File: " << "PoreBodyRadius" << std::endl;
    mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    filePoreBodyRadius.open(string(folder) + "/PoreBodyRadius.txt", std::ios::trunc);
    if(!filePoreBodyRadius.is_open()){
        std::cerr<< "Error opening file [" << "PoreBodyRadius" << ']' << std::endl;
        return;
    }
    
    std::ofstream filePoreBoundary;
    std::cout << "Opening File: " << "PoreBoundary" << std::endl;
    filePoreBoundary.open(string(folder) + "/PoreBoundaryIndex.txt" , std::ios::trunc);
    if(!filePoreBoundary.is_open()){
        std::cerr<< "Error opening file [" << "PoreBoundary" << ']' << std::endl;
        return;
    }
    
    std::ofstream filePoreBodyVolume;
    std::cout << "Opening File: " << "PoreBodyVolume" << std::endl;
    filePoreBodyVolume.open(string(folder) + "/PoreBodyVolume.txt", std::ios::trunc);
    if(!filePoreBodyVolume.is_open()){
        std::cerr<< "Error opening file [" << "PoreBodyVolume" << ']' << std::endl;
        return;
    }
    std::ofstream fileLocation;
    std::cout << "Opening File: " << "Location" << std::endl;
    fileLocation.open(string(folder) + "/PoreBodyLocation.txt", std::ios::trunc);
    if(!fileLocation.is_open()){
        std::cerr<< "Error opening file [" << "fileLocation" << ']' << std::endl;
        return;
    }
    
    std::ofstream fileNeighbor;
    std::cout << "Opening File: " << "fileNeighbor" << std::endl;
    fileNeighbor.open(string(folder) + "/PoreBodyNeighbor.txt", std::ios::trunc);
    if(!fileNeighbor.is_open()){
        std::cerr<< "Error opening file [" << "fileNeighbor" << ']' << std::endl;
        return;
    }
    
    std::ofstream fileThroatRadius;
    std::cout << "Opening File: " << "fileThroatRadius" << std::endl;
    fileThroatRadius.open(string(folder) + "/throatRadius.txt", std::ios::trunc);
    if(!fileThroatRadius.is_open()){
        std::cerr<< "Error opening file [" << "fileThroatRadius" << ']' << std::endl;
        return;
    }
    std::ofstream fileThroats;
    std::cout << "Opening File: " << "fileThroats" << std::endl;
    fileThroats.open(string(folder) + "/throatConnectivityPoreBodies.txt", std::ios::trunc);
    if(!fileThroats.is_open()){
        std::cerr<< "Error opening file [" << "fileThroats" << ']' << std::endl;
        return;
    }
    std::ofstream fileThroatFluidArea;
    std::cout << "Opening File: " << "fileThroatFluidArea" << std::endl;
    fileThroatFluidArea.open(string(folder) + "/fileThroatFluidArea.txt", std::ios::trunc);
    if(!fileThroatFluidArea.is_open()){
        std::cerr<< "Error opening file [" << "fileThroatFluidArea" << ']' << std::endl;
        return;
    }
    std::ofstream fileHydraulicRadius;
    std::cout << "Opening File: " << "fileHydraulicRadius" << std::endl;
    fileHydraulicRadius.open(string(folder) + "/fileHydraulicRadius.txt", std::ios::trunc);
    if(!fileHydraulicRadius.is_open()){
        std::cerr<< "Error opening file [" << "fileHydraulicRadius" << ']' << std::endl;
        return;
    }
    std::ofstream fileConductivity;
    std::cout << "Opening File: " << "fileConductivity" << std::endl;
    fileConductivity.open(string(folder) + "/fileConductivity.txt", std::ios::trunc);
    if(!fileConductivity.is_open()){
        std::cerr<< "Error opening file [" << "fileConductivity" << ']' << std::endl;
        return;
    }
    
    //Extract pore network based on triangulation
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
      if(cell->info().isGhost == false && cell->info().id < solver->T[solver->currentTes].cellHandles.size()){
      filePoreBodyRadius << cell->info().poreBodyRadius << '\n';
      filePoreBodyVolume << cell->info().poreBodyRadius << '\n';
      CVector center ( 0,0,0 );
      double count = 0.0;
      for ( int k=0;k<4;k++ ){ 
	  if(cell->vertex(k)->info().id() > 5){
	    center= center + (cell->vertex(k)->point()-CGAL::ORIGIN);
	    count = count + 1.0;
	  }
      }
      if(count != 0.0){center = center * (1./count);}
      fileLocation  << center<< '\n';
      for (unsigned int i=0;i<4;i++){
        if(cell->neighbor(i)->info().isGhost == false && cell->neighbor(i)->info().id < solver->T[solver->currentTes].cellHandles.size() && (cell->info().id < cell->neighbor(i)->info().id)){
	   fileNeighbor << cell->neighbor(i)->info().id << '\n';
	   fileThroatRadius << cell->info().poreThroatRadius[i] << '\n';
	   fileThroats << cell->info().id << " " << cell->neighbor(i)->info().id << '\n';
	   const CVector& Surfk = cell->info().facetSurfaces[i];
	   Real area = sqrt(Surfk.squared_length());
	   fileThroatFluidArea << cell->info().facetFluidSurfacesRatio[i] * area << '\n';
	   fileHydraulicRadius << 2.0 * solver->computeHydraulicRadius(cell,i) << '\n';
	   fileConductivity << cell->info().kNorm()[i] << '\n';
	 }
      }

      if(cell->info().isFictious == 1 && cell->info().isGhost == false && cell->info().id < solver->T[solver->currentTes].cellHandles.size()){
	//add boundary condition
	 if(cell->info().isFictious == 1 &&(cell->vertex(0)->info().id() == 3 || cell->vertex(1)->info().id() == 3  || cell->vertex(2)->info().id() == 3  || cell->vertex(3)->info().id() == 3)){
	   filePoreBoundary << "3" << '\n';
	}
	 else if(cell->info().isFictious == 1 &&(cell->vertex(0)->info().id() == 2 || cell->vertex(1)->info().id() == 2  || cell->vertex(2)->info().id() == 2  || cell->vertex(3)->info().id() == 2)){
	   filePoreBoundary << "2" << '\n';
	}
	  else{filePoreBoundary << "2" << '\n';}
      }
      if(cell->info().isFictious == 0 && cell->info().isGhost == false && cell->info().id < solver->T[solver->currentTes].cellHandles.size()){
	filePoreBoundary << "0" << '\n';       
      }
     }
    }
    fileThroatFluidArea.close(); 
    fileHydraulicRadius.close(); 
    fileConductivity.close(); 
    filePoreBodyRadius.close();
    filePoreBoundary.close();
    filePoreBodyVolume.close();
    fileLocation.close();
    fileNeighbor.close();
    fileThroatRadius.close();

}


 
 
 
 
 
 
 
 
 
 
 
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//..............................................................Library............................................................//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double TwoPhaseFlowEngine::getKappa(int numberFacets)
{
  if(numberFacets == 0){return 0; cout << endl << "Pore with zero throats? Check your data";}
  else{
    double kappa = 0.0; 
    	if(numberFacets == 4){kappa = 3.8716;}//Tetrahedra
	else if(numberFacets == 6){kappa = 8.7067;}//Octahedra
	else if(numberFacets == 8){kappa = 6.7419;}//Cube
	else if(numberFacets == 10){kappa = 5.150;}//Octahedron + hexahedron
	else if(numberFacets == 12){kappa = 24.105;} //Icosahedra
  	else if(numberFacets == 20){kappa = 22.866;} // Dodecahedron
	else{ kappa = 1.2591*float(numberFacets) - 1.1041;} //Other pore shapes
	return kappa;
  }
}

double TwoPhaseFlowEngine::getChi(int numberFacets)
{  
  
    if(numberFacets == 0){return 0; cout << endl << "Pore with zero throats? Check your data";}
    else{
      double chi = 0.0;
	if(numberFacets == 4){chi = 0.416;}//Tetrahedra
	else if(numberFacets == 6){chi = 0.525;}//Octahedra
	else if(numberFacets == 8){chi = 0.500;}//Cube
	else if(numberFacets == 10){chi = 0.4396;}//Octahedron + hexahedron
	else if(numberFacets == 12){chi =0.583; } //Icosahedra
  	else if(numberFacets == 20){chi = 0.565;} // Dodecahedron
	else{ chi = 0.0893*std::log(float(numberFacets))+0.326;}//Other pore shapes
	return chi;
  }
}


double TwoPhaseFlowEngine::getLambda(int numberFacets)
{ 
    
    if(numberFacets == 0){return 0; cout << endl << "Pore with zero throats? Check your data";}
    else{
      double lambda = 0.0;
	if(numberFacets == 4){lambda = 2.0396;}//Tetrahedra
	else if(numberFacets == 6){lambda = 1.2849;}//Octahedra
	else if(numberFacets == 8){lambda = 1;}//Cube
	else if(numberFacets == 10){lambda = 0.77102;}//Octahedron + hexahedron
	else if(numberFacets == 12){lambda =0.771025; } //Icosahedra
  	else if(numberFacets == 20){lambda = 0.50722;} // Dodecahedron
	else{ lambda = 7.12*std::pow(numberFacets,-0.89);} //Other pore shapes
	return lambda;
  }

}

double TwoPhaseFlowEngine::getN(int numberFacets)
{
      if(numberFacets == 0){return 0; cout << endl << "Pore with zero throats? Check your data";}
      else{
      double n = 0.0;
	if(numberFacets == 4){n = 6.0;}//Tetrahedra
	else if(numberFacets == 6){n = 12.0;}//Octahedra
	else if(numberFacets == 8){n = 8.0;}//Cube
	else if(numberFacets == 10){n = 12.0; /*cout << endl << "number of edges requested for octa + hexahedron!";*/ }//Octahedron + hexahedron NOTE this should not be requested for calculations!
	else if(numberFacets == 12){n = 30.0; } //Icosahedra
  	else if(numberFacets == 20){n = 30.0;} // Dodecahedron
	else{ n = 1.63 * double(numberFacets);} //Other pore shapes
	return n;
  }
}

double TwoPhaseFlowEngine::getDihedralAngle(int numberFacets)
{ //given in radians which is reported as tetha in manuscript Sweijen et al., 
    if(numberFacets == 0){return 0; cout << endl << "Pore with zero throats? Check your data";}
      else{
      double DihedralAngle = 0.0;
	if(numberFacets == 4){DihedralAngle = 1.0*std::atan(2.0*std::sqrt(2.0));}//Tetrahedra
	else if(numberFacets == 6){DihedralAngle = 1.0*std::acos(-1.0 / 3.0);}//Octahedra
	else if(numberFacets == 8){DihedralAngle = 0.5*3.1415926535;}//Cube
	else if(numberFacets == 10){DihedralAngle = (1./4.)*3.1415926535; /*cout << endl << "dihedral angle requested for octa + hexahedron!";*/ }//Octahedron + hexahedron NOTE this should not be requested for calculations!
	else if(numberFacets == 12){DihedralAngle = std::acos((-1.0 / 3.0)*std::sqrt(5.0)); } //Icosahedra
  	else if(numberFacets == 20){DihedralAngle = std::acos((-1.0 / 5.0)*std::sqrt(5.0));} // Dodecahedron
	else{ DihedralAngle = (1./4.)*3.1415926535;} //Other pore shapes
	return DihedralAngle;
  }
}

double TwoPhaseFlowEngine::getConstantC3(CellHandle cell)
{
    double c1 = 54.92*std::pow(double(cell->info().numberFacets),-1.14);
    if(cell->info().numberFacets == 4){c1 = 8.291;}
    if(cell->info().numberFacets == 6){c1 = 2.524;}
    if(cell->info().numberFacets == 8){c1 = 2.524;}
    if(cell->info().numberFacets == 10){c1 = 6.532;}
    if(cell->info().numberFacets == 12){c1 = 6.087;}
    if(cell->info().numberFacets == 20){c1 = 0.394;}
    
    
    double c3 = c1 * std::pow(2.0 * surfaceTension,3) / cell->info().mergedVolume;
    return c3; 
}

double TwoPhaseFlowEngine::getConstantC4(CellHandle cell)
{
   double c2 = 4.85*std::pow(double(cell->info().numberFacets),-1.19);
    if(cell->info().numberFacets == 4){c2 = 1.409;}
    if(cell->info().numberFacets == 6){c2 = 0.353;}
    if(cell->info().numberFacets == 8){c2 = 0.644;}
    if(cell->info().numberFacets == 10){c2 = 0.462 ;}
    if(cell->info().numberFacets == 12){c2 = 0.0989;}
    if(cell->info().numberFacets == 20){c2 = 0.245;}
   double c4 = c2 * std::pow(2.0 * surfaceTension,3) / std::pow( double (cell->info().mergedVolume),2./3.);
   return c4; 
}

double TwoPhaseFlowEngine::dsdp(CellHandle cell, double pw)
{
  
    
      if(pw == 0){std::cout << endl << "Error! water pressure is zero, while computing capillary pressure ... cellId= "<< cell->info().id;}
      double exp = std::exp(-1*getKappa(cell->info().numberFacets) * cell->info().saturation);
      double dsdp2 = (1.0 / cell->info().thresholdPressure) * std::pow((1.0 - exp),2.0) / ( getKappa(cell->info().numberFacets) * exp);
//       if(std::abs(dsdp2) > 1e10){ std::cerr << "Huge dsdp! : "<< dsdp2 << " " << exp << " "<< cell->info().thresholdPressure << " " << getKappa(cell->info().numberFacets);}

    
//     double dsdp2 = (3.0 * getConstantC3(cell) - 2.0 * getConstantC4(cell) * pw) / std::pow(pw,4);
    if(dsdp2 != dsdp2){std::cerr<<endl << "Error! sat in dsdp is nan: " << cell->info().saturation << " kappa:" <<getKappa(cell->info().numberFacets) << " exp: " << exp <<   " mergedVolume=" << cell->info().mergedVolume  << " pthreshold=" << cell->info().thresholdPressure;}
    if(dsdp2 < 0.0){std::cerr<<endl<< "Error! dsdp is negative!" << dsdp2; dsdp2 = 0.0;}
//     if(dsdp2 >1e6){std::cerr<<endl<< "Error! dsdp is huge!" << dsdp2; dsdp2 = 1e6;}
    return dsdp2;
}

double TwoPhaseFlowEngine::poreSaturationFromPcS(CellHandle cell,double pw)
{
    //Using equation: Pc = 2*surfaceTension / (Chi * PoreBodyVolume^(1/3) * (1-exp(-kappa * S)))
  double s = truncationPrecision;
     if(-1*pw > cell->info().thresholdPressure){
       s = std::log(1.0 + cell->info().thresholdPressure / pw) / (-1.0 * getKappa(cell->info().numberFacets));
     }
     if(-1*pw == cell->info().thresholdPressure){
	s = cell->info().thresholdSaturation;
     }
     if(-1*pw < cell->info().thresholdPressure){
	if(!remesh && !firstDynTPF){std::cerr<<endl<< "Error! Requesting saturation while capillary pressure is below threshold value? " << pw << " " << cell->info().thresholdPressure;}
	s = cell->info().thresholdSaturation;
     }

     if(s > 1.0 || s < 0.0){std::cout << "Error, saturation from Pc(S) curve is not correct: "<< s << " "<<  cell->info().poreId << " log:" << std::log(1.0 + cell->info().thresholdPressure / pw)<< " " << (-1.0 * getKappa(cell->info().numberFacets)) << " pw=" << pw << " " << cell->info().thresholdPressure; s = 1.0;}
     if(s != s){std::cerr<<endl << "Error! sat in PcS is nan: " << s << "  " << pw << " " << getConstantC4(cell) << " " << getConstantC3(cell) << " mergedVolume=" << cell->info().mergedVolume << " pthreshold=" << cell->info().thresholdPressure;}
  return s;
}


double TwoPhaseFlowEngine::porePressureFromPcS(CellHandle cell,double saturation)
{
   
  double pw = -1.0 * cell->info().thresholdPressure / (1.0 - std::exp(-1*getKappa(cell->info().numberFacets) * cell->info().saturation));
  if(std::exp(-1*getKappa(cell->info().numberFacets) * cell->info().saturation) == 1.0){std::cerr << endl << "Error! pw = -inf!"  << cell->info().saturation;}
  if(pw > 0){
    std::cout << "Pw is above 0! - error: "<< pw << " id=" << cell->info().poreId << " pthr=" << cell->info().thresholdPressure << " sat:" << cell->info().saturation << " kappa: " << getKappa(cell->info().numberFacets) << " " << (1.0 - std::exp(-1*getKappa(cell->info().numberFacets) * cell->info().saturation));
    pw = -1 * cell->info().thresholdPressure;
  }
  if(pw != pw){std::cout << "Non existing capillary pressure!";}
//   if(pw < 100 * waterBoundaryPressure){std::cout << "huge PC!" << pw << " saturation=" << cell->info().saturation << " hasIFace=" << cell->info().hasInterface << " NWRES=" << cell->info().isNWRes; pw = 100 * waterBoundaryPressure;}
  return pw; 
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....................................Merging of tetrahedra to find PUA............... ............................................//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TwoPhaseFlowEngine::actionMergingAlgorithm()
{
  //(1) merge tetrahedra together
  mergeCells();
  //(2) count the facets and update the mergedVolume
  countFacets();
  computeMergedVolumes();
  //(3) resolve unsolved pore throats that are too big
  adjustUnresolvedPoreThroatsAfterMerging();
  //(4) export statistics on the merging algorithm
  getMergedCellStats();
  //(5) check for volume conservation
  checkVolumeConservationAfterMergingAlgorithm();
}




void TwoPhaseFlowEngine::mergeCells()
{
      //This function finds the tetrahedra that belong together (i.e. pore throat is too big compared to pore body)
      //Start with worst case scenario, Rij / Ri > 200 (defined as criterion)towards maximumRatioPoreThroatoverPoreBody.
      //If Rij/Ri is too large, than tetrahedra are merged. Then the merged volume and nrFacets is updated. 
      //When checkign a subsequent criterion, the updated values of merged volume and nr of facets is used. 
      //The remaining unsolved Rij/Ri ratios are fixed later in the program. 
      //A limitation of max 20 merged tetrahedra is used to prevent huge pores. 
  
      int number = 1, ID = 0;
      double  dC = 0.0, criterion = 200.0;
      bool check = false;
      RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
      FiniteCellsIterator cellEnd = tri.finite_cells_end();
      maxIDMergedCells = 0;
      
      //Initialize Merged Volumes
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      cell->info().mergedVolume = cell->info().poreBodyVolume;
      cell->info().mergednr = 1;
      cell->info().mergedID = 0;
      cell->info().numberFacets = 4;
      }
      for (unsigned int i=0; i< 110; i++){
	if(i < 10){dC = (200.0 - 50.0) / 9.0;}
	if(i >=10){dC = (50.0 - maximumRatioPoreThroatoverPoreBody) / 100.0;}
	if(i == 0){dC = 0.0;}
	//Decrease the criteria for throat over body radius, so first merge the worst case scenarios.
	criterion = criterion - dC;
	if(debugTPF){cout << endl << "criterion=" << criterion;}
	for (unsigned int j = 0;j<5;j++){
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	  if(cell->info().isGhost == false && cell->info().mergedID <solver->T[solver->currentTes].cellHandles.size() && cell->info().isFictious == false && cell->info().mergednr < 20)
	  {
	 
	  for(unsigned int ngb=0;ngb<4;ngb++)
	  {
	    if(cell->neighbor(ngb)->info().mergednr < 20){
	    if(cell->neighbor(ngb)->info().isGhost == false && cell->neighbor(ngb)->info().mergedID <solver->T[solver->currentTes].cellHandles.size() && cell->neighbor(ngb)->info().isFictious == false && ((cell->info().mergedID == cell->neighbor(ngb)->info().mergedID && cell->info().mergedID != 0)==false)){
	    if((cell->info().poreThroatRadius[ngb] / ( getChi(cell->info().numberFacets) * std::pow(cell->info().mergedVolume,(1./3.)))) > criterion)
	    {
	      if(cell->info().mergedID == 0 && cell->neighbor(ngb)->info().mergedID == 0)
	      {
		cell->info().mergedID = number;
		cell->neighbor(ngb)->info().mergedID = number;
		number = number + 1;
		countFacets();
		computeMergedVolumes();
		
	      }
	      else if(cell->info().mergedID == 0 && cell->neighbor(ngb)->info().mergedID != 0)
	      {
		cell->info().mergedID = cell->neighbor(ngb)->info().mergedID;
		countFacets();
		computeMergedVolumes();
	      }
	      else if(cell->info().mergedID != 0 && cell->neighbor(ngb)->info().mergedID == 0)
	      {
		cell->neighbor(ngb)->info().mergedID = cell->info().mergedID;
		countFacets();
		computeMergedVolumes();
	      }
	    }
	  }
	  }
	}
	}
      }
	countFacets();
	computeMergedVolumes();
	}
      }
      maxIDMergedCells = number;

      // RENUMBER THE MERGED CELLS
      for(unsigned int k=1;k<maxIDMergedCells;k++){
      check = false;
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	  if(cell->info().mergedID == k){check = true;}
	}
	if(check){
	  ID = ID + 1;
	  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	    if(cell->info().mergedID == k){
	      cell->info().mergedID = ID;
	    }
	  }
       }
      }
      maxIDMergedCells = ID + 1;
      if(debugTPF){cout << endl << "EFFICIENT - RENUMBER MERGEDCELLS -- FROM: " << number << " TO: " << ID + 1;}
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	    cell->info().mergedVolume = cell->info().poreBodyVolume;
      }	
}


void TwoPhaseFlowEngine::computeMergedVolumes()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    double volume = 0.0, summ = 0.0;
    for (unsigned int mergeID = 1; mergeID < maxIDMergedCells; mergeID++){
	volume = 0.0;
	summ = 0.0;	  
	for (FiniteCellsIterator Mergecell = tri.finite_cells_begin(); Mergecell != cellEnd; Mergecell++){
	  if(Mergecell->info().mergedID == mergeID && Mergecell->info().isFictious == false && Mergecell->info().isGhost == false && Mergecell->info().id < solver->T[solver->currentTes].cellHandles.size()){
	    volume = volume + Mergecell->info().poreBodyVolume;
	    summ = summ + 1.0;
	    }
	  }
	  if(summ > 1.0){
	  for (FiniteCellsIterator Mergecell = tri.finite_cells_begin(); Mergecell != cellEnd; Mergecell++){
	    if(Mergecell->info().mergedID == mergeID && Mergecell->info().isFictious == false && Mergecell->info().isGhost == false && Mergecell->info().id < solver->T[solver->currentTes].cellHandles.size()){
	      Mergecell->info().poreBodyRadius = getChi(Mergecell->info().numberFacets)*std::pow(volume,(1./3.));
	      Mergecell->info().mergedVolume = volume;
      	      Mergecell->info().mergednr = summ;
	      }
	    }
	  }
	  if(summ <= 1.0){
	  for (FiniteCellsIterator Mergecell = tri.finite_cells_begin(); Mergecell != cellEnd; Mergecell++){
	   if(Mergecell->info().mergedID == mergeID && Mergecell->info().isFictious == false && Mergecell->info().isGhost == false && Mergecell->info().id < solver->T[solver->currentTes].cellHandles.size()){
	    cout << endl << "isMerged set to -1: " << Mergecell->info().id << " "<< Mergecell->info().poreBodyRadius << " " << Mergecell->info().poreThroatRadius[0] << " " << Mergecell->info().poreThroatRadius[1]<<" " << Mergecell->info().poreThroatRadius[2]<<" " << Mergecell->info().poreThroatRadius[3];
	    Mergecell->info().mergednr = 1;
	    Mergecell->info().mergedID = 0; 
	   }
	  }  
	  }    
    }
}



void TwoPhaseFlowEngine::countFacets()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
      int summngb = 0;
      for(unsigned int k=1;k<maxIDMergedCells;k++){
      summngb = 0;
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	if(cell->info().mergedID == k && cell->info().isGhost == false && (cell->info().isFictious == false) && (cell->info().id < solver->T[solver->currentTes].cellHandles.size())){
	 for(unsigned int i=0;i<4;i++){
	   if(cell->neighbor(i)->info().mergedID != cell->info().mergedID && cell->neighbor(i)->info().isGhost == false && (cell->neighbor(i)->info().isFictious == false) && (cell->neighbor(i)->info().id < solver->T[solver->currentTes].cellHandles.size())){
	    summngb = summngb + 1; 
	   }
	 }
	}
    }
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	if(cell->info().mergedID == k){
	  if(summngb < 4){summngb = 4;} //Less than 4 throats is not supported -> boundary problems.
	cell->info().numberFacets = summngb;
	}
    }
    }  
}


void TwoPhaseFlowEngine::getMergedCellStats()
{
  
    std::array<double,26> countFacets = {0};
    std::array<double,30> countMergedNR = {0};
    int count = 0, countTot = 0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    //In this function the amount of tetrahedra per pore is counted and reported in the terminal.	
    
    std::string NameDistributionInFacets = modelRunName;
    std::string NamedistibutionInMergedPoreUnits = modelRunName;
    NameDistributionInFacets.append("-distributionInFacets.txt");
    NamedistibutionInMergedPoreUnits.append("-distributionInMergedPoreUnits.txt");

    std::ofstream distributionInFacets;
    distributionInFacets.open(NameDistributionInFacets, std::ios::trunc);
    if(!distributionInFacets.is_open()){
      std::cerr<< "Error opening file [" << "PoreBodyRadius" << ']' << std::endl;
      return;
    }
    
    std::ofstream distibutionInMergedPoreUnits;
    distibutionInMergedPoreUnits.open(NamedistibutionInMergedPoreUnits , std::ios::trunc);
    if(!distibutionInMergedPoreUnits.is_open()){
       std::cerr<< "Error opening file [" << "PoreBoundary" << ']' << std::endl;
       return;
    }
    distributionInFacets << "The distribution in the number of pore throats per pore unit - table shows in the first column the number of pore throats and in the second column the total count"<<'\n';
    distibutionInMergedPoreUnits << "The distribution in the number of tetrahedra per merged pore unit - table shows in the first column the number of merged tetrahedra and in the second column the total count"<<'\n';

    countTot = 0;
    count = 0;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	if(cell->info().isFictious == false && cell->info().isGhost == false && cell->info().id <  solver->T[solver->currentTes].cellHandles.size()){
	  if(cell->info().numberFacets == 4){count = count + 1;}
	   countTot = countTot + 1;
	    
	  }
      }
      
      if(debugTPF){cout << endl << "Number of merged cells is:"<< count << "of the total number" << countTot <<" which is: "<< (float(count) * 100.0 / float(countTot));}

   
     for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      if(cell->info().isFictious == false && cell->info().isGhost == false && cell->info().id <  solver->T[solver->currentTes].cellHandles.size())
      {
	if(cell->info().numberFacets < 30){
	countFacets[cell->info().numberFacets - 4] = countFacets[cell->info().numberFacets - 4] + (1.0 / float(cell->info().mergednr));
      }
      }
     }
     
     for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      if(cell->info().isFictious == false && cell->info().isGhost == false && cell->info().id <  solver->T[solver->currentTes].cellHandles.size())
      {
	if(cell->info().mergednr < 30){
	countMergedNR[cell->info().mergednr - 1] = countMergedNR[cell->info().mergednr - 1] + (1.0 / float(cell->info().mergednr));
      }
      }
     }
     
     for (unsigned int i = 0; i< countFacets.size(); i++){
      if(debugTPF){cout << endl << "nrFacets: "<< (i + 4) << "-count:" << countFacets[i];}
      distributionInFacets << (i+4) << " " <<  countFacets[i] << '\n';
     }
     for (unsigned int i = 0; i< countMergedNR.size(); i++){
      if(debugTPF){cout << endl << "nrMergedUnits: "<< i+1 << "-count:" << countMergedNR[i];}
      distibutionInMergedPoreUnits << (i+1) << " " << countMergedNR[i] << '\n';
     }
      distributionInFacets.close();
      distibutionInMergedPoreUnits.close();
}


void TwoPhaseFlowEngine::adjustUnresolvedPoreThroatsAfterMerging()
{
    //Adjust the remaining pore throats, such that all throats are smaller than the pore units. 
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    int count = 0, countTot = 0;
    for (unsigned int p = 0; p < 5; p++){
    countTot = 0;
    count = 0;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      if(cell->info().isGhost == false && cell->info().isFictious == false){
	for(unsigned int i = 0; i < 4; i++){
	  if((cell->info().mergedID != cell->neighbor(i)->info().mergedID || (cell->info().mergedID == 0 && cell->neighbor(i)->info().mergedID == 0)) && 
	      cell->neighbor(i)->info().isGhost == false /*&& cell->neighbor(i)->info().mergedID < solver->T[solver->currentTes].cellHandles.size()*/){
		 countTot = countTot + 1;
		 if(cell->info().poreThroatRadius[i] >= maximumRatioPoreThroatoverPoreBody * (getChi(cell->info().numberFacets) * std::pow(cell->info().mergedVolume,(1./3.)))){ // if throat is larger than maximumRatioPoreThroatoverPoreBody time the pore body volume, then adjust pore throat radii
		    count = count + 1;
		    cell->info().poreThroatRadius[i] = std::min((maximumRatioPoreThroatoverPoreBody * getChi(cell->info().numberFacets) * std::pow(cell->info().mergedVolume,(1./3.))), cell->neighbor(i)->info().poreThroatRadius[i]);
		 }
	      }
	  }
	}
      }
    if(debugTPF){cout << endl << "Total nr Throats = " << countTot << "total throats that are too large: "<< count << "that is : "<< (float(count) * 100.0 / float(countTot)) << "%";}
    if((float(count) / float(countTot)) > 0.1){cout << endl << "Error! Too many pore throats have been adjusted, more than 10%. Simulation is stopped" << count << " tot:" << countTot; /*stopSimulation = true;*/}
    }
}


void TwoPhaseFlowEngine::checkVolumeConservationAfterMergingAlgorithm()
{     
    //Check volume of the merging of pores, especially required for truncated pore shapes.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    double volumeSingleCells = 0.0, volumeTotal = 0.0, volumeMergedCells = 0.0;
    bool check = false;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
     if(cell->info().isFictious == 0){
       volumeTotal = volumeTotal + cell->info().poreBodyVolume;
       if(cell->info().mergedID == 0){volumeSingleCells = volumeSingleCells + cell->info().poreBodyVolume;}
     }
    }
    
    for(unsigned int k=1;k<maxIDMergedCells;k++){
      check = false;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){  
    if((cell->info().mergedID == k)&&(check == false)){
      volumeMergedCells = volumeMergedCells + cell->info().mergedVolume;
      check = true;
    }
    }
    }
    //if volume is not conserved give error message
    if( std::abs((volumeTotal - volumeMergedCells - volumeSingleCells) / volumeTotal) > 1e-6){
     std::cerr << endl << "Error! Volume of pores is not conserved between merged pores and total pores: "<< "Total pore volume = " << volumeTotal<< "Volume of merged cells = "<<volumeMergedCells << "Volume of single cells =" <<volumeSingleCells;
     stopSimulation = true;
    }   
}

void TwoPhaseFlowEngine::calculateResidualSaturation()
{	
    //This function computes the entry pressures of the pore throats, as well as the saturation at which an event occurs. This saturation is used as target saturation for determining dt.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++)
    {
	//Calculate all the pore body radii based on their volumes
	cell->info().poreBodyRadius = getChi(cell->info().numberFacets)*std::pow(cell->info().mergedVolume,(1./3.));
	if(cell->info().poreBodyRadius != 0){cell->info().thresholdPressure = 2.0 * surfaceTension / cell->info().poreBodyRadius;}
	cell->info().thresholdSaturation = 1.0 - (4./3.)*3.14159265359*std::pow(getChi(cell->info().numberFacets),3);
	
	//First check all the macro pores
	if(cell->info().mergedID > 0){
	      for(unsigned int ngb = 0; ngb < 4;ngb++){
	      //Option (1): Throat radii smaller than pore body radii
		if((cell->info().poreThroatRadius[ngb] < cell->info().poreBodyRadius) &&(cell->info().mergedID != cell->neighbor(ngb)->info().mergedID)){
		  cell->info().entryPressure[ngb] = entryMethodCorrection * surfaceTension / cell->info().poreThroatRadius[ngb];
		  cell->info().entrySaturation[ngb] = poreSaturationFromPcS(cell,-1.0 * cell->info().entryPressure[ngb]);
		  
		  if(cell->info().entrySaturation[ngb] < 0.0 || cell->info().entrySaturation[ngb] > 1.0){
		    cout << endl << "Error With the entrySaturation of a pore throat! "<<  cell->info().entrySaturation[ngb] << " " << cell->info().poreThroatRadius[ngb] << " and " <<cell->info().poreBodyRadius << " " << getKappa(cell->info().numberFacets)
		    << " " <<std::log(1.0-(cell->info().poreThroatRadius[ngb] / (cell->info().poreBodyRadius * entryMethodCorrection))) << " CellID=" << cell->info().id << " MergedID ="<< cell->info().mergedID << " Facets=" << cell->info().numberFacets;
		    cout << endl << "Simulation is terminated because of an error in entry saturation";
		    stopSimulation = true;
		    }
		}
		
	      //Option (2): Throat radii bigger than pore body radii (this is an error). 
		if((cell->info().poreThroatRadius[ngb] >= cell->info().poreBodyRadius)&&(cell->neighbor(ngb)->info().isFictious == 0)&&(cell->info().mergedID != cell->neighbor(ngb)->info().mergedID)){
		  cout << endl << "Error, throat radius is larger than the pore body radius for a merged pores: " << cell->info().id << " MergedID" << cell->info().mergedID << " ThroatRadius: "<< cell->info().poreThroatRadius[ngb] << " BodyRadius: "<<cell->info().poreBodyRadius << " nr facets = "<< cell->info().numberFacets;
		  cout << endl << "Simulation is terminated because of an pore throat is larger than pore body!";
		  stopSimulation = true;
		  cell->info().entrySaturation[ngb] = 1.0;
		  cell->info().entryPressure[ngb] = 0.0;
		  }
		  
	      //Option (3): Two tetrahedra from the same pore body, thus an artificial pore throat that is deactivated here.  
		if(cell->info().mergedID == cell->neighbor(ngb)->info().mergedID){
		  cell->info().entrySaturation[ngb] = 1.0;
		  cell->info().entryPressure[ngb] = 0.0;
		}
		
	      //Option (4): Neighboring Tetrahedron is a boundary cell.
		if(cell->neighbor(ngb)->info().isFictious == true){
		  cell->info().entrySaturation[ngb] = 1.0;
		  cell->info().entryPressure[ngb] = 0.0;	  
		}
	      }  
	}
	//check all the non-merged pores
	if(cell->info().mergedID == 0){
	for(unsigned vert = 0; vert < 4; vert++){
	  //Deactive all boundary cells - which are infact individual tetrahedra. 
	  if(cell->neighbor(vert)->info().isFictious == 1){cell->info().entrySaturation[vert] = 1.0;cell->info().entryPressure[vert] = 0.0;}
	  if(cell->neighbor(vert)->info().isFictious == 0){
	    //calculate the different entry pressures and so on. 
		cell->info().entrySaturation[vert] = std::log(1.0-(2.0 * cell->info().poreThroatRadius[vert] / (cell->info().poreBodyRadius * entryMethodCorrection))) / (-1.0*getKappa(cell->info().numberFacets));
		cell->info().entryPressure[vert] = entryMethodCorrection*surfaceTension / cell->info().poreThroatRadius[vert];
		if((cell->info().entrySaturation[vert] > 1.0 && !cell->info().isFictious) || ((cell->info().entrySaturation[vert] < 0.0))){
		  cout << endl << "entry saturation error!" << cell->info().entrySaturation[vert] << " "<< 
		  cell->info().id << " " << cell->info().poreBodyRadius << " "<< cell->info().poreThroatRadius[vert] ;cell->info().entrySaturation[vert] = 1.0;
		  cout << endl << "Simulation is terminated because of an error in entry saturation!";
		  stopSimulation = true;
		}
	}
	}
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....................................Triangulation while maintaining saturation field ............................................//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TwoPhaseFlowEngine::reTriangulate()
{
      //Governing function to apply triangulation while maintaining saturation distribution.
      if(debugTPF){std::cerr << endl << "Apply retriangulation";} 
      initializationTriangulation();
      readTriangulation();      
      keepTriangulation = false;
      initialization();
      assignWaterVolumesTriangulation();
      actionMergingAlgorithm();
      equalizeSaturationOverMergedCells();
}

void TwoPhaseFlowEngine::initializationTriangulation()
{
  //Resize all relevant functions 
  
  //per sphere
  leftOverVolumePerSphere.resize(scene->bodies->size(),0);
  untreatedAreaPerSphere.resize(scene->bodies->size(),0);
  leftOverDVPerSphere.resize(scene->bodies->size(),0);
  //per tetrahedra
  finishedUpdating.resize(solver->T[solver->currentTes].cellHandles.size(),0);
  waterVolume.resize(solver->T[solver->currentTes].cellHandles.size(),0);
  deltaVoidVolume.resize(solver->T[solver->currentTes].cellHandles.size(),0);
  tetrahedra.resize(solver->T[solver->currentTes].cellHandles.size());
  solidFractionSpPerTet.resize(solver->T[solver->currentTes].cellHandles.size());
  for(unsigned int i =0; i<solver->T[solver->currentTes].cellHandles.size(); i++){
    tetrahedra[i].resize(4,0);
    solidFractionSpPerTet[i].resize(4,0);
  }
}

void TwoPhaseFlowEngine::readTriangulation()
{
  //Read all relevant information from old assembly of tetrahedra
  for (unsigned int i=0;i<scene->bodies->size();i++){
	untreatedAreaPerSphere[i] = 0.0;
	leftOverVolumePerSphere[i] = 0.0;
	leftOverDVPerSphere[i] = 0.0;
  }
      
  for (unsigned int i=0;i<solver->T[solver->currentTes].cellHandles.size();i++){
	tetrahedra[i][0] = tetrahedra[i][1] = tetrahedra[i][2] = tetrahedra[i][3] = 1e6;
	solidFractionSpPerTet[i][0] = solidFractionSpPerTet[i][1] = solidFractionSpPerTet[i][2] = solidFractionSpPerTet[i][3] = 0.0;
	waterVolume[i] = 0.0;
	deltaVoidVolume[i] = 0.0;
	finishedUpdating[i] = 0;
  }

  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
  FiniteCellsIterator cellEnd = tri.finite_cells_end(); 
  
  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
    waterVolume[cell->info().id] = cell->info().saturation * cell->info().poreBodyVolume;
    deltaVoidVolume[cell->info().id] = cell->info().dv();
    if(cell->info().isFictious){finishedUpdating[cell->info().id] = -1;}
    if(!cell->info().isFictious){
	  std::pair<int, double> pairs[4];
	  for (unsigned int i = 0;i<4;i++){
	   pairs[i] = std::make_pair(cell->vertex(i)->info().id(),std::abs(solver->fractionalSolidArea(cell,i))); 
	  }
	  
	  sort(std::begin(pairs),std::end(pairs));
 	  for(unsigned int j=0;j<4;j++){
	    tetrahedra[cell->info().id][j] = pairs[j].first;
	    solidFractionSpPerTet[cell->info().id][j] = pairs[j].second; 
	  }
	  }
      }
}

void TwoPhaseFlowEngine::assignWaterVolumesTriangulation()
{
      //Assign saturation to new assembly of tetrahedra 
      RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
      FiniteCellsIterator cellEnd = tri.finite_cells_end();   
      unsigned int saveID = 1e6;
      static unsigned int index = waterVolume.size(); 
      
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	if(!cell->info().isFictious){
	  saveID = 1e6;
	  unsigned int vert[4] = {cell->vertex(0)->info().id(),cell->vertex(1)->info().id(),cell->vertex(2)->info().id(),cell->vertex(3)->info().id()};
	  std::sort(std::begin(vert),std::end(vert));
	  for(unsigned int i=0;i<index;i++){
	    if(tetrahedra[i][0] == vert[0] && tetrahedra[i][1] == vert[1] && tetrahedra[i][2] == vert[2] && tetrahedra[i][3] == vert[3]){
		saveID = i;
		break;
	    }
	  }
	  if(saveID != 1e6){
	    cell->info().saturation = waterVolume[saveID] / cell->info().poreBodyVolume;
	    cell->info().dv() = deltaVoidVolume[saveID];
	    if(cell->info().saturation < 0.0){std::cout << endl << "Negative Sat in subFunction1 :" << cell->info().saturation << " " << waterVolume[saveID] << " " << cell->info().poreBodyVolume;}
	    finishedUpdating[saveID] = 1;
	  }
	  if(saveID == 1e6){
	    cell->info().saturation = -1;
	    for(unsigned int i=0;i<4;i++){
	      untreatedAreaPerSphere[cell->vertex(i)->info().id()] += std::abs(solver->fractionalSolidArea(cell,i));
	    }
	  } 
	}
      }
      for(unsigned int i=0;i<index;i++){
	if(finishedUpdating[i] == 0){
	  double totalArea = solidFractionSpPerTet[i][0] + solidFractionSpPerTet[i][1] + solidFractionSpPerTet[i][2] + solidFractionSpPerTet[i][3];
	  for(unsigned int j=0;j<4;j++){
	    leftOverVolumePerSphere[tetrahedra[i][j]] += (solidFractionSpPerTet[i][j] / totalArea)*waterVolume[i];
	    leftOverDVPerSphere[tetrahedra[i][j]] += (solidFractionSpPerTet[i][j] / totalArea)*deltaVoidVolume[i];
	  }
	}
      }

      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	if(cell->info().saturation == -1){
	  double vol = 0.0, dv = 0.0;
	   for(unsigned int j=0;j<4;j++){
	     vol += leftOverVolumePerSphere[cell->vertex(j)->info().id()] * (std::abs(solver->fractionalSolidArea(cell,j)) / untreatedAreaPerSphere[cell->vertex(j)->info().id()]);
	     dv += leftOverDVPerSphere[cell->vertex(j)->info().id()] * (std::abs(solver->fractionalSolidArea(cell,j)) / untreatedAreaPerSphere[cell->vertex(j)->info().id()]);
	  }
	   cell->info().saturation = vol / cell->info().poreBodyVolume;
	   cell->info().dv() = dv;
	   if(cell->info().saturation < 0.0){std::cout << endl << "Error! Negative Sat in sphere allocation: " << cell->info().saturation << " " << vol << " " << cell->info().poreBodyVolume;}
	}
      }
}



void TwoPhaseFlowEngine::equalizeSaturationOverMergedCells()
{
    double waterVolume = 0.0, volume = 0.0, leftOverVolume = 0.0, workVolume = 0.0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    
   for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++)
	{
	  if(cell->info().saturation < 0.0){std::cout << endl <<  "Error! Before Starting! Negative sat! ... " << cell->info().saturation;}
	}
    
    
    for(unsigned int k=1;k<maxIDMergedCells;k++){
	 waterVolume = 0.0;
	 leftOverVolume = 0.0;
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++)
	{
	    if(cell->info().mergedID == k){
	     waterVolume += cell->info().saturation * cell->info().poreBodyVolume; 
	     volume = cell->info().mergedVolume;
	    }
	}
	
	if(waterVolume > volume){
	  leftOverVolume = waterVolume - volume;
	  waterVolume = volume; 
	}
	if(leftOverVolume > 0.0){
	  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	      if(cell->info().mergedID == k && leftOverVolume > 0.0){
		for(unsigned int j = 0; j<4; j++){
		  if(cell->info().mergedID != cell->neighbor(j)->info().mergedID && cell->neighbor(j)->info().saturation < 1.0 && !cell->neighbor(j)->info().isFictious && leftOverVolume > 0.0){
		      workVolume  = (1.0 - cell->neighbor(j)->info().saturation) * cell->neighbor(j)->info().poreBodyVolume;
		      std::cout << endl << workVolume << " " << leftOverVolume << " " << cell->neighbor(j)->info().saturation;
		      if(workVolume <= leftOverVolume){
			leftOverVolume -= workVolume;
			cell->neighbor(j)->info().saturation = 1.0;
			std::cout << "inOne";
		      }
		      std::cout << endl << workVolume << " " << leftOverVolume << " " << cell->neighbor(j)->info().saturation ;
		      if(workVolume > leftOverVolume && leftOverVolume > 0.0){
			cell->neighbor(j)->info().saturation = (leftOverVolume + cell->neighbor(j)->info().saturation * cell->neighbor(j)->info().poreBodyVolume) / cell->neighbor(j)->info().poreBodyVolume;
			leftOverVolume = 0.0;
			std::cout << "inOne";
		      }
		      std::cout << endl << workVolume << " " << leftOverVolume << " " << cell->neighbor(j)->info().saturation ;
		  }
		}
	      }
	  }
	  
	 if(leftOverVolume > 0.0){std::cout << endl << "Error! Left over water volume: " << leftOverVolume;}
	 
	}
	
	
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++)
	{
	    if(cell->info().mergedID == k){
	     cell->info().saturation = waterVolume /  cell->info().mergedVolume; 
	    }
	}
    }
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
     if(!cell->info().isFictious && cell->info().saturation > 1.0 && cell->info().mergedID == 0){
	  leftOverVolume = (cell->info().saturation - 1.0) * cell->info().poreBodyVolume; 
	  cell->info().saturation = 1.0;
	  for(unsigned int j=0;j<4;j++){
	    if(cell->neighbor(j)->info().saturation < 1.0 && !cell->neighbor(j)->info().isFictious && leftOverVolume > 0.0){
	    workVolume  = (1.0 - cell->neighbor(j)->info().saturation) * cell->neighbor(j)->info().poreBodyVolume;
	    if(workVolume <= leftOverVolume){
		leftOverVolume -= workVolume;
		cell->neighbor(j)->info().saturation = 1.0;
		std::cout << "inOne-sec";
	    }
	    std::cout << endl << " sec-" << workVolume << " " << leftOverVolume << " " << cell->neighbor(j)->info().saturation ;
	    if(workVolume > leftOverVolume && leftOverVolume > 0.0){
			cell->neighbor(j)->info().saturation = (leftOverVolume + cell->neighbor(j)->info().saturation * cell->neighbor(j)->info().poreBodyVolume) / cell->neighbor(j)->info().poreBodyVolume;
			leftOverVolume = 0.0;
			std::cout << "inOne-sec";
	    }
	 }
      }
      if(leftOverVolume > 0.0){std::cout << "Mass left during remeshing" << leftOverVolume;}
     }
    }
    
    bool redo = false;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      if(cell->info().saturation > 1.0){
	 redo = true;
      }
    }
  if(redo){std::cout<< "redo calculation"; equalizeSaturationOverMergedCells();}
    
    
}







/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....................................Set pore network from PUA....................... ............................................//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void TwoPhaseFlowEngine::setInitialConditions()
{
    if(debugTPF){std::cerr<<endl<<"Set initial condition";}

    //four possible initial configurations are allowed: primary drainage, primary imbibition, secondary drainage, secondary imbibition
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        
      //make backup of saturated hydraulic conductivity
      for(unsigned int ngb = 0; ngb<4; ngb++){
       cell->info().kNorm2[ngb] = cell->info().kNorm()[ngb];
      }
                
      cell->info().isFictiousId = -1;
      cell->info().isWRes = false;
      cell->info().isNWRes = false;
      

      if(cell->info().isFictious){
        //boundary cells are not used here 
        cell->info().p() = 0.0;
        cell->info().saturation = 1.0;
        cell->info().hasInterface = false;
      }
      if(!cell->info().isFictious){
        //Primary drainage
        if(drainageFirst && primaryTPF){
             cell->info().p() = -1 * initialPC;
             cell->info().saturation = 1.0;
             cell->info().hasInterface = false;
        }
        //Secondary drainage (using saturation field as input parameter)
        if(drainageFirst && !primaryTPF){
             cell->info().p() = -1 * initialPC;
             if(cell->info().saturation <= cell->info().thresholdSaturation){
                cell->info().p() = porePressureFromPcS(cell,cell->info().saturation);
                cell->info().hasInterface = true;
             }
             if(cell->info().saturation > cell->info().thresholdSaturation){
                 cell->info().p() = -1*initialPC;  
                 cell->info().saturation = 1.0;
                 cell->info().hasInterface = false;
                 std::cerr << "Warning: local saturation changed for compatibility of local Pc(S)";
             }
             
        }
        //Primary imbibition
        if(!drainageFirst && primaryTPF){
             cell->info().p() = -1 * initialPC;
             cell->info().saturation = poreSaturationFromPcS(cell,-1*initialPC);
             cell->info().hasInterface = true;                                          //FIXME: hasInterface should be false, but until an imbibition criteria is implementend into solvePressure, this should remain true for testing purposes
        }
        //Secondary imbibition
        if(!drainageFirst && !primaryTPF){
             cell->info().p() = -1 * initialPC;
             if(cell->info().saturation <= cell->info().thresholdSaturation){
                cell->info().p() = porePressureFromPcS(cell,cell->info().saturation);
                cell->info().hasInterface = true;
             } 
             if(cell->info().saturation > cell->info().thresholdSaturation){
                 cell->info().p() = -1*initialPC;  
                 cell->info().saturation = 1.0;
                 cell->info().hasInterface = false;
                 std::cerr << "Warning: local saturation changed for compatibility of local Pc(S)";
             }
            
             
        }

      }
    }
}

void TwoPhaseFlowEngine::transferConditions()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
      for(unsigned int ngb = 0; ngb<4; ngb++){
       cell->info().kNorm2[ngb] = cell->info().kNorm()[ngb];
      }
      
      if(cell->info().saturation == 1.0){
	cell->info().hasInterface = false;
      }
      if(cell->info().saturation < 1.0){
	cell->info().hasInterface = true;
	cell->info().p() = porePressureFromPcS(cell,cell->info().saturation);
      }
    }
}

void TwoPhaseFlowEngine::setBoundaryConditions()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
      if(cell->info().isFictious){
	for(unsigned int j=0;j<4;j++){
	  for(unsigned int i=0;i<6;i++){
	    if(cell->vertex(j)->info().id() == i)
	    {
	      cell->info().isFictiousId = i;
	      if(bndCondIsPressure[cell->info().isFictiousId] && bndCondIsWaterReservoir[cell->info().isFictiousId] ){
		cell->info().p() = bndCondValue[cell->info().isFictiousId];
		cell->info().isWRes = true;
		waterBoundaryPressure = bndCondValue[cell->info().isFictiousId];
	      }
	      if(bndCondIsPressure[cell->info().isFictiousId] && !bndCondIsWaterReservoir[cell->info().isFictiousId]){
		cell->info().p() = bndCondValue[cell->info().isFictiousId];
		cell->info().isNWRes = true;
		airBoundaryPressure = bndCondValue[cell->info().isFictiousId];
	      }
	    }	    
	  }
	}
      }
    }
     for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
         
      //set initial interface in simulations
         
      //Drainage
      if(drainageFirst && cell->info().isNWRes){
	for (unsigned int i = 0; i<4; i++){
	  if(!cell->neighbor(i)->info().isFictious){
	    if(!deformation){
		cell->neighbor(i)->info().hasInterface = true; 
		cell->neighbor(i)->info().saturation = poreSaturationFromPcS(cell->neighbor(i),-1*initialPC);
		cell->neighbor(i)->info().p() = -1*initialPC;
		cell->neighbor(i)->info().airBC = true;
		if(cell->neighbor(i)->info().saturation != cell->neighbor(i)->info().saturation || cell->neighbor(i)->info().saturation > 1.0 || cell->neighbor(i)->info().saturation < 0.0){
		  std::cout << "Error with initial BC saturation: " << cell->neighbor(i)->info().saturation;
		}
	    }
	    if(deformation){
		cell->neighbor(i)->info().hasInterface = true;
		cell->neighbor(i)->info().p() = -1*initialPC; 
		cell->neighbor(i)->info().saturation = poreSaturationFromPcS(cell->neighbor(i),-1 * initialPC);
	    }
	    
	    //Update properties of other cells in pore unit
	    if(cell->neighbor(i)->info().mergedID > 0){
	      for (FiniteCellsIterator Mcell = tri.finite_cells_begin(); Mcell != cellEnd; Mcell++) {
		if(Mcell->info().mergedID == cell->neighbor(i)->info().mergedID){
		  Mcell->info().hasInterface = cell->neighbor(i)->info().hasInterface;
		  Mcell->info().saturation = cell->neighbor(i)->info().saturation;
		  Mcell->info().p() =  cell->neighbor(i)->info().p();
		  Mcell->info().isNWRes = cell->neighbor(i)->info().isNWRes;
		}
	      }
	    }
	  }
	}
      }
      //Imbibition                                  FIXME(thomas): Needs to be tested for both rigid packings and deforming packings
      if(!drainageFirst){
          for (unsigned int i = 0; i<4; i++){
          if(cell->info().isNWRes){
              cell->neighbor(i)->info().airBC = true;
          }
          if(cell->info().isWRes){
            cell->neighbor(i)->info().hasInterface = true; 
            cell->neighbor(i)->info().saturation = poreSaturationFromPcS(cell->neighbor(i),-1*initialPC);
            cell->neighbor(i)->info().p() = -1*initialPC;
            if(cell->neighbor(i)->info().saturation != cell->neighbor(i)->info().saturation || cell->neighbor(i)->info().saturation > 1.0 || cell->neighbor(i)->info().saturation < 0.0){
                std::cout << "Error with initial BC saturation: " << cell->neighbor(i)->info().saturation;
            }
            if(cell->neighbor(i)->info().mergedID > 0){
	      for (FiniteCellsIterator Mcell = tri.finite_cells_begin(); Mcell != cellEnd; Mcell++) {
		if(Mcell->info().mergedID == cell->neighbor(i)->info().mergedID){
		  Mcell->info().hasInterface = cell->neighbor(i)->info().hasInterface;
		  Mcell->info().saturation = cell->neighbor(i)->info().saturation;
		  Mcell->info().p() =  cell->neighbor(i)->info().p();
		  Mcell->info().isNWRes = cell->neighbor(i)->info().isNWRes;
		}
	      }
	    }
       }

      }
    }
   }

    
    
   if(waterBoundaryPressure == 0.0){waterBoundaryPressure = -1 * initialPC;}

	  
}
void TwoPhaseFlowEngine::verifyCompatibilityBC()
{
    //This is merely a function to double check boundary conditions to avoid ill-posed B.C.
    std::cerr << endl << "Boundary and initial conditions are set for: ";

    if(drainageFirst && primaryTPF){  
        std::cerr<< "Primary Drainage"; 
        if(initialPC > -1 * waterBoundaryPressure){ std::cerr << endl << "Warning, initial capillary pressure larger than imposed capillary pressure, this may cause imbibition";}
    }
    if(drainageFirst && !primaryTPF){ 
        std::cerr<< "Secondary Drainage";
        if(initialPC > -1 * waterBoundaryPressure){ std::cerr << endl << "Warning, initial capillary pressure larger than imposed capillary pressure, this may cause imbibition";}
    }
    if(!drainageFirst && primaryTPF){
        std::cerr<< "Primary Imbibition";
        if(initialPC < -1 * waterBoundaryPressure){ std::cerr << endl << "Warning, initial capillary pressure smaller than imposed capillary pressure, this may cause drainage";}
    }
    if(!drainageFirst && !primaryTPF){
        std::cerr<< "Secondary Imbibition";
        if(initialPC < -1 * waterBoundaryPressure){ std::cerr << endl << "Warning, initial capillary pressure smaller than imposed capillary pressure, this may cause drainage";}
    }
    
    std::cout << endl << "Water pressure at: " << waterBoundaryPressure << " and air pressure at: " << airBoundaryPressure << " InitialPC: "<< initialPC;

}

void TwoPhaseFlowEngine::setPoreNetwork()
{
      //Reorder cell id's
      RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
      FiniteCellsIterator cellEnd = tri.finite_cells_end();
      unsigned int i = 0;
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	if(!cell->info().isFictious){
	  if(cell->info().poreId == -1){
	    cell->info().poreId = i;
	    if(cell->info().mergedID > 0){
	      for (FiniteCellsIterator Mcell = tri.finite_cells_begin(); Mcell != cellEnd; Mcell++) {
		if(Mcell->info().mergedID == cell->info().mergedID){
		  Mcell->info().poreId = i; 
		}
	      }
	    }
	  i = i + 1; 
	  } 
	}
      }
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	if(!cell->info().isFictious){
	  if(cell->info().poreId == -1){std::cout << " cell -1 " << cell->info().id;}
	}
      }
      numberOfPores = i;
      for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	if(!cell->info().isFictious){
	  for(unsigned int k = 0; k<4; k++){
	    if(!cell->neighbor(k)->info().isFictious){
	      if(cell->info().mergedID == 0 || (cell->neighbor(k)->info().mergedID != cell->info().mergedID && cell->info().mergedID != 0)){
		cell->info().poreIdConnectivity[k] = cell->neighbor(k)->info().poreId;			//FIXME: REDUNDANT
	      }
	      else
		cell->info().poreIdConnectivity[k] = -1;
	    }
	  }
	}
      }
      bool cancel = false, firstCheck = true;;
      for (unsigned int j = 0 ; j < numberOfPores; j++){
	firstCheck = true;
	  std::vector<int> poreNeighbors;
	  std::vector<double> listOfkNorm;
	  std::vector<double> listOfEntrySaturation;
	  std::vector<double> listOfEntryPressure;
  	  std::vector<double> listOfThroatArea;
	  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	    if(cell->info().poreId == j){
	      for(unsigned int ngb = 0; ngb <4 ; ngb++){
		if(cell->neighbor(ngb)->info().poreId != j && cell->neighbor(ngb)->info().poreId != -1 ){
		  cancel = false;
		  for(unsigned int checkID = 0; checkID < poreNeighbors.size(); checkID++){
		   if( poreNeighbors[checkID] == cell->neighbor(ngb)->info().poreId){
		    cancel = true; 
// 		    std::cerr<<"skipCell";
		   }
		  }
		  if((firstCheck || !cancel) || poreNeighbors.size() == 0){
		    poreNeighbors.push_back(cell->neighbor(ngb)->info().poreId);
		    listOfkNorm.push_back(cell->info().kNorm()[ngb]);
		    listOfEntrySaturation.push_back(cell->info().entrySaturation[ngb]);
		    listOfEntryPressure.push_back(cell->info().entryPressure[ngb]);
		  
		    const CVector& Surfk = cell->info().facetSurfaces[ngb];
		    Real area = sqrt(Surfk.squared_length());
		    listOfThroatArea.push_back(area * cell->info().facetFluidSurfacesRatio[ngb]);
		    if(firstCheck){firstCheck = false;}
		  }
		}
	      }
	    }
	  }

	  
	  for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	    if(cell->info().poreId == j){
		   cell->info().poreNeighbors = poreNeighbors;
		   cell->info().listOfEntrySaturation = listOfEntrySaturation;
		   cell->info().listOfEntryPressure = listOfEntryPressure;
		   cell->info().listOfThroatArea = listOfThroatArea;
		   cell->info().listOfkNorm = listOfkNorm;
		   cell->info().listOfkNorm2 = listOfkNorm;
	    }
	  }
	  
      }    
      

}


    
void TwoPhaseFlowEngine::setListOfPores()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    bool stop = false;

    //set list of pores
    if((deformation && remesh) || firstDynTPF){
      listOfPores.clear();
      for (unsigned int j = 0 ; j < numberOfPores; j++){
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	    if(cell->info().poreId == (int) j && !cell->info().isGhost && !stop){
	      listOfPores.push_back(cell);
	      stop = true;
	    }
	  }
	  stop = false;
	}
	
	
	
    for (unsigned int i = 0 ; i < numberOfPores; i++){
	 for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	    if(cell->info().poreId == (int) i && !listOfPores[i]->info().isNWRes){
	    for(unsigned int j = 0; j<4; j++){
	     if(cell->neighbor(j)->info().isWRes){
	       listOfPores[i]->info().isWResInternal = true;
	       listOfPores[i]->info().conductivityWRes = cell->info().kNorm()[j];
	     }
	    }
	  }
	}
      }
      
      for (unsigned int i = 0 ; i < numberOfPores; i++){

	if(listOfPores[i]->info().isWResInternal){		//Important to track for centroidAverage water pressure
	  for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
	   if(!listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isWResInternal){
		listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().waterBC = true;
	    }
	  }
	}
	
	
	
	for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
	  for(unsigned int k = 0; k < listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors.size(); k++){
	    if(listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors[k] == (int) i){
	      listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfEntryPressure[k] = listOfPores[i]->info().listOfEntryPressure[j];
	      listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfThroatArea[k] = listOfPores[i]->info().listOfThroatArea[j];
	      listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfkNorm[k] = listOfPores[i]->info().listOfkNorm[j];
	      listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfkNorm2[k] = listOfPores[i]->info().listOfkNorm2[j];
	    }
	  }
	}
      }

   


      for (unsigned int i = 0 ; i < numberOfPores; i++){
	  listOfPores[i]->info().minSaturation = 1e-3;
      }
      
      
      
    }
    //update for deformation	


    //reset knorm
    for(unsigned int i = 0; i < numberOfPores; i++){

	listOfPores[i]->info().listOfkNorm.clear();
 
 	listOfPores[i]->info().listOfkNorm = listOfPores[i]->info().listOfkNorm2;
 	
 	if(listOfPores[i]->info().saturation < 1.0){
 	  for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){ 
 	    if(listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().saturation < 1.0){
 	      if(-1.0*listOfPores[i]->info().p() > listOfPores[i]->info().listOfEntryPressure[j]){	      
 		  double radiusCurvature = 0.0;
 		  if(listOfPores[i]->info().saturation < 1.0 || listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().saturation == 1.0){
 		    radiusCurvature = 2.0 * surfaceTension / (-1.0 * listOfPores[i]->info().p());
 		  }
 		  if(listOfPores[i]->info().saturation == 1.0 || listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().saturation < 1.0){
 		    radiusCurvature = 2.0 * surfaceTension / (-1.0 * listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().p());
 		  }
 		  if(listOfPores[i]->info().saturation < 1.0 || listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().saturation < 1.0){
 		    radiusCurvature = 4.0*surfaceTension / (-1.0 * (listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().p() + listOfPores[i]->info().p() ));
 		  }
 		 
 		  double areaWater = listOfPores[i]->info().listOfThroatArea[j] - 3.1415926535 * radiusCurvature * radiusCurvature;
 		  if(areaWater < 0.0){areaWater = listOfPores[i]->info().listOfThroatArea[j];}
 		 
 		 double hydraulicRad = 4.0*surfaceTension / (-1.0 * (listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().p() + listOfPores[i]->info().p() ));
 		 
 		 Point& p1 = listOfPores[i]->info();
 		 Point& p2 = listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info();
 		 CVector l = p1 - p2;
 		 double distance = sqrt(l.squared_length());
  		 listOfPores[i]->info().listOfkNorm[j] = areaWater * hydraulicRad * hydraulicRad / (viscosity * distance);
		 if(listOfPores[i]->info().listOfkNorm[j] < 1e-15){listOfPores[i]->info().listOfkNorm[j] = 1e-15;}
 		 if(listOfPores[i]->info().listOfkNorm[j] < 0.0 || listOfPores[i]->info().listOfkNorm[j] != listOfPores[i]->info().listOfkNorm[j]){std::cerr << " Error! " << listOfPores[i]->info().listOfkNorm[j];}
 		 
 		 
 		 
 		 for(unsigned int k = 0; k < listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors.size(); k++){ 
		    if(listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors[k] == (int) i){
			listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfkNorm[k] = listOfPores[i]->info().listOfkNorm[j];
		    }
		 }
 		 
 	      }
 	    }
 	  }
 	}
	

	  
	}
	
 	for(unsigned int i = 0; i < numberOfPores; i++){
 	 for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){ 
 	  for(unsigned int k = 0; k < listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors.size(); k++){
 		    if(listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().poreNeighbors[k] == (int) i){
 			listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().listOfkNorm[k] = listOfPores[i]->info().listOfkNorm[j];
 		    }
 		 }
 	  }
 	}
	
	

	
}



    
void TwoPhaseFlowEngine::solvePressure()
{	
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();  
    double oldDT = 0.0;
    
    //Define matrix, triplet list, and linear solver
    tripletList.clear(); // tripletList.resize(T_nnz);
    Eigen::VectorXd residualsList(numberOfPores);
    Eigen::VectorXd pressuresList(numberOfPores); //Solve aMatrix * pressuresList = residualsList
    
    //define lists
    if((deformation && remesh) || firstDynTPF){
	aMatrix.resize(numberOfPores,numberOfPores);
	saturationList.assign(numberOfPores,0.0);
	hasInterfaceList.assign(numberOfPores,false);
	listOfFlux.assign(numberOfPores,0.0);
	listOfMergedVolume.assign(numberOfPores,0.0);										//NOTE CHANGED AFTER PUSH ON GIT
    }
    
    //reset various lists
    for(unsigned int i = 0; i < numberOfPores; i++){
      residualsList[i] = 0.0;
      pressuresList[i] = 0.0;
      saturationList[i] = listOfPores[i]->info().saturation;
      hasInterfaceList[i] = listOfPores[i]->info().hasInterface;
      listOfFlux[i] = 0.0;
    } 
    
    //Fill matrix
    for(unsigned int i = 0; i < numberOfPores; i++){
      
       //Get diagonal coeff
       double dsdp2 = 0.0;
       double coeffA = 0.0, coeffA2 = 0.0;
       if(hasInterfaceList[i] && !firstDynTPF){
	    if(listOfPores[i]->info().p() == 0){
	      std::cout << endl << "Error, pressure = 0 "<< listOfPores[i]->info().p() << listOfPores[i]->info().id;
	      listOfPores[i]->info().p() = -1.0 * listOfPores[i]->info().thresholdPressure;
	    }
	    dsdp2 = dsdp(listOfPores[i],listOfPores[i]->info().p());
	    coeffA = dsdp2 * ((listOfPores[i]->info().mergedVolume / scene->dt) + (listOfPores[i]->info().accumulativeDV - listOfPores[i]->info().accumulativeDVSwelling)); //Only consider the change in porosity due to particle movement
	}

           
       //fill matrix off-diagonals
       if(!listOfPores[i]->info().isWResInternal ){	
	  for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){ 
	      tripletList.push_back(ETriplet(i,listOfPores[i]->info().poreNeighbors[j],-1.0 * listOfPores[i]->info().listOfkNorm[j]));
	      coeffA2 += listOfPores[i]->info().listOfkNorm[j];
	  }
       }

	//Set boundary conditions
	if(listOfPores[i]->info().isWResInternal){	
	  tripletList.push_back(ETriplet(i,i, 1.0));
	  residualsList[i] = waterBoundaryPressure;
	}
	
	//Fill matrix diagonal
	if(!listOfPores[i]->info().isWResInternal ){	
	  if(hasInterfaceList[i]){residualsList[i] += -1.0*listOfPores[i]->info().saturation * (listOfPores[i]->info().accumulativeDV - listOfPores[i]->info().accumulativeDVSwelling) + coeffA * listOfPores[i]->info().p();}
	  if(!hasInterfaceList[i] && deformation && listOfPores[i]->info().saturation > listOfPores[i]->info().minSaturation){residualsList[i] += -1.0*(listOfPores[i]->info().accumulativeDV - listOfPores[i]->info().accumulativeDVSwelling);} 
	  tripletList.push_back(ETriplet(i,i, coeffA +  coeffA2));
	}
    }
    
    
    //Solve Matrix
    aMatrix.setFromTriplets(tripletList.begin(),tripletList.end());
    eSolver.analyzePattern(aMatrix); 						
    eSolver.factorize(aMatrix); 						
    eSolver.compute(aMatrix);
    
    
    //Solve for pressure: FIXME: add check for quality of matrix, if problematic, skip all below. 
	pressuresList = eSolver.solve(residualsList);
	
	//Compute flux
	double flux = 0.0;
	double accumulativeDefFlux = 0.0;
	double waterBefore = 0.0, waterAfter = 0.0;
	double boundaryFlux = 0.0, lostVolume = 0.0;
	oldDT = scene->dt;
	
	//check water balance
 	for(unsigned int i = 0; i < numberOfPores; i++){
	  waterBefore += listOfPores[i]->info().saturation * listOfPores[i]->info().mergedVolume; 
 	}
	
	//compute flux
	for(unsigned int i = 0; i < numberOfPores; i++){
	  flux = 0.0;
	    for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
		  flux += listOfPores[i]->info().listOfkNorm[j] * (pressuresList[i] - pressuresList[ listOfPores[i]->info().poreNeighbors[j] ]) ;  
	    }
	  listOfFlux[i] = flux;
 	  if(listOfPores[i]->info().saturation > listOfPores[i]->info().minSaturation){accumulativeDefFlux += listOfPores[i]->info().accumulativeDV;}
	  if(listOfPores[i]->info().isWResInternal){	
	    boundaryFlux += flux;
	   }
	   if(!listOfPores[i]->info().isWResInternal && !hasInterfaceList[i] && std::abs(listOfFlux[i]) > 1e-15 && !deformation){
	      std::cerr << " | Flux not 0.0" << listOfFlux[i] << " isNWRES:  "<< listOfPores[i]->info().isNWRes << " saturation: "<< listOfPores[i]->info().saturation << " P:"<< listOfPores[i]->info().p() << " isNWef:" << 
	      listOfPores[i]->info().isNWResDef << "|";
	      lostVolume += listOfFlux[i] * scene->dt;
	   }	  	 
	}

	
	double summFluxList = 0.0;
	double summFluxUnsat = 0.0;
	for(unsigned int i = 0; i < numberOfPores; i++){
	  if(hasInterfaceList[i]){summFluxUnsat += listOfFlux[i];}
	  summFluxList += listOfFlux[i];
	}

	//update saturation
	for(unsigned int i = 0; i < numberOfPores; i++){
	  if(!deformation && hasInterfaceList[i] && listOfFlux[i] != 0.0 && !listOfPores[i]->info().isWResInternal){		
       	      double ds = -1.0 * scene->dt * (listOfFlux[i]) / (listOfPores[i]->info().mergedVolume +  listOfPores[i]->info().accumulativeDV * scene->dt);
	      saturationList[i] = ds + (saturationList[i] * listOfPores[i]->info().mergedVolume / (listOfPores[i]->info().mergedVolume +  listOfPores[i]->info().accumulativeDV * scene->dt )); 
 	    if(deformation){
 		saturationList[i] = saturationList[i] + (pressuresList[i] - listOfPores[i]->info().p())*dsdp(listOfPores[i],listOfPores[i]->info().p()) + (scene->dt / (listOfPores[i]->info().mergedVolume +  (listOfPores[i]->info().accumulativeDV - listOfPores[i]->info().accumulativeDVSwelling) * scene->dt )) * listOfPores[i]->info().accumulativeDVSwelling;
 	    }
	  }  
	}
	
        for(unsigned int i = 0; i < numberOfPores; i++){
	  waterAfter += saturationList[i] * (listOfPores[i]->info().mergedVolume +   listOfPores[i]->info().accumulativeDV * scene->dt); 							
        }

	
	accumulativeFlux += (summFluxList) * scene->dt;
	accumulativeDeformationFlux += accumulativeDefFlux * scene->dt;
	
	fluxInViaWBC += boundaryFlux * scene->dt;
	
	  
 	if(!deformation && std::abs(boundaryFlux * scene->dt + (waterBefore - waterAfter)) / std::abs(boundaryFlux * scene->dt) > 1e-3 && std::abs(boundaryFlux) > 1e-18){	//FIXME test has to optimized for deforming pore units
 	  std::cerr << endl << "No volume balance! Flux balance: WBFlux:" << std::abs(boundaryFlux * scene->dt + (waterBefore - waterAfter)) / std::abs(boundaryFlux * scene->dt)  << " " <<boundaryFlux * scene->dt << "Flux: " << summFluxList*scene->dt <<  "deltaVolume: " << waterBefore - waterAfter  << "Flux in IFACE: "<< summFluxUnsat * scene->dt << " lostVolume: " << lostVolume * scene->dt;
// 	  stopSimulation = true;
 	}
	// --------------------------------------find new dt -----------------------------------------------------
	double dt = 0.0, finalDT = 1e6;
	int saveID = -1;
	for(int i = 0; i < numberOfPores; i++){
	  //Time step for deforming pore units
	  if(deformation){
	    dt = -1.0 * listOfPores[i]->info().mergedVolume / (listOfPores[i]->info().accumulativeDV + listOfPores[i]->info().accumulativeDVSwelling);	//Residence time total pore volume
	    if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;saveID = -1;}
	    
	    if(listOfPores[i]->info().accumulativeDVSwelling > 0.0 || listOfPores[i]->info().accumulativeDV > 0.0){ // Residence time during increase in pore size
	      if(listOfPores[i]->info().accumulativeDVSwelling > listOfPores[i]->info().accumulativeDV){
		dt = listOfPores[i]->info().mergedVolume * (1.0 - saturationList[i]) / listOfPores[i]->info().accumulativeDVSwelling;
	      }
	      if(listOfPores[i]->info().accumulativeDVSwelling <= listOfPores[i]->info().accumulativeDV){
		dt = listOfPores[i]->info().mergedVolume * (1.0 - saturationList[i]) / listOfPores[i]->info().accumulativeDV;
	      }
	      if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;saveID = -2;}
	    }
	    if(listOfPores[i]->info().accumulativeDVSwelling < 0.0 || listOfPores[i]->info().accumulativeDV < 0.0){
		if(listOfPores[i]->info().accumulativeDVSwelling < listOfPores[i]->info().accumulativeDV){
		  dt = -1.0 * listOfPores[i]->info().mergedVolume * saturationList[i] / listOfPores[i]->info().accumulativeDVSwelling;
		}
		if(listOfPores[i]->info().accumulativeDVSwelling >= listOfPores[i]->info().accumulativeDV){
		  dt = -1.0 * listOfPores[i]->info().mergedVolume * saturationList[i] / listOfPores[i]->info().accumulativeDV;
		}
		if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;saveID = -2;}
	    }
	  }
	  
	  //Time step for dynamic flow
	  if(hasInterfaceList[i]){		
	      //thresholdSaturation
	      if(std::abs(listOfPores[i]->info().thresholdSaturation - saturationList[i]) > truncationPrecision){
		dt =  -1.0 * (listOfPores[i]->info().thresholdSaturation - saturationList[i]) * listOfPores[i]->info().mergedVolume / listOfFlux[i];
		if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;/*saveID = 1;*/}
	      }
	      //Empty pore
	      if(std::abs(0.0 - saturationList[i]) > truncationPrecision && listOfFlux[i] > 0.0){ //only for drainage
		dt =  -1.0 * (0.0 - saturationList[i]) * listOfPores[i]->info().mergedVolume / listOfFlux[i];
		if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;/*saveID = 2;*/}
	      }
	      //Saturated pore
	      if(std::abs(1.0 - saturationList[i]) > truncationPrecision && listOfFlux[i] < 0.0){ //only for imbibition
		dt =  -1.0 * (1.0 - saturationList[i]) * listOfPores[i]->info().mergedVolume / listOfFlux[i];
		if(dt > deltaTimeTruncation && dt < finalDT){finalDT = dt;/*saveID = 3;*/}
	      }
	  }
	}
	if(finalDT == 1e6){
            finalDT = deltaTimeTruncation;
            saveID = 5;
            if(!firstDynTPF && !remesh){
                std::cout << endl << "NO dt found!";
                stopSimulation = true;}            
        }
	scene->dt = finalDT * safetyFactorTimeStep;
	if(debugTPF){std::cerr<<endl<< "Time step: " << finalDT << " Limiting process:" << saveID;}
	    
	
	
	// --------------------------------------update cappilary pressure (need to correct for linearization of ds/dp)-----------------------------------------------------
	for(int i = 0; i < numberOfPores; i++){
	  if(hasInterfaceList[i] && !listOfPores[i]->info().isWResInternal && (!deformation || listOfPores[i]->info().saturation != 0.0)){
	    pressuresList[i] = porePressureFromPcS(listOfPores[i],saturationList[i]);}
	}
	

	// --------------------------------------Find invasion events-----------------------------------------------------
	for(unsigned int i = 0; i < numberOfPores; i++){
	  if(saturationList[i] > 1.0 - truncationPrecision &&  (listOfFlux[i] < 0.0 || deformation) && saturationList[i] != 1.0){
		if(saturationList[i] > 1.0){
		 if(saturationList[listOfPores[i]->info().invadedFrom] >= 1.0){
		   	waterVolumeTruncatedLost +=  (saturationList[i] - 1.0) * listOfPores[i]->info().mergedVolume;
			saturationList[i] = 1.0;
		 }
		 if(saturationList[listOfPores[i]->info().invadedFrom] < 1.0){
		   saturationList[listOfPores[i]->info().invadedFrom] += (saturationList[i] - 1.0) * listOfPores[i]->info().mergedVolume / listOfPores[listOfPores[i]->info().invadedFrom]->info().mergedVolume;
		   saturationList[i] = 1.0;
		   if(saturationList[listOfPores[i]->info().invadedFrom] > 1.0){
		    waterVolumeTruncatedLost +=  (saturationList[listOfPores[i]->info().invadedFrom] - 1.0) * listOfPores[listOfPores[i]->info().invadedFrom]->info().mergedVolume;
		    saturationList[listOfPores[i]->info().invadedFrom] = 1.0;
		   }
		 }	 
		 
		}
 		saturationList[i] = 1.0;
 		hasInterfaceList[i] = false;
		listOfPores[i]->info().isNWResDef = true;
	  }
	}
	
	//Check for drainage
	for(unsigned int i = 0; i < numberOfPores; i++){
	  if((fractionMinSaturationInvasion == -1 && hasInterfaceList[i] && saturationList[i] <  listOfPores[i]->info().thresholdSaturation) || (fractionMinSaturationInvasion > 0.0 && saturationList[i] < fractionMinSaturationInvasion)){
	    for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
	      if(airBoundaryPressure - pressuresList[listOfPores[i]->info().poreNeighbors[j]] > listOfPores[i]->info().listOfEntryPressure[j] &&
		!hasInterfaceList[listOfPores[i]->info().poreNeighbors[j]] && 
		!listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isWResInternal && 
		saturationList[listOfPores[i]->info().poreNeighbors[j]] > listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().minSaturation &&
		saturationList[listOfPores[i]->info().poreNeighbors[j]] <= 1.0		
	      ){
		  hasInterfaceList[listOfPores[i]->info().poreNeighbors[j]] = true;
		  saturationList[listOfPores[i]->info().poreNeighbors[j]] = 1.0 - truncationPrecision;
		  pressuresList[listOfPores[i]->info().poreNeighbors[j]] = porePressureFromPcS(listOfPores[i], 1.0 - truncationPrecision);
		  listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().invadedFrom = i;

 	      }
	    }
	  }
	}
	    
	
	
	//truncate saturation
	for(unsigned int i = 0; i < numberOfPores; i++){
	  if((saturationList[i] < truncationPrecision || saturationList[i] <= listOfPores[i]->info().minSaturation)){		
	    waterVolumeTruncatedLost -=  (listOfPores[i]->info().minSaturation - saturationList[i]) * listOfPores[i]->info().mergedVolume;
	    saturationList[i] = listOfPores[i]->info().minSaturation; 
// 	    hasInterfaceList[i] = false; // NOTE: in case of deactivation of empty cell, set hasInterfaceList[i] to false
	    pressuresList[i] =  porePressureFromPcS(listOfPores[i], listOfPores[i]->info().minSaturation); //waterBoundaryPressure;				
	    listOfPores[i]->info().isNWRes = true;
	    for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
		if(!hasInterfaceList[listOfPores[i]->info().poreNeighbors[j]] && !listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isNWRes && listOfFlux[listOfPores[i]->info().poreNeighbors[j]] >= 0.0 && !listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isWResInternal){
		  hasInterfaceList[listOfPores[i]->info().poreNeighbors[j]] = true;
		  saturationList[listOfPores[i]->info().poreNeighbors[j]] = 1.0 - truncationPrecision;
		  pressuresList[listOfPores[i]->info().poreNeighbors[j]] = porePressureFromPcS(listOfPores[listOfPores[i]->info().poreNeighbors[j]], 1.0 - truncationPrecision);
		}
	    }
	  }
	  
	  if(listOfPores[i]->info().isNWResDef && saturationList[i] < listOfPores[i]->info().thresholdSaturation){
	    listOfPores[i]->info().isNWResDef = false;
	  }
	  
	}
 



    
    if(deformation){
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	  cell->info().poreBodyVolume += cell->info().dv() * oldDT;			
	}
	//copyPoreDataToCells();  //NOTE: For two-way coupling this function should be activated, but it is a bit costly for computations.
    }
    for(unsigned int i = 0; i < numberOfPores; i++){
		if(deformation){
		  listOfPores[i]->info().mergedVolume  += listOfPores[i]->info().accumulativeDV * oldDT;
		  listOfPores[i]->info().poreBodyRadius = getChi(listOfPores[i]->info().numberFacets)*std::pow(listOfPores[i]->info().mergedVolume,(1./3.));			
		}
		if(saturationList[i]  > 1.0){
		 std::cerr << endl << "Error!, saturation larger than 1? "; 
		 saturationList[i] = 1.0;								//NOTE ADDED AFTER TRUNK UPDATE should be 0.0?
// 		 stopSimulation = true;
		}
		listOfPores[i]->info().saturation = saturationList[i];
		listOfPores[i]->info().p() = pressuresList[i];
		listOfPores[i]->info().hasInterface = bool(hasInterfaceList[i]);
		listOfPores[i]->info().flux = listOfFlux[i];
		listOfPores[i]->info().dv() = 0.0;							//NOTE ADDED AFTER TRUNK UPDATE
		listOfPores[i]->info().accumulativeDV = 0.0;
    }
}

void TwoPhaseFlowEngine::getQuantities()
{
  double waterVolume = 0.0, pressureWaterVolume = 0.0, waterVolume_NHJ = 0.0, pressureWaterVolume_NHJ = 0.0, waterVolumeP = 0.0, YDimension  = 0.0, simplePressureAverage = 0.0;
  voidVolume = 0.0;
    for(unsigned int i = 0; i < numberOfPores; i++){
	  voidVolume += listOfPores[i]->info().mergedVolume;
	  waterVolume += listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation;
	  YDimension += solver->cellBarycenter(listOfPores[i])[1] * listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation;
	  simplePressureAverage += listOfPores[i]->info().mergedVolume * listOfPores[i]->info().p();
	  
	  if(std::abs(listOfPores[i]->info().p()) < 1e10){
	    pressureWaterVolume += listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation * listOfPores[i]->info().p();
	    waterVolumeP += listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation;
	  }
	  if(listOfPores[i]->info().saturation < 1.0){
	   waterVolume_NHJ +=  listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation;
	   pressureWaterVolume_NHJ += listOfPores[i]->info().mergedVolume * listOfPores[i]->info().saturation * listOfPores[i]->info().p();
	  }
 	
    }
    
    double areaAveragedPressureAcc = 0.0, areaSphere = 0.0;
    airWaterInterfacialArea = 0.0;
    for(unsigned int i = 0; i < numberOfPores; i++){
	if(listOfPores[i]->info().hasInterface){
	    if(listOfPores[i]->info().saturation < 1.0 && listOfPores[i]->info().saturation >= listOfPores[i]->info().thresholdSaturation){
	      areaSphere = 4.0 * 3.14159265359 * std::pow(getChi(listOfPores[i]->info().numberFacets) * std::pow(listOfPores[i]->info().mergedVolume * (1.0 - listOfPores[i]->info().saturation),0.3333),2); 
	    }
	    if(listOfPores[i]->info().saturation < listOfPores[i]->info().thresholdSaturation && listOfPores[i]->info().saturation > 0.0 && listOfPores[i]->info().saturation > listOfPores[i]->info().minSaturation){		//FIXME FIXME FIXME 1 june 2016
	      areaSphere = 4.0 * 3.14159265359 * std::pow((2.0 * surfaceTension / (-1.0*listOfPores[i]->info().p())),2.0) + 2.0 * getN(listOfPores[i]->info().numberFacets) * (listOfPores[i]->info().poreBodyRadius - (2.0 * surfaceTension / (-1.0*listOfPores[i]->info().p()))) * 
	      (2.0 * surfaceTension / (-1.0 * listOfPores[i]->info().p())) * (2.0 * 3.14159265359 - getDihedralAngle(listOfPores[i]->info().numberFacets));
	    }
	    areaAveragedPressureAcc += areaSphere * listOfPores[i]->info().p();
	    airWaterInterfacialArea += areaSphere;
      }
    }
 
    areaAveragedPressure = areaAveragedPressureAcc / airWaterInterfacialArea;
    waterSaturation = waterVolume / voidVolume;
    waterPressure = pressureWaterVolume / waterVolumeP;
    waterPressurePartiallySatPores = pressureWaterVolume_NHJ / waterVolume_NHJ;
    simpleWaterPressure = simplePressureAverage / voidVolume;
    totalWaterVolume = waterVolume; 
    
    
    
    if(!deformation){
      double volumeWaterVBC = 0.0, volumeWaterPressureBC = 0.0, volumeWaterBC = 0.0, volumeWaterAirBC = 0.0, volumeWaterPressureAirBC = 0.0, volumeAirBC = 0.0, Ybottom = 0.0, Ytop = 0.0;
      for(unsigned int i = 0; i < numberOfPores; i++){
      if(listOfPores[i]->info().waterBC){
	volumeWaterVBC += listOfPores[i]->info().saturation * listOfPores[i]->info().mergedVolume;
	volumeWaterPressureBC += listOfPores[i]->info().saturation * listOfPores[i]->info().mergedVolume * listOfPores[i]->info().p();
	volumeWaterBC +=  listOfPores[i]->info().mergedVolume;
	Ybottom += solver->cellBarycenter(listOfPores[i])[1] * listOfPores[i]->info().mergedVolume;
      }
      if(listOfPores[i]->info().airBC){
	volumeWaterAirBC += listOfPores[i]->info().saturation * listOfPores[i]->info().mergedVolume;
	volumeWaterPressureAirBC += listOfPores[i]->info().saturation * listOfPores[i]->info().mergedVolume * listOfPores[i]->info().p();
	volumeAirBC +=  listOfPores[i]->info().mergedVolume;
	Ytop += solver->cellBarycenter(listOfPores[i])[1] * listOfPores[i]->info().mergedVolume;
      }
      }
      double Stop = volumeWaterAirBC / volumeAirBC;  //air BC
      double Sbottom = volumeWaterVBC / volumeWaterBC;//Water BC.
      double Ptop = volumeWaterPressureAirBC / volumeWaterAirBC ;
      double Pbottom = volumeWaterPressureBC /  volumeWaterVBC ;
      double z = (((Ytop / volumeAirBC) - (Ybottom / volumeWaterBC)) / 2.0) + (Ybottom / volumeWaterBC);
      double gradP =  -1.0 * (Stop - Sbottom) +  (Stop * Ptop - Sbottom * Pbottom);
      double gradZ =  -1.0 * (YDimension / waterVolume)*(Stop - Sbottom) +  ((Stop * Ytop / volumeAirBC) - (Sbottom * Ybottom / volumeWaterBC));
      centroidAverageWaterPressure = waterPressure + (1.0 / gradZ) *  (z - (YDimension / waterVolume)) * gradP;
    }
}

void TwoPhaseFlowEngine::imposeDeformationFluxTPF()
{    
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();  
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
          cell->info().dv() = cell->info().dvTPF; //Only relevant for imposed deformation from python-shell
    }
    imposeDeformationFluxTPFSwitch = true;
}

void TwoPhaseFlowEngine::updateDeformationFluxTPF()
{
  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
  FiniteCellsIterator cellEnd = tri.finite_cells_end();  
  double dv = 0.0, summ = 0.0, summBC = 0.0, summMC = 0.0, SolidVolume = 0.0, dvSwelling = 0.0;
  
  if(!imposeDeformationFluxTPFSwitch){    
    setPositionsBuffer(true);
    updateVolumes(*solver);
    double volume = 0.0, invTime = (1.0 / scene->dt);
    if(scene->dt == 0.0){std::cerr<<" No dt found!";}
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      cell->info().dv() = 0.0;
	if(!cell->info().isFictious){
	  double solidVol = getSolidVolumeInCell(cell);
	  if(solidVol < 0.0){std::cerr<<"Error! negative pore body volume! " << solidVol; solidVol = 0.0;}
	  volume = cell->info().volume()*cell->info().volumeSign  - solidVol;
	  if(volume < 0.0){
	    volume = cell->info().poreBodyVolume;
	    listOfPores[cell->info().poreId]->info().isNWRes = true;
	    listOfPores[cell->info().poreId]->info().saturation = truncationPrecision;  
	  }
	  if(cell->info().apparentSolidVolume <= 0.0){cell->info().apparentSolidVolume = solidVol;}
	  cell->info().dvSwelling = (volume - cell->info().poreBodyVolume + solidVol - cell->info().apparentSolidVolume)*invTime - cell->info().dv();
	  if(cell->info().isNWRes || listOfPores[cell->info().poreId]->info().isNWRes){cell->info().dvSwelling  = 0.0;}
	  cell->info().dv() = (volume - cell->info().poreBodyVolume)*invTime; 
	  SolidVolume += solidVol;
	  summ += cell->info().dv();
	  summBC += cell->info().dv();
	}
    }
  }
  for(int i = 0; i < numberOfPores; i++){
    dv = 0.0, dvSwelling =0.0;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
	if(cell->info().poreId == (int) i){
	      dv += cell->info().dv();
	      dvSwelling += cell->info().dvSwelling;
	 }
    }

     listOfPores[i]->info().accumulativeDV = dv;
     listOfPores[i]->info().accumulativeDVSwelling = dvSwelling;
     summMC += dv;
  }
  for(int i = 0; i < numberOfPores; i++){
   if(listOfPores[i]->info().isNWRes){
     double count = 0.0;
     for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
       if(!listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isNWRes){
	  count += 1.0;
       }
     }
     for(unsigned int j = 0; j < listOfPores[i]->info().poreNeighbors.size(); j++){
       if(!listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().isNWRes){
	 if(count != 0.0){
	  listOfPores[listOfPores[i]->info().poreNeighbors[j]]->info().accumulativeDVSwelling += listOfPores[i]->info().accumulativeDVSwelling / count;
       }
       }
      }
      listOfPores[i]->info().accumulativeDVSwelling = 0.0;
     }
   }
}

double TwoPhaseFlowEngine::getSolidVolumeInCell(CellHandle cell)
{
  //Dublicate function that depends on position buffer of particles
  //FIXME this function be replaced if function of void volume can be made dependent on updated location of particles
  double Vsolid=0;
  cell->info().apparentSolidVolume = 0.0;
  for (int i=0;i<4;i++) {
      const Vector3r& p0v = positionBufferCurrent[cell->vertex (solver->permut4[i][0])->info().id()].pos;
      const Vector3r& p1v = positionBufferCurrent[cell->vertex (solver->permut4[i][1])->info().id()].pos;
      const Vector3r& p2v = positionBufferCurrent[cell->vertex (solver->permut4[i][2])->info().id()].pos;
      const Vector3r& p3v = positionBufferCurrent[cell->vertex (solver->permut4[i][3])->info().id()].pos;
      Point p0(p0v[0],p0v[1],p0v[2]);
      Point p1(p1v[0],p1v[1],p1v[2]);
      Point p2(p2v[0],p2v[1],p2v[2]);
      Point p3(p3v[0],p3v[1],p3v[2]);
      double rad = positionBufferCurrent[cell->vertex (solver->permut4[i][0])->info().id()].radius;

      double angle =  solver->fastSolidAngle(p0,p1,p2,p3);
      cell->info().particleSurfaceArea[i] = rad*rad*angle;
      
      
      if(setFractionParticles[cell->vertex(i)->info().id()] > 0){	//should be moved
	cell->info().apparentSolidVolume += rad*rad*angle / (setFractionParticles[cell->vertex(i)->info().id()] * setFractionParticles[cell->vertex(i)->info().id()]) ; //Coupling to account for swelling in dry pores
      }
      Vsolid += (1./3.) * std::pow(rad,3) * std::abs(angle);
    }
  return Vsolid;
}


void TwoPhaseFlowEngine::updatePoreUnitProperties()
{
  
  //FIXME clean-up this function (computePoreThroatRadiusMethod2() does not include update of particle location, thus this is a quick fix

  
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();  
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++){
      if(!cell->info().isFictious){
	for(unsigned int j = 0; j<4; j++){
	  if(cell->info().poreId != cell->neighbor(j)->info().poreId){
	    double rA = positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].radius;
	    double rB = positionBufferCurrent[cell->vertex(facetVertices[j][1])->info().id()].radius;
	    double rC = positionBufferCurrent[cell->vertex(facetVertices[j][2])->info().id()].radius;
	    CVector posA(positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].pos[0],positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].pos[1],positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].pos[2] );
	    CVector posB(positionBufferCurrent[cell->vertex(facetVertices[j][1])->info().id()].pos[0],positionBufferCurrent[cell->vertex(facetVertices[j][1])->info().id()].pos[1],positionBufferCurrent[cell->vertex(facetVertices[j][1])->info().id()].pos[2] );
	    CVector posC(positionBufferCurrent[cell->vertex(facetVertices[j][2])->info().id()].pos[0],positionBufferCurrent[cell->vertex(facetVertices[j][2])->info().id()].pos[1],positionBufferCurrent[cell->vertex(facetVertices[j][2])->info().id()].pos[2] );
	    CVector B = posB - posA; //positionBufferCurrent[cell->vertex(facetVertices[j][1])->info().id()].pos - positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].pos;
	    CVector x = B/sqrt(B.squared_length());
	    CVector C = posC - posA; //positionBufferCurrent[cell->vertex(facetVertices[j][2])->info().id()].pos - positionBufferCurrent[cell->vertex(facetVertices[j][0])->info().id()].pos;
	    CVector z = CGAL::cross_product(x,C);
	    CVector y = CGAL::cross_product(x,z);
	    y = y/std::sqrt(y.squared_length());

	    double b1[2]; b1[0] = B*x; b1[1] = B*y;
	    double c1[2]; c1[0] = C*x; c1[1] = C*y;

	    double A = ((std::pow(rA,2))*(1-c1[0]/b1[0])+((std::pow(rB,2)*c1[0])/b1[0])-std::pow(rC,2)+pow(c1[0],2)+std::pow(c1[1],2)-((std::pow(b1[0],2)+std::pow(b1[1],2))*c1[0]/b1[0]))/(2*c1[1]-2*b1[1]*c1[0]/b1[0]);
	    double BB = (rA-rC-((rA-rB)*c1[0]/b1[0]))/(c1[1]-b1[1]*c1[0]/b1[0]);
	    double CC = (std::pow(rA,2)-std::pow(rB,2)+std::pow(b1[0],2)+std::pow(b1[1],2))/(2*b1[0]);
	    double D = (rA-rB)/b1[0];
	    double E = b1[1]/b1[0];
	    double F = std::pow(CC,2)+std::pow(E,2)*std::pow(A,2)-2*CC*E*A;

	    double c = -F-std::pow(A,2)+pow(rA,2);
	    double b = 2*rA-2*(D-BB*E)*(CC-E*A)-2*A*BB;
	    double a = 1-std::pow((D-BB*E),2)-std::pow(BB,2);

	    if ((std::pow(b,2)-4*a*c)<0){std::cout << "NEGATIVE DETERMINANT" << endl; }
	    double reff = (-b+std::sqrt(pow(b,2)-4*a*c))/(2*a);



	    if (cell->vertex(facetVertices[j][2])->info().isFictious || cell->vertex(facetVertices[j][1])->info().isFictious || cell->vertex(facetVertices[j][2])->info().isFictious){
		reff = -1 * reff;
	    }
	    
	    cell->info().poreThroatRadius[j] = reff;
	    
	    for(unsigned int k = 0; k < listOfPores[cell->info().poreId]->info().poreNeighbors.size(); k++){
	      if(listOfPores[cell->info().poreId]->info().poreNeighbors[k] == cell->neighbor(j)->info().id){
		listOfPores[cell->info().poreId]->info().listOfEntryPressure[k] = reff;
		for(unsigned int l = 0; l < listOfPores[listOfPores[cell->info().poreId]->info().poreNeighbors[k]]->info().poreNeighbors.size(); l++){
		    if(listOfPores[listOfPores[cell->info().poreId]->info().poreNeighbors[k]]->info().poreNeighbors[l] == listOfPores[cell->info().poreId]->info().poreId){
		      listOfPores[cell->info().poreId]->info().listOfEntryPressure[k] = reff;
		}
	      }
	    }
	    }
	}
	}
      }
    }
  //FIXME replace all above with function: computePoreThroatRadiusMethod(), which includes particle update
   
   solver->computePermeability();  
}

void TwoPhaseFlowEngine::copyPoreDataToCells()
{
    //NOTE: Don't apply this function via python directly after applying reTriangulation() via Python, this will give segment fault. 
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();  
	for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
	  if(!cell->info().isFictious){
 		cell->info().saturation = listOfPores[cell->info().poreId]->info().saturation;
 		cell->info().p() = listOfPores[cell->info().poreId]->info().p();
 		cell->info().hasInterface = bool(hasInterfaceList[cell->info().poreId]);
 		cell->info().flux = listOfFlux[cell->info().poreId];
 		cell->info().isNWRes = listOfPores[cell->info().poreId]->info().isNWRes;
 		cell->info().airWaterArea = listOfPores[cell->info().poreId]->info().airWaterArea;
 		if(deformation){
		  cell->info().mergedVolume = listOfPores[cell->info().poreId]->info().mergedVolume;												//NOTE ADDED AFTER TRUNK UPDATE
		  cell->info().poreBodyRadius = getChi(cell->info().numberFacets)*std::pow(listOfPores[cell->info().poreId]->info().mergedVolume,(1./3.));			
		}//NOTE ADDED AFTER TRUNK UPDATE
														//NOTE ADDED AFTER TRUNK UPDATE
	    }
	  }  
}
        
void TwoPhaseFlowEngine::actionTPF()
{
  iterationTPF += 1;
  if(firstDynTPF){
      std::cout << endl << "Welcome to the two-phase flow Engine" << endl << "by T.Sweijen, B.Chareyre and S.M.Hassanizadeh" << endl << "For contact: T.Sweijen@uu.nl";
      solver->computePermeability();
      scene->time = 0.0;
      initialization();
      actionMergingAlgorithm();
      calculateResidualSaturation();
      setInitialConditions();
      setBoundaryConditions();
      verifyCompatibilityBC();
      setPoreNetwork();
      scene->dt = 1e-20;
      setListOfPores();
      solvePressure();
      getQuantities();
      firstDynTPF = false;
  }
  if(!firstDynTPF && !stopSimulation){
//    bool remesh = false;
   //Time steps + deformation, but no remeshing
   scene->time = scene->time + scene->dt;
   if(deformation && !remesh){
     updateDeformationFluxTPF();
     if(int(float(iterationTPF)/10.0) == float(iterationTPF)/10.0){
	updatePoreUnitProperties();
   }
  }
   //Update pore throat radii etc. 
   
   if(deformation && remesh){
     reTriangulate(); //retriangulation + merging
     calculateResidualSaturation();
     transferConditions(); //get saturation, hasInterface from previous network
     setBoundaryConditions();
     setPoreNetwork();
   }
   setListOfPores();
   if(solvePressureSwitch){solvePressure();}
   if(deformation){if(int(float(iterationTPF)/50.0) == float(iterationTPF)/50.0){getQuantities();}}	//FIXME update of quantities has to be made more appropiate

   
//    getQuantities();//NOTE FIX
   
    if(!deformation ){
      if(!getQuantitiesUpdateCont){
	if(int(float(iterationTPF)/100.0) == float(iterationTPF)/100.0){getQuantities();}
      }
      if(getQuantitiesUpdateCont){
	getQuantities();
      }
    }

   
   if(remesh){remesh = false;} //Remesh bool is also used in solvePressure();

  }
}


void TwoPhaseFlowEngine:: updateReservoirLabel()
{
    clusters[0]->reset(); clusters[0]->label=0;
    clusters[1]->reset(); clusters[1]->label=1;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();     
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().isNWRes) clusterGetPore(clusters[0].get(),cell);
      else if (cell->info().isWRes) { clusterGetPore(clusters[1].get(),cell);
	      for (int facet = 0; facet < 4; facet ++) if (!cell->neighbor(facet)->info().isWRes) clusterGetFacet(clusters[1].get(),cell,facet);}
      else if (cell->info().label>1) continue;
      else cell->info().label=-1;
    }
}

void TwoPhaseFlowEngine::clusterGetFacet(PhaseCluster* cluster, CellHandle cell, int facet) {
	cell->info().hasInterface = true;
	double interfArea = sqrt((cell->info().facetSurfaces[facet]*cell->info().facetFluidSurfacesRatio[facet]).squared_length());
	cluster->interfaces.push_back(std::pair<std::pair<unsigned int,unsigned int>,double>(
		std::pair<unsigned int,unsigned int>(cell->info().id,cell->neighbor(facet)->info().id),interfArea));
	cluster->interfacialArea += interfArea;
	if (cluster->entryRadius < cell->info().poreThroatRadius[facet]){
		cluster->entryRadius = cell->info().poreThroatRadius[facet];
		cluster->entryPore = cell->info().id;}
}

void TwoPhaseFlowEngine::clusterGetPore(PhaseCluster* cluster, CellHandle cell) {
	cell->info().label=cluster->label;
	cluster->volume+=cell->info().poreBodyVolume;
	cluster->pores.push_back(cell);
}

vector<int> TwoPhaseFlowEngine::clusterInvadePore(PhaseCluster* cluster, CellHandle cell)
{
	//invade the pore and attach to NW reservoir, label is assigned after reset
	int label = cell->info().label;
	cell->info().saturation=0;
	cell->info().isNWRes=true;
	clusterGetPore(clusters[0].get(),cell);
	
	//update the cluster(s)
	unsigned nPores = cluster->pores.size();
	vector<int> newClusters; //for returning the list of possible sub-clusters, empty if we are removing the last pore of the base cluster
	if (nPores==0) {LOG_WARN("Invading the empty cluster id="<<label); }
	if (nPores<2) {cluster->reset(); cluster->label = label; return  newClusters;}
	FOREACH(CellHandle& cell, cluster->pores) {cell->info().label=-1;} //mark all pores, and get them back in again below
	cell->info().label=0;//mark the invaded one
	
	//find a remaining pore	
	unsigned neighborStart=0;
	while ( (cell->neighbor(neighborStart)->info().label != -1 or
		   solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(neighborStart))) 
		   and neighborStart<3 ) ++neighborStart;	
	if (neighborStart==3 and cell->neighbor(neighborStart)->info().label != -1) cerr<<"This is not supposed to happen (line "<<__LINE__<<")"<<endl;

	auto nCell = cell->neighbor(neighborStart); //use the remaining pore to start reconstruction of the cluster
	nCell->info().label = label; //assign the label of the original cluster
	cluster->reset(); //reset pores, volume, entryRadius, area... but restore label again after that
	cluster->label = label;
	updateSingleCellLabelRecursion(nCell,cluster); //rebuild 
	newClusters.push_back(cluster->label);//we will return the original cluster itself if not empty
 	
	// gen new clusters on the fly from the other neighbors of the invaded pore (for disconnected subclusters)
	for (int neighborId=neighborStart+1 ; neighborId<=3; neighborId++) {//should be =1 if the cluster remain the same -1 removed pore
		const CellHandle& nCell = cell->neighbor(neighborId);
		if (nCell->info().label != -1 or solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue; //already reached from another neighbour (connected domain): skip, else this is a new cluster
		shared_ptr<PhaseCluster> clst (new PhaseCluster());
		clst->label=clusters.size();
		newClusters.push_back(clst->label);
		clusters.push_back(clst);
		updateSingleCellLabelRecursion(nCell,clusters.back().get());
	}
	return newClusters;// return list of created clusters
}

// int TwoPhaseFlowEngine:: getMaxCellLabel()
// {
//     int maxLabel=-1;
//     RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
//     FiniteCellsIterator cellEnd = tri.finite_cells_end();
//     for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//         if (cell->info().label>maxLabel) maxLabel=cell->info().label;
//     }
//     return maxLabel;
// }

void TwoPhaseFlowEngine::updateCellLabel()
{
//     int currentLabel = getMaxCellLabel();//FIXME: A loop on cells for each new label?? is it serious??
    updateReservoirLabel();
    int currentLabel = clusters.size();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().label==-1) {
	    shared_ptr<PhaseCluster> clst (new PhaseCluster());
	    clst->label=currentLabel;
	    clusters.push_back(clst);
	    updateSingleCellLabelRecursion(cell,clusters.back().get());
            currentLabel++;
        }
    }
}

void TwoPhaseFlowEngine::updateSingleCellLabelRecursion(CellHandle cell, PhaseCluster* cluster)
{
	clusterGetPore(cluster,cell);
//     cell->info().label=label;
//     cluster->volume+=cell->info().
//     cluster->pores.push_back(cell);
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
//         if (nCell->info().Pcondition) continue;
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        //TODO:the following condition may relax to relate to nCell->info().hasInterface
        if ( (nCell->info().saturation==cell->info().saturation) && (nCell->info().label!=cell->info().label) )
		updateSingleCellLabelRecursion(nCell,cluster);
	else if (nCell->info().isNWRes) clusterGetFacet(cluster,cell,facet);
    }
}

boost::python::list TwoPhaseFlowEngine::pyClusters() {
	boost::python::list ret;
	for(vector<shared_ptr<PhaseCluster> >::iterator it=clusters.begin(); it!=clusters.end(); ++it) ret.append(*it);
	return ret;}

void TwoPhaseFlowEngine::updatePressure()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().isWRes==true) {cell->info().p()=bndCondValue[2];}
      if (cell->info().isNWRes==true) {cell->info().p()=bndCondValue[3];}
      if (isPhaseTrapped) {
	if ( cell->info().isTrapW ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
	if ( cell->info().isTrapNW) {cell->info().p()=bndCondValue[2]+cell->info().trapCapP;}
	//check cell reservoir info.
	if ( !cell->info().isWRes && !cell->info().isNWRes && !cell->info().isTrapW && !cell->info().isTrapNW ) {cerr<<"ERROR! NOT FIND Cell Info!";}
// 	{cell->info().p()=bndCondValue[2]; if (isInvadeBoundary) cerr<<"Something wrong in updatePressure.(isInvadeBoundary)";}
      }
    } 
}

void TwoPhaseFlowEngine::invasion()
{
    if (isPhaseTrapped) invasion1();
    else invasion2();
}

///mode1 and mode2 can share the same invasionSingleCell(), invasionSingleCell() ONLY change neighbor pressure and neighbor saturation, independent of reservoirInfo.
void TwoPhaseFlowEngine::invasionSingleCell(CellHandle cell)
{
    double localPressure=cell->info().p();
    double localSaturation=cell->info().saturation;
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;//FIXME:defensive
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
	if (cell->info().poreThroatRadius[facet]<0) continue;

	if ( (nCell->info().saturation==localSaturation) && (nCell->info().p() != localPressure) && ((nCell->info().isTrapNW)||(nCell->info().isTrapW)) ) {
	  nCell->info().p() = localPressure;
	  if(solver->debugOut) {cerr<<"merge trapped phase"<<endl;}
	  invasionSingleCell(nCell);} ///here we merge trapped phase back to reservoir 
	else if ( (nCell->info().saturation>localSaturation) ) {
	  double nPcThroat=surfaceTension/cell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (localPressure-nCell->info().p()>nPcThroat) && (localPressure-nCell->info().p()>nPcBody) ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    nCell->info().hasInterface=false;
	    if(solver->debugOut) {cerr<<"drainage"<<endl;}
	    if (recursiveInvasion) invasionSingleCell(nCell);
	  }
////FIXME:Introduce cell.hasInterface	  
// 	  else if( (localPressure-nCell->info().p()>nPcThroat) && (localPressure-nCell->info().p()<nPcBody) && (cell->info().hasInterface==false) && (nCell->info().hasInterface==false) ) {
// 	    if(solver->debugOut) {cerr<<"invasion paused into pore interface "<<endl;}
// 	    nCell->info().hasInterface=true;
// 	  }
// 	  else continue;
	}
	else if ( (nCell->info().saturation<localSaturation) ) {
	  double nPcThroat=surfaceTension/cell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (nCell->info().p()-localPressure<nPcBody) && (nCell->info().p()-localPressure<nPcThroat) ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    if(solver->debugOut) {cerr<<"imbibition"<<endl;}
	    if (recursiveInvasion) invasionSingleCell(nCell);
	  }
//// FIXME:Introduce cell.hasInterface	  
// 	  else if ( (nCell->info().p()-localPressure<nPcBody) && (nCell->info().p()-localPressure>nPcThroat) /*&& (cell->info().hasInterface==false) && (nCell->info().hasInterface==false)*/ ) {
// 	    nCell->info().p() = localPressure;
// 	    nCell->info().saturation=localSaturation;
// 	    if(solver->debugOut) {cerr<<"imbibition paused pore interface"<<endl;}
// 	    nCell->info().hasInterface=true;
// 	  }
// 	  else continue;
	}
	else continue;
    }
}
///invasion mode 1: withTrap
void TwoPhaseFlowEngine::invasion1()
{
    if(solver->debugOut) {cout<<"----start invasion1----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invasion1.updatePressure----"<<endl;}
    
    ///invasionSingleCell by Pressure difference, change Pressure and Saturation.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    if(isDrainageActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isNWRes)
                invasionSingleCell(cell);
        }
    }
    if(isImbibitionActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isWRes)
                invasionSingleCell(cell);
        }
    }
    if(solver->debugOut) {cout<<"----invasion1.invasionSingleCell----"<<endl;}

    ///update W, NW reservoirInfo according to cell->info().saturation
    updateReservoirs1();
    if(solver->debugOut) {cout<<"----invasion1.update W, NW reservoirInfo----"<<endl;}
    
    ///search new trapped W-phase/NW-phase, assign trapCapP, isTrapW/isTrapNW flag for new trapped phases. But at this moment, the new trapped W/NW cells.P= W/NW-Res.P. They will be updated in next updatePressure() func.
    checkTrap(bndCondValue[3]-bndCondValue[2]);
    if(solver->debugOut) {cout<<"----invasion1.checkWTrap----"<<endl;}

    ///update trapped W-phase/NW-phase Pressure //FIXME: is this necessary?
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
 	if ( cell->info().isTrapW ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
	if ( cell->info().isTrapNW) {cell->info().p()=bndCondValue[2]+cell->info().trapCapP;}
   }
    if(solver->debugOut) {cout<<"----invasion1.update trapped W-phase/NW-phase Pressure----"<<endl;}

    if(isCellLabelActivated) updateCellLabel();
    if(solver->debugOut) {cout<<"----update cell labels----"<<endl;}
}

///search trapped W-phase or NW-phase, define trapCapP=Pn-Pw. assign isTrapW/isTrapNW info.
void TwoPhaseFlowEngine::checkTrap(double pressure)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//       if( (cell->info().isFictious) && (!cell->info().Pcondition) && (!isInvadeBoundary) ) continue;
      if( (cell->info().isWRes) || (cell->info().isNWRes) || (cell->info().isTrapW) || (cell->info().isTrapNW) ) continue;
      cell->info().trapCapP=pressure;
      if(cell->info().saturation==1.0) cell->info().isTrapW=true;
      if(cell->info().saturation==0.0) cell->info().isTrapNW=true;
    }
}

void TwoPhaseFlowEngine::updateReservoirs1()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if(cell->info().Pcondition) continue;
        cell->info().isWRes = false;
        cell->info().isNWRes = false;
    }
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
        if ((*it)==NULL) continue;
        WResRecursion(*it);
    }
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
        if ((*it)==NULL) continue;
        NWResRecursion(*it);
    }
}

void TwoPhaseFlowEngine::WResRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().saturation != 1.0) continue;
        if (nCell->info().isWRes==true) continue;
        nCell->info().isWRes = true;
        nCell->info().isNWRes = false;
        nCell->info().isTrapW = false;
	nCell->info().trapCapP=0.0;	
        WResRecursion(nCell);
    }
}

void TwoPhaseFlowEngine::NWResRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().saturation != 0.0) continue;
        if (nCell->info().isNWRes==true) continue;
        nCell->info().isNWRes = true;
        nCell->info().isWRes = false;
        nCell->info().isTrapNW = false;
	nCell->info().trapCapP=0.0;	
        NWResRecursion(nCell);
    }
}

///invasion mode 2: withoutTrap
void TwoPhaseFlowEngine::invasion2()
{
    if(solver->debugOut) {cout<<"----start invasion2----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invasion2.updatePressure----"<<endl;}
    
    ///drainageSingleCell by Pressure difference, change Pressure and Saturation.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    if(isDrainageActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isNWRes)
                invasionSingleCell(cell);
        }
    }
    ///drainageSingleCell by Pressure difference, change Pressure and Saturation.
    if(isImbibitionActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isWRes)
                invasionSingleCell(cell);
        }
    }

    if(solver->debugOut) {cout<<"----invasion2.invasionSingleCell----"<<endl;}
    
    ///update W, NW reservoirInfo according to Pressure
    updateReservoirs2();
    if(solver->debugOut) {cout<<"----drainage2.update W, NW reservoirInfo----"<<endl;}    
    
}

void TwoPhaseFlowEngine::updateReservoirs2()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().p()==bndCondValue[2]) {cell->info().isWRes=true; cell->info().isNWRes=false;}
        else if (cell->info().p()==bndCondValue[3]) {cell->info().isNWRes=true; cell->info().isWRes=false;}
        else {cerr<<"drainage mode2: updateReservoir Error!"<<endl;}
    }
}

double TwoPhaseFlowEngine::getMinDrainagePc()
{
    double nextEntry = 1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isNWRes == true) {
            for (int facet=0; facet<4; facet ++) {
	      CellHandle nCell = cell->neighbor(facet);
	      if (tri.is_infinite(nCell)) continue;
                if (nCell->info().Pcondition) continue;
//                 if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isWRes == true && cell->info().poreThroatRadius[facet]>0) {
                    double nCellP = std::max( (surfaceTension/cell->info().poreThroatRadius[facet]),(surfaceTension/nCell->info().poreBodyRadius) );
//                     double nCellP = surfaceTension/cell->info().poreThroatRadius[facet];
                    nextEntry = std::min(nextEntry,nCellP);}}}}
                    
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

double TwoPhaseFlowEngine::getMaxImbibitionPc()
{
    double nextEntry = -1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isWRes == true) {
            for (int facet=0; facet<4; facet ++) {
 	      CellHandle nCell = cell->neighbor(facet);
               if (tri.is_infinite(nCell)) continue;
                if (nCell->info().Pcondition) continue;
//                 if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isNWRes == true && cell->info().poreThroatRadius[facet]>0) {
                    double nCellP = std::min( (surfaceTension/nCell->info().poreBodyRadius), (surfaceTension/cell->info().poreThroatRadius[facet]));
                    nextEntry = std::max(nextEntry,nCellP);}}}}
                    
    if (nextEntry==-1e50) {
        cout << "End imbibition !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

double TwoPhaseFlowEngine::getSaturation(bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//NW-phase volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
        poresVolume = poresVolume + cell->info().poreBodyVolume;
        if (cell->info().saturation>0.0) {
            wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

///compute forces
void TwoPhaseFlowEngine::computeFacetPoreForcesWithCache(bool onlyCache)
{
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    CVector nullVect(0,0,0);
    //reset forces
    if (!onlyCache) for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) v->info().forces=nullVect;
// 	#ifdef parallel_forces
// 	if (solver->noCache) {
// 		solver->perVertexUnitForce.clear(); solver->perVertexPressure.clear();
// 		solver->perVertexUnitForce.resize(solver->T[solver->currentTes].maxId+1);
// 		solver->perVertexPressure.resize(solver->T[solver->currentTes].maxId+1);}
// 	#endif
// 	CellHandle neighbourCell;
// 	VertexHandle mirrorVertex;
    CVector tempVect;
    //FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
    if (solver->noCache) {//WARNING:all currentTes must be solver->T[solver->currentTes], should NOT be solver->T[currentTes]
        for (FlowSolver::VCellIterator cellIt=solver->T[solver->currentTes].cellHandles.begin(); cellIt!=solver->T[solver->currentTes].cellHandles.end(); cellIt++) {
            CellHandle& cell = *cellIt;
            //reset cache
            for (int k=0; k<4; k++) cell->info().unitForceVectors[k]=nullVect;

            for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
                    const CVector& Surfk = cell->info().facetSurfaces[j];
                    //FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
                    //The ratio void surface / facet surface
                    Real area = sqrt(Surfk.squared_length());
                    if (area<=0) cerr <<"AREA <= 0!! AREA="<<area<<endl;
                    CVector facetNormal = Surfk/area;
                    const std::vector<CVector>& crossSections = cell->info().facetSphereCrossSections;
                    CVector fluidSurfk = cell->info().facetSurfaces[j]*cell->info().facetFluidSurfacesRatio[j];
                    /// handle fictious vertex since we can get the projected surface easily here
                    if (cell->vertex(j)->info().isFictious) {
                        Real projSurf=std::abs(Surfk[solver->boundary(cell->vertex(j)->info().id()).coordinate]);
                        tempVect=-projSurf*solver->boundary(cell->vertex(j)->info().id()).normal;
                        cell->vertex(j)->info().forces = cell->vertex(j)->info().forces+tempVect*cell->info().p();
                        //define the cached value for later use with cache*p
                        cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
                    }
                    /// Apply weighted forces f_k=sqRad_k/sumSqRad*f
                    CVector facetUnitForce = -fluidSurfk*cell->info().solidLine[j][3];
                    CVector facetForce = cell->info().p()*facetUnitForce;

                    for (int y=0; y<3; y++) {
                        cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + facetForce*cell->info().solidLine[j][y];
                        //add to cached value
                        cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]+facetUnitForce*cell->info().solidLine[j][y];
                        //uncomment to get total force / comment to get only pore tension forces
                        if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
                            cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces -facetNormal*cell->info().p()*crossSections[j][y];
                            //add to cached value
                            cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]-facetNormal*crossSections[j][y];
                        }
                    }
// 	#ifdef parallel_forces
// 	solver->perVertexUnitForce[cell->vertex(j)->info().id()].push_back(&(cell->info().unitForceVectors[j]));
// 	solver->perVertexPressure[cell->vertex(j)->info().id()].push_back(&(cell->info().p()));
// 	#endif
                }
        }
        solver->noCache=false;//cache should always be defined after execution of this function
    }
    if (onlyCache) return;

//     else {//use cached values when triangulation doesn't change
// 		#ifndef parallel_forces
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); cell++) {
        for (int yy=0; yy<4; yy++) cell->vertex(yy)->info().forces = cell->vertex(yy)->info().forces + cell->info().unitForceVectors[yy]*cell->info().p();
    }

//  		#else
// 		#pragma omp parallel for num_threads(ompThreads)
// 		for (int vn=0; vn<= solver->T[solver->currentTes].maxId; vn++) {
// 			VertexHandle& v = solver->T[solver->currentTes].vertexHandles[vn];
// 			const int& id =  v->info().id();
// 			CVector tf (0,0,0);
// 			int k=0;
// 			for (vector<const Real*>::iterator c = solver->perVertexPressure[id].begin(); c != solver->perVertexPressure[id].end(); c++)
// 				tf = tf + (*(solver->perVertexUnitForce[id][k++]))*(**c);
// 			v->info().forces = tf;
// 		}
// 		#endif
//     }
    if (solver->debugOut) {
        CVector totalForce = nullVect;
        for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)	{
            if (!v->info().isFictious) totalForce = totalForce + v->info().forces;
            else if (solver->boundary(v->info().id()).flowCondition==1) totalForce = totalForce + v->info().forces;
        }
        cout << "totalForce = "<< totalForce << endl;
    }
}

bool TwoPhaseFlowEngine::detectBridge(RTriangulation::Finite_edges_iterator& edge)
{
	bool dryBridgeExist=true;
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	RTriangulation::Cell_circulator cell1 = Tri.incident_cells(*edge);
	RTriangulation::Cell_circulator cell0 = cell1++;
	if(cell0->info().saturation==1) {dryBridgeExist=false; return dryBridgeExist;}
	else {
	while (cell1!=cell0) {
	  if(cell1->info().saturation==1) {dryBridgeExist=false;break;}
	  else cell1++;}
	  return dryBridgeExist;
	}
}

bool TwoPhaseFlowEngine::isCellNeighbor(unsigned int cell1, unsigned int cell2)
{
  bool neighbor=false;
  for (unsigned int i=0;i<4;i++) {
    if (solver->T[solver->currentTes].cellHandles[cell1]->neighbor(i)->info().id==cell2)
    {neighbor=true;break;}
  }
  return neighbor;
}

void TwoPhaseFlowEngine::setPoreThroatRadius(unsigned int cell1, unsigned int cell2, double radius)
{
    if (isCellNeighbor(cell1,cell2)==false) {
        cout<<"cell1 and cell2 are not neighbors."<<endl;}
    else {
        for (unsigned int i=0; i<4; i++) {
            if (solver->T[solver->currentTes].cellHandles[cell1]->neighbor(i)->info().id==cell2)
                solver->T[solver->currentTes].cellHandles[cell1]->info().poreThroatRadius[i]=radius;
            if (solver->T[solver->currentTes].cellHandles[cell2]->neighbor(i)->info().id==cell1)
                solver->T[solver->currentTes].cellHandles[cell2]->info().poreThroatRadius[i]=radius;}}
}
double TwoPhaseFlowEngine::getPoreThroatRadius(unsigned int cell1, unsigned int cell2)
{
    double r =-1.;
    if (isCellNeighbor(cell1,cell2)==false) {
        cout<<"cell1 and cell2 are not neighbors."<<endl;}
    else {
        for (unsigned int i=0; i<4; i++) {
            if (solver->T[solver->currentTes].cellHandles[cell1]->neighbor(i)->info().id==cell2)
                r = solver->T[solver->currentTes].cellHandles[cell1]->info().poreThroatRadius[i];}}
    return r;
}

#endif //TwoPhaseFLOW


