/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <yade/pkg-common/ParticleParameters.hpp>
 
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>


#include <yade/pkg-common/InteractingSphere.hpp>
#include "VolumicContactLaw.hpp"
#include<yade/extra/TesselationWrapper.h>
#include <time.h>

VolumicContactLaw::VolumicContactLaw() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum)
{
	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
}


void VolumicContactLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
// 	REGISTER_ATTRIBUTE(compacite_init);
}



void VolumicContactLaw::speedTest(MetaBody* ncb)
{
//BEGIN SPEED TEST
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	TesselationWrapper T;
	BodyContainer::iterator biBegin    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	BodyContainer::iterator bi = biBegin;
	for(  ; bi!=biEnd ; ++bi )
	{
		if ((*bi)->isDynamic) {//means "is it a sphere (not a wall)"
			const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi)->interactingGeometry.get());
			const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
			T.checkMinMax(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius);
		}
	}
	
	clock_t	T1 = clock();	
	
	for (int j=0; j<30; j++)
	{
		T.clear2();
		T.bounded = false;
		for( bi = biBegin; bi!=biEnd ; ++bi )
		{
			if ((*bi)->isDynamic) {//means "is it a sphere (not a wall)"
				const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi)->interactingGeometry.get());
				const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
				T.insert(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, (*bi)->getId());
			}
		}
		//T.AddBoundingPlanes();	
	}
	
	
	cerr << "Bouding planes apres : time = "<< difftime(clock(), T1)/ CLOCKS_PER_SEC << endl;
	T1 = clock();
	
	
	for (int j=0; j<30; j++)
	{	
		T.clear2();
		T.bounded = false;
		//T.AddBoundingPlanes();
		for( bi = biBegin; bi!=biEnd ; ++bi )
		{
			if ((*bi)->isDynamic) {//means "is it a sphere (not a wall)"
				const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi)->interactingGeometry.get());
				const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
				T.insert(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, (*bi)->getId());
			}
		
		}
	
	}	
	
	cerr << "Bouding planes avant : time = "<< difftime(clock(), T1)/ CLOCKS_PER_SEC << endl;
//END SPEED TEST




}


void VolumicContactLaw::action(MetaBody* ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

	speedTest(ncb);	

	//BEGIN VORONOI TESSELATION
	TesselationWrapper T1;	
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();

	for(  ; bi!=biEnd ; ++bi )
	{
		if ((*bi)->isDynamic) {//means "is it a sphere (not a wall)"
			const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi)->interactingGeometry.get());
			const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
			T1.insert(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, (*bi)->getId());
	
		}
	}
// 	T1.ComputeVolumes();

 

