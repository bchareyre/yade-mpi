/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//This class computes statistics of micro-variables assuming axi-symetry



#include "Tesselation.h"
#include "KinematicLocalisationAnalyser.hpp"
#include "TriaxialState.h"
#include <iostream>
#include <fstream>
#include <sstream>
//#include <utility>

int n_debug = 0;
//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
 
 
//Usefull fonction to convert int to string (define it elsewhere) 
std::string _itoa(int i) {
 std::ostringstream buffer;
 buffer << i;
 return buffer.str();
} 

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser()
{
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	consecutive = true;
	TS1=new TriaxialState;
	TS0=new TriaxialState;
}


KinematicLocalisationAnalyser::~KinematicLocalisationAnalyser()
{
	delete (TS1);
	delete (TS0);
}

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser(const char*
state_file1)
{
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	consecutive = true;
	TS1 = new(TriaxialState);
	TS0 = NULL;
	TS1->from_file(state_file1);
}

KinematicLocalisationAnalyser::KinematicLocalisationAnalyser(const char*
state_file1, const char* state_file0, bool consecutive_files)
{
	consecutive = consecutive_files;
	sphere_discretisation = SPHERE_DISCRETISATION;
	linear_discretisation = LINEAR_DISCRETISATION;
	TS1 = new(TriaxialState);
	TS0 = new(TriaxialState);
	TS1->from_file(state_file1);
	TS0->from_file(state_file0);

	Delta_epsilon(3,3) = TS1->eps3 - TS0->eps3;
	Delta_epsilon(1,1) = TS1->eps1 - TS0->eps1;
	Delta_epsilon(2,2) = TS1->eps2 - TS0->eps2;
}



void KinematicLocalisationAnalyser::SetBaseFileName (string name)
{
	base_file_name = name;
}


bool KinematicLocalisationAnalyser::SetFileNumbers ( int n0, int n1 )
{
	//TriaxialState* TS3;
	//string file_name;
	bool bf0 = false;
	bool bf1 = false;
// char buffer [50];
	if ( file_number_0 != n0 )
	{
		if ( file_number_1 != n0 )
		{
			//file_name = base_file_name + n0;
			bf0 = TS0->from_file ( ( base_file_name +_itoa ( file_number_0 ) ).c_str() );
		}
		else
		{
			delete ( TS0 );
			TS0 = TS1;
			bf0=true;
			TS1 = new ( TriaxialState );
			//file_name = base_file_name + string(n1);
			bf1 = TS1->from_file ( ( base_file_name + _itoa ( file_number_1 ) ).c_str() );
		}
	}
	else if ( n1 != file_number_1 )
	{
		//file_name = base_file_name + string(n1);
		bf0 = true;
		bf1 = TS1->from_file ( ( base_file_name + _itoa ( file_number_1 ) ).c_str() );
	}

	file_number_1 = n1;
	file_number_0 = n0;
	consecutive = ( ( n1-n0 ) ==1 );
	Delta_epsilon ( 3,3 ) = TS1->eps3 - TS0->eps3;
	Delta_epsilon ( 1,1 ) = TS1->eps1 - TS0->eps1;
	Delta_epsilon ( 2,2 ) = TS1->eps2 - TS0->eps2;
	return ( bf0 && bf1 );
}


void KinematicLocalisationAnalyser::SetConsecutive ( bool t )
{
	consecutive = t;
}

void KinematicLocalisationAnalyser::SetNO_ZERO_ID (bool t)
{
	TS0->NO_ZERO_ID = t;
	TS1->NO_ZERO_ID = t;
}


void KinematicLocalisationAnalyser::SwitchStates (void ) 
{
	TriaxialState* TStemp = TS0;
	TS0 = TS1;
	TS1 = TStemp;
}

vector<Edge_iterator>& KinematicLocalisationAnalyser::Oriented_Filtered_edges (Real Nymin, Real Nymax, vector<Edge_iterator>& filteredList)
{
	RTriangulation& T = TS1->tesselation().Triangulation();
	filteredList.clear();
	Edge_iterator ed_end = T.edges_end();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it)
	{
		if (!T.is_infinite(*ed_it)
				   && TS1->inside ( T.segment ( *ed_it ).source() )
				   && TS1->inside ( T.segment ( *ed_it ).target() ) )
		{
			Segment s = T.segment(*ed_it);
			Vecteur v = s.to_vector();
			Real ny = abs( v.y()/sqrt(s.squared_length()));		
	
			if (Nymin < ny && ny <= Nymax) filteredList.push_back(ed_it);
		}
	}
	return filteredList;
}


