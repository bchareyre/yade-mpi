#include<yade/pkg-common/GLDrawFunctors.hpp>
YADE_PLUGIN(
(GlBoundFunctor)(GlShapeFunctor)(GlIGeomFunctor)(GlIPhysFunctor)(GlStateFunctor)
(GlBoundDispatcher)(GlShapeDispatcher)(GlIGeomDispatcher)(GlIPhysDispatcher)(GlStateDispatcher)
);
YADE_REQUIRE_FEATURE(OPENGL);
