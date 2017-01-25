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
YADE_PLUGIN((TwoPhaseFlowEngineT));
YADE_PLUGIN((TwoPhaseFlowEngine));
YADE_PLUGIN((PhaseCluster));

PhaseCluster::~PhaseCluster(){}

void TwoPhaseFlowEngine::initialization()
{
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(0.0,*solver);//create a triangulation and initialize pressure in the elements (connecting with W-reservoir), everything will be contained in "solver"
// 		initializeCellIndex();//initialize cell index
// 		if(isInvadeBoundary) {computePoreThroatRadius();}
// 		else {computePoreThroatRadiusTrickyMethod1();}//save pore throat radius before drainage. Thomas, here you can also revert this to computePoreThroatCircleRadius().
		
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
    cell->info().poreThroatRadius[i] = solver->computeEffectiveRadius(cell,i);
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
    double e[3]; //edges of triangulation
    double g[3]; //gap radius between solid
    
    for (int i=0; i<3; i++) {
      pos[i] = makeVector3r(cell->vertex(facetVertices[j][i])->point().point());
      r[i] = sqrt(cell->vertex(facetVertices[j][i])->point().weight());
    }
    
    e[0] = (pos[1]-pos[2]).norm();
    e[1] = (pos[2]-pos[0]).norm();
    e[2] = (pos[1]-pos[0]).norm();
    g[0] = ((e[0]-r[1]-r[2])>0) ? 0.5*(e[0]-r[1]-r[2]):0 ;
    g[1] = ((e[1]-r[2]-r[0])>0) ? 0.5*(e[1]-r[2]-r[0]):0 ;
    g[2] = ((e[2]-r[0]-r[1])>0) ? 0.5*(e[2]-r[0]-r[1]):0 ;
    
    double rmin= (std::max(g[0],std::max(g[1],g[2]))==0) ? 1.0e-10:std::max(g[0],std::max(g[1],g[2])) ;
    double rmax = std::abs(solver->computeEffectiveRadius(cell, j));
//     if(rmin>rmax) { cerr<<"WARNING! rmin>rmax. rmin="<<rmin<<" ,rmax="<<rmax<<endl; }
    
    double deltaForceRMin = computeDeltaForce(cell,j,rmin);
    double deltaForceRMax = computeDeltaForce(cell,j,rmax);
    double effPoreRadius;
    
    if(deltaForceRMin>deltaForceRMax) { effPoreRadius=rmax; }
    else if(deltaForceRMax<0) { effPoreRadius=rmax; }
    else if(deltaForceRMin>0) { effPoreRadius=rmin; }
    else { effPoreRadius=bisection(cell,j,rmin,rmax); }
    return effPoreRadius;
}
double TwoPhaseFlowEngine::bisection(CellHandle cell, int j, double a, double b)
{
    double m = 0.5*(a+b);
    if (std::abs(b-a)>std::abs((solver->computeEffectiveRadius(cell, j)*1.0e-6))) {
        if ( computeDeltaForce(cell,j,m) * computeDeltaForce(cell,j,a) < 0 ) {
            b = m;
            return bisection(cell,j,a,b);}
        else {
            a = m;
            return bisection(cell,j,a,b);}}
    else return m;
}
//calculate radian with law of cosines. (solve $\alpha$)
double TwoPhaseFlowEngine::computeTriRadian(double a, double b, double c)
{   
  double cosAlpha = (pow(b,2) + pow(c,2) - pow(a,2))/(2*b*c);
  if (cosAlpha>1.0) {cosAlpha=1.0;} if (cosAlpha<-1.0) {cosAlpha=-1.0;}
  double alpha = acos(cosAlpha);
  return alpha;
}