// ====================================================================================================== JF
// 	Real bottom= 0, top=1, front=5 ,back=4 ,left=2 ,right=3 ;
		PhysicalParameters* p=static_cast<PhysicalParameters*> ( Body::byId ( 0 )->physicalParameters.get() );
	Real pminy = p->se3.position[1];

		PhysicalParameters* p1=static_cast<PhysicalParameters*> ( Body::byId ( 1 )->physicalParameters.get() );
	Real pmaxy = p1->se3.position[1];

		PhysicalParameters* p2=static_cast<PhysicalParameters*> ( Body::byId ( 4 )->physicalParameters.get() );
	Real pminz = p2->se3.position[2];

		PhysicalParameters* p3=static_cast<PhysicalParameters*> ( Body::byId ( 5 )->physicalParameters.get() );
	Real pmaxz = p3->se3.position[2];

		PhysicalParameters* p4=static_cast<PhysicalParameters*> ( Body::byId ( 2 )->physicalParameters.get() );
	Real pminx = p4->se3.position[0];

		PhysicalParameters* p5=static_cast<PhysicalParameters*> ( Body::byId ( 3 )->physicalParameters.get() );
	Real pmaxx = p5->se3.position[0];


 

 	T1.ComputeVolumes(pminx,pmaxx,pminy,pmaxy,pminz,pmaxz,Omega::instance().getCurrentIteration());
 

  
    


	if (Omega::instance().getCurrentIteration() <= 1)
	{
 	BodyContainer::iterator bi1 = bodies->begin();
	int b2 = 0;
// 	les parois 
	compacite_init.push_back(0);//Le vecteur compacite_init intègre la compacité initiale dans les cellules de Voronoï
	compacite_init.push_back(0);// 	les parois 
	compacite_init.push_back(0);// 	les parois 
	compacite_init.push_back(0);// 	les parois 
	compacite_init.push_back(0);// 	les parois 
	compacite_init.push_back(0);// 	les parois 
	rigidite_max_enreg = 0;
	max_force_enreg = 0;
	compacity_init = 0;
	compacity = 0;
	compacity_max = 0;
	interpenetration_max = 0;
	delta_interpenetration_max = 0;
	rigidite_max_1 = 0;
	rigidite_max_2 = 0;
	rigidite_max_3 = 0;
	info =0;


	    for( ; bi1!=biEnd ; ++bi1 )
	    {
 	     if ((*bi1)->isDynamic) 
		{
		b2 = b2+1;
		cout << "Itération = " << Omega::instance().getCurrentIteration() << endl;
		const InteractingSphere* s = YADE_CAST<InteractingSphere*>((*bi1)->interactingGeometry.get());
 		Real vol_sphere = 4.18 * (pow(s->radius,3));//Volume de la sphère

	        compacite_init.push_back ((vol_sphere/T1.Volume((*bi1)->getId()))*0.999);//La compacité dans la cellule de voronoï
		cout << "compacite cellule = " << compacite_init[b2] << endl;
// 		cout << "numéro de sphere = " << (*bi1)->getId() << endl;
 		}
	    }
	} 
	//ENDOF VORONOI TESSELATION
			
/// Non Permanents Links												///

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
			if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask) ) continue;

			const SpheresContactGeometry*    currentContactGeometry= YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactInteraction* currentContactPhysics = YADE_CAST<ElasticContactInteraction*> (contact->interactionPhysics.get());
			if((!currentContactGeometry)||(!currentContactPhysics)) continue;
	
			BodyMacroParameters* de1 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			BodyMacroParameters* de2 				= YADE_CAST<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
			
			Vector3r& shearForce 			= currentContactPhysics->shearForce;
	
// ============================================================================ 



		if (contact->isNew) // Si on a une nouvelle interpenetration detectée, on initialise 
			{
			shearForce			= Vector3r(0,0,0);
			currentContactPhysics->previousun=0.0;
			currentContactPhysics->previousFn=0.0;
			currentContactPhysics->unMax=0.0;
			}


		Real previousun = currentContactPhysics->previousun;
		Real previousFn = currentContactPhysics->previousFn;
//		Real kn = currentContactPhysics->kn;
		Real Fn; // la valeur de Fn qui va etre calculee selon différentes manieres puis affectee


		Real un=currentContactGeometry->penetrationDepth;
			
// ============================================================================ 

		if(un > currentContactPhysics->unMax)	// on est en charge, et au delà et sur la "droite principale"
			{
//			Fn = kn*un;
			currentContactPhysics->unMax = std::abs(un);
// 			cout << "je suis dans le calcul normal " << endl;
			}
//		else  On fait de la décharge
// 			{
// 			Fn = previousFn + coeff_dech * kn * (un-previousun);	// Calcul incrémental de la nvlle force
// // 			cout << "je suis dans l'autre calcul" << endl;
// 			if(std::abs(Fn) > std::abs(kn * un))		// verif qu'on ne depasse la courbe
// 				Fn = kn*un;
// 			if(Fn < 0.0 )	// verif qu'on reste positif FIXME ATTENTION ON S'EST FICHU DU NORMAL ADHESION !!!!
// 				{Fn = 0;
// // 				cout << "j'ai corrige pour ne pas etre negatif" << endl;
// 				}
// 			}
//   ==============================================================================

					

