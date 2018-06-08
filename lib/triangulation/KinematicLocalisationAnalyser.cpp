/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//This class computes statistics of micro-variables assuming axi-symetry
// #define		VERTEX_INFO_TYPE	Vertex_Info;
// #define		CELL_INFO_TYPE		Cell_Info;

#include "KinematicLocalisationAnalyser.hpp" //This one first, because it defines the info types
// #include "Tesselation.h"
// #include "TriaxialState.h"
#include <sstream>
#include "basicVTKwritter.hpp"
//#include <utility>

namespace CGT
{

int n_debug = 0;
std::string _itoa(int i)
{
	std::ostringstream buffer;
	buffer << i;
	return buffer.str();
}

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser()
{
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	consecutive = false;
	bz2=true;
	TS1=new TriaxialState;
	TS0=new TriaxialState;
}


KinematicLocalisationAnalyser::~KinematicLocalisationAnalyser()
{
	delete(TS1);
	delete(TS0);
}

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser(const char* state_file1, bool usebz2)
{
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	consecutive = false;
	bz2=true;
	TS1 = new(TriaxialState);
	TS0 = NULL;
	TS1->from_file(state_file1,/*use bz2?*/ bz2);
}

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser(const char*
		state_file1, const char* state_file0, bool consecutive_files, bool usebz2)
{
	consecutive = consecutive_files;
	bz2 = usebz2;
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	TS1 = new(TriaxialState);
	TS0 = new(TriaxialState);
	TS1->from_file(state_file1,/*use bz2?*/ bz2);
	TS0->from_file(state_file0,/*use bz2?*/ bz2);

	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;
}

const vector<Tenseur3>& KinematicLocalisationAnalyser::computeParticlesDeformation(const char* state_file1, const char* state_file0, bool usebz2)
{
	consecutive = false;
	bz2 = usebz2;
	TS1->from_file(state_file1,/*use bz2?*/ bz2);
	TS0->from_file(state_file0,/*use bz2?*/ bz2);
	//FIXME: redundant?
	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;
	return computeParticlesDeformation();
}


KinematicLocalisationAnalyser::KinematicLocalisationAnalyser(const char* base_name, int n0, int n1, bool usebz2)
{
	file_number_1 = n1;
	file_number_0 = n0;
	base_file_name = string(base_name);
	consecutive = ((n1-n0)==1);
	bz2 = usebz2;
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	TS1 = new(TriaxialState);
	TS0 = new(TriaxialState);
	std::ostringstream file_name1, file_name0;
	file_name1 << (string)(base_file_name) << n1;
	file_name0 << (string)(base_file_name) << n0;
	TS1->from_file(file_name1.str().c_str(), bz2);
	TS0->from_file(file_name0.str().c_str(), bz2);
	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;
}

void KinematicLocalisationAnalyser::SetBaseFileName(string name)
{
	base_file_name = name;
}

bool KinematicLocalisationAnalyser::SetFileNumbers(int n0, int n1)
{
	bool bf0 = false;
	bool bf1 = false;
	if (file_number_0 != n0) {
		if (file_number_1 != n0) {
			//file_name = base_file_name + n0;
			bf0 = TS0->from_file((base_file_name +_itoa(file_number_0)).c_str(), bz2);
		} else {
			delete(TS0);
			TS0 = TS1;
			bf0=true;
			TS1 = new(TriaxialState);
			//file_name = base_file_name + string(n1);
			bf1 = TS1->from_file((base_file_name + _itoa(file_number_1)).c_str(), bz2);
		}
	} else if (n1 != file_number_1) {
		//file_name = base_file_name + string(n1);
		bf0 = true;
		bf1 = TS1->from_file((base_file_name + _itoa(file_number_1)).c_str(), bz2);}
	file_number_1 = n1;
	file_number_0 = n0;
	consecutive = ((n1-n0) ==1);
	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;
	return (bf0 && bf1);
}

void KinematicLocalisationAnalyser::SetConsecutive(bool t)
{
	consecutive = t;
}

void KinematicLocalisationAnalyser::SetNO_ZERO_ID(bool t)
{
	TS0->NO_ZERO_ID = t;
	TS1->NO_ZERO_ID = t;
}

void KinematicLocalisationAnalyser::SwitchStates(void)
{
	TriaxialState* TStemp = TS0;
	TS0 = TS1;
	TS1 = TStemp;
}

vector<KinematicLocalisationAnalyser::Edge_iterator>& KinematicLocalisationAnalyser::Oriented_Filtered_edges(Real Nymin, Real Nymax, vector<Edge_iterator>& filteredList)
{
	RTriangulation& T = TS1->tesselation().Triangulation();
	filteredList.clear();
	Edge_iterator ed_end = T.edges_end();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)
				&& TS1->inside(T.segment(*ed_it).source())
				&& TS1->inside(T.segment(*ed_it).target())) {
			Segment s = T.segment(*ed_it);
			CVector v = s.to_vector();
			Real ny = std::abs(v.y()/sqrt(s.squared_length()));

			if (Nymin < ny && ny <= Nymax) filteredList.push_back(ed_it);
		}
	}
	return filteredList;
}