bool KinematicLocalisationAnalyser::ToFile (const char* output_file_name)
{
	ofstream output_file (output_file_name);
	if (!output_file.is_open())	{
		cerr << "Error opening files";
		return false;	}
		
	output_file << "sym_grad_u_total_g (wrong averaged strain):"<< endl << Tenseur_sym3 ( grad_u_total_g ) << endl;
	output_file << "Total volume = " << v_total << ", grad_u = " << endl << grad_u_total << endl << "sym_grad_u (true average strain): " << endl << Tenseur_sym3 ( grad_u_total ) << endl;
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
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it)
	{
		if (!T.is_infinite(*ed_it))
		{
			Segment s = T.segment(*ed_it);
			Vecteur v = s.to_vector();
			Real xx = abs( v.z()/sqrt(s.squared_length()));		
	
			if (xx>0.95) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);

	edges.clear();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it)
	{
		if (!T.is_infinite(*ed_it))
		{
			Segment s = T.segment(*ed_it);
			Vecteur v = s.to_vector();
			Real xx = abs( v.z()/sqrt(s.squared_length()));		
	
			if (xx<0.05) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);

	edges.clear();
	for (Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it)
	{
		if (!T.is_infinite(*ed_it))
		{
			Segment s = T.segment(*ed_it);
			Vecteur v = s.to_vector();
			Real xx = abs( v.z()/sqrt(s.squared_length()));		
	
			if (xx>0.65 && xx<0.75) edges.push_back(ed_it);
		}
	}
	NormalDisplacementDistributionToFile(edges, output_file);


	output_file.close();
	return true;
}

long KinematicLocalisationAnalyser::Filtered_contacts (TriaxialState& state)
{
	long nc1 =0;
	TriaxialState::ContactIterator cend = state.contacts_end();
	for (TriaxialState::ContactIterator cit = state.contacts_begin(); cit!=cend; ++cit)
	{
		if (state.inside((*cit)->grain1->sphere.point()) && state.inside((*cit)->grain2->sphere.point()))
			nc1 += 2;
		else if (state.inside((*cit)->grain1->sphere.point()) || state.inside((*cit)->grain2->sphere.point()))	
			++nc1;			
	}
	return nc1;
}

long KinematicLocalisationAnalyser::Filtered_neighbors ( TriaxialState& state )
{
	long nv1=0;
	RTriangulation& T = state.tesselation().Triangulation();
	Edge_iterator ed_end = T.edges_end();
	for ( Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it )
	{
		if ( !T.is_infinite ( *ed_it ) )
		{
			Segment s ( T.segment ( *ed_it ) );
			if ( state.inside ( s.source() ) &&
					state.inside ( s.target() ) ) nv1 += 2;
			else if ( state.inside ( s.source() ) ||
					  state.inside ( s.target() ) )  ++nv1;
		}
	}
	return nv1;
}

long KinematicLocalisationAnalyser::Filtered_grains (TriaxialState& state)
{
	long ng1 =0;
	TriaxialState::GrainIterator gend = state.grains_end();
	for (TriaxialState::GrainIterator git = state.grains_begin(); git!=gend;
++git) {
		if (state.inside(git->sphere.point())) ++ng1;}
	return ng1;
}

Real KinematicLocalisationAnalyser::Filtered_volume (TriaxialState& state)
{
	return 0;
}

Real KinematicLocalisationAnalyser::Contact_coordination (TriaxialState& state)
{
	return Filtered_contacts(state) / Filtered_grains(state);
}

Real KinematicLocalisationAnalyser::Neighbor_coordination (TriaxialState& state)
{
	return Filtered_neighbors(state) / Filtered_grains(state);
}


Tenseur_sym3 KinematicLocalisationAnalyser::Neighbor_fabric ( TriaxialState&
		state )
{
	RTriangulation& T = state.tesselation().Triangulation();
	Edge_iterator ed_end = T.edges_end();
	Tenseur_sym3 Tens;
	Vecteur v;
	Segment s;
	for ( Edge_iterator ed_it = T.edges_begin(); ed_it != ed_end; ++ed_it )
	{
		if ( !T.is_infinite ( *ed_it ) )
		{
			s = T.segment ( *ed_it );
			if ( state.inside ( s.source() ) &&
					state.inside ( s.target() ) )
			{
				v =
					T.segment ( *ed_it ).to_vector() * ( 1/sqrt ( T.segment ( *ed_it ).squared_length() ) );
				for ( int i=1; i<4; i++ ) for ( int j=3; j>=i; j-- )
						Tens ( i,j ) += 2*v[i-1]*v[j-1];
			}
			else if ( state.inside ( s.source() ) ||
					  state.inside ( s.target() ) )
			{
				v =
					T.segment ( *ed_it ).to_vector() * ( 1/sqrt ( T.segment ( *ed_it ).squared_length() ) );
				for ( int i=1; i<4; i++ ) for ( int j=3; j>=i; j-- )
						Tens ( i,j ) += v[i-1]*v[j-1];
			}
		}
	}
	Tens /= Filtered_neighbors ( state );
	return Tens;
}

