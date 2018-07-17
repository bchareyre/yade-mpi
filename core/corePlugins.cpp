#include<lib/factory/ClassFactory.hpp>
// make core classes known to the class factory
#include<core/Body.hpp>
#include<core/BodyContainer.hpp>
#include<core/Bound.hpp>
#include<core/Cell.hpp>
#include<core/Dispatcher.hpp>
#include<core/EnergyTracker.hpp>
#include<core/Engine.hpp>
#include<core/FileGenerator.hpp>
#include<core/Functor.hpp>
#include<core/GlobalEngine.hpp>
#include<core/Interaction.hpp>
#include<core/InteractionContainer.hpp>
#include<core/IGeom.hpp>
#include<core/IPhys.hpp>
#include<core/Material.hpp>
#include<core/PartialEngine.hpp>
#include<core/Shape.hpp>
#include<core/State.hpp>
#include<core/TimeStepper.hpp>


// these two are not accessible from python directly (though they should be in the future, perhaps)

// BOOST_CLASS_EXPORT_IMPLEMENT(BodyContainer);
BOOST_CLASS_EXPORT_IMPLEMENT(InteractionContainer);

YADE_PLUGIN((Body)(Bound)(Cell)(Dispatcher)(EnergyTracker)(Engine)(FileGenerator)(Functor)(GlobalEngine)(Interaction)(IGeom)(IPhys)(Material)(PartialEngine)(Shape)(State)(TimeStepper));

EnergyTracker::~EnergyTracker(){}