bool KinematicLocalisationAnalyser::DefToFile(const char* state_file1, const char* state_file0, const char* output_file_name, bool usebz2)
{
	consecutive = false;
	bz2 = usebz2;
	TS1->from_file(state_file1,/*use bz2?*/ bz2);
	TS0->from_file(state_file0,/*use bz2?*/ bz2);
	DefToFile(output_file_name);
	return 0;
}

bool KinematicLocalisationAnalyser::DefToFile(const char* output_file_name)
{
	computeParticlesDeformation();
	Tesselation& Tes = TS1->tesselation();
	RTriangulation& Tri = Tes.Triangulation();
	basicVTKwritter vtk(n_real_vertices, n_real_cells);
	vtk.open(output_file_name, "Output file generated by Yade's KinematicLocalisationAnalyser");

	vtk.begin_vertices();
	RTriangulation::Finite_vertices_iterator  V_it = Tri.finite_vertices_begin();
	bool beginWithFictious = V_it->info().isFictious;
	for (; V_it !=  Tri.finite_vertices_end(); ++V_it) if (!V_it->info().isFictious) vtk.file <<  V_it->point().point() << endl;
	vtk.end_vertices();

	vtk.begin_cells();
	Finite_cells_iterator cell = Tri.finite_cells_begin();
	//FIXME : Preconditions : the fictious bounds are first in the list
	for (; cell != Tri.finite_cells_end(); ++cell) {
		if (!cell->info().isFictious) vtk.write_cell(
			cell->vertex(0)->info().id()- (beginWithFictious?n_fictious_vertices:0),
			cell->vertex(1)->info().id()- (beginWithFictious?n_fictious_vertices:0),
			cell->vertex(2)->info().id()- (beginWithFictious?n_fictious_vertices:0),
			cell->vertex(3)->info().id()- (beginWithFictious?n_fictious_vertices:0)
		);
	}
	vtk.end_cells();
	vtk.begin_data("Strain_matrix", POINT_DATA, TENSORS, FLOAT);
	V_it = Tri.finite_vertices_begin();
	for (; V_it !=  Tri.finite_vertices_end(); ++V_it) {
		if (!V_it->info().isFictious)  {
			Tenseur_sym3 epsilon(ParticleDeformation[V_it->info().id()]);
			vtk.file << ParticleDeformation[V_it->info().id()] << endl;}
	}
	vtk.end_data();
	vtk.begin_data("Strain_deviator", POINT_DATA, SCALARS, FLOAT);
	V_it = Tri.finite_vertices_begin();
	for (; V_it !=  Tri.finite_vertices_end(); ++V_it) {
		if (!V_it->info().isFictious)  {
			Tenseur_sym3 epsilon(ParticleDeformation[V_it->info().id()]);
			
			vtk.write_data((float) epsilon.Deviatoric().Norme());}
			//vtk.write_data((float) epsilon.Deviatoric()(1,1)-epsilon.Deviatoric()(0,0));}
	}
	vtk.end_data();
	vtk.close();
	return true;
}

