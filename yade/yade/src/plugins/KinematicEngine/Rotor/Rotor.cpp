#include "Rotor.hpp"
#include "RigidBody.hpp"
	
Rotor::Rotor () : KinematicEngine() // encapsuler dans implicitfunction user redefini uniquement dp = || interpolateur ...
{
}

Rotor::~Rotor ()
{

}

void Rotor::processAttributes()
{

}

void Rotor::registerAttributes()
{
	KinematicEngine::registerAttributes();
}

void Rotor::moveToNextTimeStep(std::vector<shared_ptr<Body> >& bodies, float dt)
{
	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();
	
	time = dt;
		
	Quaternion q;
	Vector3 axis = Vector3(1,0,0);
	float angularVelocity = Constants::PI/40.0;
	q.fromAngleAxis(angularVelocity*dt,axis);

	Vector3 ax;
	float an;
	
	for(;ii!=iiEnd;++ii)
	{
		shared_ptr<Body>  b = bodies[(*ii)];
		
		//b->se3.translation += dp;

		//cout.precision(40);
		//q.x=0;
				
		//cout << b->se3.translation << endl;
		//cout << b->se3.rotation << endl;
		
		b->se3.translation = q*b->se3.translation;
		b->se3.rotation = q*b->se3.rotation;

		b->se3.rotation.normalize();
		b->se3.rotation.toAngleAxis(an,ax);
		//cout << b->se3.rotation << " - " << an << " " << ax << endl;
		
		b->angularVelocity = axis*angularVelocity;
		b->velocity = Vector3(0,0,0);//dp*1/dt;
		
		b->updateBoundingVolume(b->se3);
	}
	
	
}
