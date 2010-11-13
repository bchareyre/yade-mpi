#pragma once
#include<yade/core/GlobalEngine.hpp>
class FieldApplier: public GlobalEngine{
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(FieldApplier,GlobalEngine,"Base for engines applying force files on particles. Not to be used directly.",
		((int,fieldWorkIx,-1,(Attr::hidden|Attr::noSave),"Index for the work done by this field, if tracking energies."))
	);
};
REGISTER_SERIALIZABLE(FieldApplier);

