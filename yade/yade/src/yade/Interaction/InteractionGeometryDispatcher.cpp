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
		for(unsigned int i=0;i<collisionFunctors.size();i++)
			narrowCollisionDispatcher.add(collisionFunctors[i][0],collisionFunctors[i][1],collisionFunctors[i][2]);
	}
}

void InteractionGeometryDispatcher::registerAttributes()
{
	REGISTER_ATTRIBUTE(collisionFunctors);
}

void InteractionGeometryDispatcher::addCollisionFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	collisionFunctors.push_back(v);
}


void InteractionGeometryDispatcher::action(Body* body)
{
	//this->narrowCollisionPhase(b);	
	
	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	shared_ptr<Interaction> contact;

	for( ncb->interactions->gotoFirstPotential() ; ncb->interactions->notAtEndPotential() ; ncb->interactions->gotoNextPotential())
	{
		contact = ncb->interactions->getCurrent();
		
		shared_ptr<Body> b1 = (*bodies)[contact->getId1()];
		shared_ptr<Body> b2 = (*bodies)[contact->getId2()];

		contact->isReal = narrowCollisionDispatcher( b1->cm , b2->cm , b1->se3 , b2->se3 , contact );
	}
}
