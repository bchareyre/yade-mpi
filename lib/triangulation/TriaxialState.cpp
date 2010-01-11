/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/




#include "TriaxialState.h"
#include <math.h>

#ifdef USE_OGL_VIEW
#include "../vueGL/vue3d.h"
#endif

namespace CGT {

TriaxialState::TriaxialState(void) : NO_ZERO_ID(false), filter_distance(-0.1), tesselated(false) {}

TriaxialState::~TriaxialState(void)
{
	ContactIterator last = contacts_end();
	for (ContactIterator it=contacts_begin(); it!=last; ++it) {
		if (*it) delete *it;}
}

Real TriaxialState::find_parameter (const char* parameter_name, ifstream& file)
{
	/*long starting_position = file.tellg();
	file.seekg (0, ios::end);
	long length = file.tellg();
	file.seekg (starting_position, ios::beg);*/
	
	/*if (starting_position == -1) starting_position=0;
	cout << "starting position: " << starting_position << endl;
	bool restarted = false;
	if (file.tellg()==-1) {
		file.seekg( 0, ios::beg );}*/

	string buffer;
	Real value;
	file >> buffer;
	bool test = (buffer == string(parameter_name));
	cout << "buffer0 " << buffer << " test0: "<< test << endl;
	while (!test)
	{
		buffer.clear();
		file >> buffer;
		//if (buffer.compare(string(" "))==0 || buffer.empty()) continue;
		/*if (file.eof() && !restarted) {
		cout << "restarted1" << endl;
		file.seekg( 0 );
		restarted= true;}*/
		//test = ((!buffer.empty() && (buffer.compare(string(parameter_name))== 0)) || file.eof());
		test = ( buffer == string(parameter_name) || file.eof());
		//test = ( buffer == string(parameter_name) || length <= file.tellg());
		//cout << "buffer1 " << buffer << " test: "<< test << endl;
	}
	if (!file.eof()) file >> value;
	else value = 0;
	cout << string(parameter_name) << value << endl;
	//file.seekg( starting_position, ios::beg );
	return value;
}

Real TriaxialState::find_parameter (const char* parameter_name, const char* filename)
{
	ifstream statefile (filename);
	return find_parameter(parameter_name, statefile);
}


void TriaxialState::reset (void)
{
	tesselated = false;
	Tes.Clear();
	mean_radius=0;
	grains.clear();	
	ContactIterator contacts_end = contacts.end();
	for (ContactIterator it=contacts.begin(); it!=contacts_end; ++it) delete *it;
	contacts.clear();
	contacts.resize(0);	
	box.base = Point(1.0e10, 1.0e10, 1.0e10);
	box.sommet = Point(-1.0e10, -1.0e10, -1.0e10);
}

TriaxialState::GrainIterator TriaxialState::grains_begin (void)
{	
	GrainIterator git = grains.begin();
	if (NO_ZERO_ID) return ++git;
	else return git;
}

TriaxialState::ContactIterator TriaxialState::contacts_begin (void)
{
	return contacts.begin();
}

TriaxialState::GrainIterator TriaxialState::grains_end (void)
{
	return grains.end();
}

TriaxialState::ContactIterator TriaxialState::contacts_end (void)
{
	return contacts.end();
}

TriaxialState::Grain& TriaxialState::grain (unsigned int id)
{
	return grains[id];
}

Tesselation& TriaxialState::Tesselate (void)
{
	if (!tesselated)
	{
		Tes.Clear();
		GrainIterator git = grains_begin();
		GrainIterator last = grains_end();
		for (; git!=last; ++git) {
			if (git->id != -1 && git->isSphere)	
				Vertex_handle vh = Tes.insert(git->sphere.x(), git->sphere.y(), git->sphere.z(), git->sphere.weight(), git->id);
			//vh->->info() = git->translation; FIXME : this could define displacements in the triangulation itself
//			cerr << "Tes.insert(git->sphere.x(), git->sphere.y(), git->sphere.z(), git->sphere.weight(), git->id);" << endl;
		}
		tesselated = true;
		cerr << "Triangulated Grains : " << Tes.Triangulation().number_of_vertices() << endl;
	}
	return Tes;
}

Tesselation& TriaxialState::tesselation (void)
{
	return Tesselate();
}

bool TriaxialState::inside(Real x, Real y, Real z)
{
	return (x >= (box.base.x()+filter_distance*mean_radius) &&
			x <= (box.sommet.x()-filter_distance*mean_radius) &&
			y >= (box.base.y()+filter_distance*mean_radius) &&
			y <= (box.sommet.y()-filter_distance*mean_radius) &&
			z >= (box.base.z()+filter_distance*mean_radius) &&
			z <= (box.sommet.z()-filter_distance*mean_radius) );
}

bool TriaxialState::inside(Vecteur v)
{
	return TriaxialState::inside(v.x(), v.y(), v.z());
}

bool TriaxialState::inside(Point p)
{
	return TriaxialState::inside(p.x(), p.y(), p.z());
}

bool TriaxialState::from_file(const char* filename)
{
	reset();
	ifstream Statefile(filename);
	cout << filename << endl;
	if (!Statefile.is_open()) {
		cout << "Error opening files";
		return false;
	}

	//int a=0;
#ifdef USE_OGL_VIEW
	Vue3D Vue1;
#endif


	long Idg;
	Statefile >> Ng;
	//Real x, y, z, rad; //coordonn�es/rayon
	//Real tx, ty, tz;
	Point pos;
	mean_radius=0;
	Vecteur trans, rot;
	Real rad; //coordonn�es/rayon
	bool isSphere;


	grains.resize(Ng+1);
	//cout << "Ngrains =" << Ng << endl;
	if (NO_ZERO_ID) {
		GrainIterator git= grains.begin();
		git->id=0;
		git->sphere = Sphere(CGAL::ORIGIN, 0);
		git->translation = CGAL::NULL_VECTOR;
		git->rotation = CGAL::NULL_VECTOR;
	}

	long i= NO_ZERO_ID ? 1 : 0;

	for (; i <= Ng ; ++i) {
		Statefile >> Idg >> pos >> rad >> trans >> rot  >> isSphere;
		grains[Idg].id = Idg;
		grains[Idg].sphere = Sphere(pos, rad);
		grains[Idg].translation = trans;
		grains[Idg].rotation = rot;
		grains[Idg].isSphere = isSphere;

		box.base = Point(min(box.base.x(), pos.x()-rad),
						 min(box.base.y(), pos.y()-rad),
						 min(box.base.z(), pos.z()-rad));
		box.sommet = Point(max(box.sommet.x(), pos.x()+rad),
						   max(box.sommet.y(), pos.y()+rad),
						   max(box.sommet.z(), pos.z()+rad));
		mean_radius += grains[Idg].sphere.weight();
		//cout << "Idg: "<< Idg << " sphere: " << grains[Idg].sphere << " trans: " << grains[Idg].translation << endl;
	}
	mean_radius /= Ng;//rayon moyen
	//cout << filename << " loaded : " << Ng << " grains with mean radius = " << mean_radius << endl;

	long id1, id2;
	int stat;
	Vecteur c_pos, normal, old_fs, fs;
	Real old_fn, fn, frictional_work;
	Statefile >> Nc;
	contacts.resize(Nc);

	for (long i=0 ; i < Nc ; ++i) {
		cout << "hereNc"<<Nc<<"/"<<i<<endl;
		Contact* c = new Contact;
		Statefile >> id1 >> id2 >> normal >> c_pos >> old_fn >> old_fs >> fn >> fs >> frictional_work >> stat;
		
		normal = (grains[id2].sphere.point()-grains[id1].sphere.point());
		normal = normal/sqrt(pow(normal.x(),2)+pow(normal.y(),2)+pow(normal.z(),2));
		c->grain1 = &(grains[id1]);
		c->grain2 = &(grains[id2]);
		grains[id1].contacts.push_back(c);
		grains[id2].contacts.push_back(c);
		c->normal = normal;
		c->position = c_pos;
		c->old_fn = old_fn;
		c->old_fs = old_fs;
		c->fn = fn;
		c->fs = fs;
		c->frictional_work = frictional_work;
		c->status = (Contact::Status) stat;
		if (contacts[i]) delete contacts[i];
		contacts[i] = c;
	}

	//cout << "c_pos=" << contacts[10]->position << " old_fn=" << contacts[10]->old_fn << " normal=" << contacts[10]->normal << endl;
	cout << "here0"<<endl;
	//rfric = find_parameter("rfric=", Statefile);// � remettre quand les fichiers n'auront plus l'espace de trop...
	Eyn = find_parameter("Eyn", Statefile);
	Eys = find_parameter("Eys", Statefile);
	wszzh = find_parameter("wszzh", Statefile);
	wsxxd = find_parameter("wsxxd", Statefile);
	wsyyfa = find_parameter("wsyyfa", Statefile);
	eps3 = find_parameter("eps3", Statefile);
	eps1 = find_parameter("eps1", Statefile);
	eps2 = find_parameter("eps2", Statefile);
	porom = find_parameter("porom", Statefile);
	haut = find_parameter("haut", Statefile);
	larg = find_parameter("larg", Statefile);
	prof = find_parameter("prof", Statefile);
	ratio_f = find_parameter("ratio_f", Statefile);
	vit = find_parameter("vit", Statefile);
	Statefile.close();
	cout << "here1"<<endl;
	//cout << endl << "wszzh= " << wszzh << endl;

	/*GrainIterator grains_end = grains.end();
	for (GrainIterator it=grains.begin(); it!=grains_end; ++it)
	{
	 if (it==grains.begin()) ++it;
	 Vue1.Dessine_Sphere(it->sphere.x(), it->sphere.y(), it->sphere.z(), it->sphere.weight(), 10);
	}*/
	//Vue1.Affiche();

	return true;
}



bool TriaxialState::to_file(const char* filename)
{
	ofstream Statefile (filename);
	cout << filename << endl;
	if (!Statefile.is_open())	{
		cout << "Error opening files";
		return false;	}

		long Id_max = grains.size()-1;
		Statefile << Id_max << endl;
// 		Point pos;
// 		mean_radius=0;
// 		Vecteur trans, rot;
		//Real rad; //coordonn�es/rayon

		
		for (long Idg=0 ; Idg <= Id_max ; ++Idg) 
		{
			Statefile << grains[Idg].id <<	" " << grains[Idg].sphere.point() << " " << grains[Idg].sphere.weight() << " " << grains[Idg].translation << " " << grains[Idg].rotation << " "<<grains[Idg].isSphere << endl;
		}
		
		long Nc = contacts.size();
		Statefile << Nc << endl;
		for (long i=0 ; i < Nc ; ++i)
		{			
			Statefile << contacts[i]->grain1->id << " " << contacts[i]->grain2->id << " " << contacts[i]->normal << " " << contacts[i]->position << " " << contacts[i]->old_fn << " " << contacts[i]->old_fs << " " << contacts[i]->fn << " " << contacts[i]->fs << " " << contacts[i]->frictional_work << " " << contacts[i]->status << endl;
		}

		Statefile << "Eyn " << Eyn << " Eys " << Eys << " wszzh " << wszzh << " wsxxd " << wsxxd << " wsyyfa " << wsyyfa << " eps3 " << eps3 << " eps1 " << eps1 << " eps2 " << eps2 << " porom " << porom << " haut " << haut << " larg " << larg << " prof " << prof << " ratio_f " << ratio_f << " vit " << vit << endl;

		Statefile.close();
		return true;
}

} // namespace CGT
