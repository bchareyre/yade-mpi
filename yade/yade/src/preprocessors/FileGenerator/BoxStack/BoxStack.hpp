
#ifndef __BOXSTACK_H__
#define __BOXSTACK_H__

#include "Serializable.hpp"

class BoxStack : public Serializable
{
	// construction
	public : BoxStack ();
	public : ~BoxStack ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void exec();

	REGISTER_CLASS_NAME(RotatingBox);
};

REGISTER_SERIALIZABLE(BoxStack,false);

#endif // __BOXSTACK_H__

