#ifndef __MASSPSRINGBODY_H__
#define __MASSPSRINGBODY_H__

#include "ComplexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

class MassSpringBody : public ComplexBody
{
	// construction
	public : MassSpringBody ();
	public : virtual ~MassSpringBody ();

	//public : void glDrawGeometricalModel();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(MassSpringBody);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

//	REGISTER_CLASS_INDEX(MassSpringBody,ComplexBody);
	
};

REGISTER_SERIALIZABLE(MassSpringBody,false);

#endif // __MASSPSRINGBODY_H__