Tenseur_sym3 KinematicLocalisationAnalyser::Contact_fabric ( TriaxialState&
		state )
{
	Tenseur_sym3 Tens;
	Vecteur v;
	TriaxialState::ContactIterator cend = state.contacts_end();

	for ( TriaxialState::ContactIterator cit = state.contacts_begin();
			cit!=cend; ++cit )
	{
		if ( state.inside ( ( *cit )->grain1->sphere.point() ) &&
				state.inside ( ( *cit )->grain2->sphere.point() ) )
		{
			v = ( *cit )->normal;
			for ( int i=1; i<4; i++ ) for ( int j=3; j>=i; j-- )
					Tens ( i,j ) += 2*v[i-1]*v[j-1];
		}
		else if ( state.inside ( ( *cit )->grain1->sphere.point() ) ||
				  state.inside ( ( *cit )->grain2->sphere.point() ) )
		{
			v = ( *cit )->normal;
			for ( int i=1; i<4; i++ ) for ( int j=3; j>=i; j-- )
					Tens ( i,j ) += v[i-1]*v[j-1];
		}
	}
	Tens /= Filtered_contacts ( state );
	return Tens;
}





Real KinematicLocalisationAnalyser::Contact_anisotropy (TriaxialState& state)
{
	Tenseur_sym3 tens (Contact_fabric(state));
	return tens.Deviatoric().Norme()/tens.Trace();
}


Real KinematicLocalisationAnalyser::Neighbor_anisotropy (TriaxialState& state)
{
	Tenseur_sym3 tens (Neighbor_fabric(state));
	return tens.Deviatoric().Norme()/tens.Trace();
}

vector<pair<Real,Real> >& KinematicLocalisationAnalyser::
NormalDisplacementDistribution ( vector<Edge_iterator>& edges, vector<pair<Real,Real> > &row )
{
	cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
	row.clear();
	row.resize ( linear_discretisation+1 );
	vector<Real> Un_values;
	Un_values.resize ( edges.size() );
	Real UNmin ( 100000 ), UNmax ( -100000 );
	Vecteur branch, U;
	Real Un;
	Vertex_handle Vh1, Vh2;
	vector<Edge_iterator>::iterator ed_end = edges.end();
	long val_count = 0;

	cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
	for ( vector<Edge_iterator>::iterator ed_it = edges.begin();
			ed_it!=ed_end; ++ed_it )
	{
		Vh1= ( *ed_it )->first->vertex ( ( *ed_it )->second );
		Vh2= ( *ed_it )->first->vertex ( ( *ed_it )->third );
		branch = Vh1->point()- Vh2->point();
		NORMALIZE ( branch );
		if ( consecutive )
			U = TS1->grain ( Vh1->info().id() ).translation -
				TS1->grain ( Vh2->info().id() ).translation;
		else
		{
			U = ( TS1->grain ( Vh1->info().id() ).sphere.point() -
				  TS0->grain ( Vh1->info().id() ).sphere.point() )
				- ( TS1->grain ( Vh2->info().id() ).sphere.point() -
					TS0->grain ( Vh2->info().id() ).sphere.point() );
		}
		//Un = (U - (Delta_epsilon*branch))*branch; //Diff�rence par rapport � Un moyen
		Un = U*branch;
		
		UNmin = min ( UNmin,Un );
		UNmax = max ( UNmax,Un );
		Un_values[val_count++] = Un;
		//cerr << "Un=" << Un << " U=" << U << " branch=" << branch << 	endl;
	}
	cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///

	Real DUN = ( UNmax-UNmin ) /linear_discretisation;
	for ( int i = 0; i <= linear_discretisation; ++i )
	{
		row[i].first = UNmin+ ( i+0.5 ) *DUN;
		row[i].second = 0;
	}
	cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///

	val_count = val_count-1;
	cerr << "nval=" << val_count << " reserved=" << edges.size() << endl;
	for ( ; val_count>=0; --val_count )
	{
		//cerr << "n_debug0=" << n_debug << endl;   /// DEBUG LINE  ///
		row[ ( int ) ( ( Un_values[val_count]-UNmin ) /DUN ) ].second += 1;
	}
	cerr << "DUN=" << DUN << " UNmin=" << UNmin << " UNmax=" << UNmax << endl;
	return row;
	//cerr << "n_debug=" << n_debug++ << endl;   /// DEBUG LINE  ///
}


