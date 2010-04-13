#include<yade/core/GlobalEngine.hpp>
class BoundaryController: public GlobalEngine{
	virtual void action();
	YADE_CLASS_BASE_DOC(BoundaryController,GlobalEngine,"Base for engines controlling boundary conditions of simulations. Not to be used directly.");
};
REGISTER_SERIALIZABLE(BoundaryController);
