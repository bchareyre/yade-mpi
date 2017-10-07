// 2016 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#pragma once

#include<pkg/dem/CohesiveFrictionalContactLaw.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/common/PeriodicEngines.hpp>
#include<pkg/common/NormShearPhys.hpp>
#include<pkg/dem/DemXDofGeom.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/dem/ElasticContactLaw.hpp>

namespace py=boost::python;



class ElectrostaticMat: public CohFrictMat {
	public:
                YADE_CLASS_BASE_DOC_ATTRS_CTOR(ElectrostaticMat,CohFrictMat,"Electrostatic material, used in Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys and Law2_ScGeom_ElectrostaticPhys.",
                        //((Real,charge,0,,"Surface potential [mV]"))/*OLD*/
                        //((Real,DebyeCoef,0.05,,"Proportion of the radius that is the Debye length"))/*OLD*/
			,
                        createIndex();
		);
                REGISTER_CLASS_INDEX(ElectrostaticMat,CohFrictMat);
};
REGISTER_SERIALIZABLE(ElectrostaticMat);


class ElectrostaticPhys: public CohFrictPhys {
        public:
                ElectrostaticPhys(CohFrictPhys const& ); // "copy" constructor
                virtual ~ElectrostaticPhys();
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(ElectrostaticPhys,CohFrictPhys,"IPhys class containing parameters of ElectrostaticMat. Used by Law2_ScGeom_ElectrostaticPhys.",
                        ((Real,DebyeLength,1e-8,,"Debye Length [m]"))
                        ((Real,InterConst,1e-12,,"Double layer interaction constant [J]"))
                        ((Real,A,1e-19,,"Hamaker constant [J]"))
                        ((Real, eps, 0.001,,"Rugosity [-]"))
			, // ctors
                        createIndex();,
                        .def_readonly("DebyeLength",&ElectrostaticPhys::DebyeLength,"Debye Length \kappa^-1 [m]")
                        .def_readonly("InterConst",&ElectrostaticPhys::InterConst,"Interaction Constant Z [J]")
                        .def_readonly("A",&ElectrostaticPhys::A,"Hamaker Constant A [J]")
		);
		DECLARE_LOGGER;
                REGISTER_CLASS_INDEX(ElectrostaticPhys,CohFrictPhys);
};
REGISTER_SERIALIZABLE(ElectrostaticPhys);





class Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys: public Ip2_CohFrictMat_CohFrictMat_CohFrictPhys{
	public:
		virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
                FUNCTOR2D(ElectrostaticMat,ElectrostaticMat);
		DECLARE_LOGGER;
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys,Ip2_CohFrictMat_CohFrictMat_CohFrictPhys,"Ip2 creating ElectrostaticPhys from two ElectrostaticMat instances.",
                        ((Real,DebyeLength,0,,"Debye length [m]. If 0, will be calculated from fluid properties"))
                        ((Real,SurfCharge,50,,"Surface potential [mV]"))
                        ((Real,Temp,20,,"Temperature into the fluid [°C]"))
                        ((Real,RelPerm,1,,"Relative permittivity of the fluid [-]"))
                        ((Real,A,1e-19,,"Hamaker constant [J]"))
                        ((Real,Z,0,,"Interaction constant [N]. If 0, will be calculated from termal properties"))
                        ((Real,z,0,,"Surface ion valency [-]"))
                        ((Real, eps, 0.001,,"Rugosity [-]"))
                        ((vector<Vector2r>,Ions,vector<Vector2r>({Vector2r(-1,1),Vector2r(1,1)}),,"List of ions's charge and concentration (default is: 1mol/l Na(+1)Cl(-1): [(+1,1),(-1,1)]"))
                    ,,
		);
};
REGISTER_SERIALIZABLE(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys);



class Law2_ScGeom_ElectrostaticPhys: public Law2_ScGeom6D_CohFrictPhys_CohesionMoment{
	public:
		bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
                FUNCTOR2D(GenericSpheresContact,ElectrostaticPhys);
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_ElectrostaticPhys,Law2_ScGeom6D_CohFrictPhys_CohesionMoment,"Material law for electrostatic interaction according to [Mari2013]_.",,,
//                  ((Real,f_VdW,0,,"Computed Van Der Waals Force"))
//                  ((Real,f_DLE,0,,"Computed Double Layer Electrostatic Force")),,
//                            .def_readonly("f_VdW",&Law2_ScGeom_ElectrostaticPhys::f_VdW,"Computed VanDerWaals Force")
//                            .def_readonly("f_DLE",&Law2_ScGeom_ElectrostaticPhys::f_DLE,"Computed Double Layer Electrostatic Force")
//		ONLY FOR DEBUGGING PURPOSE BETWEEN 2 PARTICLES
                );
		DECLARE_LOGGER;

};
REGISTER_SERIALIZABLE(Law2_ScGeom_ElectrostaticPhys);
