
#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

#include "FileGenerator.hpp"


class HangingCloth : public FileGenerator
{
	private : int width;
	private : int height;
	private : Real stiffness ;
	private : Real springDamping ;	
	private : Real particleDamping ;
	private : Real mass;
	private : int cellSize;
	private : shared_ptr<Interaction>  spring;
	private : bool fixPoint1;
	private : bool fixPoint2;
	private : bool fixPoint3;
	private : bool fixPoint4;
	
	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
	private : shared_ptr<Interaction>& createSpring(const shared_ptr<ComplexBody>& rootBody,int i,int j);
	
	public : string generate();

	REGISTER_CLASS_NAME(HangingCloth);
};

REGISTER_SERIALIZABLE(HangingCloth,false);

#endif // __HANGINGCLOTH_H__