ofstream& KinematicLocalisationAnalyser::
NormalDisplacementDistributionToFile ( vector<Edge_iterator>& edges, ofstream& output_file )
{
	vector< pair<Real, Real> > row;
	NormalDisplacementDistribution ( edges, row );
	vector< pair<Real, Real> >::iterator r_end = row.end();

	//output part :
	output_file << "#Normal displacement distribution" << endl << "eps3=" << Delta_epsilon ( 3,3 )
	<< " eps2=" << Delta_epsilon ( 2,2 ) << " eps1=" << Delta_epsilon ( 1,1 ) << " number of neigbors: "<< edges.size()
	<< endl << "Un_min=" << 1.5*row[0].first - 0.5*row[1].first << " Un_max="
	<< row[row.size()-1].first << endl;
	cout << "#Normal displacement distribution" << endl << "eps3=" << Delta_epsilon ( 3,3 )
	<< " eps2=" << Delta_epsilon ( 2,2 ) << " eps1=" << Delta_epsilon ( 1,1 ) << " number of neigbors: "<< edges.size()
	<< endl << "Un_min=" << 1.5*row[0].first - 0.5*row[1].first << " Un_max="
	<< row[row.size()-1].first << endl;
	for ( vector< pair<Real, Real> >::iterator r_it = row.begin(); r_it != r_end; ++r_it )
	{
		output_file << r_it->first << " " << r_it->second << endl;
		cout << r_it->first << " " << r_it->second << endl;
	}
	output_file << endl;
	return output_file;

}


//vector<pair<Real,Real> >  KinematicLocalisationAnalyser::
//NormalDisplacementDistribution(TriaxialState& state, TriaxialState& state0)
//{
// vector<pair<Real,Real> > table;
// table.resize(linear_discretisation);
//
//
// return table;
//}



ofstream& KinematicLocalisationAnalyser::
ContactDistributionToFile ( ofstream& output_file )
{
	//cerr << "ContactDistributionToFile" << endl;
	vector< pair<Real, Real> > row;
	row.resize ( sphere_discretisation+1 );
	Real DZ = 1.0/sphere_discretisation;//interval in term of cos(teta)
	long nc1=0;
	long nc2=0;
	long ng1=0;
	long ng2=0;
	//cerr << "ContactDistributionToFile05" << endl;
	TriaxialState::ContactIterator cend = ( *TS1 ).contacts_end();
	TriaxialState::GrainIterator gend = ( *TS1 ).grains_end();

	for ( int i = 0; i <= sphere_discretisation; ++i )
	{
		row[i].first = ( i+0.5 ) *DZ;
		row[i].second = 0;
	}

	for ( TriaxialState::GrainIterator git = ( *TS1 ).grains_begin(); git!=gend; ++git )
	{
		if ( ( *TS1 ).inside ( git->sphere.point() ) ) ++ng1;
		else ++ng2;
	}

	for ( TriaxialState::ContactIterator cit = ( *TS1 ).contacts_begin(); cit!=cend; ++cit )
	{
		if ( ( *TS1 ).inside ( ( *cit )->grain1->sphere.point() ) && ( *TS1 ).inside ( ( *cit )->grain2->sphere.point() ) )
		{
			row[ ( int ) ( abs ( ( *cit )->normal.z() ) /DZ ) ].second += 2;
			nc1 += 2;
		}
		else
		{
			if ( ( *TS1 ).inside ( ( *cit )->grain1->sphere.point() ) || ( *TS1 ).inside ( ( *cit )->grain2->sphere.point() ) )
			{
				row[ ( int ) ( abs ( ( *cit )->normal.z() ) /DZ ) ].second += 1;
				++nc1;
			}
			//cerr << "(*cit)->normal.z(),DZ : " << (*cit)->normal.z() << " " << DZ << endl;}
			else ++nc2;
		}
	}
	//normalisation :
	Real normalize = 1.0/ ( ng1*4*DZ*3.141592653 );
	for ( int i = 0; i <= sphere_discretisation; ++i ) row[i].second *= normalize;

	//output part :
	output_file << "#Contacts distribution" << endl << "(filter dist. = " << ( *TS1 ).filter_distance
	<< ", "<< nc1 << " contacts, " << nc2 << " excluded contacts, for "<< ng1
	<<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	output_file << "max_nz number_of_contacts" << endl;
	cerr << "#Contacts distribution (filter dist. = " << ( *TS1 ).filter_distance
	<< ", "<< nc1 << " contacts, " << nc2 << " excluded contacts, for "<< ng1
	<<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	cerr << "mean_nz number_of_contacts" << endl;
	for ( int i = 0; i <= sphere_discretisation; ++i )
	{
		output_file << row[i].first << " " << row[i].second << endl;
		cerr << row[i].first << " " << row[i].second << endl;
	}

	output_file << endl;
	return output_file;
}



