#include "MassSpringBody2RigidBodyLaw.hpp"
#include "RigidBodyParameters.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"
#include "ClosestFeatures.hpp"
#include "SimpleSpringLaw.hpp"
#include "Mesh2D.hpp"

#include "SDECParameters.hpp"
#include "SDECContactGeometry.hpp"
#include "SDECLinkGeometry.hpp"
#include "SDECContactPhysics.hpp"
#include "SDECLinkPhysics.hpp"

MassSpringBody2RigidBodyLaw::MassSpringBody2RigidBodyLaw ()  : ConstitutiveLaw(), actionForce(new ActionForce) , actionMomentum(new ActionMomentum)
{
	springGroupMask = 2;
	sdecGroupMask = 1;
}


void MassSpringBody2RigidBodyLaw::registerAttributes()
{
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(springGroupMask);
}


void MassSpringBody2RigidBodyLaw::respondToInteractions(Body * body)
{
	ComplexBody * mixedBody = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = mixedBody->bodies;
	shared_ptr<InteractionContainer>& interactions = mixedBody->interactions;
//	shared_ptr<ActionContainer>& actions = mixedBody->actions;
	Real dt = Omega::instance().getTimeStep();

	for( interactions->gotoFirst() ; interactions->notAtEnd() ; interactions->gotoNext() )
	{
		const shared_ptr<Interaction>& mixedInteraction = interactions->getCurrent();
		int id1 = mixedInteraction->getId1();
		int id2 = mixedInteraction->getId2();
		
		if(  !  ( 
			( ((*bodies)[id1]->getGroupMask() & sdecGroupMask  ) && ( (*bodies)[id2]->getGroupMask() & springGroupMask) ) ||
			( ((*bodies)[id1]->getGroupMask() & springGroupMask) && ( (*bodies)[id2]->getGroupMask() & sdecGroupMask  ) )
		  ))
			continue; // skip other groups
			
		
//		if(!  mixedInteraction->interactionPhysics)
//	------------------------ this is bad and will not work, InetractionPhysicsFunctor should create this
//			mixedInteraction->interactionPhysics = shared_ptr<SDECContactPhysics>(new SDECContactPhysics);
//		if(! ((*bodies)[id1]->physicalParameters) )
//			(*bodies)[id1]->physicalParameters = shared_ptr<SDECParameters>(new SDECParameters);
//		if(! ((*bodies)[id2]->physicalParameters) )
//			(*bodies)[id2]->physicalParameters = shared_ptr<SDECParameters>(new SDECParameters);
		

// here (*bodies)[id]->physicalParameters are:
//  - SDECParameters
//  - ParticleParameters
//		cerr << "this is: " << ((*bodies)[id1]->physicalParameters)->getClassName() << endl;
//		cerr << "this is: " << ((*bodies)[id2]->physicalParameters)->getClassName() << endl;

// BEGIN - this is code just duplicated from SDECLaw for non-permanent links
		shared_ptr<SDECParameters> de1 	= dynamic_pointer_cast<SDECParameters>((*bodies)[id1]->physicalParameters);
		shared_ptr<SDECParameters> de2 	= dynamic_pointer_cast<SDECParameters>((*bodies)[id2]->physicalParameters);
		shared_ptr<SDECContactGeometry> currentContactGeometry = dynamic_pointer_cast<SDECContactGeometry>(mixedInteraction->interactionGeometry);
		shared_ptr<SDECContactPhysics> currentContactPhysics   = dynamic_pointer_cast<SDECContactPhysics> (mixedInteraction->interactionPhysics);
		
		if ( mixedInteraction->isNew)
			currentContactPhysics->shearForce			= Vector3r(0,0,0);
				
		Real un 				= currentContactGeometry->penetrationDepth;
		currentContactPhysics->normalForce	= currentContactPhysics->kn*un*currentContactGeometry->normal;

		Vector3r axis;
		Real angle;

		axis	 				= currentContactPhysics->prevNormal.cross(currentContactGeometry->normal);
		currentContactPhysics->shearForce      -= currentContactPhysics->shearForce.cross(axis);
		angle 					= dt*0.5*currentContactGeometry->normal.dot(de1->angularVelocity+de2->angularVelocity);
		axis 					= angle*currentContactGeometry->normal;
		currentContactPhysics->shearForce      -= currentContactPhysics->shearForce.cross(axis);
	

		Vector3r x				= currentContactGeometry->contactPoint;
		Vector3r c1x				= (x - de1->se3.translation);
		Vector3r c2x				= (x - de2->se3.translation);
		Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.cross(c2x)) - (de1->velocity+de1->angularVelocity.cross(c1x));
		Vector3r shearVelocity			= relativeVelocity-currentContactGeometry->normal.dot(relativeVelocity)*currentContactGeometry->normal;
		Vector3r shearDisplacement		= shearVelocity*dt;
		currentContactPhysics->shearForce      -= currentContactPhysics->ks*shearDisplacement;

		Vector3r f				= currentContactPhysics->normalForce + currentContactPhysics->shearForce;

		static_cast<ActionForce*>   ( mixedBody->actions->find( id1 , actionForce   ->getClassIndex() ).get() )->force    -= f;
		static_cast<ActionForce*>   ( mixedBody->actions->find( id2 , actionForce   ->getClassIndex() ).get() )->force    += f;
		
		static_cast<ActionMomentum*>( mixedBody->actions->find( id1 , actionMomentum->getClassIndex() ).get() )->momentum -= c1x.cross(f);
		static_cast<ActionMomentum*>( mixedBody->actions->find( id2 , actionMomentum->getClassIndex() ).get() )->momentum += c2x.cross(f);
		
		currentContactPhysics->prevNormal = currentContactGeometry->normal;
// END										








		
		
		/*	OLD VERSION ....

		shared_ptr<ClosestFeatures> cf = dynamic_pointer_cast<ClosestFeatures>(ct->interactionGeometry);
	//	FIXME : this is a hack because we don't know if id1 is the sphere or piece of massSpring
 		shared_ptr<MassSpringBody> c = dynamic_pointer_cast<MassSpringBody>( (*(ncb->bodies))[ct->getId1()] );
		shared_ptr<RigidBodyParameters> rb = dynamic_pointer_cast<RigidBodyParameters>( (*(ncb->bodies))[ct->getId2()] );
		shared_ptr<Mesh2D> mesh;
		
		if (c)
		{
			mesh = dynamic_pointer_cast<Mesh2D>(c->geometricalModel);
			for(unsigned int i=0;i<cf->verticesId.size();i++)
			{

				Vector3r p1 = cf->closestsPoints[i].first;
				Vector3r p2 = cf->closestsPoints[i].second;
				Vector3r dir = p2-p1;
				Real l = dir.normalize();
				Real relativeVelocity = dir.dot(rb->velocity);
				Real fi = 0.1*l*l/3.0+relativeVelocity*10;
				Vector3r f = fi*dir;
				rb->acceleration -= f*rb->invMass;

				c->externalForces.push_back(pair<int,Vector3r>(mesh->faces[cf->verticesId[i]][0],f));
				c->externalForces.push_back(pair<int,Vector3r>(mesh->faces[cf->verticesId[i]][1],f));
				c->externalForces.push_back(pair<int,Vector3r>(mesh->faces[cf->verticesId[i]][2],f));
			}
		}
	//////////// commented this because it hacks too much stuff...
	/	else //if (cf->verticesId.size()==0)
	/		tmpI.push_back(*ii);
	*/
	}

}

