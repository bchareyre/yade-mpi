#ifdef WIN32
#include <windows.h> // The Win32 versions of the GL header files require that you windows.h before gl.h/glu.h/glut.h, so that you get the #define types like WINGDIAPI and such
#endif

#include <GL/gl.h>
#include <GL/glut.h>

#include "Math.hpp"
#include "NonConnexBody.hpp"

NonConnexBody::NonConnexBody() : Body()
{
}
NonConnexBody::~NonConnexBody() 
{

}


void NonConnexBody::glDraw()
{	
	
	std::vector<shared_ptr<Body> >::iterator bi = bodies.begin();
	std::vector<shared_ptr<Body> >::iterator biEnd = bodies.end();
	
	glPushMatrix();
	
	float angle;
	Vector3 axis;	
	se3.rotation.toAngleAxis(angle,axis);

	glTranslatef(se3.translation[0],se3.translation[1],se3.translation[2]);
	glRotated(angle*Math::RAD_TO_DEG,axis[0],axis[1],axis[2]);
		
	glDisable(GL_LIGHTING);
	
	for(; bi != biEnd ; ++bi)
		(*bi)->glDraw();	
		
	glPopMatrix();
	
}

void NonConnexBody::processAttributes()
{

}

void NonConnexBody::registerAttributes()
{
	Body::registerAttributes();
	REGISTER_ATTRIBUTE(bodies);

	REGISTER_ATTRIBUTE(narrowCollider);
	REGISTER_ATTRIBUTE(broadCollider);
	REGISTER_ATTRIBUTE(kinematic);
	REGISTER_ATTRIBUTE(dynamic);
}

void NonConnexBody::updateBoundingVolume(Se3& )
{

}

void NonConnexBody::updateCollisionModel(Se3& )
{

}

void NonConnexBody::moveToNextTimeStep(float dt)
{
	// FIXME : hardcoded nbsubtimestep !
	int nbSubStep = 2;
	for(int i=0;i<nbSubStep;i++)
	{
		// serach for potential collision (maybe in to steps for hierarchical simulation)
		broadCollider->broadPhaseCollisionTest(bodies,interactions);

		// this has to split the contact list into several constact list according to the physical type
		// (RigidBody,FEMBody ...) of colliding body
		narrowCollider->narrowCollisionPhase(bodies,interactions);		

		// for each contact list we call the correct dynamic engine
		dynamic->respondToCollisions(bodies,interactions,dt/(float)nbSubStep); //effectiveDt == dynamic->...

		// we call each kinematic engine one after the other
		kinematic->moveToNextTimeStep(bodies,dt/(float)nbSubStep);

		//for each body call its dynamic internal engine

		// maybe use internal engine as an dynamic engine et kinematic engine
	}

	
}
