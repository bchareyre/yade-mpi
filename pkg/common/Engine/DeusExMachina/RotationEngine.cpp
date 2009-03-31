// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>

#include"RotationEngine.hpp"
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/extra/Shop.hpp>

#include<yade/pkg-common/LinearInterpolate.hpp>

YADE_PLUGIN("RotationEngine","SpiralEngine","InterpolatingSpiralEngine");


void InterpolatingSpiralEngine::applyCondition(MetaBody* rb){
	Real virtTime=wrap ? Shop::periodicWrap(rb->simulationTime,*times.begin(),*times.rbegin()) : rb->simulationTime;
	angularVelocity=linearInterpolate<Real>(virtTime,times,angularVelocities,_pos);
	linearVelocity=angularVelocity*slope;
	SpiralEngine::applyCondition(rb);
}

void SpiralEngine::applyCondition(MetaBody* rb){
	Real dt=Omega::instance().getTimeStep();
	axis.Normalize();
	Quaternionr q;
	q.FromAxisAngle(axis,angularVelocity*dt);
	angleTurned+=angularVelocity*dt;
	shared_ptr<BodyContainer> bodies = rb->bodies;
	FOREACH(body_id_t id,subscribedBodies){
		assert(id<bodies->size());
		Body* b=Body::byId(id,rb).get();
		ParticleParameters* rbp=YADE_CAST<RigidBodyParameters*>(b->physicalParameters.get());
		assert(rbp);
		// translation
		rbp->se3.position+=dt*linearVelocity*axis;
		// rotation
		rbp->se3.position=q*(rbp->se3.position-axisPt)+axisPt;
		rbp->se3.orientation=q*rbp->se3.orientation;
		rbp->se3.orientation.Normalize(); // to make sure
	}
}

RotationEngine::RotationEngine()
{
	rotateAroundZero = false;
	zeroPoint = Vector3r(0,0,0);
}


void RotationEngine::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(angularVelocity);
	REGISTER_ATTRIBUTE(rotationAxis);
	REGISTER_ATTRIBUTE(rotateAroundZero);
	REGISTER_ATTRIBUTE(zeroPoint);
}

void RotationEngine::applyCondition(MetaBody *ncb)
{
    rotationAxis.Normalize();

	shared_ptr<BodyContainer> bodies = ncb->bodies;

	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	Real dt = Omega::instance().getTimeStep();

	Quaternionr q;
	q.FromAxisAngle(rotationAxis,angularVelocity*dt);

	Vector3r ax;
	Real an;
	
	for(;ii!=iiEnd;++ii)
	{
		RigidBodyParameters * rb = static_cast<RigidBodyParameters*>((*bodies)[*ii]->physicalParameters.get());

		rb->angularVelocity	= rotationAxis*angularVelocity;

		if(rotateAroundZero)
        {
            const Vector3r l = rb->se3.position-zeroPoint;
			rb->se3.position	= q*l+zeroPoint; 
            rb->velocity		= rb->angularVelocity.Cross(l);
		}
			
		rb->se3.orientation	= q*rb->se3.orientation;
		rb->se3.orientation.Normalize();
		rb->se3.orientation.ToAxisAngle(ax,an);
		
	}


}