// 	 		cout << "Un_sphere_sphere= " << un << endl;
			
// 			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0) *currentContactGeometry->normal;

	
									
			//BEGIN USE VOLUME
			Real vol1 = ((*bodies)[id1]->isDynamic ? T1.Volume(id1) : 0);
			Real vol2 = ((*bodies)[id2]->isDynamic ? T1.Volume(id2) : 0);
			Real solid1 = ((*bodies)[id1]->isDynamic ? 4.18 * (pow(currentContactGeometry->radius1,3)) : 0);
			Real solid2 = ((*bodies)[id2]->isDynamic ? 4.18 * (pow(currentContactGeometry->radius2,3)) : 0);
			Real compacity = (solid1+solid2)/(vol1+vol2);

			Real delta_interp = (un-previousun); 



// ====================================================================================================== JF
// 			if (id1==8)
// 			{
// 			cout << "il doit être constant = " << compacite_init[8] << endl;
// 			cout << "il varie = " << vol1 << endl;
// 			}
// 			else if (id2==8)
// 			{
// 			cout << "il doit être constant = " << compacite_init[8] << endl;
// 			cout << "il varie = " << vol2 << endl;
// 			}
			
		Real compacity1 = (solid1/vol1);
		Real compacity2 = (solid2/vol2);
//		Real compacity1 = compacity;
//		Real compacity2 = compacity;
		Real rigidite = 20.5;
		Real ecrouissage = 0.24;
		Real alpha1 = 0.82-0.68*ecrouissage;//=0.6568
		Real alpha2 = 4.2*(1-0.5*ecrouissage);//= 3.696
		Real beta1 = 1.335/(1-0.5*ecrouissage);// = 1.5170454
		Real beta2 = 2;
		Real gamma1 = (0.84/(0.3+0.5*ecrouissage))-2.05;// = -0.05
		Real delta1 = 8;
		Real r1 = currentContactGeometry->radius1;
		Real r2 = currentContactGeometry->radius2;
		Real rmoy= 2*(r1*r2)/(r1+r2);
		Real rigidite_max = (1*10000*rmoy*pow(3.14/6,1/3))/(3*(1-2*0.43));

	 	

