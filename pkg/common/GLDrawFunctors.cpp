#include<yade/pkg/common/GLDrawFunctors.hpp>
#ifdef YADE_OPENGL
	YADE_PLUGIN(
		(GlBoundFunctor)(GlShapeFunctor)(GlIGeomFunctor)(GlIPhysFunctor)(GlStateFunctor)
		(GlBoundDispatcher)(GlShapeDispatcher)(GlIGeomDispatcher)(GlIPhysDispatcher)(GlStateDispatcher)
	);
#endif
