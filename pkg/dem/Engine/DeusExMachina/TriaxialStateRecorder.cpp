/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialStateRecorder.hpp"
//#include <yade/pkg-common/RigidBodyParameters.hpp>
#include <yade/pkg-common/ParticleParameters.hpp>
//#include <yade/pkg-dem/BodyMacroParameters.hpp>
//#include <yade/pkg-common/Force.hpp>
//#include <yade/pkg-dem/ElasticContactLaw.hpp>
//#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/pkg-common/Sphere.hpp>

//#include <yade/pkg-dem/SpheresContactGeometry.hpp>
//#include <yade/pkg-dem/ElasticContactInteraction.hpp>

#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>

#include <yade/pkg-dem/SpheresContactGeometry.hpp>
#include <yade/pkg-dem/ElasticContactInteraction.hpp>



CREATE_LOGGER(TriaxialStateRecorder);

TriaxialStateRecorder::TriaxialStateRecorder () : DataRecorder()

{
	outputFile = "TriaxialStateRecord";
	interval = 1;
	porosity = 1.;
		
	//triaxialStressController = NULL;
}


void TriaxialStateRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		bool file_exists = std::ifstream (outputFile.c_str()); //if file does not exist, we will write colums titles
		ofile.open(outputFile.c_str(), std::ios::app);
		//if (!file_exists) ofile<<"iteration s11 s22 s33 e11 e22 e33 unb_force porosity kineticE" << endl;
		if (!file_exists) ofile<<"iteration fn11 fn22 fn33 fn111 fn222 fn333 Position_right unb_f porosity Energie_Cinétique" << endl;
	}
}


void TriaxialStateRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(porosity);
}


bool TriaxialStateRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void TriaxialStateRecorder::action (MetaBody * ncb )
{
	if ( !triaxialCompressionEngine )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
		for ( ;itFirst!=itLast; ++itFirst )
		{
			if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine" ) //|| (*itFirst)->getBaseClassName() == "TriaxialCompressionEngine")
			{
				LOG_DEBUG ( "stress controller engine found" );
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );
				//triaxialCompressionEngine = shared_ptr<TriaxialCompressionEngine> (static_cast<TriaxialCompressionEngine*> ( (*itFirst).get()));
			}
		}
	}
	if ( ! ( Omega::instance().getCurrentIteration() % triaxialCompressionEngine->computeStressStrainInterval == 0 ) )
		triaxialCompressionEngine->computeStressStrain ( ncb );



	/// Compute kinetic energy and porosity :

	Real Vs=0, kinematicE = 0;
	Real V = ( triaxialCompressionEngine->height ) * ( triaxialCompressionEngine->width ) * ( triaxialCompressionEngine->depth );

	BodyContainer::iterator bi = ncb->bodies->begin();
	BodyContainer::iterator biEnd = ncb->bodies->end();

	for ( ; bi!=biEnd; ++bi )

	{
		const shared_ptr<Body>& b = *bi;

		//int geometryIndex = b->geometricalModel->getClassIndex();

		if ( b->isDynamic )
		{
			const shared_ptr<ParticleParameters>& pp =
				YADE_PTR_CAST<ParticleParameters> ( b->physicalParameters );
			const Vector3r& v = pp->velocity;
			kinematicE +=
				0.5* ( pp->mass ) * ( v[0]*v[0]+v[1]*v[1]+v[2]*v[2] );

			const shared_ptr<Sphere>& sphere =
				YADE_PTR_CAST<Sphere> ( b->geometricalModel );
			Vs += 1.3333333*Mathr::PI*pow ( sphere->radius, 3 );

		}
	}
	porosity = ( V - Vs ) /V;









// ======================================================================== 

// 	ofile << lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[0] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[1] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[2] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->ComputeUnbalancedForce ( ncb ) ) << " "
// 	<< lexical_cast<string> ( porosity ) << " "
// 	<< lexical_cast<string> ( kinematicE )
// 	<< endl;



// ======================================================================== 
	if ( Omega::instance().getCurrentIteration() % 500 == 0 || Omega::instance().getCurrentIteration() == 0 )
	{

	ofile << lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[0] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[1] ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[2] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_right][0] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_top][1] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_front][2] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_left][0] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_bottom][1] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->force[triaxialCompressionEngine->wall_back][2] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->position_right ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->position_top ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->position_front ) << " "

	<< lexical_cast<string> ( triaxialCompressionEngine->ComputeUnbalancedForce ( ncb ) ) << " "
	<< lexical_cast<string> ( porosity ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->position_left ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->position_bottom ) << " "
// 	<< lexical_cast<string> ( triaxialCompressionEngine->position_back ) << " "
 	<< lexical_cast<string> ( kinematicE ) << " " << endl;
// 	<< lexical_cast<string> ( force_sphere ) << " " << endl;
	}
//=======






}

YADE_PLUGIN();