// 	cout << " Valeur de   (un-previousun)  est égale  à  " << un-previousun << endl;
// 
// 	cout << " Valeur de   (un)  est égale  à  " << un << endl;

  
	if ((*bodies)[id1]->isDynamic && (*bodies)[id2]->isDynamic)
	{


	Real compacite_init_jf=(compacite_init[id2]+compacite_init[id1])/2;

//  Real k11 = (alpha1*exp(beta1*un/r1)+gamma1)*(1-exp((-delta1*un)/r1));
  Real k12 = 15*(1-(ecrouissage/2))*(pow(compacity-compacite_init_jf,beta2))/(1-compacity);

// 	if (k12>rigidite_max) k12=rigidite_max;
	
//  Real k1 = k11 * k12;* (un-previousun)

 Real k111 = (0.97-0.58*ecrouissage)*exp(1.75*(1*un/rmoy)*(1+(ecrouissage/2)));
 Real k121 = (-4+15/(1+3*ecrouissage))*exp(-8*(1*un/rmoy));
 Real k1111 = rmoy*rigidite*(k111 + k121 + k12);

	if (k1111>rigidite_max | k1111<0 ){
 //	cout << "SUPERIEUR A LA RIGIDITE DU MATERIAU SUPERIEUR A LA RIGIDITE DU MATERIAU" << endl;
	k1111=rigidite_max;
	cout << " OUIOUIOUIOUIOUIOUIOUIOUI Rgidite max    " << endl;
	
	}

 
	
//  Real k21 = (alpha1*exp(beta1*un/r2)+gamma1)*(1-exp((-delta1*un)/r2));
  Real k22 = 15*(1-(ecrouissage/2))*(pow(compacity-compacite_init_jf,beta2))/(1-compacity);
//  Real k22 = alpha2*(pow(compacity2-compacite_init[id2],beta2))/(pow(1-compacity2,0.5));

// 	if (k22>rigidite_max) k22=rigidite_max;	

  k111 = (0.97-0.58*ecrouissage)*exp(1.75*(1*un/rmoy)*(1+(ecrouissage/2)));
  k121 = (-4+15/(1+3*ecrouissage))*exp(-8*(1*un/rmoy));
 Real k2222 = rmoy*rigidite*(k111 + k121 + k22);



	if (k2222>rigidite_max | k2222<0){
//	 cout << "SUPERIEUR A LA RIGIDITE DU MATERIAU SUPERIEUR A LA RIGIDITE DU MATERIAU" << endl;
	k2222=rigidite_max;
        cout << " OUIOUIOUIOUIOUIOUIOUIOUI Rgidite max    " << endl;
	
	}


// *Real k1 = currentContactPhysics->kn*std::max(un,(Real) 0)/+alpha2/(pow(1-compacity1,0.5));
// Real k2 = currentContactPhysics->kn*std::max(un,(Real) 0)+alpha2/(po*/w(1-compacity2,0.5));
// 	cout << "k2 = " << k2 << endl;
// 	cout << "numero id1 " << id1 << endl;
// 	cout << "numero id2 " << id2 << endl;
//  	cout << "rayon id1 " << r1 << endl;
//  	cout << "rayon id2 " << r2 << endl;
// 	cout << "interpenetration " << un << endl;
// 	cout << "compacity1 " << compacity1 << endl;
// 	cout << "compacity2 " << compacity2 << endl;
// 	cout << "compacity1_init " << compacite_init[id1] << endl;
// 	cout << "compacity2_init " << compacite_init[id2] << endl;

	if (compacity_max<compacity1) compacity_max=compacity1;

	if (compacity_max<compacity2) compacity_max=compacity2;

 
	
		if (k1111<=k2222) 
		{


 		 Fn = (k2222 * delta_interp) + previousFn;

		currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;
// 		cout << "Fn appliquee " << Fn << endl << endl;
		// actualisation :
		currentContactPhysics->previousFn = Fn;
		currentContactPhysics->previousun = un;
// 			cout << "Valeur de la rigidite  = " << k2222 << endl;

// 		cout << "Valeur de la force1  = " << previousFn + Fn << endl;

		//if (k2222>rigidite_max_enreg){
		max_force_enreg = Fn;
		rigidite_max_enreg = k2222;
		compacity_init = compacite_init[id2];
		compacity = compacity2;
		interpenetration_max = un;
		delta_interpenetration_max = delta_interp;
		rigidite_max_1 = k111;
		rigidite_max_2 = k121;
		rigidite_max_3 = k22;
		info = 1;
		//}




// cout << "========================================================================================== " << endl;
// 		cout << " Force entre sphere             =             " << pow(r1,2)*rigidite*k1 <<endl;
//  		cout << "interpenetration " << un << endl;
// 		cout << "compacity_init1 " << compacite_init[id1] << endl;
//  		cout << "compacity1           " << compacity1 << endl;
// 		cout << "numero de la sphere " << id1 << endl;
// 		cout << "numero de la sphere " << id2 << endl;
//   		cout << "Force entre spheres    =           " << pow(r1,2)*rigidite*k1 << endl;

		}
		else if	(k1111>k2222) 
		{



 		 Fn = (k1111 * delta_interp) + previousFn;

		currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;

// 		cout << "Valeur de la force2  = " << previousFn + Fn << endl;
// 		cout << "Fn appliquee " << Fn << endl << endl;
		// actualisation :
		currentContactPhysics->previousFn = Fn;
		currentContactPhysics->previousun = un;



		//if (k1111>rigidite_max_enreg){
		max_force_enreg = Fn;
		rigidite_max_enreg = k1111;
		compacity_init = compacite_init[id1];
		compacity = compacity1;
		interpenetration_max = un;
		delta_interpenetration_max = delta_interp;
		rigidite_max_1 = k111;
		rigidite_max_2 = k121;
		rigidite_max_3 = k12;
		info = 2;
		//}

// cout << "========================================================================================== " << endl;
// 	cout << " Force entre sphere             =             " << pow(r2,2)*rigidite*k2 <<endl;
//  		cout << "interpenetration " << un << endl;
// 		cout << "compacity_init2 " << compacite_init[id2] << endl;
//  		cout << "compacity2          " << compacity2 << endl;
// 		cout << "numero de la sphere " << id2 << endl;
// 		cout << "numero de la sphere " << id1 << endl;
//  		cout << "Force entre spheres     =         " << pow(r2,2)*rigidite*k2 << endl;
//  		currentContactPhysics->normalForce = k2*currentContactGeometry->normal;
		}
 



	}
	else if ((*bodies)[id1]->isDynamic)
	{
	

   rigidite_max = (1*10000*r1*pow(3.14/6,1/3))/(3*(1-2*0.43));

// Real k11 = (alpha1*exp(beta1*2*un/r1)+gamma1)*(1-exp((-delta1*2*un)/r1));
  Real k12 = 15*(1-(ecrouissage/2))*(pow(compacity1-compacite_init[id1],beta2))/(1-compacity1);

 	
	
//  Real k1 = k11 * k12;* (un-previousun)


 Real k111 = (0.97-0.58*ecrouissage)*exp(1.75*(3*un/r1)*(1+(ecrouissage/2)));
 Real k121 = (-4+15/(1+3*ecrouissage))*exp(-8*(3*un/r1));
 Real k1111 = r1*1*rigidite*(k111 + k121 + k12);


		if (k1111>rigidite_max | k1111<0 )
		{ k1111=rigidite_max;
		cout << " OUIOUIOUIOUIOUIOUIOUIOUI Rgidite max    " << endl;
		}

// 	if (k1111>rigidite_max ){
// //	 cout << "SUPERIEUR A LA RIGIDITE DU MATERIAU SUPERIEUR A LA RIGIDITE DU MATERIAU" << endl;
// 	k1111=rigidite_max;
// 	}

	if (compacity_max<compacity1) compacity_max=compacity1;


 	Fn = (k1111 * delta_interp) + previousFn;

		currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;
// 		cout << "Fn appliquee " << Fn << endl << endl;
		// actualisation :
		currentContactPhysics->previousFn = Fn;
		currentContactPhysics->previousun = un;

	//	if (k1111>rigidite_max_enreg){
		max_force_enreg = Fn;
		rigidite_max_enreg = k1111;
		compacity_init = compacite_init[id1];
		compacity = compacity1;
		interpenetration_max = un;
		delta_interpenetration_max = delta_interp;
		rigidite_max_1 = k111;
		rigidite_max_2 = k121;
		rigidite_max_3 = k12;
		info = 3;
	//	}
// cout << "========================================================================================== " << endl;
// 		cout << "interpenetration " << un << endl;
// 		cout << "compacity_init1 " << compacite_init[id1] << endl;
// 		cout << "compacity1 " << compacity1 << endl;
// 		cout << "force normal =  " << pow(r1,2)*rigidite*k1 << endl;
// 		cout << "numero de la sphere " << id1 << endl;

//  		cout << "r1 " << r1 << endl;
//  		cout << "r2 " << r2 << endl;
// 		cout << "rmoy " << rmoy << endl;
// 		cout << "alpha1 = " << alpha1 << endl;
// 		cout << "alpha2 = " << alpha2 << endl;
// 		cout << "beta1 = " << beta1 << endl;
// 		cout << "beta2 = " << beta2 << endl;
// 		cout << "gamma1 = " << gamma1 << endl;
// 		cout << "delta1 = " << delta1 << endl;

// 		cout << "Force_MUR_sphere                 =                                                " << pow(r1,2)*rigidite*k1 << endl;

	}
	else if ((*bodies)[id2]->isDynamic)
	{
	
//  Real k2 = (alpha1*exp((beta1*2*un)/rmoy)+gamma1)*(1-exp((-delta1*2*un)/rmoy))+alpha2*(pow(compacity2-compacite_init[id2],beta2))/(pow(1-compacity2,0.5));

// Real k21 = (alpha1*exp(beta1*1*un/r2)+gamma1)*(1-exp((-delta1*1*un)/r2));
  Real k22 = 15*(1-(ecrouissage/2))*(pow(compacity2-compacite_init[id2],beta2))/(1-compacity2);
//  Real k2 = k21 * k22;

	rigidite_max = (1*10000*r2*pow(3.14/6,1/3))/(3*(1-2*0.43));
		

 Real k212 = (0.97-0.58*ecrouissage)*exp(1.75*(3*un/r2)*(1+(ecrouissage/2)));
 Real k222 = (-4+15/(1+3*ecrouissage))*exp(-8*(3*un/r2));
 Real k2222 = r2*1*rigidite*(k212 + k222 + k22);

	if (k2222>rigidite_max | k2222<0 ) 
	{k2222=rigidite_max;
 	cout << " OUIOUIOUIOUIOUIOUIOUIOUI Rgidite max    " << endl;
	}	



	if (compacity_max<compacity2) compacity_max=compacity2;

	Fn = (k2222  *1*delta_interp) + previousFn;

		currentContactPhysics->normalForce	= Fn*currentContactGeometry->normal;
// 		cout << "Fn appliquee " << Fn << endl << endl;
		// actualisation :
		currentContactPhysics->previousFn = Fn;
		currentContactPhysics->previousun = un;


		//if (k2222>rigidite_max_enreg){
		max_force_enreg = Fn;

		rigidite_max_enreg = k2222;
		compacity_init = compacite_init[id2];
		compacity = compacity2;
		interpenetration_max = un;
		delta_interpenetration_max = delta_interp;
		rigidite_max_1 = k212;
		rigidite_max_2 = k222;
		rigidite_max_3 = k22;
		info = 4;
		//}

// cout << "========================================================================================== " << endl;
// 		cout << "interpenetration " << un << endl;
// 		cout << "compacity_init2 " << compacite_init[id2] << endl;
// 		cout << "compacity2 " << compacity2 << endl;
// 		cout << "force normal =  " << pow(r2,2)*rigidite*k2 << endl;
// 		cout << "numero de la sphere " << id2 << endl;

// 		cout << "Force_MUR_sphere                 =                                                  " << pow(r2,2)*rigidite*k2 << endl;
//  		cout << "r2 " << r2 << endl;
// 		cout << "r1 " << r1 << endl;
// 		cout << "pow(r2,2)*(alpha1*exp((beta1*2*un)/rmoy)+gamma1)                    " << pow(r2,2)*(alpha1*exp((beta1*2*un)/rmoy)+gamma1) << endl;
// 		cout << "(1-exp(-delta1*((2*un)/rmoy)))                   " << (1-exp(-delta1*((2*un)/rmoy))) << endl;
// 		cout << "alpha2*(pow(compacity2-compacite_init[id2],beta2))/(pow(1-compacity2,0.5))                              " << alpha2*(pow(compacity2-compacite_init[id2],beta2))/(pow(1-compacity2,0.5)) << endl;


	}
	else
	{
// 	cout << "numero id1 " << id1 << endl;
// 	cout << "numero id2 " << id2 << endl;
// 	currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0) *currentContactGeometry->normal //loi normale
//  * max((Real) 1,pow(compacity*1.42, 3)); // + effet volumique

 	}
 
