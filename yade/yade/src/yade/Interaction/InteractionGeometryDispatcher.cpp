#include "InteractionGeometryDispatcher.hpp"
#include "ComplexBody.hpp"

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
	
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> interaction;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		interaction = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[interaction->getId1()];
		shared_ptr<Body> b2 = (*bodies)[interaction->getId2()];

		interaction->isReal = interactionGeometryDispatcher( b1->interactionGeometry , b2->interactionGeometry , b1->physicalParameters->se3 , b2->physicalParameters->se3 , interaction );
	}
}
