
#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

#include "FileGenerator.hpp"


class HangingCloth : public FileGenerator
{
	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(HangingCloth);
};

REGISTER_SERIALIZABLE(HangingCloth,false);

#endif // __HANGINGCLOTH_H__

