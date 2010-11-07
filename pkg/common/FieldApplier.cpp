#include<yade/pkg/common/FieldApplier.hpp>
#include<stdexcept>
void FieldApplier::action(){ throw std::runtime_error("FieldApplier must not be used in simulations directly (FieldApplier::action called)."); }
YADE_PLUGIN((FieldApplier));

