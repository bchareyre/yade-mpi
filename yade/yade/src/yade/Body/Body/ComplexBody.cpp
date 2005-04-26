#include "Math.hpp"

#include "ComplexBody.hpp"
#include "Actor.hpp"

////////// Containers
#include "InteractionVecSet.hpp"
#include "InteractionHashMap.hpp"
#include "BodyRedirectionVector.hpp"
#include "BodyAssocVector.hpp"
#include "ActionParameterVectorVector.hpp"
//////////


// FIXME - who is to decide which class to use by default?
ComplexBody::ComplexBody() :
	  Body()
	, bodies(new BodyRedirectionVector)
	, persistentInteractions(new InteractionVecSet)
	, volatileInteractions(new InteractionVecSet)
	, actionParameters(new ActionParameterVectorVector)
{	
	actors.clear();
	initializers.clear();
}

void ComplexBody::postProcessAttributes(bool deserializing)
{
	if (deserializing)
	{
		vector<shared_ptr<Actor> >::iterator i    = initializers.begin();
		vector<shared_ptr<Actor> >::iterator iEnd = initializers.end();
		for( ; i != iEnd ; ++i)
			if ((*i)->isActivated())
				(*i)->action(this);
				
	//	initializers.clear(); // FIXME - we want to delate ONLY some of them!
	//                                       because when you save and load file, you still want some initializers, but not all of them. Eg - you don't want VRML loader, or FEM loader, but you want BoundingVolumeDispatcher. Maybe we need two list of initilizers? One that 'survive' between load and save, and others that are deleted on first time?
	}
}

void ComplexBody::registerAttributes()
{
	Body::registerAttributes();
	REGISTER_ATTRIBUTE(actors);
	REGISTER_ATTRIBUTE(initializers);
	REGISTER_ATTRIBUTE(bodies);
	REGISTER_ATTRIBUTE(volatileInteractions);
	REGISTER_ATTRIBUTE(persistentInteractions);
//	REGISTER_ATTRIBUTE(actionParameters); // FIXME - needed or not ?
}


void ComplexBody::moveToNextTimeStep()
{
	vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
	vector<shared_ptr<Actor> >::iterator aiEnd = actors.end();
	for(;ai!=aiEnd;++ai)
		if ((*ai)->isActivated())
			(*ai)->action(this);
}