bool KinematicLocalisationAnalyser::DistribsToFile(const char* output_file_name)
{
	ofstream output_file(output_file_name);
	if (!output_file.is_open()) {
		cerr << "Error opening files";
		return false;}

	output_file << "sym_grad_u_total_g (wrong averaged strain):"<< endl << Tenseur_sym3(grad_u_total_g) << endl;
	output_file << "Total volume = " << v_total << ", grad_u = " << endl << grad_u_total << endl << "sym_grad_u (true average strain): " << endl << Tenseur_sym3(grad_u_total) << endl;
	output_file << "Macro strain = " << Delta_epsilon << endl;

	ContactDistributionToFile(output_file);
	AllNeighborDistributionToFile(output_file);

	TS1->filter_distance = 2.0;
	ContactDistributionToFile(output_file);
	AllNeighborDistributionToFile(output_file);

	TS1->filter_distance = 4.0;
	ContactDistributionToFile(output_file);
	AllNeighborDistributionToFile(output_file);

	output_file << "Contact_fabric : ";
	output_file << (Tenseur_sym3) Contact_fabric(*TS1);// << endl;
	output_file << "Contact_anisotropy : " << Contact_anisotropy(*TS1) << endl << endl;
	output_file << "Neighbor_fabric : " << Neighbor_fabric(*TS1) << endl;
	output_file << "Neighbor_anisotropy : " << Neighbor_anisotropy(*TS1) << endl << endl;

	RTriangulation& T = TS1->tesselation().Triangulation();
	Edge_iterator ed_end = T.edges_end();
	vector<Edge_iterator> edges;
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)) {
			Segment s = T.segment(*ed_it);
			CVector v = s.to_vector();
			Real xx = std::abs(v.z()/sqrt(s.squared_length()));

			if (xx>0.95) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);

	edges.clear();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)) {
			Segment s = T.segment(*ed_it);
			CVector v = s.to_vector();
			Real xx = std::abs(v.z()/sqrt(s.squared_length()));

			if (xx<0.05) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);

	edges.clear();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)) {
			Segment s = T.segment(*ed_it);
			CVector v = s.to_vector();
			Real xx = std::abs(v.z()/sqrt(s.squared_length()));

			if (xx>0.65 && xx<0.75) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);
	output_file.close();
	return true;
}

long KinematicLocalisationAnalyser::Filtered_contacts(TriaxialState& state)
{
	long nc1 =0;
	TriaxialState::ContactIterator cend = state.contacts_end();
	for (TriaxialState::ContactIterator cit = state.contacts_begin(); cit!=cend; ++cit) {
		if (state.inside((*cit)->grain1->sphere.point()) && state.inside((*cit)->grain2->sphere.point()))
			nc1 += 2;
		else if (state.inside((*cit)->grain1->sphere.point()) || state.inside((*cit)->grain2->sphere.point()))
			++nc1;}
	return nc1;
}

long KinematicLocalisationAnalyser::Filtered_neighbors(TriaxialState& state)
{
	long nv1=0;
	RTriangulation& T = state.tesselation().Triangulation();
	Edge_iterator ed_end = T.edges_end();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)) {
			Segment s(T.segment(*ed_it));
			if (state.inside(s.source()) &&
					state.inside(s.target())) nv1 += 2;
			else if (state.inside(s.source()) ||
					 state.inside(s.target()))  ++nv1;
		}
	}
	return nv1;
}

long KinematicLocalisationAnalyser::Filtered_grains(TriaxialState& state)
{
	long ng1 =0;
	TriaxialState::GrainIterator gend = state.grains_end();
	for (TriaxialState::GrainIterator git = state.grains_begin(); git!=gend;
			++git) {
		if (state.inside(git->sphere.point())) ++ng1;
	}
	return ng1;
}

Real KinematicLocalisationAnalyser::Filtered_volume(TriaxialState& state)
{
	return 0;
}

Real KinematicLocalisationAnalyser::Contact_coordination(TriaxialState& state)
{
	return Filtered_contacts(state) / Filtered_grains(state);
}

Real KinematicLocalisationAnalyser::Neighbor_coordination(TriaxialState& state)
{
	return Filtered_neighbors(state) / Filtered_grains(state);
}


Tenseur_sym3 KinematicLocalisationAnalyser::Neighbor_fabric(TriaxialState&
		state)
{
	RTriangulation& T = state.tesselation().Triangulation();
	Edge_iterator ed_end = T.edges_end();
	Tenseur_sym3 Tens;
	CVector v;
	Segment s;
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it) {
		if (!T.is_infinite(*ed_it)) {
			s = T.segment(*ed_it);
			if (state.inside(s.source()) &&
					state.inside(s.target())) {
				v =
					T.segment(*ed_it).to_vector() * (1/sqrt(T.segment(*ed_it).squared_length()));
				for (int i=1; i<4; i++) for (int j=3; j>=i; j--)
						Tens(i,j) += 2*v[i-1]*v[j-1];
			} else if (state.inside(s.source()) ||
					   state.inside(s.target())) {
				v =
					T.segment(*ed_it).to_vector() * (1/sqrt(T.segment(*ed_it).squared_length()));
				for (int i=1; i<4; i++) for (int j=3; j>=i; j--)
						Tens(i,j) += v[i-1]*v[j-1];
			}
		}
	}
	Tens /= Filtered_neighbors(state);
	return Tens;
}