double TwoPhaseFlowEngine::computeDeltaForce(CellHandle cell,int j, double rC)
{
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
    
    double lNW = (rad[0][0]+rad[1][1]+rad[2][2])*rC;
    double lNS = (rad[3][0]-rad[1][0]-rad[2][0])*r[0] + (rad[3][1]-rad[2][1]-rad[0][1])*r[1] + (rad[3][2]-rad[1][2]-rad[0][2])*r[2] ;
    double lInterface=lNW+lNS;
    
    double sW0=0.5*rRc[1]*rRc[2]*sin(rad[0][0])-0.5*rad[0][0]*pow(rC,2)-0.5*rad[0][1]*pow(r[1],2)-0.5*rad[0][2]*pow(r[2],2) ;
    double sW1=0.5*rRc[2]*rRc[0]*sin(rad[1][1])-0.5*rad[1][1]*pow(rC,2)-0.5*rad[1][2]*pow(r[2],2)-0.5*rad[1][0]*pow(r[0],2) ;
    double sW2=0.5*rRc[0]*rRc[1]*sin(rad[2][2])-0.5*rad[2][2]*pow(rC,2)-0.5*rad[2][0]*pow(r[0],2)-0.5*rad[2][1]*pow(r[1],2) ;
    double sW=sW0+sW1+sW2;
    double sVoid=sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double sInterface=sVoid-sW;

    double deltaF = lInterface - sInterface/rC;//deltaF=surfaceTension*(perimeterPore - areaPore/rCap)
    return deltaF;
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


void TwoPhaseFlowEngine::savePoreNetwork()
{
    std::ofstream filePoreBodyRadius;
    std::cout << "Opening File: " << "PoreBodyRadius" << std::endl;
    filePoreBodyRadius.open("PoreBodyRadius.txt", std::ios::trunc);
    if(!filePoreBodyRadius.is_open()){
        std::cerr<< "Error opening file [" << "PoreBodyRadius" << ']' << std::endl;
        return;
    }
    
    std::ofstream filePoreBoundary;
    std::cout << "Opening File: " << "PoreBoundary" << std::endl;
    filePoreBoundary.open("PoreBoundary.txt" , std::ios::trunc);
    if(!filePoreBoundary.is_open()){
        std::cerr<< "Error opening file [" << "PoreBoundary" << ']' << std::endl;
        return;
    }
    
    std::ofstream filePoreBodyVolume;
    std::cout << "Opening File: " << "PoreBodyVolume" << std::endl;
    filePoreBodyVolume.open("PoreBodyVolume.txt", std::ios::trunc);
    if(!filePoreBodyVolume.is_open()){
        std::cerr<< "Error opening file [" << "PoreBodyVolume" << ']' << std::endl;
        return;
    }
    std::ofstream fileLocation;
    std::cout << "Opening File: " << "Location" << std::endl;
    fileLocation.open("Location.txt", std::ios::trunc);
    if(!fileLocation.is_open()){
        std::cerr<< "Error opening file [" << "fileLocation" << ']' << std::endl;
        return;
    }
    
    std::ofstream fileNeighbor;
    std::cout << "Opening File: " << "fileNeighbor" << std::endl;
    fileNeighbor.open("neighbor.txt", std::ios::trunc);
    if(!filePoreBoundary.is_open()){
        std::cerr<< "Error opening file [" << "fileNeighbor" << ']' << std::endl;
        return;
    }
    
    std::ofstream fileThroatRadius;
    std::cout << "Opening File: " << "fileThroatRadius" << std::endl;
    fileThroatRadius.open("throatRadius.txt", std::ios::trunc);
    if(!filePoreBoundary.is_open()){
        std::cerr<< "Error opening file [" << "fileThroatRadius" << ']' << std::endl;
        return;
    }
    std::ofstream fileThroats;
    std::cout << "Opening File: " << "fileThroats" << std::endl;
    fileThroats.open("Throats.txt", std::ios::trunc);
    if(!filePoreBoundary.is_open()){
        std::cerr<< "Error opening file [" << "fileThroats" << ']' << std::endl;
        return;
    }
    
    
    cout << solver->T[solver->currentTes].cellHandles.size();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
      if(cell->info().isGhost == false && cell->info().id < solver->T[solver->currentTes].cellHandles.size()){
      filePoreBodyRadius << cell->info().poreBodyRadius << '\n';
       filePoreBodyVolume << cell->info().poreBodyRadius << '\n';
    	CVector center ( 0,0,0 );
	for ( int k=0;k<4;k++ ){ center= center + 0.25* (cell->vertex(k)->point()-CGAL::ORIGIN);}

 	fileLocation  << center<< '\n';
	for (unsigned int i=0;i<4;i++){
	 if(cell->neighbor(i)->info().isGhost == false && cell->neighbor(i)->info().id < solver->T[solver->currentTes].cellHandles.size() && (cell->info().id < cell->neighbor(i)->info().id)){
	   fileNeighbor << cell->neighbor(i)->info().id << '\n';
	   fileThroatRadius << cell->info().poreThroatRadius[i] << '\n';
	   fileThroats << cell->info().id << " " << cell->neighbor(i)->info().id << '\n';
	 }
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


    filePoreBodyRadius.close();
    filePoreBoundary.close();
    filePoreBodyVolume.close();
    fileLocation.close();
    fileNeighbor.close();
    fileThroatRadius.close();


}


double TwoPhaseFlowEngine:: computePoreSatAtInterface(int ID)
{
//     //This function calculates the new saturation of pore at the interface between wetting/nonwetting 
//     //filled pores. It substracts the outgoing flux from the water volume  
//     RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
//     FiniteCellsIterator cellEnd = tri.finite_cells_end();
//     for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
//       if(cell->info().id == ID){
//     
//     double qout = 0.0, Vw = 0.0, dt = 0.0;
//     
//     for(unsigned int ngb = 0; ngb < 4; ngb++)
//     {
//       //find out/influx of water
//        if((cell->neighbor(ngb)->info().isWRes) && (cell->neighbor(ngb)->info().isFictious == 0)){
// 	qout= qout + std::abs(cell->info().kNorm() [ngb])*(cell->info().p()-cell->neighbor ( ngb )->info().p()); 
//        }
//     }
//    
//     Vw = (cell->info().saturation * cell->info().poreBodyVolume) - (qout * scene->dt); 
//     
//     //Functionality to calculate the smallest time step
//     if((1e-16 < Vw) && (Vw < 1e16)){
//       if((1e-16 < qout) && (qout < 1e16)){
// 	dt = (Vw / qout);
// 	if(dt!=0.0){dtDynTPF = std::min(dt,dtDynTPF);}
//       }
//     }
//     cell->info().saturation = Vw / cell->info().poreBodyVolume;
//       
//    // The following constrains check the value of saturation
//     if(std::abs(cell->info().saturation) < 1e-6){cell->info().saturation = 0.0;} // To prevent dt going to 0
//     if(cell->info().saturation < 0.0){
//       cout << endl << "dt was too large!, negative saturation in cell "<< cell->info().id;
//       cell->info().saturation = 0.0;    
//       }
//     if(cell->info().saturation > 1.0){
//       cout << endl <<"dt was too large!,saturation larger than 1 in cell " << cell->info().id;
//       cell->info().saturation = 1.0;}
//       return cell->info().saturation;
//       }  
// }
  return 0;
}


void TwoPhaseFlowEngine:: computePoreCapillaryPressure(CellHandle cell)
{
  //This formula relates the pore-saturation to the capillary-pressure, and the water-pressure
  //based on Joekar-Niasar, for cubic pores. NOTE: Needs to be changed into a proper set of equations 
/*
  double Re = 0.0, Pc = 0.0, Pg = 0.0;  
  
  for(unsigned int i = 0; i<4;i++)
  {
    Re = max(Re,cell->info().poreThroatRadius[i]);
  }
  Pc = surfaceTension / (Re * (1.0-exp(-6.83 * cell->info().saturation)));
  Pg = std::max(bndCondValue[2],bndCondValue[3]);
  cell->info().p() = Pg - Pc; */
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


#endif //TwoPhaseFLOW
 
