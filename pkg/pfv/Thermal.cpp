
#define THERMAL 
#ifdef THERMAL
#include<pkg/pfv/Thermal.hpp>
 
YADE_PLUGIN((ThermalState));
ThermalState::~ThermalState(){};

#endif//THERMAL