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

#define ln(x) log(x)

namespace py=boost::python;


class LubricationPhys: public NormShearPhys {
        public:
                LubricationPhys(NormShearPhys const& ); // "copy" constructor
                virtual ~LubricationPhys();
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(LubricationPhys,NormShearPhys,"IPhys class for Lubrication w/o FlowEngine. Used by Law2_ScGeom_LubricationPhys.",
                ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                ((shared_ptr<IPhys>,otherPhys,0,,"Other physics combined"))
                , // ctors
                createIndex();,
                      .def_readonly("eta",&LubricationPhys::eta,"Fluid viscosity [Pa.s]")
                );
                DECLARE_LOGGER;
                REGISTER_CLASS_INDEX(LubricationPhys,NormShearPhys);
};
REGISTER_SERIALIZABLE(LubricationPhys);


class Ip2_ElastMat_ElastMat_LubricationPhys: public IPhysFunctor{
        public:
                virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
                FUNCTOR2D(ElastMat,ElastMat);
                DECLARE_LOGGER;
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Ip2_ElastMat_ElastMat_LubricationPhys,IPhysFunctor,"Ip2 creating LubricationPhys from two Material instances.",
                        ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                        ((shared_ptr<IPhysFunctor>, otherPhysFunctor,0,,"Other physics to combine with lubrication.")),,
                .def_readwrite("otherPhys",&Ip2_ElastMat_ElastMat_LubricationPhys::otherPhysFunctor,"Other physics to combine with lubrication")
                );
};
REGISTER_SERIALIZABLE(Ip2_ElastMat_ElastMat_LubricationPhys);


class Law2_ScGeom_LubricationPhys: public LawFunctor{
        public:
                bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
                FUNCTOR2D(GenericSpheresContact,LubricationPhys);
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_LubricationPhys,LawFunctor,"Material law for lubrication between two spheres.",
                    ((bool,activateLubrication,false,,"Activate lubrication (default: false)"))
                    ((shared_ptr<LawFunctor>, otherLawFunctor,0,,"Other interaction law to combine with lubrication")),,
                    .def_readwrite("activateLubrication",&Law2_ScGeom_LubricationPhys::activateLubrication,"Activate lubrication (default: false)")
                    .def_readwrite("otherLaw",&Law2_ScGeom_LubricationPhys::otherLawFunctor,"Other interaction law to combine with lubrication")
                );
                DECLARE_LOGGER;

};
REGISTER_SERIALIZABLE(Law2_ScGeom_LubricationPhys);
