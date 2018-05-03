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
//                 LubricationPhys(ViscElPhys const& ); // "copy" constructor
                virtual ~LubricationPhys();
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(LubricationPhys,ViscElPhys,"IPhys class for Lubrication w/o FlowEngine. Used by Law2_ScGeom_ImplicitLubricationPhys.",
                ((Real,eta,1,Attr::readonly,"Fluid viscosity [Pa.s]"))
                ((Real,eps,0.001,,"Roughness: fraction of radius used as roughness [-]"))
                ((Real,kno,0.0,,"Coefficient for normal stiffness (Hertzian-like contact) [N/m^(3/2)]"))
                ((Real,nun,0.0,,"Coefficient for normal lubrication [N.s]"))
                ((Real,mum,0.3,,"Friction coefficient [-]"))
                ((Real,ue,0.,Attr::readonly,"Surface deflection (ue) at t-dt [m]"))
                ((Real,u,-1,Attr::readonly,"Interfacial distance (u) at t-dt [m]"))
				((Real,prev_un,0,Attr::readonly,"Nondeformed distance (un) at t-dt [m]"))
				((Real,prevDotU,0,Attr::readonly,"du/dt from previous integration - used for trapezoidal scheme (see :yref:`Law2_ScGeom_ImplicitLubricationPhys::resolution` for choosing resolution scheme)"))
                ((Real,delta,0,Attr::readonly,"$\\log(u)$ - used for scheme with $\\delta=\\log(u)$ variable change"))
                ((bool,contact,false,Attr::readonly,"The spheres are in contact"))
                ((bool,slip,false,Attr::readonly,"The contact is slipping"))
				((Vector3r,normalContactForce,Vector3r::Zero(),Attr::readonly,"Normal contact force [N]"))
				((Vector3r,shearContactForce,Vector3r::Zero(),Attr::readonly,"Frictional contact force [N]"))
				((Vector3r,normalLubricationForce,Vector3r::Zero(),Attr::readonly,"Normal lubrication force [N]"))
				((Vector3r,shearLubricationForce,Vector3r::Zero(),Attr::readonly,"Shear lubrication force [N]"))
                , // ctors
                createIndex();,
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
                        ((Real,eps,0.001,,"Roughness: fraction of radius enlargement for contact"))
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
		
		// integration of the gap by implicit theta method, adaptative sub-stepping is used if solutionless, the normal force is returned
		// prevDotU, un_prev, and u_prev are modified after execution (and ready for next step)
			Real trapz_integrate_u(Real& prevDotU, Real& un_prev, Real& u_prev, Real un_curr,
							const Real& nu, Real k, const Real& keps, const Real& eps, 
							Real dt, bool withContact, int depth=0);
		
			Real normalForce_trapezoidal(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew /* FIXME: delete those variables */);
			
			Real normalForce_NewtonRafson(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew);
			Real newton_integrate_u(Real const& un, Real const& nu, Real const& dt, Real const& k, Real const& g, Real const& u_prev, Real const& eps, int depth=0);
			
			Real normalForce_NRAdimExp(LubricationPhys *phys, ScGeom* geom, Real undot, bool isNew);
			Real NRAdimExp_integrate_u(Real const& un, Real const& eps, Real const& alpha, Real & prevDotU, Real const& dt, Real const& prev_d, int depth=0);
			
			void shearForce_firstOrder(LubricationPhys *phys, ScGeom* geom);
			void shearForce_firstOrder_log(LubricationPhys *phys, ScGeom* geom);
			
			void computeShearForceAndTorques(LubricationPhys *phys, ScGeom* geom, State * s1, State *s2, Vector3r & Cr, Vector3r & Ct);
			void computeShearForceAndTorques_log(LubricationPhys *phys, ScGeom* geom, State * s1, State *s2, Vector3r & Cr, Vector3r & Ct);
		
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_ImplicitLubricationPhys,
			LawFunctor,
			"Material law for lubrication and contact between two spheres, resolved implicitly.",
			// ATTR
			((bool,warnedOnce,false,, "some warnings will appear only once, turn this flag false to see them again"))
			((bool,activateNormalLubrication,true,,"Activate normal lubrication (default: true)"))
			((bool,activateTangencialLubrication,true,,"Activate tangencial lubrication (default: true)"))
			((bool,activateTwistLubrication,true,,"Activate twist lubrication (default: true)"))
			((bool,activateRollLubrication,true,,"Activate roll lubrication (default: true)"))
			((bool,debug,false,,"Write debug informations"))
			((int,maxSubSteps,4,,"max recursion depth of adaptative timestepping in the theta-method, the minimal time interval is thus :yref:`Omega::dt<O.dt>`$/2^{depth}$. If still not converged the integrator will switch to backward Euler."))
			((Real,theta,0.55,,"parameter of the 'theta'-method, 1: backward Euler, 0.5: trapezoidal rule, 0: not used,  0.55: suggested optimum)"))
			((int,resolution,0,,"Change normal component resolution method, 0: Iterative exact resolution (theta method, linear contact), 1: Newton-Rafson dimentionless resolution (theta method, linear contact), 2: Newton-Rafson with nonlinear surface deflection (Hertzian-like contact)"))
			((Real, NewtonRafsonTol, 1.e-10,,"Tolerance for Newton-Rafson resolution"))
			((int, NewtonRafsonMaxIter, 20,,"Maximum iterations for Newton-Rafson resolution"))
			,// CTOR
			,// PY
			.def("getStressForEachBody",&Law2_ScGeom_ImplicitLubricationPhys::PyGetStressForEachBody,"Get stresses tensors for each bodies: normal contact stress, shear contact stress, normal lubrication stress, shear lubrication stress.")
			.staticmethod("getStressForEachBody")
			.def("getTotalStresses",&Law2_ScGeom_ImplicitLubricationPhys::PyGetTotalStresses,"Get total stresses tensors: normal contact stress, shear contact stress, normal lubrication stress, shear lubrication stress")
			.staticmethod("getTotalStresses")
                );
                DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_ImplicitLubricationPhys);
