// 2017 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#pragma once

#include<pkg/common/ElastMat.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/PeriodicEngines.hpp>
#include<pkg/common/NormShearPhys.hpp>
#include<pkg/dem/DemXDofGeom.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/dem/ElasticContactLaw.hpp>
#include<pkg/dem/ViscoelasticPM.hpp>

namespace py=boost::python;


class LubricationPhys: public ViscElPhys {
        public:
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(LubricationPhys,ViscElPhys,"IPhys class for Lubrication w/o FlowEngine. Used by Law2_ScGeom_ImplicitLubricationPhys.",
                ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                ((Real,eps,0.001,,"Rugosity: fraction of radius used as rugosity [-]"))
                ((Real,kno,0.0,,"Coeficient for normal stiffness"))
                ((Real,kso,0.0,,"Coeficient for tangeancial stiffness"))
                ((Real,nun,0.0,,"Normal viscosity coefficient"))
                ((Real,mum,0.3,,"Friction coefficient [-]"))
                ((Real,ue,0.,,"Surface deflection at t-dt [m]"))
                ((Real,u,-1,,"u at t-dt [m]"))
		((Real,prev_un,0,,"un at t-dt [m]"))
		((Real,prevDotU,0,Attr::readonly,"nu/k*du/dt from previous integration - used for trapezoidal scheme (:yref:`LubricationPhys::trapezoidalScheme`)"))
//                ((Real,due,0.,,"Last increment of ue"))
//                ((Real,dtm,0.,,"dt optim."))
//                ((Real,delta,0,,"exponantial solution"))
                ((bool,contact,false,,"Spheres in contact"))
                ((bool,slip,false,,"Slip condition"))
		((Vector3r,normalContactForce,Vector3r::Zero(),,"Normal contact force"))
		((Vector3r,shearContactForce,Vector3r::Zero(),,"Frictional contact force"))
		((Vector3r,normalLubricationForce,Vector3r::Zero(),,"Normal lubrication force"))
		((Vector3r,shearLubricationForce,Vector3r::Zero(),,"Shear lubrication force"))
                , // ctors
                createIndex();,
                      .def_readonly("eta",&LubricationPhys::eta,"Fluid viscosity [Pa.s]")
                      .def_readonly("eps",&LubricationPhys::eps,"Rugosity [-]")
                      .def_readonly("ue",&LubricationPhys::ue,"Surface deflection [m]")
                      .def_readonly("normalContactForce",&LubricationPhys::normalContactForce,"Normal component of contact force [N]")
                      .def_readonly("shearContactForce",&LubricationPhys::shearContactForce,"Shear component of contact force [N]")
                      .def_readonly("normalLubricationForce",&LubricationPhys::normalLubricationForce,"Normal component of lubrication force [N]")
                      .def_readonly("shearLubricationForce",&LubricationPhys::shearLubricationForce,"Normal component of lubrication force [N]")
//                      .def_readonly("ue2",&LubricationPhys::ue2,"Rejected solution of ue [m]")
                      .def_readonly("contact",&LubricationPhys::contact,"Spheres in contact")
                      .def_readonly("slip",&LubricationPhys::slip,"Slip contact")
//                      .def_readonly("dtm",&LubricationPhys::dtm,"Dtm")
                );
                DECLARE_LOGGER;
                REGISTER_CLASS_INDEX(LubricationPhys,ViscElPhys);
};
REGISTER_SERIALIZABLE(LubricationPhys);


class Ip2_FrictMat_FrictMat_LubricationPhys: public IPhysFunctor{
        public:
                virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
                FUNCTOR2D(FrictMat,FrictMat);
                DECLARE_LOGGER;
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Ip2_FrictMat_FrictMat_LubricationPhys,IPhysFunctor,"Ip2 creating LubricationPhys from two Material instances.",
                        ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                        ((Real,eps,0.001,,"Rugosity: fraction of radius used as rugosity"))
                                                  ,,
                );
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_LubricationPhys);


