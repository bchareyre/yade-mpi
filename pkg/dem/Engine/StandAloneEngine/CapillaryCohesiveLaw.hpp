//
// C++ Interface: CapillaryCohesiveLaw
/*************************************************************************
*  Copyright (C) 2006 by luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CAPILLARY_COHESIVE_LAW_HPP
#define CAPILLARY_COHESIVE_LAW_HPP

#include <yade/core/InteractionSolver.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>

// ajouts
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Parameters

{ 
  public :
  Real V;
  Real F;
  Real delta1;
  Real delta2;
  int index1;
  int index2;

  Parameters();
  Parameters(const Parameters &source);
  ~Parameters();
} ;


const int NB_R_VALUES = 10;

class PhysicalAction;
class capillarylaw; // fait appel a la classe def plus bas

class CapillaryCohesiveLaw : public InteractionSolver
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;
		
	public :
		int sdecGroupMask;
		Real CapillaryPressure;
		shared_ptr<capillarylaw> capillary;
						
		CapillaryCohesiveLaw();
		void action(Body* body);

	protected : 
		void registerAttributes();
	REGISTER_CLASS_NAME(CapillaryCohesiveLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);

};

class TableauD
{
	public:
		Real D;
		std::vector<std::vector<Real> > data;
		Parameters Interpolate3(Real P, int& index);
		
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
		Parameters Interpolate2(Real D, Real P, int& index1, int& index2);
		
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
		Parameters Interpolate(Real R1, Real R2, Real D, Real P, int* index);
		
		void fill (const char* filename);
};

REGISTER_SERIALIZABLE(CapillaryCohesiveLaw,false);

#endif // CAPILLARY_COHESIVE_LAW_HPP

