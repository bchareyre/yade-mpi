
#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

#include "Serializable.hpp"


class HangingCloth : public Serializable
{
	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	public : void processAttributes();
	public : void registerAttributes();

	public : void exec();

	REGISTER_CLASS_NAME(HangingCloth);
};

REGISTER_SERIALIZABLE(HangingCloth,false);

#endif // __HANGINGCLOTH_H__

