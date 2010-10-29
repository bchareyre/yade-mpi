#include<yade/lib-factory/ClassFactory.hpp>
// make core classes known to the class factory
#include<yade/core/Body.hpp>
#include<yade/core/BodyContainer.hpp>
#include<yade/core/Bound.hpp>
#include<yade/core/Cell.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/core/EnergyTracker.hpp>
#include<yade/core/Engine.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/InteractionContainer.hpp>
#include<yade/core/IGeom.hpp>
#include<yade/core/IPhys.hpp>
#include<yade/core/Material.hpp>
#include<yade/core/PartialEngine.hpp>
#include<yade/core/Shape.hpp>
#include<yade/core/State.hpp>
#include<yade/core/TimeStepper.hpp>

// these two are not accessible from python directly (though they should be in the future, perhaps)
BOOST_CLASS_EXPORT_IMPLEMENT(BodyContainer);
BOOST_CLASS_EXPORT_IMPLEMENT(InteractionContainer);

YADE_PLUGIN((Body)(Bound)(Cell)(Dispatcher)(EnergyTracker)(Engine)(FileGenerator)(Functor)(GlobalEngine)(Interaction)(IGeom)(IPhys)(Material)(PartialEngine)(Shape)(State)(TimeStepper));

EnergyTracker::~EnergyTracker(){} // vtable

//BOOST_CLASS_EXPORT(OpenMPArrayAccumulator<Real>);
//BOOST_CLASS_EXPORT(OpenMPAccumulator<Real>);