class Law2_ScGeom_ImplicitLubricationPhys: public LawFunctor{
        public:
                bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
                FUNCTOR2D(GenericSpheresContact,LubricationPhys);
                static void getStressForEachBody(vector<Matrix3r>& NCStresses, vector<Matrix3r>& SCStresses, vector<Matrix3r>& NLStresses, vector<Matrix3r>& SLStresses );
                static py::tuple PyGetStressForEachBody();
                static void getTotalStresses(Matrix3r& NCStresses, Matrix3r& SCStresses, Matrix3r& NLStresses, Matrix3r& SLStresses);
                static py::tuple PyGetTotalStresses();
		
				// integration of the gap by implicit trapezoidal rule, adaptative sub-stepping is used if solutionless
				// prevDotU is modified after execution (and ready for next step)
				Real normalForce_trapezoidalScheme(Real const& un, LubricationPhys* phys, ScGeom *geom, Scene* scene);
				Real trpz_integrate_u(Real& prevDotU, const Real& un_prev, Real un_curr, const Real& u_prev,
						      const Real& nu, Real k, const Real& keps, const Real& eps, 
		   				      Real dt, bool withContact, bool firstOrder);
				
				
				void shearForce_firsorderScheme(Real const& u, LubricationPhys* phys, ScGeom *geom, Scene* scene);
		
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_ImplicitLubricationPhys,
			LawFunctor,
			"Material law for lubrication and contact between two spheres, resolved implicitly.",
			// ATTR
                          ((bool,activateNormalLubrication,true,,"Activate normal lubrication (default: true)"))
                          ((bool,activateTangencialLubrication,true,,"Activate tangencial lubrication (default: true)"))
                          ((bool,activateTwistLubrication,true,,"Activate twist lubrication (default: true)"))
                          ((bool,activateRollLubrication,true,,"Activate roll lubrication (default: true)"))
//                          ((int,solution,4,,"Resolution method (default: 4)"))
                          ((bool,debug,false,,"Write debug informations"))  
			  
			  ((Real,trpzWeight,1,,"a coefficient in the trapezoidal method"))
			  ((Real,theta,0.5,,"parameter of the 'theta'-method, 1=backward Euler, 0.5=trapezoidal"))
			  ((unsigned int,resolutionMethod,0,,"Change resolution method. 0=>implicit integration, by the trapezoidal rule (2nd order) or backward Euler (1st order) depending on :yref:`Law2_ScGeom_ImplicitLubricationPhys::theta`."))
                        ,// CTOR
			,// PY
                          .def_readwrite("activateNormalLubrication",&Law2_ScGeom_ImplicitLubricationPhys::activateNormalLubrication,"Activate normal lubrication (default: true)")
                          .def_readwrite("activateTangencialLubrication",&Law2_ScGeom_ImplicitLubricationPhys::activateTangencialLubrication,"Activate tangencial lubrication (default: true)")
                          .def_readwrite("activateTwistLubrication",&Law2_ScGeom_ImplicitLubricationPhys::activateTwistLubrication,"Activate twist lubrication (default: true)")
                          .def_readwrite("activateRollLubrication",&Law2_ScGeom_ImplicitLubricationPhys::activateRollLubrication,"Activate roll lubrication (default: true)")
//                          .def_readwrite("solution",&Law2_ScGeom_ImplicitLubricationPhys::solution,"Choose resolution method")
                          .def("getStressForEachBody",&Law2_ScGeom_ImplicitLubricationPhys::PyGetStressForEachBody,"Get stresses for each bodies: normal contact stress, shear contact stress, normal lubrication stress, shear lubrication stress")
                          .staticmethod("getStressForEachBody")
                          .def("getTotalStresses",&Law2_ScGeom_ImplicitLubricationPhys::PyGetTotalStresses,"Get total stresses: normal contact stress, shear contact stress, normal lubrication stress, shear lubrication stress")
                          .staticmethod("getTotalStresses")
                );
                DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_ImplicitLubricationPhys);