Tenseur_sym3 KinematicLocalisationAnalyser::Contact_fabric(TriaxialState&
		state)
{
	Tenseur_sym3 Tens;
	CVector v;
	TriaxialState::ContactIterator cend = state.contacts_end();

	for (TriaxialState::ContactIterator cit = state.contacts_begin();
			cit!=cend; ++cit) {
		if (state.inside((*cit)->grain1->sphere.point()) &&
				state.inside((*cit)->grain2->sphere.point())) {
			v = (*cit)->normal;
			for (int i=1; i<4; i++) for (int j=3; j>=i; j--)
					Tens(i,j) += 2*v[i-1]*v[j-1];
		} else if (state.inside((*cit)->grain1->sphere.point()) ||
				   state.inside((*cit)->grain2->sphere.point())) {
			v = (*cit)->normal;
			for (int i=1; i<4; i++) for (int j=3; j>=i; j--)
					Tens(i,j) += v[i-1]*v[j-1];}
	}
	Tens /= Filtered_contacts(state);
	return Tens;
}

Real KinematicLocalisationAnalyser::Contact_anisotropy(TriaxialState& state)
{
	Tenseur_sym3 tens(Contact_fabric(state));
	return tens.Deviatoric().Norme()/tens.Trace();
}

Real KinematicLocalisationAnalyser::Neighbor_anisotropy(TriaxialState& state)
{
	Tenseur_sym3 tens(Neighbor_fabric(state));
	return tens.Deviatoric().Norme()/tens.Trace();
}

vector<pair<Real,Real> >& KinematicLocalisationAnalyser::
NormalDisplacementDistribution(vector<Edge_iterator>& edges, vector<pair<Real,Real> > &row)
{
	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
	row.clear();
	row.resize(linear_discretisation+1);
	vector<Real> Un_values;
	Un_values.resize(edges.size());
	Real UNmin(100000), UNmax(-100000);
	CVector branch, U;
	Real Un;
	Vertex_handle Vh1, Vh2;
	vector<Edge_iterator>::iterator ed_end = edges.end();
	long val_count = 0;

	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
	for (vector<Edge_iterator>::iterator ed_it = edges.begin();
			ed_it!=ed_end; ++ed_it) {
		Vh1= (*ed_it)->first->vertex((*ed_it)->second);
		Vh2= (*ed_it)->first->vertex((*ed_it)->third);
		branch = Vh1->point().point()- Vh2->point().point();
		NORMALIZE(branch);
		if (consecutive)
			U = TS1->grain(Vh1->info().id()).translation -
				TS1->grain(Vh2->info().id()).translation;
		else {
			U = (TS1->grain(Vh1->info().id()).sphere.point() -
				 TS0->grain(Vh1->info().id()).sphere.point())
				- (TS1->grain(Vh2->info().id()).sphere.point() -
				   TS0->grain(Vh2->info().id()).sphere.point());
		}
		//Un = (U - (Delta_epsilon*branch))*branch; //Diff�rence par rapport � Un moyen
		Un = U*branch;

		UNmin = min(UNmin,Un);
		UNmax = max(UNmax,Un);
		Un_values[val_count++] = Un;
		//cerr << "Un=" << Un << " U=" << U << " branch=" << branch <<  endl;
	}
	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///

	Real DUN = (UNmax-UNmin) /linear_discretisation;
	for (int i = 0; i <= linear_discretisation; ++i) {
		row[i].first = UNmin+ (i+0.5) *DUN;
		row[i].second = 0;
	}
	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///

	val_count = val_count-1;
	//cerr << "nval=" << val_count << " reserved=" << edges.size() << endl;
	for (; val_count>=0; --val_count) {
		//cerr << "n_debug0=" << n_debug << endl;   /// DEBUG LINE  ///
		row[(int)((Un_values[val_count]-UNmin) /DUN)].second += 1;
	}
	//cerr << "DUN=" << DUN << " UNmin=" << UNmin << " UNmax=" << UNmax << endl;
	return row;
	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
}


