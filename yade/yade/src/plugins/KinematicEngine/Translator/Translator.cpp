#include "Translator.hpp"
//#include "RigidBody.hpp"
#include "Constants.hpp"
#include "NonConnexBody.hpp"

Translator::Translator () : KinematicEngine() // encapsuler dans implicitfunction user redefini uniquement dp = || interpolateur ...
{
}

Translator::~Translator ()
{

}

void Translator::processAttributes()
{
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
// check this to see wrong behaviour
//	if( Omega::instance().getIter() > 1000 )
//		return;

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	float dt = Omega::instance().dt;
	time = dt;


	for(;ii!=iiEnd;++ii)
	{
		shared_ptr<Body>  b = bodies[(*ii)];


		b->se3.translation	+= dt*velocity*translationAxis;

		b->velocity		= translationAxis*velocity;

		// FIXME : this shouldn't be there
		b->updateBoundingVolume(b->se3);
	}


}
