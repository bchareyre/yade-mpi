#include "Translator.hpp"
#include "RigidBodyParameters.hpp"
#include "ComplexBody.hpp"

void Translator::postProcessAttributes(bool deserializing)
{
	if(deserializing)
		translationAxis.normalize();
}

void Translator::registerAttributes()
{
	KinematicEngine::registerAttributes();
	REGISTER_ATTRIBUTE(velocity);
	REGISTER_ATTRIBUTE(translationAxis);
}

void Translator::moveToNextTimeStep(Body * body)
{

	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

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
	{
		RigidBodyParameters * rb = static_cast<RigidBodyParameters*>((*bodies)[*ii]->physicalParameters.get());

		// FIXME - specify intervals of activity for an actor => use isActivated method
		//if( Omega::instance().getIter() > 1000 )
		//	b->velocity		= Vector3r(0,0,0);
		//else
		//{
			rb->se3.translation	+= sign*dt*velocity*translationAxis;
			rb->velocity		=  sign*velocity*translationAxis;
		//}
	}


}