ofstream& KinematicLocalisationAnalyser::
NormalDisplacementDistributionToFile(vector<Edge_iterator>& edges, ofstream& output_file)
{
	vector< pair<Real, Real> > row;
	NormalDisplacementDistribution(edges, row);
	vector< pair<Real, Real> >::iterator r_end = row.end();

	//output part :
	output_file << "#Normal displacement distribution" << endl << "eps3=" << Delta_epsilon(3,3)
	<< " eps2=" << Delta_epsilon(2,2) << " eps1=" << Delta_epsilon(1,1) << " number of neigbors: "<< edges.size()
	<< endl << "Un_min=" << 1.5*row[0].first - 0.5*row[1].first << " Un_max="
	<< row[row.size()-1].first << endl;
	cout << "#Normal displacement distribution" << endl << "eps3=" << Delta_epsilon(3,3)
	<< " eps2=" << Delta_epsilon(2,2) << " eps1=" << Delta_epsilon(1,1) << " number of neigbors: "<< edges.size()
	<< endl << "Un_min=" << 1.5*row[0].first - 0.5*row[1].first << " Un_max="
	<< row[row.size()-1].first << endl;
	for (vector< pair<Real, Real> >::iterator r_it = row.begin(); r_it != r_end; ++r_it) {
		output_file << r_it->first << " " << r_it->second << endl;
		cout << r_it->first << " " << r_it->second << endl;
	}
	output_file << endl;
	return output_file;
}

ofstream& KinematicLocalisationAnalyser::
ContactDistributionToFile(ofstream& output_file)
{
	//cerr << "ContactDistributionToFile" << endl;
	vector< pair<Real, Real> > row;
	row.resize(sphere_discretisation+1);
	Real DZ = 1.0/sphere_discretisation;//interval in term of cos(teta)
	long nc1=0;
	long nc2=0;
	long ng1=0;
	long ng2=0;
	//cerr << "ContactDistributionToFile05" << endl;
	TriaxialState::ContactIterator cend = (*TS1).contacts_end();
	TriaxialState::GrainIterator gend = (*TS1).grains_end();

	for (int i = 0; i <= sphere_discretisation; ++i) {
		row[i].first = (i+0.5) *DZ;
		row[i].second = 0;
	}

	for (TriaxialState::GrainIterator git = (*TS1).grains_begin(); git!=gend; ++git) {
		if ((*TS1).inside(git->sphere.point())) ++ng1;
		else ++ng2;
	}

	for (TriaxialState::ContactIterator cit = (*TS1).contacts_begin(); cit!=cend; ++cit) {
		if ((*TS1).inside((*cit)->grain1->sphere.point()) && (*TS1).inside((*cit)->grain2->sphere.point())) {
			row[(int)(std::abs((*cit)->normal.z()) /DZ)].second += 2;
			nc1 += 2;
		} else {
			if ((*TS1).inside((*cit)->grain1->sphere.point()) || (*TS1).inside((*cit)->grain2->sphere.point())) {
				row[(int)(std::abs((*cit)->normal.z()) /DZ)].second += 1;
				++nc1;
			}
			//cerr << "(*cit)->normal.z(),DZ : " << (*cit)->normal.z() << " " << DZ << endl;}
			else ++nc2;
		}
	}
	//normalisation :
	Real normalize = 1.0/ (ng1*4*DZ*3.141592653);
	for (int i = 0; i <= sphere_discretisation; ++i) row[i].second *= normalize;

	//output part :
	output_file << "#Contacts distribution" << endl << "(filter dist. = " << (*TS1).filter_distance
	<< ", "<< nc1 << " contacts, " << nc2 << " excluded contacts, for "<< ng1
	<<"/"<< (ng1+ng2) << " grains)" << endl;
	output_file << "max_nz number_of_contacts" << endl;
	cerr << "#Contacts distribution (filter dist. = " << (*TS1).filter_distance
	<< ", "<< nc1 << " contacts, " << nc2 << " excluded contacts, for "<< ng1
	<<"/"<< (ng1+ng2) << " grains)" << endl;
	cerr << "mean_nz number_of_contacts" << endl;
	for (int i = 0; i <= sphere_discretisation; ++i) {
		output_file << row[i].first << " " << row[i].second << endl;
		cerr << row[i].first << " " << row[i].second << endl;
	}
	output_file << endl;
	return output_file;
}


