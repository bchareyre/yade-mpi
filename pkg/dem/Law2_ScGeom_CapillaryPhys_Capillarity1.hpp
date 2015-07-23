//
// C++ Interface: Law2_ScGeom_CapillaryPhys_Capillarity
/*************************************************************************
* Copyright (C) 2006 by luc Scholtes *
* luc.scholtes@hmg.inpg.fr *
* *
* This program is free software; it is licensed under the terms of the *
* GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <core/GlobalEngine.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

#include <vector>
#include <list>
#include <utility>
#include <pkg/dem/CapillaryPhys1.hpp>
#include<pkg/common/Dispatching.hpp>
#include <string>
#include <iostream>
#include <fstream>

/**
This law allows one to take into account capillary forces/effects between spheres coming from the presence of interparticular liquid bridges (menisci).
refs:
- (french, lot of documentation) L. Scholtes, PhD thesis -> http://tel.archives-ouvertes.fr/tel-00363961/en/
- (english, less...) L. Scholtes et al. Micromechanics of granular materials with capillary effects. International Journal of Engineering Science 2009,(47)1, 64-75

The law needs ascii files M(r=i) with i=R1/R2 to work (downloaded from https://yade-dem.org/wiki/CapillaryTriaxialTest). They contain a set of results from the resolution of the Laplace-Young equation for different configurations of the interacting geometry and must be placed in the bin directory (where yade exec file is situated) to be taken into account.
The control parameter is the capillary pressure (or suction) Delta_u, defined as the difference between gas and liquid pressure: Delta_u = u_gas - u_liquid
Liquid bridges properties (volume V, extent over interacting grains delta1 and delta2) are computed as a result of Delta_u and the interacting geometry (spheres radii and interparticular distance)

Rk: - the formulation is valid only for pendular menisci involving two grains (pendular regime).
- an algorithm was developed by B. Chareyre to identify menisci overlaps on each spheres (menisci fusion).
- some assumptions can be made to reduce capillary forces when menisci overlap (binary->F_cap=0 if at least 1 overlap, linear->F_cap=F_cap/numberOfOverlaps)
*/

/// !!! This version is deprecated. It should be updated to the new formalism -> ToDo !!!



/// R = ratio(RadiusParticle1 on RadiusParticle2). Here, 10 R values from interpolation files (yade/extra/capillaryFiles), R = 1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9
//const int NB_R_VALUES = 10;

// class capillarylaw1; // fait appel a la classe def plus bas
class Interaction;


///This container class is used to check if meniscii overlap. Wet interactions are put in a series of lists, with one list per body.
class BodiesMenisciiList1
{
private:
vector< list< shared_ptr<Interaction> > > interactionsOnBody;

//shared_ptr<Interaction> empty;

public:
BodiesMenisciiList1();
BodiesMenisciiList1(Scene* body);
bool prepare(Scene* scene);
bool insert(const shared_ptr<Interaction>& interaction);
bool remove(const shared_ptr<Interaction>& interaction);
list< shared_ptr<Interaction> >& operator[] (int index);
int size();
void display();


bool initialized;
};

/// This is the constitutive law
class Law2_ScGeom_CapillaryPhys_Capillarity1 : public GlobalEngine
{
public :
    void checkFusion();

    static DT dtVbased;
    static DT dtPbased;
    std::vector<MeniscusPhysicalData> solutions;
    int switchTriangulation;//to detect switches between P-based and V-based data


    BodiesMenisciiList1 bodiesMenisciiList;

    void action();
     Real intEnergy();
     Real swInterface();
     Real wnInterface();
     Real waterVolume();
     void solver(Real suction, bool reset);
     void triangulateData();

    YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_CapillaryPhys_Capillarity1,GlobalEngine,"This law allows one to take into account capillary forces/effects between spheres coming from the presence of interparticular liquid bridges (menisci).\n\nThe control parameter is the capillary pressure (or suction) Uc = ugas - Uliquid. Liquid bridges properties (volume V, extent over interacting grains delta1 and delta2) are computed as a result of the defined capillary pressure and of the interacting geometry (spheres radii and interparticular distance).\n\nReferences: in english [Scholtes2009b]_; more detailed, but in french [Scholtes2009d]_.\n\nThe law needs ascii files M(r=i) with i=R1/R2 to work (see https://yade-dem.org/index.php/CapillaryTriaxialTest). These ASCII files contain a set of results from the resolution of the Laplace-Young equation for different configurations of the interacting geometry."
    "\n\nIn order to allow capillary forces between distant spheres, it is necessary to enlarge the bounding boxes using :yref:`Bo1_Sphere_Aabb::aabbEnlargeFactor` and make the Ig2 define define distant interactions via:yref:`interactionDetectionFactor<Ig2_Sphere_Sphere_ScGeom::interactionDetectionFactor>`. It is also necessary to disable interactions removal by the constitutive law (:yref:`Law2<Law2_ScGeom_FrictPhys_CundallStrack::neverErase>=True`). The only combinations of laws supported are currently capillary law + :yref:`Law2_ScGeom_FrictPhys_CundallStrack` and capillary law + :yref:`Law2_ScGeom_MindlinPhys_Mindlin` (and the other variants of Hertz-Mindlin).\n\nSee CapillaryPhys-example.py for an example script.",
                   ((Real,capillaryPressure,0.,,"Value of the capillary pressure Uc defines as Uc=Ugas-Uliquid"))
		  ((Real,totalVolumeofWater,-1.,,"Value of imposed water volume"))
		  ((Real,liquidTension,0.073,,"Value of the superficial water tension in N/m"))
		  ((Real,epsilonMean,0.,," Mean Value of the roughness"))//old value was epsilon
		  ((Real,disp,0.,," Dispersion from the mean Value of the roughness"))//added now
		  ((Real,interactionDetectionFactor,1.5,,"defines critical distance for deleting interactions. Must be consistent with the Ig2 value."))
                   ((bool,fusionDetection,false,,"If true potential menisci overlaps are checked"))
                   ((bool,initialized,false,," "))
                   ((bool,binaryFusion,true,,"If true, capillary forces are set to zero as soon as, at least, 1 overlap (menisci fusion) is detected"))
                   ((bool,hertzOn,false,,"|yupdate| true if hertz model is used"))
                   ((string,inputFilename,string("capillaryfile.txt"),,"the file with meniscus solutions, used for interpolation."))
                   ((bool,createDistantMeniscii,false,,"Generate meniscii between distant spheres? Else only maintain the existing one. For modeling a wetting path this flag should always be false. For a drying path it should be true for one step (initialization) then false, as in the logic of [Scholtes2009c]_"))
                   ((bool,imposePressure,true,," If True, suction is imposed and is constant if not Volume is imposed-Undrained test"))   
		  ((bool,totalVolumeConstant,true,," in undrained test there are 2 options, If True, the total volume of water is imposed,if false the volume of each meniscus is kept constant: in this case capillary pressure can be imposed for initial distribution of meniscus or it is the total volume that can be imposed initially")) 
		   ,switchTriangulation=-1,
		  .def("intEnergy",&Law2_ScGeom_CapillaryPhys_Capillarity1::intEnergy,"define the energy of interfaces in unsaturated pendular state")
// 		  .def("sninterface",&Law2_ScGeom_CapillaryPhys_Capillarity1::sninterface,"define the amount of solid-non-wetting interfaces in unsaturated pendular state")
		  .def("swInterface",&Law2_ScGeom_CapillaryPhys_Capillarity1::swInterface,"define the amount of solid-wetting interfaces in unsaturated pendular state")
		  .def("wnInterface",&Law2_ScGeom_CapillaryPhys_Capillarity1::wnInterface,"define the amount of wetting-non-wetiing interfaces in unsaturated pendular state")
		  .def("waterVolume",&Law2_ScGeom_CapillaryPhys_Capillarity1::waterVolume,"return the total value of water in the sample")
     );
};



REGISTER_SERIALIZABLE(Law2_ScGeom_CapillaryPhys_Capillarity1);