ofstream& KinematicLocalisationAnalyser::
AllNeighborDistributionToFile ( ofstream& output_file )
{
	vector< pair<Real, Real> > row;
	row.resize ( sphere_discretisation );
	Real DZ = 1.0/sphere_discretisation;
	long nv1=0;
	long nv2=0;
	long nv3=0;
	long ng1=0;
	long ng2=0;

	for ( int i = 0; i < sphere_discretisation; ++i )
	{
		row[i].first = ( i+0.5 ) *DZ;
		row[i].second = 0;
	}

	TriaxialState::GrainIterator gend = ( *TS1 ).grains_end();
	for ( TriaxialState::GrainIterator git = ( *TS1 ).grains_begin(); git!=gend; ++git )
	{
		if ( ( *TS1 ).inside ( git->sphere.point() ) ) ++ng1;
		else ++ng2;
	}

	RTriangulation& T = ( *TS1 ).tesselation().Triangulation();
	Segment s;
	Vecteur v;
	for ( Edge_iterator ed_it = T.edges_begin(); ed_it != T.edges_end(); ed_it++ )
	{
		if ( !T.is_infinite ( *ed_it ) )
		{
			s = T.segment ( *ed_it );
			if ( ( *TS1 ).inside ( s.source() ) && ( *TS1 ).inside ( s.target() ) )
			{
				v = s.to_vector();
				row[ ( int ) ( abs ( v.z() /sqrt ( s.squared_length() ) ) /DZ ) ].second += 2;
				nv1 += 2;
			}
			else
			{
				if ( ( *TS1 ).inside ( s.source() ) || ( *TS1 ).inside ( s.target() ) )
				{
					v = s.to_vector();
					row[ ( int ) ( abs ( v.z() /sqrt ( s.squared_length() ) ) /DZ ) ].second += 1;
					++nv1;
				}
				else ++nv2;
			}
		}
		else ++nv3;
	}

	Real normalize = 1.0/ ( ng1*4*DZ*3.141592653 );
	for ( int i = 0; i < sphere_discretisation; ++i ) row[i].second *= normalize;

	output_file << "#Neighbors distribution" << endl << "(filter dist. = " << ( *TS1 ).filter_distance
	<< ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	<< nv3 << " infinite, for "<< ng1 <<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	output_file << "max_nz number_of_neighbors" << endl;
	cerr << "#Neighbors distribution" << endl << "(filter dist. = " << ( *TS1 ).filter_distance
	<< ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	<< nv3 << " infinite, for "<< ng1 <<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	cerr << "mean_nz number_of_neighbors" << endl;
	for ( int i = 0; i < sphere_discretisation; ++i )
	{
		output_file << row[i].first << " " << row[i].second << endl;
		cerr << row[i].first << " " << row[i].second << endl;
	}

	output_file << endl;
	return output_file;
}