ofstream& KinematicLocalisationAnalyser::
AllNeighborDistributionToFile(ofstream& output_file)
{
	vector< pair<Real, Real> > row;
	row.resize(sphere_discretisation);
	Real DZ = 1.0/sphere_discretisation;
	long nv1=0;
	long nv2=0;
	long nv3=0;
	long ng1=0;
	long ng2=0;

	for (int i = 0; i < sphere_discretisation; ++i) {
		row[i].first = (i+0.5) *DZ;
		row[i].second = 0;
	}

	TriaxialState::GrainIterator gend = (*TS1).grains_end();
	for (TriaxialState::GrainIterator git = (*TS1).grains_begin(); git!=gend; ++git) {
		if ((*TS1).inside(git->sphere.point())) ++ng1;
		else ++ng2;
	}

	RTriangulation& T = (*TS1).tesselation().Triangulation();
	Segment s;
	CVector v;
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != T.edges_end(); ed_it++) {
		if (!T.is_infinite(*ed_it)) {
			s = T.segment(*ed_it);
			if ((*TS1).inside(s.source()) && (*TS1).inside(s.target())) {
				v = s.to_vector();
				row[(int)(std::abs(v.z() /sqrt(s.squared_length())) /DZ)].second += 2;
				nv1 += 2;
			} else {
				if ((*TS1).inside(s.source()) || (*TS1).inside(s.target())) {
					v = s.to_vector();
					row[(int)(std::abs(v.z() /sqrt(s.squared_length())) /DZ)].second += 1;
					++nv1;
				} else ++nv2;
			}
		} else ++nv3;
	}

	Real normalize = 1.0/ (ng1*4*DZ*3.141592653);
	for (int i = 0; i < sphere_discretisation; ++i) row[i].second *= normalize;

	output_file << "#Neighbors distribution" << endl << "(filter dist. = " << (*TS1).filter_distance
	<< ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	<< nv3 << " infinite, for "<< ng1 <<"/"<< (ng1+ng2) << " grains)" << endl;
	output_file << "max_nz number_of_neighbors" << endl;
	cerr << "#Neighbors distribution" << endl << "(filter dist. = " << (*TS1).filter_distance
	<< ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	<< nv3 << " infinite, for "<< ng1 <<"/"<< (ng1+ng2) << " grains)" << endl;
	cerr << "mean_nz number_of_neighbors" << endl;
	for (int i = 0; i < sphere_discretisation; ++i) {
		output_file << row[i].first << " " << row[i].second << endl;
		cerr << row[i].first << " " << row[i].second << endl;
	}
	output_file << endl;
	return output_file;
}

void KinematicLocalisationAnalyser::
SetForceIncrements(void)    //WARNING : This function will modify the contact lists : add virtual (lost)) contacts in state 1 and modify old_force and force in state 0, execute this function after all other force analysis functions if you want to avoid problems
{
	//if (true) cerr << "SetForceIncrements"<< endl;
//  vector< pair<Real, Real> > row;
//  row.resize ( sphere_discretisation );
//  Real DZ = 1.0/sphere_discretisation;
	long Nc0 = TS0->contacts.size();
	long Nc1 = TS1->contacts.size();
	n_persistent = 0; n_new = 0; n_lost = 0;
	long lost_in_state0 = 0;

	for (int i = 0; i < Nc0; ++i) {
		TS0->contacts[i]->visited = false;
		if (TS0->contacts[i]->status == TriaxialState::Contact::LOST) ++lost_in_state0;
	}
	for (int i = 0; i < Nc1; ++i) TS1->contacts[i]->visited = false;
	//cerr << "Nc1 "<<Nc1<<", Nc0 "<<Nc0<<" ("<<Nc0-lost_in_state0<<" real)"<<endl;
	for (int i = 0; i < Nc0; ++i) {
		// cerr << 1;
		if (TS0->contacts[i]->status != TriaxialState::Contact::LOST) {
			//  cerr << 2;
			for (int j = 0; j < Nc1; ++j) {

				if (TS0->contacts[i]->grain1->id == TS1->contacts[j]->grain1->id && TS0->contacts[i]->grain2->id == TS1->contacts[j]->grain2->id) { // This is a PERSISTENT contact (i.e. it is present in state 0 and 1)
					//TS0->contacts[i]->visited = true;
					TS1->contacts[j]->visited = true;
					//TS0->contacts[i]->status = TriaxialState::Contact::PERSISTENT;
					TS1->contacts[j]->status = TriaxialState::Contact::PERSISTENT;
					TS1->contacts[j]->old_fn = TS0->contacts[i]->fn;
					TS1->contacts[j]->old_fs = TS0->contacts[i]->fs;
					++n_persistent;
					break;
				} else if (j+1==Nc1) { //This contact was not found in state 1, add it as a LOST contact
					//    cerr << 3 << endl;
					TriaxialState::Contact* c = new TriaxialState::Contact;
					c->visited = true;
					c->status = TriaxialState::Contact::LOST;
					c->grain1 = TS0->contacts[i]->grain1;
					c->grain2 = TS0->contacts[i]->grain2;
					c->position = TS0->contacts[i]->position;
					c->normal = TS0->contacts[i]->normal;
					c->old_fn = TS0->contacts[i]->fn;
					c->fn = 0;
					c->old_fs = TS0->contacts[i]->fs;
					c->frictional_work = TS0->contacts[i]->frictional_work;
					c->fs = CGAL::NULL_VECTOR;
					TS1->contacts.push_back(c);
					++Nc1;
					++n_lost;
					break;
				}
			}
		}
	}
	//cerr << 4;
	for (int j = 0; j < Nc1; ++j) { //This contact was not visited, it is a NEW one
		//cerr << 5;
		if (!TS1->contacts[j]->visited /*&& TS1->contacts[j]->status != TriaxialState::Contact::LOST*/) {
			//cerr << 6;
			TS1->contacts[j]->status = TriaxialState::Contact::NEW;
			TS1->contacts[j]->old_fn = 0;
			TS1->contacts[j]->old_fs = CGAL::NULL_VECTOR;
			++n_new;
		}
	}
}

