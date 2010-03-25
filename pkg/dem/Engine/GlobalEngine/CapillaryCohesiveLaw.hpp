//
// C++ Interface: CapillaryCohesiveLaw
/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/GlobalEngine.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

// ajouts
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>

/**
This law allows to take into account capillary cohesion between spheres as a result of interparticular liquid bridges (menisci).
refs:
- (french, lot of documentation) L. Scholtes, PhD thesis -> http://tel.archives-ouvertes.fr/tel-00363961/en/
- (english, less...) L. Scholtes et al. Micromechanics of granular materials with capillary effects. International Journal of Engineering Science 2009,(47)1, 64-75 

The law needs ascii files M(r=i) with i=R1/R2 to work (see in yade/extra/capillaryFiles). They contain a set of results from the resolution of the Laplace-Young equation for different configurations of the interacting geometry
The control parameter is the capillary pressure (or suction) Delta_u = u_gas - u_liquid
Liquid bridges properties (volume V, extent over interacting grains delta1 and delta2) are computed as a result of Delta_u and the interacting geometry (spheres radii and interparticular distance)

Rk: - the formulation is valid only for pendular menisci involving two grains (pendular regime).
    - an algorithm was developed by B. Chareyre to identify menisci overlaps on each spheres (menisci fusion).
    - some assumptions can be made to reduce capillary forces when menisci overlap (binary->F_cap=0 if at least 1 overlap, linear->F_cap=F_cap/numberOfOverlaps)
*/

/// !!! This version is deprecated. It should be updated to the new formalism -> ToDo !!!

/// a class to store meniscus parameters -> Rk: is it really needed since CapillaryParameters exist?
class MeniscusParameters
{
public :
	Real V;
	Real F;
	Real delta1;
	Real delta2;
	int index1;
	int index2;

	MeniscusParameters();
	MeniscusParameters(const MeniscusParameters &source);
	~MeniscusParameters();
};

/// R = ratio(RadiusParticle1 on RadiusParticle2). Here, 10 R values from interpolation files (yade/extra/capillaryFiles), R = 1, 1.1, 1.25, 1.5, 1.75, 2, 3, 4, 5, 10
const int NB_R_VALUES = 10;

class capillarylaw; // fait appel a la classe def plus bas
class Interaction;

///This container class is used to check if meniscii overlap. Wet interactions are put in a series of lists, with one list per body.
class BodiesMenisciiList
{
	private:
		vector< list< shared_ptr<Interaction> > > interactionsOnBody;
		
		//shared_ptr<Interaction> empty;
		
	public:
		BodiesMenisciiList();
		BodiesMenisciiList(Scene* body);
		bool prepare(Scene* scene);
		bool insert(const shared_ptr<Interaction>& interaction);
		bool remove(const shared_ptr<Interaction>& interaction);
		list< shared_ptr<Interaction> >& operator[] (int index);
		int size();
		void display();
		
		
		bool initialized;
};

/// This is the constitutive law
// class CapillaryCohesiveLaw : public InteractionSolver
// {
// 	public :
// 		int sdecGroupMask;
// 		Real CapillaryPressure;
// 		bool fusionDetection;//If yes, a BodiesMenisciiList is maintained and updated at each time step
// 		bool binaryFusion;//if true, capillary forces are set to zero as soon as 1 fusion at least is detected
// 		void checkFusion(Scene * ncb);
// 		shared_ptr<capillarylaw> capillary;
// 		BodiesMenisciiList bodiesMenisciiList;
// 						
// 		CapillaryCohesiveLaw();
// 		void action(Scene * ncb);
// 		REGISTER_ATTRIBUTES(InteractionSolver,(sdecGroupMask)(CapillaryPressure)(fusionDetection)(binaryFusion));
// 		virtual void postProcessAttributes(bool deserializing);
// 	REGISTER_CLASS_NAME(CapillaryCohesiveLaw);
// 	REGISTER_BASE_CLASS_NAME(InteractionSolver);
// 
// };

class CapillaryCohesiveLaw : public GlobalEngine
{
	public :
		void checkFusion();
		shared_ptr<capillarylaw> capillary;
		BodiesMenisciiList bodiesMenisciiList;
		
		void action();
		virtual void postProcessAttributes(bool deserializing);
		
	YADE_CLASS_BASE_DOC_ATTRS(CapillaryCohesiveLaw,GlobalEngine,"Rk: deprecated -> needs some work to be conform to the new formalism! This law allows to take into account capillary cohesion between spheres as a result of interparticular liquid bridges (menisci). refs: 1- infrench, (lot of documentation) L. Scholtes, PhD thesis -> http://tel.archives-ouvertes.fr/tel-00363961/en/ - 2 in english (less documentation) L. Scholtes et al. Micromechanics of granular materials with capillary effects. International Journal of Engineering Science 2009,(47)1, 64-75. The law needs ascii files M(r=i) with i=R1/R2 to work (see in yade/extra/capillaryFiles). They contain a set of results from the resolution of the Laplace-Young equation for different configurations of the interacting geometry. The control parameter is the capillary pressure (or suction) Uc = Cgas - Uliquid. Liquid bridges properties (volume V, extent over interacting grains delta1 and delta2) are computed as a result of the defined capillary pressure and of the interacting geometry (spheres radii and interparticular distance).",
				  ((int,sdecGroupMask,1,"? the interaction only considers particles with same mask ?"))
				  ((Real,CapillaryPressure,0.,"Value of the capillary pressure Uc defines as Ugas-Uliquid"))
				  ((bool,fusionDetection,false,"If true potential menisci overlaps are checked"))
				  ((bool,binaryFusion,true,"If true, capillary forces are set to zero as soon as, at least, 1 overlap (menisci fusion) is detected"))
				  );
};


class TableauD
{
	public:
		Real D;
		std::vector<std::vector<Real> > data;
		MeniscusParameters Interpolate3(Real P, int& index);
		
  		TableauD();
  		TableauD(std::ifstream& file);
  		~TableauD();
};

// Fonction d'ecriture de tableau, utilisee dans le constructeur pour test 
class Tableau;
std::ostream& operator<<(std::ostream& os, Tableau& T);

class Tableau
{	
	public: 
		Real R;
		std::vector<TableauD> full_data;
		MeniscusParameters Interpolate2(Real D, Real P, int& index1, int& index2);
		
		std::ifstream& operator<< (std::ifstream& file);
		
		Tableau();
    		Tableau(const char* filename);
    		~Tableau();
};

class capillarylaw
{
	public:
		capillarylaw();
		std::vector<Tableau> data_complete;
		MeniscusParameters Interpolate(Real R1, Real R2, Real D, Real P, int* index);
		
		void fill (const char* filename);
};

REGISTER_SERIALIZABLE(CapillaryCohesiveLaw);



