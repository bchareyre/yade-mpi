#include "RotationEngine.hpp"
#include <yade-common/RigidBodyParameters.hpp>
#include <yade/MetaBody.hpp>

RotationEngine::RotationEngine()
{
	rotateAroundZero = false;
}

void RotationEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(angularVelocity);
	REGISTER_ATTRIBUTE(rotationAxis);
	REGISTER_ATTRIBUTE(rotateAroundZero);
}

void RotationEngine::applyCondition(Body * body)
{

	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	Real dt = Omega::instance().getTimeStep();
	time = dt;

	Quaternionr q;
	q.fromAxisAngle(rotationAxis,angularVelocity*dt);

	Vector3r ax;
	Real an;
	
	for(;ii!=iiEnd;++ii)
	{
		RigidBodyParameters * rb = static_cast<RigidBodyParameters*>((*bodies)[*ii]->physicalParameters.get());

		if(rotateAroundZero)
			rb->se3.position	= q*rb->se3.position; // for RotatingBox
			
		rb->se3.orientation	= q*rb->se3.orientation;

		rb->se3.orientation.normalize();
		rb->se3.orientation.toAxisAngle(ax,an);
		
		rb->angularVelocity	= rotationAxis*angularVelocity;
		rb->velocity		= Vector3r(0,0,0);
	}


}