void KinematicLocalisationAnalyser::SetDisplacementIncrements(void)
{
	TriaxialState::GrainIterator gend = TS1->grains_end();
	for (TriaxialState::GrainIterator git = TS1->grains_begin(); git!=gend; ++git)
		if (git->id >= 0) git->translation =  TS1->grain(git->id).sphere.point() - TS0->grain(git->id).sphere.point();
	consecutive = true;
}

ofstream& KinematicLocalisationAnalyser::
StrictNeighborDistributionToFile(ofstream& output_file)
{
	return output_file;
}

CVector KinematicLocalisationAnalyser::Deplacement(Finite_cells_iterator cell, int facet)  
{
	CVector v(0.f, 0.f, 0.f);
	int id;// ident. de la particule
	CVector fixedPoint = 0.5*((TS0->box.base-CGAL::ORIGIN)+(TS0->box.sommet-CGAL::ORIGIN));
	for (int i=0; i<4; i++) {
		//  char msg [256];
		if (i!=facet) {
			id = cell->vertex(i)->info().id();
			CVector meanFieldDisp =CVector(TS0->grain(id).sphere.point().x(), TS0->grain(id).sphere.point().y(), TS0->grain(id).sphere.point().z())-fixedPoint;
			if (1){//fluctuations
				meanFieldDisp = CVector(
				meanFieldDisp[0]*Delta_epsilon(1,1),
				meanFieldDisp[1]*Delta_epsilon(2,2),
				meanFieldDisp[2]*Delta_epsilon(3,3));
			} else meanFieldDisp=CVector(0,0,0);
			if (consecutive) v = v + TS1->grain(id).translation-meanFieldDisp;
			else  v = v + (TS1->grain(id).sphere.point() - TS0->grain(id).sphere.point()-meanFieldDisp);
		}
	}
	v = v*0.333333333333;
	return v;
}

void KinematicLocalisationAnalyser::Grad_u(Finite_cells_iterator cell, int facet, CVector &V, Tenseur3& T)
{
	CVector S = cross_product((cell->vertex(l_vertices[facet][1])->point().point())
							  - (cell->vertex(l_vertices[facet][0])->point().point()),
							  (cell->vertex(l_vertices[facet][2])->point().point()) -
							  (cell->vertex(l_vertices[facet][1])->point().point())) /2.f;

	Somme(T, V, S);
}

void KinematicLocalisationAnalyser::Grad_u(Finite_cells_iterator cell,
		Tenseur3& T, bool vol_divide)// Calcule le gradient de d�p.
{
	T.reset();
	CVector v;
	for (int facet=0; facet<4; facet++) {
		v = Deplacement(cell, facet);
		Grad_u(cell, facet, v, T);
	}
	if (vol_divide) T/= Tesselation::Volume(cell);
}

