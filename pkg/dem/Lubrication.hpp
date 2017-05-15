 
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

namespace py=boost::python;

#include<pkg/common/NormShearPhys.hpp>
#include<pkg/common/Dispatching.hpp>


class LubricationPhys: public NormShearPhys {
        public:
                LubricationPhys(NormShearPhys const& ); // "copy" constructor
                virtual ~LubricationPhys();
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(ElectrostaticPhys,FrictPhys,"IPhys class for Lubrication w/o FlowEngine. Used by Law2_ScGeom_LubricationPhys.",
                        ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                        , // ctors
                        createIndex();,
                        .def_readonly("eta",&LubricationPhys::eta,"Fluid viscosity [Pa.s]")
                );
                DECLARE_LOGGER;
                REGISTER_CLASS_INDEX(ElectrostaticPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(ElectrostaticPhys);


class Ip2_Material_Material_LubricationPhys: public IPhysFunctor{
        public:
                virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
                FUNCTOR2D(Material,Material);
                DECLARE_LOGGER;
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Ip2_Material_Material_LubricationPhys,IPhysFunctor,"Ip2 creating LubricationPhys from two Material instances.",
                        ((Real,eta,1,,"Fluid viscosity [Pa.s]"))
                    ,,
                );
};
REGISTER_SERIALIZABLE(Ip2_Material_Material_LubricationPhys);


class Law2_ScGeom_LubricationPhys: public LawFunctor{
        public:
                bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
                FUNCTOR2D(GenericSpheresContact,LubricationPhys);
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_LubricationPhys,LawFunctor,"Material law for lubrication between two spheres.",,,
                );
                DECLARE_LOGGER;

};
REGISTER_SERIALIZABLE(Law2_ScGeom_LubricationPhys);
