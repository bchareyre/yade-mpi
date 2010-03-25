/*************************************************************************
*  Copyright (C) 2006 by luc scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "ContactStressRecorder.hpp"
#include <yade/pkg-common/ElastMat.hpp>
//#include <yade/pkg-common/ParticleParameters.hpp>
//#include <yade/pkg-common/Force.hpp>
#include <yade/pkg-common/Sphere.hpp>
//#include <yade/pkg-dem/BodyMacroParameters.hpp>
#include <yade/pkg-dem/ElasticContactLaw.hpp>

#include <yade/pkg-dem/ScGeom.hpp>
#include <yade/pkg-dem/FrictPhys.hpp>

#include <yade/pkg-dem/TriaxialStressController.hpp>
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/Scene.hpp>
#include <boost/lexical_cast.hpp>

YADE_PLUGIN((ContactStressRecorder));
CREATE_LOGGER ( ContactStressRecorder );

//// related to OLD CODE!!!
// ContactStressRecorder::ContactStressRecorder () : Recorder()/*, actionForce ( new Force )*/
// {
// 	outputFile = "";
// 	interval = 1;
// 
// 	height = 0;
// 	width = 0;
// 	depth = 0;
// 	thickness = 0;
// 	upperCorner = Vector3r ( 0,0,0 );
// 	lowerCorner = Vector3r ( 0,0,0 );
// 
// 	sphere_ptr = shared_ptr<Shape> ( new Sphere );
// 	SpheresClassIndex = sphere_ptr->getClassIndex();
// 
// 	//triaxialCompressionEngine = NULL;
// 	//sampleCapPressEng = new SampleCapillaryPressureEngine;
// 
// }

void ContactStressRecorder::postProcessAttributes ( bool deserializing )
{
	if ( deserializing )
	{
		ofile.open ( outputFile.c_str() );
	}
}

bool ContactStressRecorder::isActivated()
{
	return ( ( scene->currentIteration % interval == 0 ) && ( ofile ) );
}

