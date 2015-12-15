#pragma once
#include<pkg/common/PFacet.hpp>
#include<pkg/common/GLDrawFunctors.hpp>
#include<pkg/common/Facet.hpp>
#include<core/Shape.hpp>

#ifdef YADE_OPENGL
class Gl1_PFacet : public GlShapeFunctor
{	
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	RENDERS(PFacet);
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_PFacet,GlShapeFunctor,"Renders :yref:`Facet` object",
	  ((bool,wire,false,,"Only show wireframe (controlled by ``glutSlices`` and ``glutStacks``."))
	);
};

REGISTER_SERIALIZABLE(Gl1_PFacet);
#endif


