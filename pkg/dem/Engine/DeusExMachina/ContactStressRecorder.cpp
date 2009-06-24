/*************************************************************************
*  Copyright (C) 2006 by luc scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ContactStressRecorder.hpp"
#include <yade/pkg-common/RigidBodyParameters.hpp>
#include <yade/pkg-common/ParticleParameters.hpp>
//#include <yade/pkg-common/Force.hpp>
#include <yade/pkg-common/Sphere.hpp>
#include <yade/pkg-dem/BodyMacroParameters.hpp>
#include <yade/pkg-dem/ElasticContactLaw.hpp>

#include <yade/pkg-dem/SpheresContactGeometry.hpp>
#include <yade/pkg-dem/ElasticContactInteraction.hpp>

#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>

CREATE_LOGGER ( ContactStressRecorder );

ContactStressRecorder::ContactStressRecorder () : DataRecorder()/*, actionForce ( new Force )*/
{
	outputFile = "";
	interval = 1;

	height = 0;
	width = 0;
	depth = 0;
	thickness = 0;
	upperCorner = Vector3r ( 0,0,0 );
	lowerCorner = Vector3r ( 0,0,0 );

	sphere_ptr = shared_ptr<GeometricalModel> ( new Sphere );
	SpheresClassIndex = sphere_ptr->getClassIndex();

	//triaxCompEng = NULL;
	//sampleCapPressEng = new SampleCapillaryPressureEngine;

}


void ContactStressRecorder::postProcessAttributes ( bool deserializing )
{
	if ( deserializing )
	{
		ofile.open ( outputFile.c_str() );
	}
}


void ContactStressRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE ( outputFile );
	REGISTER_ATTRIBUTE ( interval );

	REGISTER_ATTRIBUTE ( wall_bottom_id );
	REGISTER_ATTRIBUTE ( wall_top_id );
	REGISTER_ATTRIBUTE ( wall_left_id );
	REGISTER_ATTRIBUTE ( wall_right_id );
	REGISTER_ATTRIBUTE ( wall_front_id );
	REGISTER_ATTRIBUTE ( wall_back_id );

	REGISTER_ATTRIBUTE ( height );
	REGISTER_ATTRIBUTE ( width );
	REGISTER_ATTRIBUTE ( depth );
	REGISTER_ATTRIBUTE ( thickness );
	REGISTER_ATTRIBUTE ( upperCorner );
	REGISTER_ATTRIBUTE ( lowerCorner );

}


bool ContactStressRecorder::isActivated()
{
	return ( ( Omega::instance().getCurrentIteration() % interval == 0 ) && ( ofile ) );
}


