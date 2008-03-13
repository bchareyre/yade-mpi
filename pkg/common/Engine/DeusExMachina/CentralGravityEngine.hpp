#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<yade/pkg-common/Force.hpp>

/* Engine attracting all bodies towards a central body, using Newton's gravity law.
 *
 */

class CentralGravityEngine: public DeusExMachina {
	private:
		int cachedForceClassIndex;
	public:
		body_id_t centralBody;
		Real kappa;
		CentralGravityEngine(){ shared_ptr<Force> f(new Force); cachedForceClassIndex=f->getClassIndex(); }
		virtual ~CentralGravityEngine(){};
		virtual void applyCondition(Body*);
	protected:
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(centralBody); REGISTER_ATTRIBUTE(kappa);}
		REGISTER_CLASS_NAME(CentralGravityEngine);
		REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(CentralGravityEngine,false);

