#include "InteractionGeometryDispatcher.hpp"
#include "NonConnexBody.hpp"

InteractionGeometryDispatcher::InteractionGeometryDispatcher () : Actor()
{
}

InteractionGeometryDispatcher::~InteractionGeometryDispatcher ()
{

}

void InteractionGeometryDispatcher::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<interactionGeometryFunctors.size();i++)
			interactionGeometryDispatcher.add(interactionGeometryFunctors[i][0],interactionGeometryFunctors[i][1],interactionGeometryFunctors[i][2]);
	}
}

void InteractionGeometryDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(interactionGeometryFunctors);
}

void InteractionGeometryDispatcher::addInteractionGeometryFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	interactionGeometryFunctors.push_back(v);
}


void InteractionGeometryDispatcher::action(Body* body)
{
	//this->narrowCollisionPhase(b);	
	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> interaction;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		interaction = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body> b2 = (*bodies)[interaction->getId2()];

		interaction->isReal = interactionGeometryDispatcher( b1->cm , b2->cm , b1->se3 , b2->se3 , interaction );
	}
}
