 
#ifndef __SIMPLESCENE_H__
#define __SIMPLESCENE_H__

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

REGISTER_CLASS(RotatingBox,false);

#endif // __SIMPLESCENE_H__
 
