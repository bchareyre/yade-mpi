#include<yade/pkg-common/GLDrawFunctors.hpp>
YADE_PLUGIN(
(GlBoundFunctor)(GlShapeFunctor)(GlInteractionGeometryFunctor)(GlInteractionPhysicsFunctor)(GlStateFunctor)
(GlBoundDispatcher)(GlShapeDispatcher)(GlInteractionGeometryDispatcher)(GlInteractionPhysicsDispatcher)(GlStateDispatcher)
);
YADE_REQUIRE_FEATURE(OPENGL);
