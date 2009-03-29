/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include"ResultantForceEngine.hpp"

#ifndef BEX_CONTAINER

	#include<yade/pkg-common/ParticleParameters.hpp>
	#include<yade/pkg-common/Force.hpp>
	#include<yade/pkg-dem/GlobalStiffness.hpp>
	#include<Wm3Math.h>
	#include<yade/lib-base/yadeWm3.hpp>
	#include<yade/lib-base/yadeWm3Extra.hpp>



	#include<yade/core/MetaBody.hpp>


	ResultantForceEngine::ResultantForceEngine() : actionParameterGlobalStiffness(new GlobalStiffness), actionParameterForce(new Force)
	{
		interval =1;
		damping = 0.1;
		force = Vector3r::ZERO;
		previoustranslation = Vector3r::ZERO;
		stiffness = Vector3r::ZERO;
		max_vel = 0.001;
	}

	ResultantForceEngine::~ResultantForceEngine()
	{
	}


	void ResultantForceEngine::registerAttributes()
	{
		DeusExMachina::registerAttributes();
		REGISTER_ATTRIBUTE(interval);
		REGISTER_ATTRIBUTE(damping);
		REGISTER_ATTRIBUTE(force);
		REGISTER_ATTRIBUTE(previoustranslation);
		REGISTER_ATTRIBUTE(stiffness);
		REGISTER_ATTRIBUTE(max_vel);
	}



	void ResultantForceEngine::applyCondition(MetaBody* ncb)
	{
		//cerr << "void ResultantForceEngine::applyCondition(Body* body)" << std::endl;
		shared_ptr<BodyContainer>& bodies = ncb->bodies;
		
		std::vector<int>::const_iterator ii = subscribedBodies.begin();
		std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
		
		//cerr << "std::vector<int>::const_iterator iiEnd = subscribedBodies.end();" << Omega::instance().getCurrentIteration() << std::endl;
		
		
		
		for(;ii!=iiEnd;++ii)
		{
		//cerr << "for(;ii!=iiEnd;++ii)" << std::endl;
		//if( bodies->exists(*ii) ) 
		//{
			//Update stiffness only if it has been computed by StiffnessCounter (see "interval")
			if (Omega::instance().getCurrentIteration() % interval == 0)	stiffness =
			(static_cast<GlobalStiffness*>( ncb->physicalActions->find (*ii, actionParameterGlobalStiffness->getClassIndex() ).get() ))->stiffness;
		
			//cerr << "static_cast<GlobalStiffness*>( ncb->physicalActions->find (*ii, actionParameterGlobalStiffness->getClassIndex() ).get() ))->stiffness" << std::endl;
			
			if(PhysicalParameters* p = dynamic_cast<PhysicalParameters*>((*bodies)[*ii]->physicalParameters.get()))
			{
				//cerr << "dynamic_cast<PhysicalParameters*>((*bodies)[*ii]->physicalParameters.get()" << std::endl;
	//			GlobalStiffness* sm = static_cast<GlobalStiffness*>( ncb->physicalActions->find (*ii, actionParameterGlobalStiffness->getClassIndex() ).get() );
				
				Vector3r effectiveforce =
					static_cast<Force*>( ncb->physicalActions->find( *ii,actionParameterForce->getClassIndex() ).get() )->force; 
				Vector3r deltaf (effectiveforce - force);
				Vector3r translation 
					(stiffness.X()==0 ? Mathr::Sign(deltaf.X())*max_vel : Mathr::Sign(deltaf.X())*std::min( abs(deltaf.X()/stiffness.X()), max_vel),
					stiffness.Y()==0 ? Mathr::Sign(deltaf.Y())*max_vel : Mathr::Sign(deltaf.Y())*std::min( abs(deltaf.Y()/stiffness.Y()), max_vel),
					stiffness.Z()==0 ? Mathr::Sign(deltaf.Z())*max_vel : Mathr::Sign(deltaf.Z())*std::min( abs(deltaf.Z()/stiffness.Z()), max_vel) );
				previoustranslation = (1-damping)*translation + 0.9*previoustranslation;// formula for "steady-flow" evolution with fluctuations
				p->se3.position	+= previoustranslation;
				//p->velocity		=  previoustranslation/dt;//FIXME : useless???	
			}		
		//}
		}
	}


	YADE_PLUGIN();

#endif