cout << "Rigidite maximale initiale                     = " << rigidite_max << endl;
		cout << "LA RIGIDITE DU MATERIAU     =          " << rigidite_max_enreg  << endl;

		cout << "LA FORCE MAXI     =          " << max_force_enreg << endl ;

		cout << "Compacite_init     =          " << compacity_init << endl ;

		cout << "Compacite    =          " << compacity << endl ;

		cout << "Interpenetration maximale    =    " << interpenetration_max << endl;

		cout << " Delata interpenetration     =    " << delta_interpenetration_max << endl;

		cout << " Rigigidite_max_1     =    " << rigidite_max_1 << endl;

		cout << " Rigigidite_max_2     =    " << rigidite_max_2 << endl;

		cout << " Rigigidite_max_3     =    " << rigidite_max_3 << endl;

		cout << " Rmoyen     =    " << rmoy << endl;

		cout << " INFO     =    " << info << endl;

			
// 			currentContactPhysics->normalForce=currentContactPhysics->kn*std::max(un,(Real) 0) *currentContactGeometry->normal //loi normale
// 			* max((Real) 1,pow(compacity*1.42, 3)); // + effet volumique
			//END OF USE VOLUME
	

			

			Vector3r axis;
			Real angle;
	
	/// Here is the code with approximated rotations 	 ///
			
			axis = currentContactPhysics->prevNormal.Cross(currentContactGeometry->normal);
			shearForce -= shearForce.Cross(axis);
			angle = dt*0.5*currentContactGeometry->normal.Dot(de1->angularVelocity+de2->angularVelocity);
			axis = angle*currentContactGeometry->normal;
			shearForce -= shearForce.Cross(axis);

		
	/// Here is the code with exact rotations 		 ///
	
	// 		Quaternionr q;
	//
	// 		axis					= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
	// 		angle					= acos(currentContactGeometry->normal.dot(currentContactPhysics->prevNormal));
	// 		q.fromAngleAxis(angle,axis);
	//
	// 		currentContactPhysics->shearForce	= currentContactPhysics->shearForce*q;
	//
	// 		angle					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
	// 		axis					= currentContactGeometry->normal;
	// 		q.fromAngleAxis(angle,axis);
	// 		currentContactPhysics->shearForce	= q*currentContactPhysics->shearForce;
	
	/// 							 ///
	
			Vector3r x				= currentContactGeometry->contactPoint;
			Vector3r c1x				= (x - de1->se3.position);
			Vector3r c2x				= (x - de2->se3.position);
			Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
			Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.Dot(relativeVelocity)*currentContactGeometry->normal;
			Vector3r shearDisplacement		= shearVelocity*dt;
