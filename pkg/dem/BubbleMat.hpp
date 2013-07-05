#pragma once

#include<yade/core/Material.hpp>
#include<yade/core/IPhys.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/common/Dispatching.hpp>

namespace py=boost::python;


/********************** BubbleMat ****************************/
class BubbleMat : public Material {
	public:
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(BubbleMat,Material,"material for bubble interactions, for use with other Bubble classes",
		((Real,surfaceTension,0.07197,,"The surface tension in the fluid surrounding the bubbles. The default value is that of water at 25 degrees Celcius."))
		,
		createIndex();
		density=1; // TODO density default value
	);
	REGISTER_CLASS_INDEX(BubbleMat,Material);
};
REGISTER_SERIALIZABLE(BubbleMat);


/********************** BubblePhys ****************************/
class BubblePhys : public IPhys {
	public:

	static Real computeForce(Real penetrationDepth, Real surfaceTension, Real rAvg, int newtonIter, Real newtonTol);

	virtual ~BubblePhys(){};
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(BubblePhys,IPhys,"Physics of bubble-bubble interactions, for use with BubbleMat",
		((Vector3r,normalForce,Vector3r::Zero(),,"Normal force"))
		((Real,surfaceTension,NaN,,"Surface tension of the surrounding liquid"))
		((Real,fN,NaN,,"Contact normal force"))
		((Real,rAvg,NaN,,"Average radius of the two interacting bubbles"))
		((Real,newtonIter,50,,"Maximum number of force iterations allowed"))
		((Real,newtonTol,1e-6,,"Convergence criteria for force iterations"))
		,
		createIndex();
		,
		.def("computeForce",&BubblePhys::computeForce,"Computes the normal force acting between the two interacting bubbles using the Newton-Rhapson method")
		.staticmethod("computeForce")
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(BubblePhys,IPhys);
};
REGISTER_SERIALIZABLE(BubblePhys);



/********************** Ip2_BubbleMat_BubbleMat_BubblePhys ****************************/
class Ip2_BubbleMat_BubbleMat_BubblePhys : public IPhysFunctor{
	public:
	virtual void go(const shared_ptr<Material>& m1, const shared_ptr<Material>& m2, const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(BubbleMat,BubbleMat);
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_BubbleMat_BubbleMat_BubblePhys,IPhysFunctor,"Generates bubble interactions.Used in the contact law Law2_ScGeom_BubblePhys_Bubble.",
	);
};
REGISTER_SERIALIZABLE(Ip2_BubbleMat_BubbleMat_BubblePhys);


/********************** Law2_ScGeom_BubblePhys_Bubble ****************************/
class Law2_ScGeom_BubblePhys_Bubble : public LawFunctor{
	public:
	void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* interaction);
	FUNCTOR2D(GenericSpheresContact,BubblePhys);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_BubblePhys_Bubble,LawFunctor,"Constitutive law for Bubble model.",
		,
		/*ctor*/,
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_BubblePhys_Bubble);
