
#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

#include "FileGenerator.hpp"


class HangingCloth : public FileGenerator
{
	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void generate();

	REGISTER_CLASS_NAME(HangingCloth);
};

REGISTER_SERIALIZABLE(HangingCloth,false);

#endif // __HANGINGCLOTH_H__

