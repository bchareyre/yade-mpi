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
	, initialInteractions(new InteractionVecSet)
	, runtimeInteractions(new InteractionVecSet)
	, actionParameters(new ActionParameterVectorVector)
{	
}

void ComplexBody::postProcessAttributes(bool deserializing)
{
	if (deserializing)
	{
		vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
		vector<shared_ptr<Actor> >::iterator aiEnd = actors.end();
		for(;ai!=aiEnd;++ai)
			if ( (*ai)->isInitializer() )
				(*ai)->action(this);
				
		for( ai = actors.begin() , aiEnd = actors.end() ; ai != aiEnd ; ++ai)
			if( (*ai)->isInitializer() && (*ai)->removeAfter() )
			{
				actors.erase(ai); // delete from actors, and start searching again.
				ai = actors.begin();
				aiEnd = actors.end();
			} 
	}
}

void ComplexBody::registerAttributes()
{
	Body::registerAttributes();
	REGISTER_ATTRIBUTE(actors);
	REGISTER_ATTRIBUTE(bodies);
	REGISTER_ATTRIBUTE(runtimeInteractions);
	REGISTER_ATTRIBUTE(initialInteractions);
//	REGISTER_ATTRIBUTE(actionParameters); // FIXME - needed or not ?
}


void ComplexBody::moveToNextTimeStep()
{
	vector<shared_ptr<Actor> >::iterator ai    = actors.begin();
	vector<shared_ptr<Actor> >::iterator aiEnd =  actors.end();
	for(;ai!=aiEnd;++ai)
		if ((*ai)->isActivated())
			(*ai)->action(this);
}
