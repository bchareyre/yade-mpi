#include"NormShearPhys.hpp"
YADE_PLUGIN((NormPhys)(NormShearPhys));
/* At least one virtual function must be in the shared object; let's put empty desctructors here
 * Otherwise downcasting via dynamic_cast will not work (no vtable in the shared lib?)
 */
NormPhys::~NormPhys(){};
NormShearPhys::~NormShearPhys(){};

