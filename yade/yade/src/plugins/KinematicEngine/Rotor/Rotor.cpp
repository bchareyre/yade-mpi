#include "Rotor.hpp"
#include "RigidBody.hpp"
#include "NonConnexBody.hpp"

Rotor::Rotor () : KinematicEngine() // encapsuler dans implicitfunction user redefini uniquement dp = || interpolateur ...
{
}

Rotor::~Rotor ()
{

}

void Rotor::postProcessAttributes(bool)
{

}

void Rotor::registerAttributes()
{
	KinematicEngine::registerAttributes();
	REGISTER_ATTRIBUTE(angularVelocity);
	REGISTER_ATTRIBUTE(rotationAxis);
}

void Rotor::moveToNextTimeStep(Body * body)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	float dt = Omega::instance().getTimeStep();
	time = dt;

	Quaternionr q;
	q.fromAxisAngle(rotationAxis,angularVelocity*dt);

	Vector3r ax;
	Real an;
	
	for(;ii!=iiEnd;++ii)
	{
		shared_ptr<Body>  b = (*bodies)[*ii];

		//b->se3.translation += dp;

		b->se3.translation	= q*b->se3.translation;
		//cout << "  ## " << endl << b->se3.rotation.w() <<  " " << b->se3.rotation.x() << " " <<  b->se3.rotation.y() <<   " " <<  b->se3.rotation.z() << endl;
		b->se3.rotation		= q*b->se3.rotation;
		//cout << endl << b->se3.rotation.w() <<  " " << b->se3.rotation.x() << " " <<  b->se3.rotation.y() <<   " " <<  b->se3.rotation.z() << endl;

		b->se3.rotation.normalize();
		b->se3.rotation.toAxisAngle(ax,an);

		b->angularVelocity	= rotationAxis*angularVelocity;
		b->velocity		= Vector3r(0,0,0);

		// FIXME : this shouldn't be there
		//b->updateBoundingVolume(b->se3);
	}


}