//  			shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
//=====================================================================================================================================
			shearForce 			       -= currentContactPhysics->ks*shearDisplacement;
			
//			cout << "KSKSKSKSKSKSKSKSKS     =    " << ks << endl;
// =================================================================================================================

 
	
	// PFC3d SlipModel, is using friction angle. CoulombCriterion
			Real maxFs = currentContactPhysics->normalForce.SquaredLength() * std::pow(currentContactPhysics->tangensOfFrictionAngle,2);
			//cout << "TANGENTOfFrictionAngle     =    " << currentContactPhysics->tangensOfFrictionAngle << endl;

			if( shearForce.SquaredLength() > maxFs )
			{
				maxFs = Mathr::Sqrt(maxFs) / shearForce.Length();
				shearForce *= maxFs;
			}
	////////// PFC3d SlipModel
	
			Vector3r f				= currentContactPhysics->normalForce + shearForce;
			
	// it will be some macro(	body->physicalActions,	ActionType , bodyId )
			static_cast<Force*>   ( ncb->physicalActions->find( id1 , actionForceIndex).get() )->force    -= f;
			static_cast<Force*>   ( ncb->physicalActions->find( id2 , actionForceIndex ).get() )->force    += f;
			
			static_cast<Momentum*>( ncb->physicalActions->find( id1 , actionMomentumIndex ).get() )->momentum -= c1x.Cross(f);
			static_cast<Momentum*>( ncb->physicalActions->find( id2 , actionMomentumIndex ).get() )->momentum += c2x.Cross(f);
			
			currentContactPhysics->prevNormal = currentContactGeometry->normal;
		}
	}
}

 
YADE_PLUGIN();
