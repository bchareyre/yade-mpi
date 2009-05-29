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
//#include <yade/pkg-dem/ElasticContactLaw.hpp>
//#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
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
		if (!file_exists) ofile<<"iteration s11 s22 s33 e11 e22 e33 unb_force porosity kineticE" << endl;
		//if (!file_exists) ofile<<"iteration fn11 fn22 fn33 fn111 fn222 fn333 Position_right unb_f porosity Energie_Cinétique" << endl;
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
		if ( !triaxialCompressionEngine ) LOG_DEBUG ( "stress controller engine NOT found" );
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

			Vs += 1.3333333*Mathr::PI*pow ( YADE_PTR_CAST<InteractingSphere>( b->interactingGeometry)->radius, 3 );

		}
	}
	porosity = ( V - Vs ) /V;









// ======================================================================== 

 	ofile << lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[0] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[1] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[2] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->ComputeUnbalancedForce ( ncb ) ) << " "
 	<< lexical_cast<string> ( porosity ) << " "
 	<< lexical_cast<string> ( kinematicE )
 	<< endl;


/*
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
*/





}
/*
TriaxialStressController::ComputeLoveStress ( MetaBody * ncb )
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real f1_el_x=0, f1_el_y=0, f1_el_z=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;

	Real sig11_el=0, sig22_el=0, sig33_el=0, sig12_el=0, sig13_el=0,
 sig23_el=0;
	//, Vwater = 0,
 Real kinematicE = 0;

 InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
 InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();

 Real j = 0;
 Real FT[3][3] = {{0,0,0},{0,0,0},{0,0,0}};

 for ( ; ii!=iiEnd ; ++ii )
 {
	 if ( ( *ii )->isReal() )
	 {
		 const shared_ptr<Interaction>& interaction = *ii;

		 unsigned int id1 = interaction -> getId1();
		 unsigned int id2 = interaction -> getId2();

		 SpheresContactGeometry* currentContactGeometry  =
				 static_cast<SpheresContactGeometry*> ( interaction->interactionGeometry.
				 get() );

		 ElasticContactInteraction* currentContactPhysics =
				 static_cast<ElasticContactInteraction*>
				 ( interaction->interactionPhysics.get() );

		 Real fn = currentContactPhysics->normalForce.Length();

		 if ( fn!=0 )

				//if (currentContactGeometry->penetrationDepth >= 0)

		 {
			 j=j+1;

			 Vector3r fel =
					 currentContactPhysics->normalForce + currentContactPhysics->shearForce;

			 f1_el_x=fel[0];
			 f1_el_y=fel[1];
			 f1_el_z=fel[2];

			 int geometryIndex1 =
					 ( *bodies ) [id1]->geometricalModel->getClassIndex();
			 int geometryIndex2 =
					 ( *bodies ) [id2]->geometricalModel->getClassIndex();

			 if ( geometryIndex1 == geometryIndex2 )

			 {
				 BodyMacroParameters* de1 =
						 static_cast<BodyMacroParameters*> ( ( *bodies ) [id1]->physicalParameters.get() );
				 x1 = de1->se3.position[0];
				 y1 = de1->se3.position[1];
				 z1 = de1->se3.position[2];


				 BodyMacroParameters* de2 =
						 static_cast<BodyMacroParameters*> ( ( *bodies ) [id2]->physicalParameters.get() );
				 x2 = de2->se3.position[0];
				 y2 = de2->se3.position[1];
				 z2 = de2->se3.position[2];

					///Calcul des contraintes elastiques spheres/spheres

				 sig11_el = sig11_el + f1_el_x* ( x2 - x1 );
				 sig22_el = sig22_el + f1_el_y* ( y2 - y1 );
				 sig33_el = sig33_el + f1_el_z* ( z2 - z1 );
				 sig12_el = sig12_el + f1_el_x* ( y2 - y1 );
				 sig13_el = sig13_el + f1_el_x* ( z2 - z1 );
				 sig23_el = sig23_el + f1_el_y* ( z2 - z1 );

			 }

			 else

			 {
				 Vector3r l =
						 std::min ( currentContactGeometry->radius2,
						 currentContactGeometry->radius1 )
						 *currentContactGeometry->normal;

					/// Calcul des contraintes elastiques spheres/parois

				 sig11_el = sig11_el + f1_el_x*l[0];
				 sig22_el = sig22_el + f1_el_y*l[1];
				 sig33_el = sig33_el + f1_el_z*l[2];
				 sig12_el = sig12_el + f1_el_x*l[1];
				 sig13_el = sig13_el + f1_el_x*l[2];
				 sig23_el = sig23_el + f1_el_y*l[2];

			 }

				/// fabric tensor

			 Vector3r normal = currentContactGeometry->normal;

			 for ( int i=0; i<3; ++i )
			 {
				 for ( int n=0; n<3; ++n )
				 {
						//fabricTensor[i][n]
					 FT[i][n]
							 += normal[i]*normal[n];
				 }
			 }

		 }
	 }
 }
}*/



YADE_PLUGIN();
