// 2004 © Janek Kozicki <cosurgi@berlios.de>
// 2007,2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<yade/pkg-common/FieldApplier.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/Body.hpp>
#ifdef YADE_OPENMP
	#include<omp.h>
#endif

/*! Homogeneous gravity field; applies gravity×mass force on all bodies. */
class GravityEngine: public FieldApplier{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(GravityEngine,FieldApplier,"Engine applying constant acceleration to all bodies.",
		((Vector3r,gravity,Vector3r::Zero(),"Acceleration [kgms⁻²]"))
		,/*ctor*/,/*py*/
	);
};
REGISTER_SERIALIZABLE(GravityEngine);


/*! Engine attracting all bodies towards a central body (doesn't depend on distance);
 *
 * @todo This code has not been yet tested at all.
 */
class CentralGravityEngine: public FieldApplier {
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(CentralGravityEngine,FieldApplier,"Engine applying acceleration to all bodies, towards a central body.",
		((Body::id_t,centralBody,Body::ID_NONE,"The :yref:`body<Body>` towards which all other bodies are attracted."))
		((Real,accel,0,"Acceleration magnitude [kgms⁻²]"))
		((bool,reciprocal,false,"If true, acceleration will be applied on the central body as well."))
		,,
	);
};
REGISTER_SERIALIZABLE(CentralGravityEngine);

/*! Apply acceleration (independent of distance) directed towards an axis.
 *
 */
class AxialGravityEngine: public FieldApplier {
	public:
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(AxialGravityEngine,FieldApplier,"Apply acceleration (independent of distance) directed towards an axis.",
		((Vector3r,axisPoint,Vector3r::Zero(),"Point through which the axis is passing."))
		((Vector3r,axisDirection,Vector3r::UnitX(),"direction of the gravity axis (will be normalized automatically)"))
		((Real,acceleration,0,"Acceleration magnitude [kgms⁻²]"))
	);
};
REGISTER_SERIALIZABLE(AxialGravityEngine);

