#include "Translator.hpp"
//#include "RigidBody.hpp"
#include "ComplexBody.hpp"

Translator::Translator () : KinematicEngine() // encapsuler dans implicitfunction user redefini uniquement dp = || interpolateur ...
{
}

Translator::~Translator ()
{

}

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
		shared_ptr<Body>  b = (*bodies)[*ii];

		// FIXME - specify intervals of activity for an actor
		//if( Omega::instance().getIter() > 1000 )
		//	b->velocity		= Vector3r(0,0,0);
		//else
		//{

			b->se3.translation	+= sign*dt*velocity*translationAxis;

			b->velocity		=  sign*velocity*translationAxis;

		// FIXME : this shouldn't be there
			//b->updateBoundingVolume(b->se3);
		//}
	}


}
