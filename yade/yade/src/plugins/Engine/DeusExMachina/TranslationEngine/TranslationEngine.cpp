#include "TranslationEngine.hpp"
#include "ParticleParameters.hpp"
#include "MetaBody.hpp"

void TranslationEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
		translationAxis.normalize();
}

void TranslationEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(velocity);
	REGISTER_ATTRIBUTE(translationAxis);
}

void TranslationEngine::applyCondition(Body * body)
{

	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	Real dt = Omega::instance().getTimeStep();
	time = dt;
	static int sign = 1;

	//if ((Omega::instance().getCurrentIteration()/1000)%2==0)
		sign = 1;
	//else
	//	sign = -1;

	for(;ii!=iiEnd;++ii)
		if(ParticleParameters* p = dynamic_cast<ParticleParameters*>((*bodies)[*ii]->physicalParameters.get()))
		{

		// FIXME - specify intervals of activity for an actor => use isActivated method
		//if( Omega::instance().getIter() > 1000 )
		//	b->velocity		= Vector3r(0,0,0);
		//else
		//{
			p->se3.position		+= sign*dt*velocity*translationAxis;
			p->velocity		=  sign*velocity*translationAxis;
		//}
		
		}
		else if(PhysicalParameters* b = dynamic_cast<PhysicalParameters*>((*bodies)[*ii]->physicalParameters.get()))
		{ // NOT everyone has velocity !
			b->se3.position		+= sign*dt*velocity*translationAxis;
		}
		else
		{
			std::cerr << "TranslationEngine::applyCondition, WARNING! dynamic_cast failed! for id: " << *ii << std::endl; // FUCK! I've literally spent two days - 10hours to find a mistake caused by static_cast!!, we should never use static_cast when in DEBUG mode !!!!!
		}


}
