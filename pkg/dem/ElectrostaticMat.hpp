// 2016 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

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



class ElectrostaticMat: public FrictMat {
	public:
                YADE_CLASS_BASE_DOC_ATTRS_CTOR(ElectrostaticMat,FrictMat,"Electrostatic material, used in Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys and Law2_ScGeom_ElectrostaticPhys.",          
                        //((Real,charge,0,,"Surface potential [mV]"))/*OLD*/
                        //((Real,DebyeCoef,0.05,,"Proportion of the radius that is the Debye length"))/*OLD*/
			,
			createIndex();
		);
                REGISTER_CLASS_INDEX(ElectrostaticMat,FrictMat);
};
REGISTER_SERIALIZABLE(ElectrostaticMat);


class ElectrostaticPhys: public FrictPhys {
        public:
                ElectrostaticPhys(FrictPhys const& ); // "copy" constructor
                virtual ~ElectrostaticPhys();
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(ElectrostaticPhys,FrictPhys,"IPhys class containing parameters of ElectrostaticMat. Used by Law2_ScGeom_ElectrostaticPhys.",
                        //((Real,DebyeCoef,0.05,,"Proportion of the radius that is the Debye length"))/*OLD*/
                        ((Real,DebyeLength,1e-6,,"Debye Length [m]"))/*OLD*/
                        ((Real,InterConst,1e-10,,"Double layer interaction constant"))/*OLD*/
                        ((Real,A,1e-19,,"Hamaker constant [J]"))
			, // ctors
                        createIndex();,
		);
		DECLARE_LOGGER;
                REGISTER_CLASS_INDEX(ElectrostaticPhys,FrictPhys);
};
REGISTER_SERIALIZABLE(ElectrostaticPhys);





class Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys: public Ip2_FrictMat_FrictMat_FrictPhys{
	public:
		virtual void go(const shared_ptr<Material>& material1, const shared_ptr<Material>& material2, const shared_ptr<Interaction>& interaction);
                FUNCTOR2D(ElectrostaticMat,ElectrostaticMat);
		DECLARE_LOGGER;
                YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys,Ip2_FrictMat_FrictMat_FrictPhys,"Ip2 creating ElectrostaticPhys from two ElectrostaticMat instances.",
                        ((Real,DebyeLength,0,,"Debye length [m]. If 0, will be calculated from fluid properties"))
                        ((Real,SurfCharge,50,,"Surface potential [mV]"))
                        ((Real,Temp,20,,"Temperature into the fluid [°C]"))
                        ((Real,RelPerm,1,,"Relative permittivity of the fluid [-]"))
                        ((Real,A,1e-19,,"Hamaker constant [J]"))
                        ((Real,Z,0,,"Interaction constant [N]. If 0, will be calculated from termal properties"))
                        ((vector<Vector2r>,Ions,vector<Vector2r>({Vector2r(-1,1),Vector2r(1,1)}),,"List of ions's charge and concentration (default is: 1mol/l Na(+1)Cl(-1): [(+1,1),(-1,1)]"))
                    ,,
		);
};
REGISTER_SERIALIZABLE(Ip2_ElectrostaticMat_ElectrostaticMat_ElectrostaticPhys);



class Law2_ScGeom_ElectrostaticPhys: public Law2_ScGeom_FrictPhys_CundallStrack{
	public:
		bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
                FUNCTOR2D(GenericSpheresContact,ElectrostaticPhys);
                YADE_CLASS_BASE_DOC(Law2_ScGeom_ElectrostaticPhys,Law2_ScGeom_FrictPhys_CundallStrack,"Material law for electrostatic interaction according to [Mari2013]_."
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_ElectrostaticPhys);
