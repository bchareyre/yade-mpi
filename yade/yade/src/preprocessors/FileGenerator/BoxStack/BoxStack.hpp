
#ifndef __BOXSTACK_H__
#define __BOXSTACK_H__

#include "FileGenerator.hpp"

class BoxStack : public FileGenerator
{
	// construction
	public : BoxStack ();
	public : ~BoxStack ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(BoxStack);
};

REGISTER_SERIALIZABLE(BoxStack,false);

#endif // __BOXSTACK_H__