void ContactStressRecorder::action ( MetaBody * ncb )
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	if ( !triaxCompEng )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = ncb->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = ncb->engines.end();
		for ( ;itFirst!=itLast; ++itFirst )
		{
			if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine" ) //|| (*itFirst)->getBaseClassName() == "TriaxialCompressionEngine")
			{
				LOG_DEBUG ( "stress controller engine found" );
				triaxCompEng =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );
				//triaxialCompressionEngine = shared_ptr<TriaxialCompressionEngine> (static_cast<TriaxialCompressionEngine*> ( (*itFirst).get()));
			}
		}
		if ( !triaxCompEng ) LOG_DEBUG ( "stress controller engine NOT found" );
	}

	Real f1_el_x=0, f1_el_y=0, f1_el_z=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;

	Real sig11_el=0, sig22_el=0, sig33_el=0, sig12_el=0, sig13_el=0,  sig23_el=0;
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
				static_cast<SpheresContactGeometry*> ( interaction->interactionGeometry.get() );

			ElasticContactInteraction* currentContactPhysics =
				static_cast<ElasticContactInteraction*> ( interaction->interactionPhysics.get() );

			Real fn = currentContactPhysics->normalForce.Length();

			if ( fn!=0 )

				//if (currentContactGeometry->penetrationDepth >= 0)

			{
				j=j+1;

				Vector3r fel = currentContactPhysics->normalForce + currentContactPhysics->shearForce;

				f1_el_x=fel[0];
				f1_el_y=fel[1];
				f1_el_z=fel[2];

				int geometryIndex1 =
					( *bodies ) [id1]->geometricalModel->getClassIndex();
				int geometryIndex2 =
					( *bodies ) [id2]->geometricalModel->getClassIndex();

				if ( geometryIndex1 == geometryIndex2 )

				{
					BodyMacroParameters* de1 = static_cast<BodyMacroParameters*> ( ( *bodies ) [id1]->physicalParameters.get() );
					x1 = de1->se3.position[0];
					y1 = de1->se3.position[1];
					z1 = de1->se3.position[2];


					BodyMacroParameters* de2 = static_cast<BodyMacroParameters*> ( ( *bodies ) [id2]->physicalParameters.get() );
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
					Vector3r l = std::min ( currentContactGeometry->radius2, currentContactGeometry->radius1 ) *currentContactGeometry->normal;

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

	/// FabricTensor

	//Real traceFT = (FT[0][0]+FT[1][1]+FT[2][2])/j;

	/// calcul de l'energie cinetique:

	Real nbElt = 0, SR = 0, Vs=0, Rbody=0, Rmin=1, Rmax=0;

	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();

	for ( ; bi!=biEnd; ++bi )

	{
		shared_ptr<Body> b = *bi;

		int geometryIndex = b->geometricalModel->getClassIndex();

		if ( geometryIndex == SpheresClassIndex )
		{
			nbElt +=1;
			ParticleParameters* pp = static_cast<ParticleParameters*> ( b->physicalParameters.get() );
			Vector3r v = pp->velocity;
			kinematicE +=
				0.5* ( pp->mass ) * ( v[0]*v[0]+v[1]*v[1]+v[2]*v[2] );

			Sphere* sphere = static_cast<Sphere*> ( b->geometricalModel.get() );
			Rbody = sphere->radius;
			if ( Rbody<Rmin ) Rmin = Rbody;
			if ( Rbody>Rmax ) Rmax = Rbody;
			SR+=Rbody;

			Vs += 1.3333333*Mathr::PI* ( Rbody*Rbody*Rbody );

		}
	}

	/// coordination number

	Real coordN = 0;
	coordN = 2* ( j/nbElt ); // ????????????????????????????????????????????

	/// Calcul des contraintes "globales"

	Real SIG_11_el=0, SIG_22_el=0, SIG_33_el=0, SIG_12_el=0, SIG_13_el=0, SIG_23_el=0;

	// volume de l'echantillon


//  PhysicalParameters* p_bottom = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get());
//  PhysicalParameters* p_top   =  static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get());
//  PhysicalParameters* p_left  = static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get());
//  PhysicalParameters* p_right = static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.get());
//  PhysicalParameters* p_front = static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.get());
//  PhysicalParameters* p_back  = static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get());


//  height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
//  width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
//  depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;

	Real Rmoy = SR/nbElt;
	if ( Omega::instance().getCurrentIteration() == 1 )
	{
		cerr << "Rmoy = "<< Rmoy << "  Rmin = " << Rmin << "  Rmax = " <<
		Rmax << endl;
	}

	Real V = ( triaxCompEng->height ) * ( triaxCompEng->width ) * ( triaxCompEng->depth );

	SIG_11_el = sig11_el/V;
	SIG_22_el = sig22_el/V;
	SIG_33_el = sig33_el/V;
	SIG_12_el = sig12_el/V;
	SIG_13_el = sig13_el/V;
	SIG_23_el = sig23_el/V;

	/// calcul des deformations

//  Real EPS_11=0, EPS_22=0, EPS_33=0;
//
//  Real width_0 = upperCorner[0]-lowerCorner[0], height_0 =
//  upperCorner[1]-lowerCorner[1],
//  depth_0 = upperCorner[2]-lowerCorner[2];
//
// // EPS_11 = (width_0 - width)/width_0;
//  EPS_11 = std::log(width_0) - std::log(width);
// // EPS_22 = (height_0 - height)/height_0;
//  EPS_22 = std::log(height_0) - std::log(height);
// // EPS_33 = (depth_0 - depth)/depth_0;
//  EPS_33 = std::log(depth_0) - std::log(depth);

	/// porosity

	Real Vv = V - Vs;

	Real n = Vv/V;
//  Real e = Vv/Vs;

//  mise a zero des deformations qd comp triaxiale commence
//  if (triaxCompEng->compressionActivated) {  }

	/// UnbalancedForce

	Real equilibriumForce = triaxCompEng->ComputeUnbalancedForce ( ncb );
//  Real equilibriumForce = sampleCapPressEng->ComputeUnbalancedForce(body);

	if ( Omega::instance().getCurrentIteration() % 100 == 0 )
	{
		cerr << "current Iteration " << Omega::instance().getCurrentIteration()
		<< endl;
	}

	ofile /*<< lexical_cast<string>(Omega::instance().getSimulationTime()) << " "*/
	<< lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << "  "
	<< lexical_cast<string> ( kinematicE ) << " "
	<< lexical_cast<string> ( equilibriumForce ) <<" "
	<< lexical_cast<string> ( n ) <<" "
//   << lexical_cast<string>(e)<<" "
	<< lexical_cast<string> ( coordN ) <<"   "
	<< lexical_cast<string> ( SIG_11_el ) << " "
	<< lexical_cast<string> ( SIG_22_el ) << " "
	<< lexical_cast<string> ( SIG_33_el ) << " "
	<< lexical_cast<string> ( SIG_12_el ) << " "
	<< lexical_cast<string> ( SIG_13_el ) << " "
	<< lexical_cast<string> ( SIG_23_el ) << "   "
	<< lexical_cast<string> ( triaxCompEng->strain[0] ) << " "
	<< lexical_cast<string> ( triaxCompEng->strain[1] ) << " "
	<< lexical_cast<string> ( triaxCompEng->strain[2] ) << "   "
	<< lexical_cast<string> ( FT[0][0]/j ) <<" "
	<< lexical_cast<string> ( FT[0][1]/j ) <<" "
	<< lexical_cast<string> ( FT[0][2]/j ) <<" "
	<< lexical_cast<string> ( FT[1][0]/j ) <<" "
	<< lexical_cast<string> ( FT[1][1]/j ) <<" "
	<< lexical_cast<string> ( FT[1][2]/j ) <<" "
	<< lexical_cast<string> ( FT[2][0]/j ) <<" "
	<< lexical_cast<string> ( FT[2][1]/j ) <<" "
	<< lexical_cast<string> ( FT[2][2]/j ) <<" "
	<< endl;

}

YADE_PLUGIN();
