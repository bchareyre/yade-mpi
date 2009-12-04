/*************************************************************************
*  Copyright (C) 2006 by luc scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryStressRecorder.hpp"
//#include <yade/pkg-common/RigidBodyParameters.hpp>
//#include <yade/pkg-common/ParticleParameters.hpp>
#include <yade/pkg-common/InteractingSphere.hpp>
#include <yade/pkg-common/ElasticMat.hpp>
#include <yade/pkg-dem/CapillaryParameters.hpp>
#include <yade/pkg-dem/CapillaryCohesiveLaw.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>

#include <yade/core/Omega.hpp>
#include <yade/core/Scene.hpp>
#include <boost/lexical_cast.hpp>

CREATE_LOGGER(CapillaryStressRecorder);

CapillaryStressRecorder::CapillaryStressRecorder () : Recorder()

{
	outputFile = "";
	interval = 1;
	sphere_ptr = shared_ptr<Shape> (new InteractingSphere);
	SpheresClassIndex = sphere_ptr->getClassIndex();
// 	height = 0;
// 	width = 0;
// 	depth = 0;
// 	thickness = 0;
	
// 	upperCorner = Vector3r(0,0,0);
// 	lowerCorner = Vector3r(0,0,0);
	
}


void CapillaryStressRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		//bool file_exists = std::ifstream (outputFile.c_str()); //if file does not exist, we will write colums titles
		ofile.open(outputFile.c_str(), std::ios::app);
		//if (!file_exists) ofile<<"iteration s11 s22 s33 e11 e22 e33 unb_force porosity kineticE" << endl;
		
	}
}



bool CapillaryStressRecorder::isActivated(Scene* rootBody)
{
	return ((rootBody->currentIteration % interval == 0) && (ofile));
}


void CapillaryStressRecorder::action(Scene * ncb)
{
	if ( !triaxialCompressionEngine )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
		for ( ;itFirst!=itLast; ++itFirst )
		{
			if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine")
// 			  || (*itFirst)->getBaseClassName() == "TriaxialCompressionEngine")
			{
				LOG_DEBUG ( "stress controller engine found" );
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );
				//triaxialCompressionEngine = shared_ptr<TriaxialCompressionEngine> (static_cast<TriaxialCompressionEngine*> ( (*itFirst).get()));
			}
		}
		if ( !triaxialCompressionEngine ) LOG_DEBUG ( "stress controller engine NOT found" );
	}
// 	if ( ! ( Omega::instance().getCurrentIteration() % triaxialCompressionEngine->computeStressStrainInterval == 0 )) triaxialCompressionEngine->computeStressStrain ( ncb );	

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
		
	Real f1_cap_x=0, f1_cap_y=0, f1_cap_z=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;
	
	Real sig11_cap=0, sig22_cap=0, sig33_cap=0, sig12_cap=0, sig13_cap=0,
	sig23_cap=0, Vwater = 0, CapillaryPressure = 0;
	
	InteractionContainer::iterator ii    = ncb->interactions->begin();
        InteractionContainer::iterator iiEnd = ncb->interactions->end();
        
        int j = 0;
        
        for(  ; ii!=iiEnd ; ++ii ) 
        {
                if ((*ii)->isReal())
                {	
                	const shared_ptr<Interaction>& interaction = *ii;
                
                	CapillaryParameters* meniscusParameters		= static_cast<CapillaryParameters*>(interaction->interactionPhysics.get());
                        
                        if (meniscusParameters->meniscus)
                        {
                	
                	j=j+1;
                	
                        unsigned int id1 = interaction -> getId1();
			unsigned int id2 = interaction -> getId2();
			
			Vector3r fcap = meniscusParameters->Fcap;
			
			f1_cap_x=fcap[0];
			f1_cap_y=fcap[1];
			f1_cap_z=fcap[2];
			
			Body* de1 = (*bodies)[id1].get();
			Body* de2 = (*bodies)[id2].get();
			
// 			BodyMacroParameters* de1 		= static_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
			x1 = de1->state->pos[0];
			y1 = de1->state->pos[1];
			z1 = de1->state->pos[2];


// 			BodyMacroParameters* de2 		= static_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());
			x2 = de2->state->pos[0];
			y2 = de2->state->pos[1];
			z2 = de2->state->pos[2];

			///Calcul des contraintes capillaires "locales"
			
			sig11_cap = sig11_cap + f1_cap_x*(x1 - x2);
			sig22_cap = sig22_cap + f1_cap_y*(y1 - y2);
			sig33_cap = sig33_cap + f1_cap_z*(z1 - z2);
			sig12_cap = sig12_cap + f1_cap_x*(y1 - y2);
			sig13_cap = sig13_cap + f1_cap_x*(z1 - z2);
			sig23_cap = sig23_cap + f1_cap_y*(z1 - z2);
			
			/// Calcul du volume d'eau
			
 			Vwater += meniscusParameters->Vmeniscus;
 			CapillaryPressure=meniscusParameters->CapillaryPressure;
			
			}
			
                }
        }	

// 	if (Omega::instance().getCurrentIteration() % 10 == 0) 
// 	{cerr << "interactions capillaires = " << j << endl;}

	/// volume de l'echantillon
// 	PhysicalParameters* p_bottom =
// static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.
// get());
// 	PhysicalParameters* p_top   =	
// static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get(
// ));
// 	PhysicalParameters* p_left =
// static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get
// ());
// 	PhysicalParameters* p_right =
// static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.
// get());
// 	PhysicalParameters* p_front =
// static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.
// get());
// 	PhysicalParameters* p_back =
// static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get
// ());
// 	
// 	
// 	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() -
// thickness;
// 	width = p_right->se3.position.X() - p_left->se3.position.X() -
// thickness;
// 	depth = p_front->se3.position.Z() - p_back->se3.position.Z() -
// thickness;
// 
// 	Real Vech = (height) * (width) * (depth)

	Real V = ( triaxialCompressionEngine->height ) * ( triaxialCompressionEngine->width ) * ( triaxialCompressionEngine->depth );

	// degre de saturation/porosite	
	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	
	Real Vs = 0, Rbody = 0, SR = 0;
// 	int n = 0;
	
	for ( ; bi!=biEnd; ++bi) 
	
	{	
		shared_ptr<Body> b = *bi;
		
		int geometryIndex = b->shape->getClassIndex();
		//cerr << "model = " << geometryIndex << endl;
		
		if (geometryIndex == SpheresClassIndex)
		{
			InteractingSphere* sphere =
		static_cast<InteractingSphere*>(b->shape.get());
			Rbody = sphere->radius;
			SR+=Rbody;
			
			Vs += 1.333*Mathr::PI*(Rbody*Rbody*Rbody);
		}
	}
	
	Real Vv = V - Vs;
	
	//Real n = Vv/Vech;
	Real Sr = 100*Vwater/Vv;
	if (Sr>100) Sr=100;
	Real w = 100*Vwater/V;
	if (w>(100*Vv/V)) w=100*(Vv/V);
	
	/// Calcul des contraintes "globales"
	
	Real SIG_11_cap=0, SIG_22_cap=0, SIG_33_cap=0, SIG_12_cap=0, SIG_13_cap=0, SIG_23_cap=0;
	
	SIG_11_cap = sig11_cap/V;
	SIG_22_cap = sig22_cap/V;
	SIG_33_cap = sig33_cap/V;
	SIG_12_cap = sig12_cap/V;
	SIG_13_cap = sig13_cap/V;
	SIG_23_cap = sig23_cap/V;
	
	ofile << lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << " "
		<< lexical_cast<string>(SIG_11_cap) << " " 
		<< lexical_cast<string>(SIG_22_cap) << " " 
		<< lexical_cast<string>(SIG_33_cap) << " " 
		<< lexical_cast<string>(SIG_12_cap) << " "
		<< lexical_cast<string>(SIG_13_cap)<< " "
		<< lexical_cast<string>(SIG_23_cap)<< "   "
		<< lexical_cast<string>(CapillaryPressure) << " "
		<< lexical_cast<string>(Sr)<< " " 
		<< lexical_cast<string>(w)<< " "
		<< endl;
	
}

YADE_PLUGIN((CapillaryStressRecorder));

//YADE_REQUIRE_FEATURE(PHYSPAR);

