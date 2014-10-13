 
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
#define TWOPHASEFLOW
#ifdef TWOPHASEFLOW

#include "TwoPhaseFlowEngine.hpp"

YADE_PLUGIN((TwoPhaseFlowEngineT));



YADE_PLUGIN((TwoPhaseFlowEngine));

void TwoPhaseFlowEngine::fancyFunction(Real what) {std::cerr<<"yes, I'm a new function"<<std::endl;}




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


void TwoPhaseFlowEngine::computePoreThroatRadius()
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




#endif //TwoPhaseFLOW
 
