#include "NarrowInteractor.hpp"

NarrowInteractor::NarrowInteractor () : Actor()
{
}

NarrowInteractor::~NarrowInteractor ()
{

}

void NarrowInteractor::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		for(unsigned int i=0;i<collisionFunctors.size();i++)
			narrowCollisionDispatcher.add(collisionFunctors[i][0],collisionFunctors[i][1],collisionFunctors[i][2]);
	}
}

void NarrowInteractor::registerAttributes()
{
	REGISTER_ATTRIBUTE(collisionFunctors);
}

void NarrowInteractor::addCollisionFunctor(const string& str1,const string& str2,const string& str3)
{
	vector<string> v;
	v.push_back(str1);
	v.push_back(str2);
	v.push_back(str3);
	collisionFunctors.push_back(v);
}


void NarrowInteractor::action(Body* b)
{
	this->narrowCollisionPhase(b);
}
