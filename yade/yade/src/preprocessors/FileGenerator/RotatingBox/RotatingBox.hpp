
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "Serializable.hpp"

class RotatingBox : public Serializable
{
	// construction
	public : RotatingBox ();
	public : ~RotatingBox ();

	public : void processAttributes();
	public : void registerAttributes();

	public : void exec();

	REGISTER_CLASS_NAME(RotatingBox);
};

REGISTER_SERIALIZABLE(RotatingBox,false);

#endif // __ROTATINGBOX_H__

