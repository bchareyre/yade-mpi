/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CapillaryRecorder.hpp"
#include <yade/pkg-common/ElasticMat.hpp>
//#include <yade/pkg-common/ParticleParameters.hpp>
#include <yade/pkg-dem/CapillaryParameters.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/World.hpp>
#include <boost/lexical_cast.hpp>


CapillaryRecorder::CapillaryRecorder () : Recorder()
{
	outputFile = "";
	interval = 1;
}


void CapillaryRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}



bool CapillaryRecorder::isActivated(World* rootBody)
{
	return ((rootBody->currentIteration % interval == 0) && (ofile));
}


void CapillaryRecorder::action(World * ncb)
{
	Real fx=0, fy=0, fz=0;
	
	ncb->bex.sync();
	Vector3r force=ncb->bex.getForce(bigBallId);
	
		
		fx=force[0];
		fy=force[1];
		fz=force[2];
		
	// R1 R2 -> geometricalModel // �modifier cf capllaryStressRecorder
	Real R1 = 0.001;
	Real R2 = 0.001;
	
// 	//physicalParameters de bigBall
// 	
// 	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 	Material* bigBallpp =
// 	static_cast<GranularMat*>((*bodies)[bigBallId]->material.get())
// ; ;
// 		
// 		Real x = bigBallpp-> se3.position[0];
// 		
// 		Real Dintergranular = x - (R1+R2);	
// 	
// 	// capillary parameters
	InteractionContainer::iterator ii    = ncb->interactions->begin();
        InteractionContainer::iterator iiEnd = ncb->interactions->end();
        
        Real Vtotal = 0;
        Real CapillaryPressure = 0;
        
        for(  ; ii!=iiEnd ; ++ii ) 
        {
                if ((*ii)->isReal() )
                {
                        const shared_ptr<Interaction>& interaction = *ii;
                         
                        CapillaryParameters* meniscusParameters		= static_cast<CapillaryParameters*>(interaction->interactionPhysics.get());
                        
                        if (meniscusParameters->meniscus)
                        {
                         
                        CapillaryPressure = meniscusParameters->CapillaryPressure;
                         
                        Vtotal += meniscusParameters->Vmeniscus;
                        
                        }
                        
                        // else Vtotal = 0; // �voir!!!
                }
        }	

	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(fx) << " " 
		<< lexical_cast<string>(fy) << " " 
		<< lexical_cast<string>(fz) << " " 
		<< lexical_cast<string>(CapillaryPressure) << " "
//		<< lexical_cast<string>(Dintergranular)<< " "
		<< lexical_cast<string>(Vtotal)<< " " << endl;
	
	
}

YADE_PLUGIN((CapillaryRecorder));

//YADE_REQUIRE_FEATURE(PHYSPAR);

