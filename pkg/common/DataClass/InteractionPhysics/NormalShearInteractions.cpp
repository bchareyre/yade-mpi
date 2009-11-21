#include"NormalShearInteractions.hpp"
YADE_PLUGIN((NormalInteraction)(NormalShearInteraction));
/* At least one virtual function must be in the shared object; let's put empty desctructors here
 * Otherwise downcasting via dynamic_cast will not work (no vtable in the shared lib?)
 */
NormalInteraction::~NormalInteraction(){};
NormalShearInteraction::~NormalShearInteraction(){};

