#pragma once

#include<yade/core/Material.hpp>
#include<yade/core/IPhys.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/common/Dispatching.hpp>

namespace py=boost::python;


/********************** BubbleMat ****************************/
class BubbleMat : public Material {
	public:
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(BubbleMat,Material,"TODO DOC",
		((Real,surfaceTension,1/*TODO some realistic value*/,,"TODO DOC"))
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
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(BubblePhys,IPhys,"TODO DOC",
		((Vector3r,normalForce,Vector3r::Zero(),,"TODO DOC"))
		((Real,surfaceTension,NaN,,"TODO DOC"))
		((Real,fN,NaN,,"TODO DOC"))
		((Real,rAvg,NaN,,"TODO DOC"))
		((Real,newtonIter,50,,"TODO DOC"))
		((Real,newtonTol,1e-6,,"TODO DOC"))
		,
		createIndex();
		,
		.def("computeForce",&BubblePhys::computeForce,"TODO DOC")
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
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_BubbleMat_BubbleMat_BubblePhys,IPhysFunctor,"TODO DOC",
	);
};
REGISTER_SERIALIZABLE(Ip2_BubbleMat_BubbleMat_BubblePhys);


/********************** Law2_ScGeom_BubblePhys_Bubble ****************************/
class Law2_ScGeom_BubblePhys_Bubble : public LawFunctor{
	public:
	void go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* interaction);
	FUNCTOR2D(GenericSpheresContact,BubblePhys);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_BubblePhys_Bubble,LawFunctor,"TODO DOC",
		,
		/*ctor*/,
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_BubblePhys_Bubble);