void KinematicLocalisationAnalyser::
SetForceIncrements ( void ) //WARNING : This function will modify the contact lists : add virtual (lost)) contacts in state 1 and modify old_force and force in state 0, execute this function after all other force analysis functions if you want to avoid problems
{
	if ( true ) cerr << "SetForceIncrements"<< endl;
//  vector< pair<Real, Real> > row;
//  row.resize ( sphere_discretisation );
//  Real DZ = 1.0/sphere_discretisation;
	long Nc0 = TS0->contacts.size();
	long Nc1 = TS1->contacts.size();

//  long nv1=0;
//  long nv2=0;
//  long nv3=0;
//  long ng1=0;
//  long ng2=0;
	n_persistent = 0; n_new = 0; n_lost = 0;
	long lost_in_state0 = 0;

	for ( int i = 0; i < Nc0; ++i ) {
		TS0->contacts[i]->visited = false;
		if (TS0->contacts[i]->status == TriaxialState::Contact::LOST) ++lost_in_state0;}
	for ( int i = 0; i < Nc1; ++i ) TS1->contacts[i]->visited = false;
	cerr << "Nc1 "<<Nc1<<", Nc0 "<<Nc0<<" ("<<Nc0-lost_in_state0<<" real)"<<endl;
	for ( int i = 0; i < Nc0; ++i )
	{
	//	cerr << 1;
		if ( TS0->contacts[i]->status != TriaxialState::Contact::LOST )
		{
	//		cerr << 2;
			for ( int j = 0; j < Nc1; ++j )
			{
				
				if ( TS0->contacts[i]->grain1->id == TS1->contacts[j]->grain1->id && TS0->contacts[i]->grain2->id == TS1->contacts[j]->grain2->id) // This is a PERSISTENT contact (i.e. it is present in state 0 and 1)
				{
					//TS0->contacts[i]->visited = true;
					TS1->contacts[j]->visited = true;
					//TS0->contacts[i]->status = TriaxialState::Contact::PERSISTENT;
					TS1->contacts[j]->status = TriaxialState::Contact::PERSISTENT;
					TS1->contacts[j]->old_fn = TS0->contacts[i]->fn;
					TS1->contacts[j]->old_fs = TS0->contacts[i]->fs;
					++n_persistent;
					break;
				}
				else if ( j+1==Nc1 ) //This contact was not found in state 1, add it as a LOST contact
				{
	//				cerr << 3 << endl;
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
					TS1->contacts.push_back ( c );
					++Nc1;
					++n_lost;
					break;
				}
			}
		}
	}
	//cerr << 4;
	for ( int j = 0; j < Nc1; ++j ) //This contact was not visited, it is a NEW one
	{
		//cerr << 5;
		if ( !TS1->contacts[j]->visited /*&& TS1->contacts[j]->status != TriaxialState::Contact::LOST*/)
		{
			//cerr << 6;
			TS1->contacts[j]->status = TriaxialState::Contact::NEW;
			TS1->contacts[j]->old_fn = 0;
			TS1->contacts[j]->old_fs = CGAL::NULL_VECTOR;
			++n_new;
		}
	}
	//cerr << 7;
	if ( true ) cerr << "Contact Status : "<< n_persistent << " persistent, "<< n_new << " new, "<< n_lost << " lost"<< endl;
	/*
	RGrid1D table;

	for ( Edge_iterator ed_it = T.edges_begin(); ed_it != T.edges_end(); ed_it++ )
	{
	 if ( !T.is_infinite ( *ed_it ) )
	 {
	  s = T.segment ( *ed_it );
	  if ( ( *TS1 ).inside ( s.source() ) && ( *TS1 ).inside ( s.target() ) )
	  {
	   v = s.to_vector();
	   row[ ( int ) ( abs ( v.z() /sqrt ( s.squared_length() ) ) /DZ ) ].second += 2;
	   nv1 += 2;
	  }
	  else
	  {
	   if ( ( *TS1 ).inside ( s.source() ) || ( *TS1 ).inside ( s.target() ) )
	   {
	    v = s.to_vector();
	    row[ ( int ) ( abs ( v.z() /sqrt ( s.squared_length() ) ) /DZ ) ].second += 1;
	    ++nv1;
	   }
	   else ++nv2;
	  }
	 }
	 else ++nv3;
	}

	Real normalize = 1.0/ ( ng1*4*DZ*3.141592653 );
	for ( int i = 0; i < sphere_discretisation; ++i ) row[i].second *= normalize;

	output_file << "#Neighbors distribution" << endl << "(filter dist. = " << ( *TS1 ).filter_distance
	  << ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	  << nv3 << " infinite, for "<< ng1 <<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	output_file << "max_nz number_of_neighbors" << endl;
	cerr << "#Neighbors distribution" << endl << "(filter dist. = " << ( *TS1 ).filter_distance
	  << ", "<< nv1 << " neighbors + " << nv2 << " excluded + "
	  << nv3 << " infinite, for "<< ng1 <<"/"<< ( ng1+ng2 ) << " grains)" << endl;
	cerr << "mean_nz number_of_neighbors" << endl;
	for ( int i = 0; i < sphere_discretisation; ++i )
	{
	 output_file << row[i].first << " " << row[i].second << endl;
	 cerr << row[i].first << " " << row[i].second << endl;
	}

	output_file << endl;
	return output_file;*/
}



void KinematicLocalisationAnalyser::SetDisplacementIncrements ( void )
{
	TriaxialState::GrainIterator gend = TS1->grains_end();
	for (TriaxialState::GrainIterator git = TS1->grains_begin(); git!=gend; ++git) 
		if (git->id >= 0) git->translation =  TS1->grain ( git->id ).sphere.point() - TS0->grain ( git->id ).sphere.point();
	consecutive = true;
			
	
}



ofstream& KinematicLocalisationAnalyser::
StrictNeighborDistributionToFile (ofstream& output_file)
{
	return output_file;
}







// Tenseur3 KinematicLocalisationAnalyser::Grad_u (Point &p1, Point &p2, Point &p3)
// {
// 	Tenseur3 T;
// 	Vecteur V = (Deplacement(p1)+Deplacement(p2)+Deplacement(p3))/3.00;
// 	Grad_u(p1, p2, p3, V, T);
// 	return T;
// 	//Vecteur V = (Deplacement(p1)+Deplacement(p2)+Deplacement(p3))/3;
// }

//Vecteur KinematicLocalisationAnalyser::Deplacement (Point &p) {return (p-CGAL::ORIGIN)/100;}

Vecteur KinematicLocalisationAnalyser::Deplacement ( Finite_cells_iterator cell, int facet ) // D�p. moyen sur une facette
{
	Vecteur v ( 0.f, 0.f, 0.f );
	int id;// ident. de la particule
	for ( int i=0; i<4; i++ )
	{
		//  char msg [256];
		if ( i!=facet )
		{
			id = cell->vertex ( i )->info().id();
			if ( consecutive )
				v = v + TS1->grain ( id ).translation;
			else  v = v + ( TS1->grain ( id ).sphere.point() - TS0->grain ( id ).sphere.point() );

			//for tests with affine displacement field
			//if ((TS1->grain(id).sphere.point().y()+TS1->grain(id).sphere.point().z())>0.035)//a discontinuity
			//v = v + Vecteur(0, 0.01*TS1->grain(id).sphere.point().x(), 0);
		}
	}
	v = v*0.333333333333333333333333;
	return v;
}



