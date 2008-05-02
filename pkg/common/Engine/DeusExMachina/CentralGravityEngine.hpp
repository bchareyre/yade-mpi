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
		virtual void applyCondition(MetaBody*);
	protected:
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(centralBody); REGISTER_ATTRIBUTE(kappa);}
		NEEDS_BEX("Force");
		REGISTER_CLASS_NAME(CentralGravityEngine);
		REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(CentralGravityEngine,false);

/*! Apply acceleration (independent of distance) directed towards an axis.
 *
 */
class AxialGravityEngine: public DeusExMachina {
	private:
		int cachedForceClassIndex;
	public:
		//! point through which the axis is passing
		Vector3r axisPoint;
		//! direction of the gravity axis (may not be normalized)
		Vector3r axisDirection;
		//! magnitude of acceleration that will be applied
		Real acceleration;
		AxialGravityEngine(){ shared_ptr<Force> f(new Force); cachedForceClassIndex=f->getClassIndex(); }
		virtual ~AxialGravityEngine(){};
		virtual void applyCondition(MetaBody*);
	protected:
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(axisPoint); REGISTER_ATTRIBUTE(axisDirection); REGISTER_ATTRIBUTE(acceleration); }
		NEEDS_BEX("Force");
		REGISTER_CLASS_NAME(AxialGravityEngine);
		REGISTER_BASE_CLASS_NAME(DeusExMachina);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(AxialGravityEngine,false);

