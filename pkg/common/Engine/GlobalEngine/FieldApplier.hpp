#pragma once
#include<yade/core/GlobalEngine.hpp>
class FieldApplier: public GlobalEngine{
	virtual void action();
	YADE_CLASS_BASE_DOC(FieldApplier,GlobalEngine,"Base for engines controlling boundary conditions of simulations. Not to be used directly.");
};
REGISTER_SERIALIZABLE(FieldApplier);

