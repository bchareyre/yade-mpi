// Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/core/DeusExMachina.hpp>
#include<yade/pkg-common/Force.hpp>

/* Homogeneous gravity field; applies gravity*mass force on all bodies.
 *
 * @bug is a DeusExMachina, but doesn't care about subscribedBodies.
 */
class GravityEngine : public DeusExMachina{
		int cachedForceClassIndex;
	public :
		Vector3r gravity;
		GravityEngine(): gravity(Vector3r::ZERO){shared_ptr<Force> f(new Force); cachedForceClassIndex=f->getClassIndex();};
		virtual ~GravityEngine(){};
		virtual void applyCondition(MetaBody*);
	protected :
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(gravity);}
	NEEDS_BEX("Force");
	REGISTER_CLASS_NAME(GravityEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(GravityEngine,false);


/* Engine attracting all bodies towards a central body (doesn't depend on distance);
 *
 * @todo This code has not been yet tested at all.
 * @bug is a DeusExMachina, but doesn't care about subscribedBodies.
 */
class CentralGravityEngine: public DeusExMachina {
	private:
		int cachedForceClassIndex;
	public:
		//! The body towards which all other bodies are attracted.
		body_id_t centralBody;
		//! acceleration towards the central body
		Real accel;
		//! Whether to apply reciprocal force to the central body as well
		bool reciprocal;
		CentralGravityEngine(){ shared_ptr<Force> f(new Force); cachedForceClassIndex=f->getClassIndex(); reciprocal=false; }
		virtual ~CentralGravityEngine(){};
		virtual void applyCondition(MetaBody*);
	protected:
		virtual void registerAttributes(){REGISTER_ATTRIBUTE(centralBody); REGISTER_ATTRIBUTE(accel); REGISTER_ATTRIBUTE(reciprocal); }
		NEEDS_BEX("Force");
		REGISTER_CLASS_NAME(CentralGravityEngine);
		REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(CentralGravityEngine,false);

/*! Apply acceleration (independent of distance) directed towards an axis.
 *
 * @bug is a DeusExMachina, but doesn't care about subscribedBodies.
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
};
REGISTER_SERIALIZABLE(AxialGravityEngine,false);