const vector<Tenseur3>& KinematicLocalisationAnalyser::computeParticlesDeformation(void)
{
	Tesselation& Tes = TS1->tesselation();
	RTriangulation& Tri = Tes.Triangulation();
	Tenseur3 grad_u;
	Real v;
	v_total = 0;
	v_solid_total = 0;
	grad_u_total = NULL_TENSEUR3;
	v_total_g = 0;
	grad_u_total_g = NULL_TENSEUR3;
	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;

	//compute Voronoi tesselation (i.e. voronoi center of each cell)
	if (!Tes.computed) Tes.compute();
	if (ParticleDeformation.size() != (unsigned int)(Tes.Max_id() + 1)) {
		ParticleDeformation.clear();
		ParticleDeformation.resize(Tes.Max_id() + 1);
	}
	//reset volumes and tensors of each particle
	n_real_vertices = 0; n_fictious_vertices=0;
	for (RTriangulation::Finite_vertices_iterator  V_it=Tri.finite_vertices_begin(); V_it !=  Tri.finite_vertices_end(); V_it++) {
		//cerr << V_it->info().id() << endl;
		V_it->info().v() =0;//WARNING : this will erase previous values if some have been computed
		ParticleDeformation[V_it->info().id()]=NULL_TENSEUR3;
		if (!V_it->info().isFictious) ++n_real_vertices; else ++n_fictious_vertices;
	}
	Finite_cells_iterator cell = Tri.finite_cells_begin();
	Finite_cells_iterator cell0 = Tri.finite_cells_end();
	
	//compute grad_u and volumes of all cells in the triangulation, and assign them to each of the vertices ( volume*grad_u is added here rather than grad_u, the weighted average is computed later )
	//define the number of non-fictious cells, i.e. not in contact with a boundary
	n_real_cells=0;
	for (; cell != cell0; cell++) { // calcule la norme du d�viateur dans chaque cellule
		cell->info().isFictious = (cell->vertex(0)->info().isFictious || cell->vertex(1)->info().isFictious || cell->vertex(2)->info().isFictious || cell->vertex(3)->info().isFictious);
		if (!cell->info().isFictious) { 
			Grad_u(cell, grad_u, false);   // false : don't divide by volume, here grad_u = volume of cell * average grad_u in cell, the final value is divided by the total volume later (see below)
			//cerr << "grad_u=" << grad_u << endl;
			v = Tri.tetrahedron(cell).volume();
			grad_u_total += grad_u; v_total += v; ++n_real_cells;
			for (unsigned int index=0; index<4; index++) {
				cell->vertex(index)->info().v() += v;   //WARNING2 : this will affect values which differ from the volumes of voronoi cells
				//cerr << "ParticleDeformation[cell->vertex (" << cell->vertex ( index )->info().id() << ")"<< endl;
				ParticleDeformation[cell->vertex(index)->info().id()] += grad_u;
			}
		}
	}
	//Do we delete volume and grad_u for particles on the border?
//  Tesselation::Vector_Vertex border_vertices;
//  Tes.Voisins(Tri.infinite_vertex(), border_vertices);
//  unsigned int l = border_vertices.size();
//  for (unsigned int i=0; i<l; ++i) {
//   //cerr << "border " << i << endl;
//   border_vertices[i]->info().v() =0;
//
//   ParticleDeformation[border_vertices[i]->info().id()]=NULL_TENSEUR3;
//  }

	//Divide sum(v*grad_u) by sum(v) to get the average grad_u on each particle
	for (RTriangulation::Finite_vertices_iterator  V_it = Tri.finite_vertices_begin(); V_it !=  Tri.finite_vertices_end(); V_it++) {
		v_total_g += V_it->info().v();
		v_solid_total += 4.188790*pow(V_it->point().weight(),1.5);//4.18... = 4/3*PI; and here, weight is rad²
		grad_u_total_g += ParticleDeformation[V_it->info().id()];
		if (V_it->info().v()) ParticleDeformation[V_it->info().id()]/=V_it->info().v();
	}
	grad_u_total_g /= v_total_g;
	if(1){
	cerr << "sym_grad_u_total_g (wrong averaged strain):"<< endl << Tenseur_sym3(grad_u_total_g) << endl;
	if (v_total) grad_u_total /= v_total;
	cerr << "Total volume = " << v_total << ", grad_u = " << endl << grad_u_total << endl << "sym_grad_u (true average strain): " << endl << Tenseur_sym3(grad_u_total) << endl;
	cerr << "Macro strain : "<< endl << Delta_epsilon << endl;
	}
	return ParticleDeformation;
}

Real KinematicLocalisationAnalyser::computeMacroPorosity(void)
{
	return (1-v_solid_total/(TS1->haut*TS1->larg*TS1->prof));
}

} // namespace CGT
