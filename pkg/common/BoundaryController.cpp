#include<yade/pkg-common/BoundaryController.hpp>
#include<stdexcept>
void BoundaryController::action(){ throw std::runtime_error("BoundaryController must not be used in simulations directly (BoundaryController::action called)."); }
YADE_PLUGIN((BoundaryController));
