// 2004 © Janek Kozicki <cosurgi@berlios.de> 
// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once

#include<yade/core/PartialEngine.hpp>

class ForceEngine : public PartialEngine{
	public :
		Vector3r force;
		ForceEngine(): force(Vector3r::ZERO){};
		virtual ~ForceEngine(){};
		virtual void applyCondition(Scene*);
	REGISTER_CLASS_AND_BASE(ForceEngine,PartialEngine);
	REGISTER_ATTRIBUTES(PartialEngine,(force));
};
REGISTER_SERIALIZABLE(ForceEngine);

/* Engine for applying force of varying magnitude but constant direction
 * on subscribed bodies. times and magnitudes must have the same length,
 * direction (normalized automatically) gives the orientation.
 *
 * As usual with interpolating engines: the first magnitude is used before the first
 * time point, last magnitude is used after the last time point. Wrap specifies whether
 * time wraps around the last time point to the first time point.
 */
class InterpolatingDirectedForceEngine: public ForceEngine{
	size_t _pos;
	public:
		//! Time readings
		vector<Real> times;
		//! Force magnitude readings
		vector<Real> magnitudes;
		//! Constant force direction (normalized automatically)
		Vector3r direction;
		//! wrap to the beginning of the sequence if beyond the last time point
		bool wrap;
		InterpolatingDirectedForceEngine(): _pos(0),direction(Vector3r::UNIT_X),wrap(false){};
		virtual ~InterpolatingDirectedForceEngine(){};
		virtual void applyCondition(Scene*);
	REGISTER_CLASS_AND_BASE(InterpolatingDirectedForceEngine,ForceEngine);
	REGISTER_ATTRIBUTES(ForceEngine,(times)(magnitudes)(direction)(wrap));
};
REGISTER_SERIALIZABLE(InterpolatingDirectedForceEngine);


