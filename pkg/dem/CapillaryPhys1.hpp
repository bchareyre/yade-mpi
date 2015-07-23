/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<pkg/dem/FrictPhys.hpp>
#include<pkg/dem/DelaunayInterpolation.hpp>
#include<pkg/common/Dispatching.hpp>
#include<pkg/common/ElastMat.hpp>

class MeniscusPhysicalData {
public:
    double R;
    double volume;
    double distance;
    double surface;
    double energy;
    double force;
    double succion;
    double delta1;
    double delta2;
    double arcLength;
    bool ending;
    //default ctor
    MeniscusPhysicalData() : R(0), volume(0), distance(0), surface(0), energy(0), force(0), succion(0), delta1(0), delta2(0), arcLength(0), ending(1)  {}
    //ctor with input values
    MeniscusPhysicalData(const double& r, const double& v, const double& d, const double& s, const double& e, const double& f, const double& p, const double& a1, const double& a2, const double& arc, bool end) : R(r), volume(v), distance(d), surface(s), energy(e), force(f), succion(p), delta1(a1), delta2(a2), arcLength(arc), ending(end) {}

    //a minimal list of operators for the interpolation
    //these operators are requirements for the DataType template parameter of interpolate()
    //FIXME: algebra operations must include energy, perimeter, and any other new variable for including them in the interpolation
    MeniscusPhysicalData& operator+= (const MeniscusPhysicalData& m2) {
        R+=m2.R; volume+=m2.volume; distance+=m2.distance; surface+=m2.surface; energy+=m2.energy; force+=m2.force; succion+=m2.succion; delta1+=m2.delta1; delta2+=m2.delta2; arcLength+=m2.arcLength; ending=(ending && m2.ending);
        return *this;}

    MeniscusPhysicalData operator* (const double& fact) const {
        return MeniscusPhysicalData(fact*R, fact*volume, fact*distance, fact*surface, fact*energy, fact*force, fact*succion, fact*delta1, fact*delta2, fact*arcLength, ending);}

    const MeniscusPhysicalData& operator= (const MeniscusPhysicalData& m1) {
        R=m1.R; volume=m1.volume; distance=m1.distance; surface=m1.surface; energy=m1.energy; force=m1.force; succion=m1.succion; delta1=m1.delta1; delta2=m1.delta2; arcLength=m1.arcLength; ending=m1.ending;
        return *this;}
};

//The structure for the meniscus: physical properties + cached values for fast interpolation
class Meniscus {
    public:
    typedef MeniscusPhysicalData Data;
        Data data;    //the variables of Laplace's problem
        DT::Cell_handle cell;        //pointer to the last location in the triangulation, for faster locate()
        std::vector<K::Vector_3> normals;// 4 normals relative to the current cell

        Meniscus() : data(), cell(DT::Cell_handle()), normals(std::vector<K::Vector_3>()) {}
};

class CapillaryPhys1 : public FrictPhys
{
	public :
		int currentIndexes [4]; // used for faster interpolation (stores previous positions in tables)
		Meniscus m;
		
		virtual ~CapillaryPhys1();

	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CapillaryPhys1,FrictPhys,"Physics (of interaction) for Law2_ScGeom_CapillaryPhys_Capillarity.",
				 ((bool,meniscus,false,,"Presence of a meniscus if true"))
				 ((bool,isBroken,false,,"If true, capillary force is zero and liquid bridge is inactive."))
				 ((bool,computeBridge,true,,"If true, capillary bridge will be computed if not it will be ignored."))
				 ((Real,capillaryPressure,0.,,"Value of the capillary pressure Uc defines as Ugas-Uliquid"))
				 ((Real,vMeniscus,0.,,"Volume of the menicus"))
				 ((Real,Delta1,0.,,"Defines the surface area wetted by the meniscus on the smallest grains of radius R1 (R1<R2)"))
				 ((Real,Delta2,0.,,"Defines the surface area wetted by the meniscus on the biggest grains of radius R2 (R1<R2)"))
				 ((Vector3r,fCap,Vector3r::Zero(),,"Capillary Force produces by the presence of the meniscus"))
				 ((Real,SInterface,0.,,"Fluid-Gaz Interfacial area"))
				 ((Real,arcLength,0.,,"Arc Length of the Fluid-Gaz Interface"))
				 ((short int,fusionNumber,0.,,"Indicates the number of meniscii that overlap with this one"))
				 ,createIndex();currentIndexes[0]=currentIndexes[1]=currentIndexes[2]=currentIndexes[3]=0;
				 );
	REGISTER_CLASS_INDEX(CapillaryPhys1,FrictPhys);
};
REGISTER_SERIALIZABLE(CapillaryPhys1);

class Ip2_FrictMat_FrictMat_CapillaryPhys1 : public IPhysFunctor
{
	public :
		virtual void go(	const shared_ptr<Material>& b1,
					const shared_ptr<Material>& b2,
					const shared_ptr<Interaction>& interaction);

	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(Ip2_FrictMat_FrictMat_CapillaryPhys1,IPhysFunctor, "RelationShips to use with Law2_ScGeom_CapillaryPhys_Capillarity1\n\n In these RelationShips all the interaction attributes are computed. \n\n.. warning::\n\tas in the others :yref:`Ip2 functors<IPhysFunctor>`, most of the attributes are computed only once, when the interaction is new.",
				       ((bool,computeDefault,true,,"bool to assign the default value of computeBridge.")),;
	  
	);
	
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_CapillaryPhys1);
 
