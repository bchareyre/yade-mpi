// 2004 © Janek Kozicki <cosurgi@berlios.de>
// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Interaction.hpp>

/*! Homogeneous gravity field; applies gravity×mass force on all bodies. */
class GravityEngine: public GlobalEngine{
	public:
		//! gravity constant
		Vector3r gravity;
		GravityEngine(): gravity(Vector3r::ZERO){};
		virtual ~GravityEngine(){};
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(GravityEngine,GlobalEngine,"Engine applying constant acceleration to all bodies.",
		((gravity,"Acceleration [kgms⁻²]"))
	);
};
REGISTER_SERIALIZABLE(GravityEngine);


/*! Engine attracting all bodies towards a central body (doesn't depend on distance);
 *
 * @todo This code has not been yet tested at all.
 */
class CentralGravityEngine: public GlobalEngine {
	public:
		//! The body towards which all other bodies are attracted.
		body_id_t centralBody;
		//! acceleration towards the central body
		Real accel;
		//! Whether to apply reciprocal force to the central body as well
		bool reciprocal;
		CentralGravityEngine(){ reciprocal=false; }
		virtual ~CentralGravityEngine(){};
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(CentralGravityEngine,GlobalEngine,"Engine applying acceleration to all bodies, towards a central body.",
		((centralBody,"The body towards which all other bodies are attracted"))
		((accel,"Acceleration magnitude [kgms⁻²]"))
		((reciprocal,"If true, acceleration will be applied on the central body as well (default: false)."))
	);
};
REGISTER_SERIALIZABLE(CentralGravityEngine);

/*! Apply acceleration (independent of distance) directed towards an axis.
 *
 */
class AxialGravityEngine: public GlobalEngine {
	public:
		//! point through which the axis is passing
		Vector3r axisPoint;
		//! direction of the gravity axis (may not be normalized)
		Vector3r axisDirection;
		//! magnitude of acceleration that will be applied
		Real acceleration;
		AxialGravityEngine(){ }
		virtual ~AxialGravityEngine(){};
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(AxialGravityEngine,GlobalEngine,"Apply acceleration (independent of distance) directed towards an axis.",
		((axisPoint,"Point through which the axis is passing."))
		((axisDirection,"direction of the gravity axis (will be normalized automatically)"))
		((acceleration,"Acceleration magnitude [kgms⁻²]"))
	);
};
REGISTER_SERIALIZABLE(AxialGravityEngine);

