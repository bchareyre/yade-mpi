#ifdef WIN32
#include <windows.h> // The Win32 versions of the GL header files require that you windows.h before gl.h/glu.h/glut.h, so that you get the #define types like WINGDIAPI and such
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include "Math.hpp"

#include "NonConnexBody.hpp"
#include "InteractionVecSet.hpp"
//#include "InteractionHashMap.hpp"
#include "BodyAssocVec.hpp"
#include "BoundingVolumeUpdator.hpp"

// FIXME - who is to decide which class to use by default?
NonConnexBody::NonConnexBody() : Body() , bodies(new BodyAssocVec) , permanentInteractions(shared_ptr<InteractionContainer>(new InteractionVecSet))
{

containSubBodies = true;

}

NonConnexBody::~NonConnexBody()
{

}


void NonConnexBody::glDrawGeometricalModel()
{
	//glPushMatrix();

	//Real angle;
	//Vector3r axis;
	//se3.rotation.toAxisAngle(axis,angle);

	//glTranslatef(se3.translation[0],se3.translation[1],se3.translation[2]);
	//glRotated(angle*Mathr::RAD_TO_DEG,axis[0],axis[1],axis[2]);

	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
		bodies->getCurrent()->glDrawGeometricalModel();
	

	//glPopMatrix();
}

void NonConnexBody::glDrawBoundingVolume()
{
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
		bodies->getCurrent()->glDrawBoundingVolume();
	bv->glDraw();
}

void NonConnexBody::glDrawCollisionGeometry()
{
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
		bodies->getCurrent()->glDrawCollisionGeometry();
	cm->glDraw();
}

#include "CollisionGeometrySet.hpp"

void NonConnexBody::postProcessAttributes(bool deserializing)
{
	if (deserializing)
	{
		// FIXME : build that with CollisionGeometryFactory
		shared_ptr<CollisionGeometrySet> set = dynamic_pointer_cast<CollisionGeometrySet>(cm);
		for(unsigned int i=0;i<bodies->size();i++)
		{
			set->collisionGeometries.push_back((*bodies)[i]->cm);
		}

	// to build bounding volume if there is a boundingvolume updator in the actor list
	// FIXME : I don't know is this is so dirty to do that here
	vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
	vector<shared_ptr<Actor> >::iterator aiEnd =  actors.end();
	for(;ai!=aiEnd;++ai)
		if (dynamic_pointer_cast<BoundingVolumeUpdator>(*ai))
			(*ai)->action(this);
	}
			
	Body::postProcessAttributes(deserializing);
}

void NonConnexBody::registerAttributes()
{
	Body::registerAttributes();

	REGISTER_ATTRIBUTE(permanentInteractions);
	REGISTER_ATTRIBUTE(bodies);
//	REGISTER_ATTRIBUTE(narrowCollider);
//	REGISTER_ATTRIBUTE(broadCollider);
//	REGISTER_ATTRIBUTE(kinematic);
}


void NonConnexBody::moveToNextTimeStep()
{

	vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
	vector<shared_ptr<Actor> >::iterator aiEnd =  actors.end();
	for(;ai!=aiEnd;++ai)
		if ((*ai)->isActivated())
			(*ai)->action(this);

// FIND INTERACTIONS
	// serach for potential collision (maybe in to steps for hierarchical simulation)
//	if (broadCollider!=0)
//		broadCollider->broadCollisionTest(bodies,interactions);
	// this has to split the contact list into several constact list according to the physical type
	// (RigidBody,FEMBody ...) of colliding body
//	if (narrowCollider!=0)
//		narrowCollider->narrowCollisionPhase(bodies,interactions);
// MOTION
	// for each contact list we call the correct dynamic engine
	//if (dynamic!=0)
	//	dynamic->respondToCollisions(this,interactions); //effectiveDt == dynamic->...
	//if (kinematic!=0)
	//	kinematic->moveToNextTimeStep(bodies);

//	shared_ptr<Body> b;
//	for( b = bodies->getFirst() ; bodies->hasCurrent() ; b = bodies->getNext() )
//		b->moveToNextTimeStep();

	// FIXME : do we need to walk through the tree of objects in a different way than below  (recursively in NCB... ) ??
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
		bodies->getCurrent()->moveToNextTimeStep();

}