void KinematicLocalisationAnalyser::Grad_u ( Finite_cells_iterator cell, int facet, Vecteur &V, Tenseur3& T )
{
	Vecteur S = cross_product ( ( cell->vertex ( l_vertices[facet][1] )->point() )
					- ( cell->vertex ( l_vertices[facet][0] )->point() ),
					( cell->vertex ( l_vertices[facet][2] )->point() ) -
					( cell->vertex ( l_vertices[facet][1] )->point() ) ) /2.f;
	Somme ( T, V, S );
}


// void KinematicLocalisationAnalyser::Grad_u (Point &p1, Point &p2, Point &p3, Vecteur &V, Tenseur3& T) // rotation 1->2->3 orient�e vers l'ext�rieur
// {		
// 	Vecteur S = 0.5*cross_product(p2-p1, p3-p2);
//     	Somme (T, V, S);
// }

void KinematicLocalisationAnalyser::Grad_u (Finite_cells_iterator cell,
Tenseur3& T, bool vol_divide)// Calcule le gradient de d�p. 
{
	/*char msg [256];
	sprintf(msg, "Exec Grad_u (Finite_cells_iterator cell, Tenseur3& T, bool
vol_divide)");
	Udata::out(msg);
	sprintf(msg, "***  Hv = \n %f %f %f \n %f %f %f \n %f %f %f \n",  
	T(1,1), T(1,2), T(1,3), T(2,1), T(2,2), T(2,3),
	T(3,1), T(3,2), T(3,3));
	Udata::out(msg);*/
	T.reset();
	Vecteur v;
	for (int facet=0; facet<4; facet++)
	{
		v = Deplacement(cell, facet);
		Grad_u (cell, facet, v, T);
	}
	if (vol_divide) T/= Tesselation::Volume(cell);
}






// Calcul du tenseur d'orientation des voisins
// Tenseur_sym3 Orientation_voisins (Tesselation &Tes)
// {
// 	RTriangulation& T = Tes.Triangulation();
// 	Tenseur3 Tens;
// 	Vecteur v;
// 	long Nv = 0; //nombre de voisins
// 	for (Edge_iterator ed_it = T.edges_begin(); ed_it != T.edges_end(); ed_it++)
// 	{	
// 		if (!T.is_infinite(*ed_it))
// 		{
// 			Nv++;
// 			v = T.segment(*ed_it).to_vector()/sqrt(T.segment(*ed_it).squared_length());
// 			for (int i=1; i<4; i++) for (int j=3; j>=i; j--)Tens(i,j) += v[i-1]*v[j-1];
// 		}
// 	} 
// 	Tens /= Nv;
// 	return Tens;
// }






//
//;------------------------------------------------
//;orient_n_Y
//;calcul de la distribution des orientations de contact
//;(angle entre n et l'axe y)
//;param�tres d'entr�e : N_classes (nombre de classes), e_filtre (�paisseur de la bordure supprim�e)
//;sortie : table 20
//
//def orient_n_Z
//
//X_min = w_x(waddg) + e_filtre
//X_max = l + w_x(waddd) - e_filtre
//Y_min = w_y(waddfa) + e_filtre
//Y_max = l + w_y(waddfo) - e_filtre
//Z_min = w_z(waddb) + e_filtre
//Z_max = h + w_z(waddh) - e_filtre
//DZ_classes = 1./(N_classes*1.)
//
//command
//table 20 erase
//table 21 erase
//endcommand
//loop temp (0, N_classes)
//	xtable (20, temp+1) = temp * DZ_classes
//endloop
//
//cp=contact_head
//N_filtre1 = 0
//n_cont = 0
//loop while cp#null
//	if c_nforce(cp) # 0
//	n_cont = n_cont +1
//	if filtre1 = 1
//	N_filtre1 = N_filtre1 +1
//		classe = int(abs(c_zun(cp))/DZ_classes) + 1
//		ytable (20, classe) = ytable (20, classe) +1
//	endif
//	endif
//cp=c_next(cp)
//endloop
//
//command
//print N_filtre1 n_cont
//set logfile orient.txt
//set log on ov
//pr table 20
//set log off
//set logfile res.dat
//endcommand
//;Normalisation :
//
//end

