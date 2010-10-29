#pragma once

#include<yade/core/GlobalEngine.hpp>

class Scene;
class ForceResetter: public GlobalEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC(ForceResetter,GlobalEngine,"Reset all forces stored in Scene::forces (``O.forces`` in python). Typically, this is the first engine to be run at every step. In addition, reset those energies that should be reset, if energy tracing is enabled.");
};
REGISTER_SERIALIZABLE(ForceResetter);


