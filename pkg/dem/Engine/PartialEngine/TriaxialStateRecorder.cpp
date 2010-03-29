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
#include <yade/pkg-dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include <yade/core/Omega.hpp>
#include <yade/core/Scene.hpp>
#include <boost/lexical_cast.hpp>
#include <yade/pkg-dem/ScGeom.hpp>
#include <yade/pkg-dem/FrictPhys.hpp>

CREATE_LOGGER(TriaxialStateRecorder);
TriaxialStateRecorder::~TriaxialStateRecorder() {};

void TriaxialStateRecorder::action ()
{
	// at the beginning of the file; write column titles
	if(out.tellp()==0){
		out<<"iteration s11 s22 s33 e11 e22 e33 unb_force porosity kineticE"<<endl;
	}
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
	if ( ! ( Omega::instance().getCurrentIteration() % triaxialCompressionEngine->computeStressStrainInterval == 0 ) )
		triaxialCompressionEngine->computeStressStrain ();

	/// Compute kinetic energy and porosity :

	Real Vs=0, kinematicE = 0;
	Real V = ( triaxialCompressionEngine->height ) * ( triaxialCompressionEngine->width ) * ( triaxialCompressionEngine->depth );

	BodyContainer::iterator bi = scene->bodies->begin();
	BodyContainer::iterator biEnd = scene->bodies->end();

	for ( ; bi!=biEnd; ++bi )
	{
		const shared_ptr<Body>& b = *bi;
		if ( b->isDynamic ){
			const Vector3r& v = b->state->vel;
			kinematicE +=
				0.5* ( b->state->mass ) * ( v[0]*v[0]+v[1]*v[1]+v[2]*v[2] );

			Vs += 1.3333333*Mathr::PI*pow ( YADE_PTR_CAST<Sphere>( b->shape)->radius, 3 );}
	}
	porosity = ( V - Vs ) /V;
	
	out << lexical_cast<string> ( Omega::instance().getCurrentIteration() ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_right][0] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_front][2] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[0] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[1] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->strain[2] ) << " "
 	<< lexical_cast<string> ( triaxialCompressionEngine->ComputeUnbalancedForce () ) << " "
 	<< lexical_cast<string> ( porosity ) << " "
 	<< lexical_cast<string> ( kinematicE )
 	<< endl;
}


YADE_PLUGIN((TriaxialStateRecorder));
