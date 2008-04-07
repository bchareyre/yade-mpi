/*************************************************************************
*  Copyright (C) 2006 by luc scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialStateRecorder.hpp"
//#include <yade/pkg-common/RigidBodyParameters.hpp>
//#include <yade/pkg-common/ParticleParameters.hpp>
//#include <yade/pkg-dem/BodyMacroParameters.hpp>
//#include <yade/pkg-common/Force.hpp>
//#include <yade/pkg-dem/ElasticContactLaw.hpp>
//#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>

//#include <yade/pkg-dem/SpheresContactGeometry.hpp>
//#include <yade/pkg-dem/ElasticContactInteraction.hpp>

#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>

CREATE_LOGGER(TriaxialStateRecorder);

TriaxialStateRecorder::TriaxialStateRecorder () : DataRecorder()

{
	outputFile = "TriaxialStateRecord";
	interval = 1;
		
	//triaxialStressController = NULL;
}


void TriaxialStateRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}


void TriaxialStateRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	
	//REGISTER_ATTRIBUTE(wall_bottom_id);
 	//REGISTER_ATTRIBUTE(wall_top_id);
//  	REGISTER_ATTRIBUTE(wall_left_id);
//  	REGISTER_ATTRIBUTE(wall_right_id);
//  	REGISTER_ATTRIBUTE(wall_front_id);
//  	REGISTER_ATTRIBUTE(wall_back_id);
//  	
//  	REGISTER_ATTRIBUTE(height);
// 	REGISTER_ATTRIBUTE(width);
// 	REGISTER_ATTRIBUTE(depth);
// 	REGISTER_ATTRIBUTE(thickness);

}


bool TriaxialStateRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void TriaxialStateRecorder::action(Body * body)
{
    MetaBody * ncb = static_cast<MetaBody*>(body);

    if (!triaxialCompressionEngine)
    {
        vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
        vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
        for (;itFirst!=itLast; ++itFirst)
        {
            if ((*itFirst)->getClassName() == "TriaxialCompressionEngine" ) //|| (*itFirst)->getBaseClassName() == "TriaxialCompressionEngine")
            {
                LOG_DEBUG("stress controller engine found");
                triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> (*itFirst);
                //triaxialCompressionEngine = shared_ptr<TriaxialCompressionEngine> (static_cast<TriaxialCompressionEngine*> ( (*itFirst).get()));
            }
        }
    }
    if ( !(Omega::instance().getCurrentIteration() % triaxialCompressionEngine->computeStressStrainInterval == 0) )
    	triaxialCompressionEngine->computeStressStrain(ncb);
    
    ofile << lexical_cast<string>(Omega::instance().getCurrentIteration()) << " "
    << lexical_cast<string>(triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->strain[0]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->strain[1]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->strain[2]) << " "
    << lexical_cast<string>(triaxialCompressionEngine->ComputeUnbalancedForce(body)) << " "
    << endl;

/*

    shared_ptr<BodyContainer>& bodies = ncb->bodies;

    /// dimensions de l'echantillon

    PhysicalParameters* p_bottom = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get());
    PhysicalParameters* p_top   =  static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get());
    PhysicalParameters* p_left 	= static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get());
    PhysicalParameters* p_right = static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.get());
    PhysicalParameters* p_front = static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.get());
    PhysicalParameters* p_back 	= static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get());


    height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
    width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
    depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;

    //cerr << "height " << height << " width " << width << " depth " << depth << endl;

    /// calcul des contraintes via forces resultantes sur murs

    Real SIG_wall_11 = 0, SIG_wall_22 = 0, SIG_wall_33 = 0;

    Vector3r F_wall_11 = static_cast<Force*>( ncb->physicalActions->find(wall_left_id, actionForce->getClassIndex() ). get() )->force;
    //cerr << "F_wall_11 " << F_wall_11;

    SIG_wall_11 = F_wall_11[0]/(depth*height);

    Vector3r F_wall_22 = static_cast<Force*>( ncb->physicalActions->find(wall_top_id, actionForce->getClassIndex() ). get() )->force;
    //cerr << " F_wall_22 " << F_wall_22;

    SIG_wall_22 = F_wall_22[1]/(depth*width);

    Vector3r F_wall_33 = static_cast<Force*>( ncb->physicalActions->find(wall_front_id, actionForce->getClassIndex() ). get() )->force;
    //cerr << " F_wall_33 " << F_wall_33 << endl;
    SIG_wall_33 = F_wall_33[2]/(width*height);

    ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " "
    << lexical_cast<string>(SIG_wall_11) << " "
    << lexical_cast<string>(p_left->se3.position.X()) << " "
    << lexical_cast<string>(SIG_wall_22) << " "
    << lexical_cast<string>(p_top->se3.position.Y()) << " "
    << lexical_cast<string>(SIG_wall_33) << " "
    << lexical_cast<string>(p_front->se3.position.Z()) << " "
    << endl;*/

}

YADE_PLUGIN();
