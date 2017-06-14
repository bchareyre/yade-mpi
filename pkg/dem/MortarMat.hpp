// 2016 © Jan Stránský <jan.stransky@fsv.cvut.cz> 

#pragma once

#include<pkg/common/ElastMat.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/PeriodicEngines.hpp>
#include<pkg/common/NormShearPhys.hpp>
#include<pkg/dem/DemXDofGeom.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>

namespace py=boost::python;



class MortarMat: public FrictMat {
	public:
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(MortarMat,FrictMat,"Material for mortar interface, used in Ip2_MortarMat_MortarMat_MortarPhys and Law2_ScGeom_MortarPhys_Lourenco. Default values according to ",
			((Real,young,1e9,,"Normal elastic modulus [Pa]"))
			((Real,poisson,1,,"Shear to normal modulus ratio"))
			((Real,frictionAngle,.25,,"Friction angle"))
			//
			((Real,tensileStrength,1e6,,"tensileStrength [Pa]"))
			((Real,compressiveStrength,10e6,,"compressiveStrength [Pa]"))
			((Real,cohesion,1e6,,"cohesion [Pa]"))
			((Real,ellAspect,3,,"aspect ratio of elliptical 'cap'. Value >1 means the ellipse is longer along normal stress axis."))
			((bool,neverDamage,false,,"If true, interactions remain elastic regardless stresses"))
			,
			createIndex();
		);
		REGISTER_CLASS_INDEX(MortarMat,FrictMat);
};
REGISTER_SERIALIZABLE(MortarMat);






class MortarPhys: public FrictPhys {
	public:
		Real sigmaN;
		Vector3r sigmaT;
		virtual ~MortarPhys();
		bool failureCondition(Real sigmaN, Real sigmaT);
		YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(MortarPhys,FrictPhys,"IPhys class containing parameters of MortarMat. Used by Law2_ScGeom_MortarPhys_Lourenco.",
			((Real,tensileStrength,NaN,,"tensileStrength [Pa]"))
			((Real,compressiveStrength,NaN,,"compressiveStrength [Pa]"))
			((Real,cohesion,NaN,,"cohesion [Pa]"))
			((Real,ellAspect,NaN,,"aspect ratio of elliptical 'cap'. Value >1 means the ellipse is longer along normal stress axis."))
			((Real,crossSection,NaN,,"Crosssection of interaction"))
			((bool,neverDamage,false,,"If true, interactions remain elastic regardless stresses"))
			, // ctors
			createIndex();
			,
			.def_readonly("sigmaN",&MortarPhys::sigmaN,"Current normal stress |yupdate|")
			.def_readonly("sigmaT",&MortarPhys::sigmaT,"Current shear stress |yupdate|")
			.def("failureCondition",&MortarPhys::failureCondition,"Failure condition from normal stress and norm of shear stress (false=elastic, true=damaged)")
		);
		DECLARE_LOGGER;
		REGISTER_CLASS_INDEX(MortarPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(MortarPhys);





class Ip2_MortarMat_MortarMat_MortarPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
		FUNCTOR2D(MortarMat,MortarMat);
		DECLARE_LOGGER;
		YADE_CLASS_BASE_DOC_ATTRS(Ip2_MortarMat_MortarMat_MortarPhys,IPhysFunctor,"Ip2 creating MortarPhys from two MortarMat instances.",
			((long,cohesiveThresholdIter,2,,"Should new contacts be cohesive? They will before this iter#, they will not be afterwards. If <=0, they will never be."))
		);
};
REGISTER_SERIALIZABLE(Ip2_MortarMat_MortarMat_MortarPhys);



class Law2_ScGeom_MortarPhys_Lourenco: public LawFunctor{
	public:
		virtual bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
		FUNCTOR2D(GenericSpheresContact,MortarPhys);
		YADE_CLASS_BASE_DOC(Law2_ScGeom_MortarPhys_Lourenco,LawFunctor,"Material law for mortar layer according to [Lourenco1994]_. The contact behaves elastic until brittle failure when reaching strength envelope. The envelope has three parts.\n\nTensile with condition $\\sigma_N-f_t$.\n\nShear part with Mohr-Coulomb condition $|\\sigma_T|+\\sigma_N\\tan\\varphi-c$.\n\nCompressive part with condition $\\sigma_N^2+A^2\\sigma_T^2-f_c^2$\n\nThe main idea is to begin simulation with this model and when the contact is broken, to use standard non-cohesive Law2_PolyhedraGeom_PolyhedraPhys_Volumetric."
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_MortarPhys_Lourenco);
