
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "FileGenerator.hpp"

class RotatingBox : public FileGenerator
{
	private : int nbSpheres;
	private : int nbBoxes;
	
	// construction
	public : RotatingBox ();
	public : ~RotatingBox ();

	public : virtual void registerAttributes();
	public : virtual string generate();

	REGISTER_CLASS_NAME(RotatingBox);
};

REGISTER_SERIALIZABLE(RotatingBox,false);

#endif // __ROTATINGBOX_H__

