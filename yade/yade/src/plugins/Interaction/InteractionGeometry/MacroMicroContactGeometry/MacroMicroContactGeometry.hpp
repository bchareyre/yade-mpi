#ifndef __SDECCONTACTMODEL_H__
#define __SDECCONTACTMODEL_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"


class MacroMicroContactGeometry : public InteractionGeometry
{
	public : virtual ~MacroMicroContactGeometry();

	public : Real radius1;
	public : Real radius2;
	public : Vector3r normal;			// new unit normal of the contact plane.
	public : Real penetrationDepth;
	public : Vector3r contactPoint;	
	
	REGISTER_CLASS_NAME(MacroMicroContactGeometry);
};

REGISTER_SERIALIZABLE(MacroMicroContactGeometry,false);

#endif // __SDECCONTACTMODEL_H__
