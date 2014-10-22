 
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
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line

#include "TwoPhaseFlowEngine.hpp"
#ifdef TWOPHASEFLOW

YADE_PLUGIN((TwoPhaseFlowEngineT));
YADE_PLUGIN((TwoPhaseFlowEngine));

void TwoPhaseFlowEngine::initializeCellIndex()
{
    int k=0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().index=k++;}
}

void TwoPhaseFlowEngine::computePoreBodyVolume()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        cell->info().poreBodyVolume = std::abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
    }
}

void TwoPhaseFlowEngine:: computePoreSatAtInterface(CellHandle cell)
{
    //This function calculates the new saturation of pore at the interface between wetting/nonwetting 
    //filled pores. It substracts the outgoing flux from the water volume  
    double qout = 0.0, Vw = 0.0;
    
    for(unsigned int ngb = 0; ngb < 4; ngb++)
    {
      //find out/influx of water
      if(cell->neighbor(ngb)->info().isWRes){
	qout= qout + std::abs(cell->info().kNorm() [ngb])*(cell->neighbor ( ngb )->info().p()-cell->info().p());    
      }
    }
   
    Vw = cell->info().saturation * cell->info().poreBodyVolume - (qout * scene->dt);  
    cell->info().saturation = Vw / cell->info().poreBodyVolume;
   
   
   // The following constrains check the value of saturation
    if(std::abs(cell->info().saturation) < 1e-6){cell->info().saturation = 0.0;} // To prevent dt going to 0
    if(cell->info().saturation < 0.0){
      cout << endl << "dt was too large!, negative saturation in cell "<< cell->info().id;
      cell->info().saturation = 0.0;    
      }
    if(cell->info().saturation > 1.0){
      cout << endl <<"dt was too large!,saturation larger than 1 in cell " << cell->info().id;
      cell->info().saturation = 1.0;}
}

void TwoPhaseFlowEngine:: computePoreCapillaryPressure(CellHandle cell)
{
  //This formula relates the pore-saturation to the capillary-pressure, and the water-pressure
  //based on Joekar-Niasar, for cubic pores. NOTE: Needs to be changed into a proper set of equations 

  double Re = 0.0, Pc = 0.0, Pg = 0.0;  
  
  for(unsigned int i = 0; i<4;i++)
  {
    Re = max(Re,cell->info().poreThroatRadius[i]);
  }
  Pc = surfaceTension / (Re * (1.0-exp(-6.83 * cell->info().saturation)));
  Pg = std::max(bndCondValue[2],bndCondValue[3]);
  cell->info().p() = Pg - Pc; 
}

void TwoPhaseFlowEngine::computePoreThroatCircleRadius()
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
	M(0,0) = 0.0;
	M(1,0) = d01;
	M(2,0) = d02;
	M(3,0) = d03;
	M(4,0) = pow((r0+Rin),2);
	M(5,0) = 1.0;
	
	M(0,1) = d01;
	M(1,1) = 0.0;
	M(2,1) = d12;
	M(3,1) = d13;
	M(4,1) = pow((r1+Rin),2);
	M(5,1) = 1.0;
	
	M(0,2) = d02; 
	M(1,2) = d12;
	M(2,2) = 0.0;
	M(3,2) = d23;
	M(4,2) = pow((r2+Rin),2);
	M(5,2) = 1.0;
	
	M(0,3) = d03;
	M(1,3) = d13;
	M(2,3) = d23;
	M(3,3) = 0.0;
	M(4,3) = pow((r3+Rin),2);
	M(5,3) = 1.0;
	
	M(0,4) = pow((r0+Rin),2);
	M(1,4) = pow((r1+Rin),2);
	M(2,4) = pow((r2+Rin),2);
	M(3,4) = pow((r3+Rin),2);
	M(4,4) = 0.0;
	M(5,4) = 1.0;
	
	M(0,5) = 1.0;
	M(1,5) = 1.0;
	M(2,5) = 1.0;
	M(3,5) = 1.0;
	M(4,5) = 1.0;
	M(5,5) = 0.0;
	
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
	
	  cout << endl << i << " "<<Rin << " "<< dR << " "<< M.determinant();
	  if(i > 4000){
	    cout << endl << "error, finding solution takes too long cell:" << cell->info().id;
	    check = true;
	  }
	  if ( std::abs(tempR - Rin)/Rin < 0.001){check = true;}
	
      }
    cell -> info().poreBodyRadius = Rin;
   }
}

void TwoPhaseFlowEngine::computePoreThroatRadius()
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
    
    if(deltaForceRMin>deltaForceRMax) {
      effPoreRadius=rmax;
    }
    else if(deltaForceRMax<0) {
      effPoreRadius=rmax;
    }
    else if(deltaForceRMin>0) {
      effPoreRadius=rmin;
    }
    else {
      effPoreRadius=bisection(cell,j,rmin,rmax);
    }
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
}

#endif //TwoPhaseFLOW
 
