
#ifndef __BOXSTACK_H__
#define __BOXSTACK_H__

#include "FileGenerator.hpp"

class BoxStack : public FileGenerator
{
	// construction
	public : BoxStack ();
	public : ~BoxStack ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void generate();

	REGISTER_CLASS_NAME(RotatingBox);
};

REGISTER_SERIALIZABLE(BoxStack,false);

#endif // __BOXSTACK_H__

