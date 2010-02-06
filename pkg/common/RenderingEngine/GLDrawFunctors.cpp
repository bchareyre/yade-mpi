#include<yade/pkg-common/GLDrawFunctors.hpp>
YADE_PLUGIN((GlBoundFunctor)(GlShapeFunctor)(GlInteractionGeometryFunctor)(GlInteractionPhysicsFunctor)(GlStateFunctor));
YADE_REQUIRE_FEATURE(OPENGL);