void ContactStressRecorder::action ()
{
	shared_ptr<BodyContainer>& bodies = scene->bodies;

	if ( !triaxialCompressionEngine )
	{
		vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();
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

	Real f1_el_x=0, f1_el_y=0, f1_el_z=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;

	Real sig11_el=0, sig22_el=0, sig33_el=0, sig12_el=0, sig13_el=0,  sig23_el=0;
	Real kinematicE = 0;
	Real j = 0;
	Real FT[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
	
	InteractionContainer::iterator ii    = scene->interactions->begin();
	InteractionContainer::iterator iiEnd = scene->interactions->end();

	for ( ; ii!=iiEnd ; ++ii )
	{
		if ( ( *ii )->isReal() )
		{
			const shared_ptr<Interaction>& interaction = *ii;

			unsigned int id1 = interaction -> getId1();
			unsigned int id2 = interaction -> getId2();

			ScGeom* currentContactGeometry = static_cast<ScGeom*> ( interaction->interactionGeometry.get() );
			FrictPhys* currentContactPhysics = static_cast<FrictPhys*> ( interaction->interactionPhysics.get() );

			Real fn = currentContactPhysics->normalForce.Length();

			if ( fn!=0 ) {
			  
			  j=j+1;
			  Vector3r fel = currentContactPhysics->normalForce + currentContactPhysics->shearForce;
			  
			  f1_el_x=fel[0];
			  f1_el_y=fel[1];
			  f1_el_z=fel[2];

			  int geometryIndex1 = ( *bodies ) [id1]->shape->getClassIndex();
			  int geometryIndex2 = ( *bodies ) [id2]->shape->getClassIndex();

			  if ( geometryIndex1 == geometryIndex2 ) {
			    
			    Body* de1 = (*bodies)[id1].get();
			    Body* de2 = (*bodies)[id2].get();
			    x1 = de1->state->pos[0];
			    y1 = de1->state->pos[1];
			    z1 = de1->state->pos[2];
			    x2 = de2->state->pos[0];
			    y2 = de2->state->pos[1];
			    z2 = de2->state->pos[2];
					
			    /// stresses from contact forces between spheres
  
			    sig11_el = sig11_el + f1_el_x* ( x2 - x1 );
			    sig22_el = sig22_el + f1_el_y* ( y2 - y1 );
			    sig33_el = sig33_el + f1_el_z* ( z2 - z1 );
			    sig12_el = sig12_el + f1_el_x* ( y2 - y1 );
			    sig13_el = sig13_el + f1_el_x* ( z2 - z1 );
			    sig23_el = sig23_el + f1_el_y* ( z2 - z1 );
			  }
			  else {
			    
			    Vector3r l = std::min ( currentContactGeometry->radius2, currentContactGeometry->radius1 ) *currentContactGeometry->normal;

			    /// stresses from contact forces between spheres and boxes
			    
			    sig11_el = sig11_el + f1_el_x*l[0];
			    sig22_el = sig22_el + f1_el_y*l[1];
			    sig33_el = sig33_el + f1_el_z*l[2];
			    sig12_el = sig12_el + f1_el_x*l[1];
			    sig13_el = sig13_el + f1_el_x*l[2];
			    sig23_el = sig23_el + f1_el_y*l[2];
			  }

			  /// fabric tensor
			  
			  Vector3r normal = currentContactGeometry->normal;
			  for ( int i=0; i<3; ++i ) {
			    for ( int n=0; n<3; ++n ) {
			      
			      FT[i][n] += normal[i]*normal[n];
			    }
			  }
			}
		}
	}

	/// FabricTensor
	//Real traceFT = (FT[0][0]+FT[1][1]+FT[2][2])/j;

	/// Kinematic Energy

	Real nbElt = 0, SR = 0, Vs=0, Rbody=0;
// 	Real Rmin=1, Rmax=0;

	BodyContainer::iterator bi = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();

	for ( ; bi!=biEnd; ++bi ) {
	  
	  shared_ptr<Body> b = *bi;
	  Sphere* sphere = static_cast<Sphere*>(b->shape.get());
	  
// 	  // related to OLD CODE with SphereClassIndex!
// 	  int geometryIndex = b->shape->getClassIndex();
// 	  if ( geometryIndex == SpheresClassIndex ) {

	  if (sphere) { // should be enough to distinguish spheres from walls (boxes) -> to be verified!
	  
	    nbElt +=1;
	    Vector3r v = b->state->vel;
	    kinematicE += 0.5* ( b->state->mass ) * ( v[0]*v[0]+v[1]*v[1]+v[2]*v[2] );

	    Sphere* sphere = static_cast<Sphere*> ( b->shape.get() );
	    Rbody = sphere->radius;
	    //if ( Rbody<Rmin ) Rmin = Rbody;
	    //if ( Rbody>Rmax ) Rmax = Rbody;
	    SR+=Rbody;
	    Vs += 1.3333333*Mathr::PI* ( Rbody*Rbody*Rbody );
	  }
	}

	/// coordination number

	Real coordN = 0;
	coordN = 2* ( j/nbElt );

// 	Real Rmoy = SR/nbElt;
// 	if ( Omega::instance().getCurrentIteration() == 1 )
// 	{
// 		cerr << "Rmoy = "<< Rmoy << "  Rmin = " << Rmin << "  Rmax = " << Rmax << endl;
// 	}
	
	/// homogeneized stresses

	Real SIG_11_el=0, SIG_22_el=0, SIG_33_el=0, SIG_12_el=0, SIG_13_el=0, SIG_23_el=0;

	// Sample volume
	Real V = ( triaxialCompressionEngine->height ) * ( triaxialCompressionEngine->width ) * ( triaxialCompressionEngine->depth );

	SIG_11_el = sig11_el/V;
	SIG_22_el = sig22_el/V;
	SIG_33_el = sig33_el/V;
	SIG_12_el = sig12_el/V;
	SIG_13_el = sig13_el/V;
	SIG_23_el = sig23_el/V;

	/// porosity

	Real Vv = V - Vs;
	Real n = Vv/V;
	//Real e = Vv/Vs;

	/// UnbalancedForce
	//Real equilibriumForce = triaxialCompressionEngine->ComputeUnbalancedForce ( scene );

// 	if ( Omega::instance().getCurrentIteration() % 100 == 0 )
// 	{
// 		cerr << "current Iteration " << Omega::instance().getCurrentIteration()
// 		<< endl;
// 	}

	ofile /*<< lexical_cast<string>(Omega::instance().getSimulationTime()) << " "*/
	<< lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << "  "
	<< lexical_cast<string> ( kinematicE ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->ComputeUnbalancedForce(scene) ) <<" "
	<< lexical_cast<string> ( n ) <<" "
//   << lexical_cast<string>(e)<<" "
	<< lexical_cast<string> ( coordN ) <<"   "
	<< lexical_cast<string> ( SIG_11_el ) << " "
	<< lexical_cast<string> ( SIG_22_el ) << " "
	<< lexical_cast<string> ( SIG_33_el ) << " "
	<< lexical_cast<string> ( SIG_12_el ) << " "
	<< lexical_cast<string> ( SIG_13_el ) << " "
	<< lexical_cast<string> ( SIG_23_el ) << "   "
	<< lexical_cast<string> ( triaxialCompressionEngine->strain[0] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->strain[1] ) << " "
	<< lexical_cast<string> ( triaxialCompressionEngine->strain[2] ) << "   "
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



