/*************************************************************************
* Copyright (C) 2006 by luc Scholtes *
* luc.scholtes@hmg.inpg.fr *
* *
* This program is free software; it is licensed under the terms of the *
* GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//Modifs : Parameters renamed as MeniscusParameters
//id1/id2 as id1 is the smallest grain, FIXME : wetting angle?
//FIXME : in triaxialStressController, change test about null force in updateStiffnessccc
//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can just uncomment the following line
#define LAW2_SCGEOM_CAPILLARYPHYS_Capillarity1
#ifdef LAW2_SCGEOM_CAPILLARYPHYS_Capillarity1

#include <pkg/dem/Law2_ScGeom_CapillaryPhys_Capillarity1.hpp>
#include <pkg/common/ElastMat.hpp>

#include <pkg/dem/ScGeom.hpp>
#include <pkg/dem/HertzMindlin.hpp>
#include <core/Omega.hpp>
#include <core/Scene.hpp>
#include <lib/base/Math.hpp>
#include <iostream>
#include <fstream>


DT Law2_ScGeom_CapillaryPhys_Capillarity1::dtVbased;
DT Law2_ScGeom_CapillaryPhys_Capillarity1::dtPbased;

     Real Law2_ScGeom_CapillaryPhys_Capillarity1::intEnergy()
{
	Real energy=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		ScGeom* currentGeometry = static_cast<ScGeom*>(I->geom.get());
		CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>(I->phys.get());
 		if(phys) {
		  if (phys->SInterface!=0){
		       energy += liquidTension*(phys->SInterface-4*3.141592653589793238462643383279502884*(pow(currentGeometry->radius1,2))-4*3.141592653589793238462643383279502884*(pow(currentGeometry->radius2,2)));}
		}
 	}
	return energy;
} 


     Real Law2_ScGeom_CapillaryPhys_Capillarity1::wnInterface()
{
	Real wn=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		ScGeom* currentGeometry = static_cast<ScGeom*>(I->geom.get());
		CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>(I->phys.get());
 		if(phys) {
		  if (phys->SInterface!=0){
		    		  
			wn += (phys->SInterface-2*3.141592653589793238462643383279502884*(pow(currentGeometry->radius1,2)*(1+cos(phys->Delta1))+pow(currentGeometry->radius2,2)*(1+cos(phys->Delta2))));
		  }
		}
 	}
	return wn;
} 
 
    Real Law2_ScGeom_CapillaryPhys_Capillarity1::swInterface()
{
	Real sw=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		ScGeom* currentGeometry = static_cast<ScGeom*>(I->geom.get());
		CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>(I->phys.get());
 		if(phys) {
			sw += (2*3.141592653589793238462643383279502884*(pow(currentGeometry->radius1,2)*(1-cos(phys->Delta1))+pow(currentGeometry->radius2,2)*(1-cos(phys->Delta2))));
		  
		}
 	}
	return sw;
} 


     Real Law2_ScGeom_CapillaryPhys_Capillarity1::waterVolume()
{
	Real volume=0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions){
		if(!I->isReal()) continue;
		CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>(I->phys.get());
 		if(phys) {
		  volume += phys->vMeniscus;}
 	}
	return volume;
} 

void Law2_ScGeom_CapillaryPhys_Capillarity1::triangulateData() {
    /// We get data from a file and input them in triangulations
    if (solutions.size()>0) {LOG_WARN("Law2_ScGeom_CapillaryPhys_Capillarity1 asking triangulation for the second time. Ignored."); return;}
    ifstream file (inputFilename.c_str());
    if (!file.is_open()) { LOG_ERROR("No data file found for capillary law. Check path and inputFilename."); return;}

    // convention R,v,d,s,e,f,p,a1,a2,dummy (just for the example, define your own,
    // dummy is because has too much values per line - with one useless extra colum,)
    MeniscusPhysicalData dat;
    double ending;
    while ( file.good() ) {     
        file >>dat.succion>>dat.force>>dat.distance>>dat.volume>>dat.surface>>dat.arcLength>>dat.delta1>>dat.delta2>>dat.R>>ending;
	dat.ending=(bool) ending;
        solutions.push_back(dat);
    }
    file.close();
    // Make lists of points with index, so we can use range insertion, more efficient
    // see http://doc.cgal.org/latest/Triangulation_3/index.html#Triangulation_3SettingInformationWhileInserting
    std::vector< std::pair<K::Point_3,unsigned> > pointsP, pointsV;
    for (unsigned int k=0; k<solutions.size(); k++) {
        pointsP.push_back(std::make_pair(K::Point_3(solutions[k].R, solutions[k].succion, solutions[k].distance),k));
        pointsV.push_back(std::make_pair(K::Point_3(solutions[k].R, solutions[k].volume, solutions[k].distance),k));
    }
    // and now range insertion
    dtPbased.insert(pointsP.begin(), pointsP.end());
    dtVbased.insert(pointsV.begin(), pointsV.end());
}
 
YADE_PLUGIN((Law2_ScGeom_CapillaryPhys_Capillarity1));

int firstIteration=1;
int x=0;

void Law2_ScGeom_CapillaryPhys_Capillarity1::action()
{
    bool switched = (switchTriangulation == (imposePressure or totalVolumeConstant));
    switchTriangulation = (imposePressure or totalVolumeConstant);
    InteractionContainer::iterator ii = scene->interactions->begin();
    InteractionContainer::iterator iiEnd = scene->interactions->end();  
    if (imposePressure) {
      solver(capillaryPressure,switched);
    }
    else{
      if (((totalVolumeConstant || (!totalVolumeConstant && firstIteration==1)) && totalVolumeofWater!=-1) || (totalVolumeConstant && totalVolumeofWater==-1))
      { 
	if (!totalVolumeConstant) x=1;
	totalVolumeConstant=1;
	Real p0=capillaryPressure;
	Real slope;
	Real eps=0.0000000001;
	solver(p0,switched);
	Real V0=waterVolume();
	if (totalVolumeConstant && totalVolumeofWater==-1 && firstIteration==1){
	  totalVolumeofWater=V0;
	  firstIteration+=1;
	}
	Real p1=capillaryPressure+0.1;
	solver(p1,switched);
	Real V1=waterVolume();
	while (abs((totalVolumeofWater-V1)/totalVolumeofWater)>eps){
	  slope= (p1-p0)/(V1-V0);
	  p0=p1;
	  V0=V1;
	  p1=p1-slope*(V1-totalVolumeofWater);
	  if (p1<0) {
	    cout<< "The requested volume of water is quite big, the simulation will continue at constant suction.:"<< p0 <<endl;
  	    capillaryPressure=p0;
 	    imposePressure=1;
 	    break;
	  }
	  solver(p1,switched);
	  V1=waterVolume();
	  capillaryPressure=p1;
	}
	if (x==1){
	  totalVolumeConstant=0;
	  firstIteration+=1;
	}
      }
      else{
	if ((!totalVolumeConstant && firstIteration==1) && totalVolumeofWater==-1){
	  totalVolumeConstant=1;
	  solver(capillaryPressure,switched);
	  firstIteration+=1;
	  totalVolumeConstant=0;
	}
	else 
	{ 
	  solver(capillaryPressure,switched);
	}

      }
    }
    for (ii= scene->interactions->begin(); ii!=iiEnd ; ++ii) {
      CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>((*ii)->phys.get());
        if ((*ii)->isReal() && phys-> computeBridge==true) {
            CapillaryPhys1* cundallContactPhysics=NULL;
            MindlinCapillaryPhys* mindlinContactPhysics=NULL;
            if (!hertzOn) cundallContactPhysics = static_cast<CapillaryPhys1*>((*ii)->phys.get());//use CapillaryPhys for linear model
            else mindlinContactPhysics = static_cast<MindlinCapillaryPhys*>((*ii)->phys.get());//use MindlinCapillaryPhys for hertz model

            if ((hertzOn && mindlinContactPhysics->meniscus) || (!hertzOn && cundallContactPhysics->meniscus)) {
                if (fusionDetection) {//version with effect of fusion
//BINARY VERSION : if fusionNumber!=0 then no capillary force
                    short int& fusionNumber = hertzOn?mindlinContactPhysics->fusionNumber:cundallContactPhysics->fusionNumber;
                    if (binaryFusion) {
                        if (fusionNumber!=0) {	//cerr << "fusion" << endl;
                            hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap = Vector3r::Zero();
                            continue;
                        }
                    }
//LINEAR VERSION : capillary force is divided by (fusionNumber + 1) - NOTE : any decreasing function of fusionNumber can be considered in fact
                    else if (fusionNumber !=0) hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap /= (fusionNumber+1.);
                }
                scene->forces.addForce((*ii)->getId1(), hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap);
                scene->forces.addForce((*ii)->getId2(),-(hertzOn?mindlinContactPhysics->fCap:cundallContactPhysics->fCap));
            }
        }
    }

}
void Law2_ScGeom_CapillaryPhys_Capillarity1::checkFusion()
{
//Reset fusion numbers
    InteractionContainer::iterator ii = scene->interactions->begin();
    InteractionContainer::iterator iiEnd = scene->interactions->end();
    for( ; ii!=iiEnd ; ++ii ) {
        if ((*ii)->isReal()) {
            if (!hertzOn) static_cast<CapillaryPhys1*>((*ii)->phys.get())->fusionNumber=0;
            else static_cast<MindlinCapillaryPhys*>((*ii)->phys.get())->fusionNumber=0;
        }
    }

    std::list< shared_ptr<Interaction> >::iterator firstMeniscus, lastMeniscus, currentMeniscus;
    Real angle1 = -1.0;
    Real angle2 = -1.0;

    for ( int i=0; i< bodiesMenisciiList.size(); ++i ) { // i is the index (or id) of the body being tested
        CapillaryPhys1* cundallInteractionPhysics1=NULL;
        MindlinCapillaryPhys* mindlinInteractionPhysics1=NULL;
        CapillaryPhys1* cundallInteractionPhysics2=NULL;
        MindlinCapillaryPhys* mindlinInteractionPhysics2=NULL;
        if ( !bodiesMenisciiList[i].empty() ) {
            lastMeniscus = bodiesMenisciiList[i].end();
            for ( firstMeniscus=bodiesMenisciiList[i].begin(); firstMeniscus!=lastMeniscus; ++firstMeniscus ) { //FOR EACH MENISCUS ON THIS BODY...
                currentMeniscus = firstMeniscus;
                ++currentMeniscus;
                if (!hertzOn) {
                    cundallInteractionPhysics1 = YADE_CAST<CapillaryPhys1*>((*firstMeniscus)->phys.get());
                    if (i == (*firstMeniscus)->getId1()) angle1=cundallInteractionPhysics1->Delta1;//get angle of meniscus1 on body i
                    else angle1=cundallInteractionPhysics1->Delta2;
                }
                else {
                    mindlinInteractionPhysics1 = YADE_CAST<MindlinCapillaryPhys*>((*firstMeniscus)->phys.get());
                    if (i == (*firstMeniscus)->getId1()) angle1=mindlinInteractionPhysics1->Delta1;//get angle of meniscus1 on body i
                    else angle1=mindlinInteractionPhysics1->Delta2;
                }
                for ( ; currentMeniscus!= lastMeniscus; ++currentMeniscus) { //... CHECK FUSION WITH ALL OTHER MENISCII ON THE BODY
                    if (!hertzOn) {
                        cundallInteractionPhysics2 = YADE_CAST<CapillaryPhys1*>((*currentMeniscus)->phys.get());
                        if (i == (*currentMeniscus)->getId1()) angle2=cundallInteractionPhysics2->Delta1;//get angle of meniscus2 on body i
                        else angle2=cundallInteractionPhysics2->Delta2;
                    }
                    else {
                        mindlinInteractionPhysics2 = YADE_CAST<MindlinCapillaryPhys*>((*currentMeniscus)->phys.get());
                        if (i == (*currentMeniscus)->getId1()) angle2=mindlinInteractionPhysics2->Delta1;//get angle of meniscus2 on body i
                        else angle2=mindlinInteractionPhysics2->Delta2;
                    }
                    if (angle1==0 || angle2==0) cerr << "THIS SHOULD NOT HAPPEN!!"<< endl;


                    Vector3r normalFirstMeniscus = YADE_CAST<ScGeom*>((*firstMeniscus)->geom.get())->normal;
                    Vector3r normalCurrentMeniscus = YADE_CAST<ScGeom*>((*currentMeniscus)->geom.get())->normal;

                    Real normalDot = 0;
                    if ((*firstMeniscus)->getId1() == (*currentMeniscus)->getId1() || (*firstMeniscus)->getId2() == (*currentMeniscus)->getId2()) normalDot = normalFirstMeniscus.dot(normalCurrentMeniscus);
                    else normalDot = - (normalFirstMeniscus.dot(normalCurrentMeniscus));

                    Real normalAngle = 0;
                    if (normalDot >= 0 ) normalAngle = Mathr::FastInvCos0(normalDot);
                    else normalAngle = ((Mathr::PI) - Mathr::FastInvCos0(-(normalDot)));

                    if ((angle1+angle2)*Mathr::DEG_TO_RAD > normalAngle) {
                        if (!hertzOn) {
                            ++(cundallInteractionPhysics1->fusionNumber);    //count +1 if 2 meniscii are overlaping
                            ++(cundallInteractionPhysics2->fusionNumber);
                        }
                        else {
                            ++(mindlinInteractionPhysics1->fusionNumber);
                            ++(mindlinInteractionPhysics2->fusionNumber);
                        }
                    };
                }
            }
        }
    }
}


BodiesMenisciiList1::BodiesMenisciiList1(Scene * scene)
{
    initialized=false;
    prepare(scene);
}

bool BodiesMenisciiList1::prepare(Scene * scene)
{
    interactionsOnBody.clear();
    shared_ptr<BodyContainer>& bodies = scene->bodies;

    Body::id_t MaxId = -1;
    BodyContainer::iterator bi = bodies->begin();
    BodyContainer::iterator biEnd = bodies->end();
    for( ; bi!=biEnd ; ++bi )
    {
        MaxId=max(MaxId, (*bi)->getId());
    }
    interactionsOnBody.resize(MaxId+1);
    for ( unsigned int i=0; i<interactionsOnBody.size(); ++i )
    {
        interactionsOnBody[i].clear();
    }

    InteractionContainer::iterator ii = scene->interactions->begin();
    InteractionContainer::iterator iiEnd = scene->interactions->end();
    for( ; ii!=iiEnd ; ++ii ) {
        if ((*ii)->isReal()) {
            if (static_cast<CapillaryPhys1*>((*ii)->phys.get())->meniscus) insert(*ii);
        }
    }

    return initialized=true;
}

bool BodiesMenisciiList1::insert(const shared_ptr< Interaction >& interaction)
{
    interactionsOnBody[interaction->getId1()].push_back(interaction);
    interactionsOnBody[interaction->getId2()].push_back(interaction);
    return true;
}


bool BodiesMenisciiList1::remove(const shared_ptr< Interaction >& interaction)
{
    interactionsOnBody[interaction->getId1()].remove(interaction);
    interactionsOnBody[interaction->getId2()].remove(interaction);
    return true;
}

list< shared_ptr<Interaction> >& BodiesMenisciiList1::operator[] (int index)
{
    return interactionsOnBody[index];
}

int BodiesMenisciiList1::size()
{
    return interactionsOnBody.size();
}

void BodiesMenisciiList1::display()
{
    list< shared_ptr<Interaction> >::iterator firstMeniscus;
    list< shared_ptr<Interaction> >::iterator lastMeniscus;
    for ( unsigned int i=0; i<interactionsOnBody.size(); ++i )
    {
        if ( !interactionsOnBody[i].empty() )
        {
            lastMeniscus = interactionsOnBody[i].end();
            for ( firstMeniscus=interactionsOnBody[i].begin(); firstMeniscus!=lastMeniscus; ++firstMeniscus )
            {
                if ( *firstMeniscus ) {
                    if ( firstMeniscus->get() )
                        cerr << "(" << ( *firstMeniscus )->getId1() << ", " << ( *firstMeniscus )->getId2() <<") ";
                    else cerr << "(void)";
                }
            }
            cerr << endl;
        }
        else cerr << "empty" << endl;
    }
}

BodiesMenisciiList1::BodiesMenisciiList1()
{
    initialized=false;
}

void Law2_ScGeom_CapillaryPhys_Capillarity1::solver(Real suction,bool reset)
{
     
    if (!scene) cerr << "scene not defined!";
    shared_ptr<BodyContainer>& bodies = scene->bodies;
    if (dtPbased.number_of_vertices ()<1 ) triangulateData();
    if (fusionDetection && !bodiesMenisciiList.initialized) bodiesMenisciiList.prepare(scene);
    InteractionContainer::iterator ii = scene->interactions->begin();
    InteractionContainer::iterator iiEnd = scene->interactions->end();
     bool hertzInitialized = false;
       Real i=0;

      for (; ii!=iiEnd ; ++ii) {

 	i+=1;
	CapillaryPhys1* phys = dynamic_cast<CapillaryPhys1*>((*ii)->phys.get());////////////////////////////////////////////////////////////////////////////////////////////
 

        if ((*ii)->isReal() && phys-> computeBridge==true) {
            const shared_ptr<Interaction>& interaction = *ii;
            if (!hertzInitialized) {//NOTE: We are assuming that only one type is used in one simulation here
                if (CapillaryPhys1::getClassIndexStatic()==interaction->phys->getClassIndex()) hertzOn=false;
                else if (MindlinCapillaryPhys::getClassIndexStatic()==interaction->phys->getClassIndex()) hertzOn=true;
                else LOG_ERROR("The capillary law is not implemented for interactions using"<<interaction->phys->getClassName());
            }
            hertzInitialized = true;
            CapillaryPhys1* cundallContactPhysics=NULL;
            MindlinCapillaryPhys* mindlinContactPhysics=NULL;

/// contact physics depends on the contact law, that is used (either linear model or hertz model)
            if (!hertzOn) cundallContactPhysics = static_cast<CapillaryPhys1*>(interaction->phys.get());//use CapillaryPhys for linear model
            else mindlinContactPhysics = static_cast<MindlinCapillaryPhys*>(interaction->phys.get());//use MindlinCapillaryPhys for hertz model

            unsigned int id1 = interaction->getId1();
            unsigned int id2 = interaction->getId2();

/// interaction geometry search (this test is to compute capillarity only between spheres (probably a better way to do that)
            int geometryIndex1 = (*bodies)[id1]->shape->getClassIndex(); // !!!
            int geometryIndex2 = (*bodies)[id2]->shape->getClassIndex();
            if (!(geometryIndex1 == geometryIndex2)) continue;

/// definition of interacting objects (not necessarily in contact)
            ScGeom* currentContactGeometry = static_cast<ScGeom*>(interaction->geom.get());
	    
/// Interacting Grains:
// If you want to define a ratio between YADE sphere size and real sphere size
            Real alpha=1;
            Real R1 = alpha*std::max(currentContactGeometry->radius2,currentContactGeometry->radius1);
            Real R2 =alpha*std::min(currentContactGeometry->radius2,currentContactGeometry->radius1);
	  
	    shared_ptr<BodyContainer>& bodies = scene->bodies;

	    Real N=bodies->size();
            Real epsilon1,epsilon2;
 	    Real ran1= (N-id1+0.5)/(N+1);
	    Real ran2= (N-id2+0.5)/(N+1);
	    epsilon1 = epsilonMean*(2*(ran1-0.5)* disp +1);//addednow
	    epsilon2 = epsilonMean*(2*(ran2-0.5)* disp +1);
//   	    cout << epsilon1 << "separate" <<epsilon2 <<endl;
	    R1 = R1-epsilon1*R1;  
	    R2 =R2-epsilon2*R2;
	    
/// intergranular distance
            Real D = alpha*(-(currentContactGeometry->penetrationDepth))+epsilon1*R1+epsilon2*R2;
	    if ((currentContactGeometry->penetrationDepth>=0)|| D<=0 || createDistantMeniscii) { //||(scene->iter < 1) ) // a simplified way to define meniscii everywhere
                if (!hertzOn) {
                    if (fusionDetection && !cundallContactPhysics->meniscus) bodiesMenisciiList.insert((*ii));
                    cundallContactPhysics->meniscus=true;
                } else {
                    if (fusionDetection && !mindlinContactPhysics->meniscus) bodiesMenisciiList.insert((*ii));
                    mindlinContactPhysics->meniscus=true;
                }
                
            
            }
            Real Dinterpol = D/R1;
 
/// Suction (Capillary pressure):
	    if (imposePressure || (!imposePressure && totalVolumeConstant)){
	      Real Pinterpol = 0;
	      if (!hertzOn) Pinterpol = cundallContactPhysics->isBroken ? 0 : suction*R1/liquidTension;
	      else Pinterpol = mindlinContactPhysics->isBroken ? 0 : suction*R1/liquidTension;
	      if (!hertzOn) cundallContactPhysics->capillaryPressure = suction;
	      else mindlinContactPhysics->capillaryPressure = suction;
	      /// Capillary solution finder:
	      if ((Pinterpol>=0) && (hertzOn? mindlinContactPhysics->meniscus : cundallContactPhysics->meniscus)) {//FIXME: need an "else {delete}"
		MeniscusPhysicalData solution = interpolate1(dtPbased,K::Point_3(R2/R1, Pinterpol, Dinterpol), cundallContactPhysics->m, solutions, reset);
/// capillary adhesion force
		Real Finterpol = solution.force;
		Vector3r fCap = Finterpol*R1*liquidTension*currentContactGeometry->normal;
		if (!hertzOn) cundallContactPhysics->fCap = fCap;
		else mindlinContactPhysics->fCap = fCap;
/// meniscus volume
//FIXME: hardcoding numerical constants is bad practice generaly, and it probably reveals a flaw in that case (Bruno)
		Real Vinterpol = solution.volume*pow(R1,3);
		Real SInterface = solution.surface*pow(R1,2);
		if (!hertzOn) { 
		   cundallContactPhysics->vMeniscus = Vinterpol;
                    cundallContactPhysics->SInterface = SInterface;
                    if (Vinterpol > 0) cundallContactPhysics->meniscus = true;
                    else cundallContactPhysics->meniscus = false;
                } else {
                    mindlinContactPhysics->vMeniscus = Vinterpol;
                    if (Vinterpol > 0) mindlinContactPhysics->meniscus = true;
                    else mindlinContactPhysics->meniscus = false;
                }
                if (cundallContactPhysics->meniscus== false) cundallContactPhysics->SInterface=4*3.141592653589793238462643383279502884*(pow(R1,2))+4*3.141592653589793238462643383279502884*(pow(R2,2));
                if (!Vinterpol) {
                    if ((fusionDetection) || (hertzOn ? mindlinContactPhysics->isBroken : cundallContactPhysics->isBroken)) bodiesMenisciiList.remove((*ii));
/// FIXME: the following D>(...) test is wrong, should be based on penetrationDepth, and should "continue" after erasing
		    if (D>((interactionDetectionFactor-1)*(currentContactGeometry->radius2+currentContactGeometry->radius1))) scene->interactions->requestErase(interaction);
                }
/// wetting angles
                if (!hertzOn) {
                    cundallContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                    cundallContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                } else {
                    mindlinContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                    mindlinContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                }
            }
	      
	    }
	    else{
	      if (cundallContactPhysics->vMeniscus==0 and cundallContactPhysics->capillaryPressure!=0){//FIXME: test capillaryPressure consistently (!0 or >0 or >=0?!)
		Real Pinterpol = 0;
	        if (!hertzOn) Pinterpol = cundallContactPhysics->isBroken ? 0 : cundallContactPhysics->capillaryPressure*R1/liquidTension;
	        else Pinterpol = mindlinContactPhysics->isBroken ? 0 : cundallContactPhysics->capillaryPressure*R1/liquidTension;
// 	        if (!hertzOn) cundallContactPhysics->capillaryPressure = suction;
// 	        else mindlinContactPhysics->capillaryPressure = suction;
	        /// Capillary solution finder:
	        if ((Pinterpol>=0) && (hertzOn? mindlinContactPhysics->meniscus : cundallContactPhysics->meniscus)) {
		  MeniscusPhysicalData solution = interpolate1(dtPbased,K::Point_3(R2/R1, Pinterpol, Dinterpol), cundallContactPhysics->m, solutions, reset);
/// capillary adhesion force
		  Real Finterpol = solution.force;
		  Vector3r fCap = Finterpol*R1*liquidTension*currentContactGeometry->normal;
		  if (!hertzOn) cundallContactPhysics->fCap = fCap;
		  else mindlinContactPhysics->fCap = fCap;
/// meniscus volume
//FIXME: hardcoding numerical constants is bad practice generaly, and it probably reveals a flaw in that case (Bruno)
		  Real Vinterpol = solution.volume*pow(R1,3);
		  Real SInterface = solution.surface*pow(R1,2);
		  if (!hertzOn) { 
		      cundallContactPhysics->vMeniscus = Vinterpol;
                      cundallContactPhysics->SInterface = SInterface;
                      if (Vinterpol > 0) cundallContactPhysics->meniscus = true;
                      else cundallContactPhysics->meniscus = false;
                  } else {
                      mindlinContactPhysics->vMeniscus = Vinterpol;
                      if (Vinterpol > 0) mindlinContactPhysics->meniscus = true;
                      else mindlinContactPhysics->meniscus = false;
                  }
                  if (cundallContactPhysics->meniscus== false) cundallContactPhysics->SInterface=4*3.141592653589793238462643383279502884*(pow(R1,2))+4*3.141592653589793238462643383279502884*(pow(R2,2));
                  if (!Vinterpol) {
                      if ((fusionDetection) || (hertzOn ? mindlinContactPhysics->isBroken : cundallContactPhysics->isBroken)) bodiesMenisciiList.remove((*ii));
		      if (D>((interactionDetectionFactor-1)*(currentContactGeometry->radius2+currentContactGeometry->radius1))) scene->interactions->requestErase(interaction);
                  }
/// wetting angles
                  if (!hertzOn) {
                      cundallContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                      cundallContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                  } else {
                      mindlinContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                      mindlinContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                  }
	      }
	      }
 	      else{
		Real Vinterpol = 0;
		if (!hertzOn) { Vinterpol = cundallContactPhysics->vMeniscus/pow(R1,3);  
	        }
	        else Vinterpol = mindlinContactPhysics->vMeniscus;
	        /// Capillary solution finder:
	        if ((hertzOn? mindlinContactPhysics->meniscus : cundallContactPhysics->meniscus)) {
		  MeniscusPhysicalData solution = interpolate2(dtVbased,K::Point_3(R2/R1, Vinterpol, Dinterpol), cundallContactPhysics->m, solutions,reset);
/// capillary adhesion force
	          Real Finterpol = solution.force;
                  Vector3r fCap = Finterpol*R1*liquidTension*currentContactGeometry->normal;
                  if (!hertzOn) cundallContactPhysics->fCap = fCap;
                  else mindlinContactPhysics->fCap = fCap;
/// suction and interfacial area
		
                  Real Pinterpol = solution.succion*liquidTension/R1;
                  Real SInterface = solution.surface*pow(R1,2);
                  if (!hertzOn) { 
                      cundallContactPhysics->capillaryPressure = Pinterpol;
                      cundallContactPhysics->SInterface = SInterface;
                      if (Finterpol > 0) cundallContactPhysics->meniscus = true;
                      else{
		        cundallContactPhysics->vMeniscus=0;
		        cundallContactPhysics->meniscus = false;
		      }
                  } else {
                      mindlinContactPhysics->capillaryPressure = Pinterpol;
                      if (Finterpol > 0) mindlinContactPhysics->meniscus = true;
                      else {
		        cundallContactPhysics->vMeniscus=0;
		        mindlinContactPhysics->meniscus = false;
		      }
                  }
                  if (!Vinterpol) {
                      if ((fusionDetection) || (hertzOn ? mindlinContactPhysics->isBroken : cundallContactPhysics->isBroken)) bodiesMenisciiList.remove((*ii));
                      if (D>((interactionDetectionFactor-1)*(currentContactGeometry->radius2+currentContactGeometry->radius1))) scene->interactions->requestErase(interaction);
                  }
/// wetting angles
                  if (!hertzOn) {
                      cundallContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                      cundallContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                  } else {
                      mindlinContactPhysics->Delta1 = max(solution.delta1,solution.delta2);
                      mindlinContactPhysics->Delta2 = min(solution.delta1,solution.delta2);
                  }
		}
	      }
	    }
	    


///interaction is not real //If the interaction is not real, it should not be in the list
        } else if (fusionDetection) bodiesMenisciiList.remove((*ii));
      

    }
    if (fusionDetection) checkFusion();

}

#endif //LAW2_SCGEOM_CAPILLARYPHYS_Capillarity1
