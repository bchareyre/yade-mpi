/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/**
@author Bruno Chareyre
*/
#pragma once

#include "Tesselation.h"
#include <vector>
#include<boost/iostreams/filtering_stream.hpp>

/*! \class TriaxialState
 * \brief A storage class with ascii input/output for bodies, contacts, and macro-variables. Yade packings are first converted to this object type, before being processed in KinematicLocalisationAnalyser.
 * 
 */

namespace CGT {
using namespace std;

class TriaxialState
{
public:
	
	typedef _Tesselation<SimpleTriangulationTypes> 			Tesselation;
	typedef Tesselation::RTriangulation				RTriangulation;
	class Contact;
	class Grain;
	typedef struct {Point base; Point sommet;}			Box;
	typedef vector<Contact*>					VectorContact;
	typedef vector<Grain>						VectorGrain;
	typedef VectorContact::iterator					ContactIterator;
	typedef VectorGrain::iterator					GrainIterator;

	class Grain {	public :
					int id;
					bool isSphere;
					Sphere sphere;
					Vecteur translation;
					Vecteur rotation;
					VectorContact contacts;
					
					Grain(void) {id=-1; isSphere=true;}
				};
	class Contact { public :
					enum Status {NEW, PERSISTENT, LOST};
					
					Grain* grain1;
					Grain* grain2;
					Vecteur position;
					Vecteur normal;
					Real fn;
					Vecteur fs;
					Real old_fn;
					Vecteur old_fs;
					Real frictional_work;
					bool visited;
					Status status;
					Contact(void) {visited=false; status=PERSISTENT;}
				};

	TriaxialState(void);
	~TriaxialState(void);
		
	bool from_file(const char* filename, bool bz2=false);
	bool to_file(const char* filename, bool bz2=false);
	bool inside(Real x, Real y, Real z);
	bool inside(Vecteur v);
	bool inside(Point p);
	static Real find_parameter (const char* parameter_name, const char* filename);
	static Real find_parameter (const char* parameter_name, boost::iostreams::filtering_istream& file);
	static Real find_parameter (const char* parameter_name, ifstream& file);
	void reset (void);

	GrainIterator grains_begin (void);
	ContactIterator contacts_begin (void);
	GrainIterator grains_end (void);
	ContactIterator contacts_end (void);
	Tesselation& tesselation (void);
	Grain& grain (unsigned int id);


	//Public member data :
	bool NO_ZERO_ID;//Is there a body with id=0?
	Real mean_radius;
	Box box;
	Real filter_distance;	//distance de filtrage au voisinage des parois - normalis�e par le rayon moyen 
	long Ng, Nc;
	Real rfric, Eyn, Eys, wszzh, wsxxd, wsyyfa, eps1, eps2, eps3, porom, haut, larg, prof, ratio_f, vit;
	VectorContact contacts;
	VectorGrain grains;

private :
	Tesselation Tes;	
	Tesselation& Tesselate (void);

	//Private member data :
	bool tesselated;


};

} // namespace CGT