void KinematicLocalisationAnalyser::ComputeParticlesDeformation ( void )
{
	//cerr << "compute particle deformation" << endl;
	Tesselation& Tes = TS1->tesselation();
	RTriangulation& Tri = Tes.Triangulation();
	Tenseur3 grad_u;
	Real v;
	v_total = 0;
	v_solid_total = 0;
	grad_u_total = NULL_TENSEUR3;
	v_total_g = 0;
	grad_u_total_g = NULL_TENSEUR3;
	Delta_epsilon ( 3,3 ) = TS1->eps3 - TS0->eps3;
	Delta_epsilon ( 1,1 ) = TS1->eps1 - TS0->eps1;
	Delta_epsilon ( 2,2 ) = TS1->eps2 - TS0->eps2;

	//Compute Voronoi tesselation (i.e. voronoi center of each cell)
	if ( !Tes.Computed() ) Tes.Compute();
	//cerr << "ParticleDeformation.size() = " << ParticleDeformation.size() << endl;
	if ( ParticleDeformation.size() != ( Tes.Max_id() + 1 ) )
	{
		//cerr << "resize to " << Tes.Max_id() + 1 << endl;
		ParticleDeformation.clear();
		ParticleDeformation.resize ( Tes.Max_id() + 1 );
	}
	//cerr << "ENDOF ParticleDeformation.size() = " << ParticleDeformation.size() << endl;
	//reset volumes and tensors of each particle
	for ( RTriangulation::Finite_vertices_iterator  V_it =
				Tri.finite_vertices_begin (); V_it !=  Tri.finite_vertices_end (); V_it++ )
	{
		//cerr << V_it->info().id() << endl;
		V_it->info().v() =0;//WARNING : this will erase previous values if some have been computed
		ParticleDeformation[V_it->info().id() ]=NULL_TENSEUR3;
	}
	//cerr << "RTriangulation::Finite_vertices_iterator  V_it  = " << ParticleDeformation.size() << endl;

	Finite_cells_iterator cell = Tri.finite_cells_begin();
	Finite_cells_iterator cell0 = Tri.finite_cells_end();


	//Compute grad_u and volumes of all cells in the triangulation, and assign them to each of the vertices ( volume*grad_u is added here rather than grad_u, the weighted average is computed later )
	//cerr << "for ( ; cell != cell0; cell++ )" << endl;
	for ( ; cell != cell0; cell++ ) // calcule la norme du d�viateur dans chaque cellule
	{
		//cerr << "ij=" <<ij++<<endl;
		//cerr << "ij2=" <<ij2++<<endl;
		//if (!cell->info()->isFictious) //FIXME
		Grad_u ( cell, grad_u, false );// false : don't divide by volume, here grad_u = volume of cell * average grad_u in cell, the final value is divided by the total volume later (see below)
		//cerr << "grad_u=" << grad_u << endl;
		v = Tri.tetrahedron ( cell ).volume();
		grad_u_total += grad_u;
		v_total += v;
		for ( unsigned int index=0; index<4; index++ )
		{
			cell->vertex ( index )->info().v() += v;//WARNING2 : this will affect values which differ from the volumes of voronoi cells
			//cerr << "ParticleDeformation[cell->vertex (" << cell->vertex ( index )->info().id() << ")"<< endl;
			ParticleDeformation[cell->vertex ( index )->info().id() ] += grad_u;
		}
	}

	//Delete volume and grad_u for particles on the border FIXME : replace that using isFictious flags?
	Tesselation::Vector_Vertex border_vertices;
	Tes.Voisins ( Tri.infinite_vertex (), border_vertices );
	unsigned int l = border_vertices.size();
	//cerr << "l=" << l << endl;
	
	//cerr << "for ( ; cell != cell0; cell++ )" << endl;
	for ( unsigned int i=0; i<l; ++i )
	{
		//cerr << "border " << i << endl;
		border_vertices[i]->info().v() =0;

		ParticleDeformation[border_vertices[i]->info().id() ]=NULL_TENSEUR3;
	}

	//Divide sum(v*grad_u) by sum(v) to get the average grad_u on each particle
	for ( RTriangulation::Finite_vertices_iterator  V_it = Tri.finite_vertices_begin (); V_it !=  Tri.finite_vertices_end (); V_it++ )
	{
		v_total_g += V_it->info().v();
		v_solid_total += 4.188790*pow(V_it->point().weight(),1.5);//4.18... = 4/3*PI; and here, weight is rad²
		grad_u_total_g += ParticleDeformation[V_it->info().id() ];
		if ( V_it->info().v() ) ParticleDeformation[V_it->info().id() ]/=V_it->info().v();
	}
	grad_u_total_g /= v_total_g;
	cerr << "sym_grad_u_total_g (wrong averaged strain):"<< endl << Tenseur_sym3 ( grad_u_total_g ) << endl;

	if ( v_total ) grad_u_total /= v_total;
	cerr << "Total volume = " << v_total << ", grad_u = " << endl << grad_u_total << endl << "sym_grad_u (true average strain): " << endl << Tenseur_sym3 ( grad_u_total ) << endl;
	cerr << "Macro strain : "<< endl << Delta_epsilon << endl;
	
}

Real KinematicLocalisationAnalyser::ComputeMacroPorosity (void)
{
	return (1-v_solid_total/(TS1->haut*TS1->larg*TS1->prof));
}



