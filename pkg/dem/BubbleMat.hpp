#pragma once

#include<core/Material.hpp>
#include<core/IPhys.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/common/Dispatching.hpp>

namespace py=boost::python;


/********************** BubbleMat ****************************/
class BubbleMat : public Material {
	public:
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(BubbleMat,Material,"material for bubble interactions, for use with other Bubble classes",
		((Real,surfaceTension,0.07197,,"The surface tension in the fluid surrounding the bubbles. The default value is that of water at 25 degrees Celcius."))
		,
		createIndex();
		density=1000;
	);
	REGISTER_CLASS_INDEX(BubbleMat,Material);
};
REGISTER_SERIALIZABLE(BubbleMat);


/********************** BubblePhys ****************************/
class BubblePhys : public IPhys {
	private:

	Real coeffA,coeffB; //Coefficents for artificial curve
  
	public:

	void computeCoeffs(Real pctMaxForce,Real surfaceTension, Real c1);
	static Real computeForce(Real separation, Real surfaceTension, Real rAvg, int newtonIter, Real newtonTol, Real c1, Real fN, BubblePhys* phys);

	virtual ~BubblePhys(){};
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(BubblePhys,IPhys,"Physics of bubble-bubble interactions, for use with BubbleMat",
		((Vector3r,normalForce,Vector3r::Zero(),,"Normal force"))
		((Real,surfaceTension,NaN,,"Surface tension of the surrounding liquid"))
		((Real,fN,NaN,,"Contact normal force"))
		((Real,rAvg,NaN,,"Average radius of the two interacting bubbles"))
		((Real,Dmax,NaN,,"Maximum penetrationDepth of the bubbles before the force displacement curve changes to an artificial exponential curve. Setting this value will have no effect. See Law2_ScGeom_BubblePhys_Bubble::pctMaxForce for more information"))
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
	private:
	  
	  Real c1; //Coeff used for many contacts
  
	public:
	virtual bool go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* interaction);
	FUNCTOR2D(GenericSpheresContact,BubblePhys);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_BubblePhys_Bubble,LawFunctor,"Constitutive law for Bubble model.",
		((Real,pctMaxForce,0.1,,"Chan[2011] states the contact law is valid only for small interferences; therefore an exponential force-displacement curve models the contact stiffness outside that regime (large penetration). This artificial stiffening ensures that bubbles will not pass through eachother or completely overlap during the simulation. The maximum force is Fmax = (2*pi*surfaceTension*rAvg). pctMaxForce is the percentage of the maximum force dictates the separation threshold, Dmax, for each contact. Penetrations less than Dmax calculate the reaction force from the derived contact law, while penetrations equal to or greater than Dmax calculate the reaction force from the artificial exponential curve."))
		((Real,surfaceTension,0.07197,,"The surface tension in the liquid surrounding the bubbles. The default value is that of water at 25 degrees Celcius."))
		,
		/*ctor*/,
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_BubblePhys_Bubble);
