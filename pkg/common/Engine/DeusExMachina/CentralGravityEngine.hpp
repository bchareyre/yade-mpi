#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<yade/pkg-common/Force.hpp>

/* Engine attracting all bodies towards a central body, using Newton's gravity law.
 *
 * http://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation.
 * Unlike in real gravity, there are no forces exerted on the central body.
 * Should there be? This way, we don't have energy conservation.
 *
 * @todo This code has not been yet tested at all.
 *
 */

class CentralGravityEngine: public DeusExMachina {
	private:
		int cachedForceClassIndex;
	public:
		//! The body towards which all other bodies are attracted.
		body_id_t centralBody;
		//! The gravity constant. Must be big enough to have realistic influence when masses are small.
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

