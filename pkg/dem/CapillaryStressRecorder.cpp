/*************************************************************************
*  Copyright (C) 2006 by luc scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryStressRecorder.hpp"
#include <yade/pkg/common/Sphere.hpp>
#include <yade/pkg/common/ElastMat.hpp>
#include <yade/pkg/dem/CapillaryPhys.hpp>
#include <yade/pkg/dem/Law2_ScGeom_CapillaryPhys_Capillarity.hpp>
#include <yade/pkg/dem/TriaxialCompressionEngine.hpp>

#include <yade/core/Omega.hpp>
#include <yade/core/Scene.hpp>
#include <boost/lexical_cast.hpp>

YADE_PLUGIN((CapillaryStressRecorder));
CREATE_LOGGER(CapillaryStressRecorder);

void CapillaryStressRecorder::action()
{
	shared_ptr<BodyContainer>& bodies = scene->bodies;
  
	// at the beginning of the file; write column titles
	if(out.tellp()==0){
		out<<"iteration Scap11 Scap22 Scap33 Scap12 Scap13 Scap23 Uc Sr w"<<endl;
	}
	if ( !triaxialCompressionEngine )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();
		for ( ;itFirst!=itLast; ++itFirst )
		{
			if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine")
			{
				LOG_DEBUG ( "stress controller engine found" );
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );
			}
		}
		if ( !triaxialCompressionEngine ) {
			LOG_ERROR ("Stress controller engine not found, the recorder cannot be used."); return;}
	}
	
	Real f1_cap_x=0, f1_cap_y=0, f1_cap_z=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;
	Real sig11_cap=0, sig22_cap=0, sig33_cap=0, sig12_cap=0, sig13_cap=0,
	sig23_cap=0, Vwater = 0, capillaryPressure = 0;
	int j = 0;
		
	InteractionContainer::iterator ii    = scene->interactions->begin();
        InteractionContainer::iterator iiEnd = scene->interactions->end();
        for(  ; ii!=iiEnd ; ++ii ) 
        {
                if ((*ii)->isReal())
                {	
                	const shared_ptr<Interaction>& interaction = *ii;
                
                	CapillaryPhys* meniscusParameters = static_cast<CapillaryPhys*>(interaction->phys.get());
                        
                        if (meniscusParameters->meniscus)
                        {
                	
                	j=j+1;
                	
                        unsigned int id1 = interaction -> getId1();
			unsigned int id2 = interaction -> getId2();
			
			Vector3r fcap = meniscusParameters->fCap;
			
			f1_cap_x=fcap[0];
			f1_cap_y=fcap[1];
			f1_cap_z=fcap[2];
			
			Body* de1 = (*bodies)[id1].get();
			Body* de2 = (*bodies)[id2].get();
			
			x1 = de1->state->pos[0];
			y1 = de1->state->pos[1];
			z1 = de1->state->pos[2];
			x2 = de2->state->pos[0];
			y2 = de2->state->pos[1];
			z2 = de2->state->pos[2];

			/// capillary stresses from contact forces
			
			sig11_cap = sig11_cap + f1_cap_x*(x1 - x2);
			sig22_cap = sig22_cap + f1_cap_y*(y1 - y2);
			sig33_cap = sig33_cap + f1_cap_z*(z1 - z2);
			sig12_cap = sig12_cap + f1_cap_x*(y1 - y2);
			sig13_cap = sig13_cap + f1_cap_x*(z1 - z2);
			sig23_cap = sig23_cap + f1_cap_y*(z1 - z2);
			
			/// Liquid volume
			
 			Vwater += meniscusParameters->vMeniscus;
 			capillaryPressure=meniscusParameters->capillaryPressure;
			
			}
			
                }
        }	

	/// Sample volume

	Real V = ( triaxialCompressionEngine->height ) * ( triaxialCompressionEngine->width ) * ( triaxialCompressionEngine->depth );

	/// Solid volume
	Real Vs = 0, Rbody = 0, SR = 0;

	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for ( ; bi!=biEnd; ++bi) {
	  
	  shared_ptr<Body> b = *bi;
	  if (b->shape->getClassIndex()!=Sphere::getClassIndexStatic()) continue;
	  Sphere* sphere = static_cast<Sphere*>(b->shape.get());

	  if (sphere) {	  
	    Rbody = sphere->radius;
	    SR+=Rbody;
	    Vs += 1.333*Mathr::PI*(Rbody*Rbody*Rbody);
	  }
	}
	
	Real Vv = V - Vs;
	Real Sr = 100*Vwater/Vv;
	if (Sr>100) Sr=100;
	Real w = 100*Vwater/V;
	if (w>(100*Vv/V)) w=100*(Vv/V);
	
	/// homogeneized capillary stresses
	
	Real SIG_11_cap=0, SIG_22_cap=0, SIG_33_cap=0, SIG_12_cap=0, SIG_13_cap=0, SIG_23_cap=0;
	
	SIG_11_cap = sig11_cap/V;
	SIG_22_cap = sig22_cap/V;
	SIG_33_cap = sig33_cap/V;
	SIG_12_cap = sig12_cap/V;
	SIG_13_cap = sig13_cap/V;
	SIG_23_cap = sig23_cap/V;
	
	out << lexical_cast<string> ( scene->iter ) << " "
		<< lexical_cast<string>(SIG_11_cap) << " " 
		<< lexical_cast<string>(SIG_22_cap) << " " 
		<< lexical_cast<string>(SIG_33_cap) << " " 
		<< lexical_cast<string>(SIG_12_cap) << " "
		<< lexical_cast<string>(SIG_13_cap)<< " "
		<< lexical_cast<string>(SIG_23_cap)<< "   "
		<< lexical_cast<string>(capillaryPressure) << " "
		<< lexical_cast<string>(Sr)<< " " 
		<< lexical_cast<string>(w)<< " "
		<< endl;
	
}

