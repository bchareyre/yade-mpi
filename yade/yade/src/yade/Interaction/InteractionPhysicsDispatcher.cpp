#include "InteractionPhysicsDispatcher.hpp"
#include "ComplexBody.hpp"

InteractionPhysicsDispatcher::InteractionPhysicsDispatcher () : Actor()
{
}

InteractionPhysicsDispatcher::~InteractionPhysicsDispatcher ()
{

}

void InteractionPhysicsDispatcher::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<interactionPhysicsFunctors.size();i++)
			interactionPhysicsDispatcher.add(interactionPhysicsFunctors[i][0],interactionPhysicsFunctors[i][1],interactionPhysicsFunctors[i][2]);
	}
}

void InteractionPhysicsDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(interactionPhysicsFunctors);
}

void InteractionPhysicsDispatcher::addInteractionPhysicsFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	interactionPhysicsFunctors.push_back(v);
}


void InteractionPhysicsDispatcher::action(Body* body)
{
	//this->narrowCollisionPhase(b);	
	
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> interaction;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		interaction = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body> b2 = (*bodies)[interaction->getId2()];

		if (interaction->isReal)
			interactionPhysicsDispatcher( b1 , b2 , interaction );
	}
}
