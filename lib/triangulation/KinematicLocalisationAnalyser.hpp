/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/*! \brief Computes statistics of micro-variables assuming axi-symetry.
	
 */

#pragma once
#include "TriaxialState.h"
#include "Tenseur3.h"
//class TriaxialState;

namespace CGT {

#define SPHERE_DISCRETISATION 20; //number of "teta" intervals on the unit sphere
#define LINEAR_DISCRETISATION 200; //number of intervals on segments like [UNmin,UNmax]
	
// l_vertices : definition de l'ordre de parcours des sommets
// pour la facette k, les indices des 3 sommets sont dans la colonne k
const int l_vertices [4][3] = { {1, 2, 3}, {0, 3, 2}, {3, 0, 1}, {2, 1, 0} };

class KinematicLocalisationAnalyser
{
	public:
		typedef vector< pair<Real,Real> > RGrid1D;
		typedef vector<vector <Real> >  RGrid2D;
		typedef vector<vector<vector<Real> > > RGrid3D;

		KinematicLocalisationAnalyser();
		KinematicLocalisationAnalyser ( const char* state_file1 );
		KinematicLocalisationAnalyser ( const char* state_file1, const char* state_file0, bool consecutive_files = true );
		KinematicLocalisationAnalyser ( const char* base_name, int file_number0, int file_number1 );

		~KinematicLocalisationAnalyser();

		void SetBaseFileName ( string name );
		bool SetFileNumbers ( int n0, int n1 );
		void SetConsecutive (bool);
		void SetNO_ZERO_ID (bool);
		void SwitchStates ( void );
		

		bool DistribsToFile (const char* output_file_name);
		///Write the averaged deformation on each grain in a file (vertices and cells lists included in the file), no need to call ComputeParticlesDeformation()
		bool DefToFile (const char* output_file_name = "deformations");
		ofstream& ContactDistributionToFile ( ofstream& output_file );
		ofstream& AllNeighborDistributionToFile ( ofstream& output_file );
		ofstream& StrictNeighborDistributionToFile ( ofstream& output_file );
		ofstream& NormalDisplacementDistributionToFile ( vector<Edge_iterator>& edges, ofstream& output_file );

		long Filtered_contacts ( TriaxialState& state );
		long Filtered_neighbors ( TriaxialState& state );
		long Filtered_grains ( TriaxialState& state );
		Real Filtered_volume ( TriaxialState& state );
		Real Contact_coordination ( TriaxialState& state );
		Real Neighbor_coordination ( TriaxialState& state );
		Tenseur_sym3 Neighbor_fabric ( TriaxialState& state );
		Tenseur_sym3 Contact_fabric ( TriaxialState& state );
		Real Contact_anisotropy ( TriaxialState& state );
		Real Neighbor_anisotropy ( TriaxialState& state );
		
		void SetForceIncrements (void);
		void SetDisplacementIncrements (void);

		///Add surface*displacement to T
		void Grad_u ( Finite_cells_iterator cell, int facet, Vecteur &V, Tenseur3& T );
		///Compute grad_u in cell (by default, T= average grad_u in cell, if !vol_divide, T=grad_u*volume
		void Grad_u ( Finite_cells_iterator cell, Tenseur3& T, bool vol_divide=true );
		/// Compute grad_u for all particles, by summing grad_u of all adjaent cells using current states
		const vector<Tenseur3>& ComputeParticlesDeformation (void);
		/// Do everything in one step by giving some final (file1) and initial (file0) positions 
		const vector<Tenseur3>& ComputeParticlesDeformation(const char* state_file1, const char* state_file0);
		///Compute porisity from cumulated spheres volumes and positions of boxes
		Real ComputeMacroPorosity (void );


		Vecteur Deplacement ( Cell_handle cell );  //donne le d�placement d'un sommet de voronoi
		Vecteur Deplacement ( Finite_cells_iterator cell, int facet ); //mean displacement on a facet
		

		// Calcul du tenseur d'orientation des voisins
		//Tenseur_sym3 Orientation_voisins (Tesselation& Tes);
		
		//Set the list of edges of a given orientation (orientation defined via the z-coordinate of the normal)
		vector<Edge_iterator>& Oriented_Filtered_edges (Real Nymin, Real Nymax, vector<Edge_iterator>& filteredList);
		
		vector<pair<Real,Real> >& NormalDisplacementDistribution ( vector<Edge_iterator>& edges, vector<pair<Real,Real> >& row );
		//vector<pair<Real,Real> > NormalDisplacementDistribution(TriaxialState& state, TriaxialState& state0);

		//member data
		int sphere_discretisation;
		int linear_discretisation;
		Tenseur_sym3 Delta_epsilon;
		Tenseur3 grad_u_total;
		vector<Tenseur3> ParticleDeformation;		
		Tenseur3 grad_u_total_g;//grad_u averaged on extended grain cells
		TriaxialState *TS1, *TS0;
	private:

		int file_number_1, file_number_0;
		//Characteristic size of particles
		string base_file_name;   //Base name of state-files, complete name is (base_name+state_number).
		bool consecutive; //Are the two triax states consecutive? if "false" displacements are re-computed
		//from the two source files; if "true" one file is enough.
		Real v_solid_total;//solid volume in the box
		Real v_total;//volume of the box
		Real v_total_g;//summed volumes of extended grain cells
		long n_persistent, n_new, n_lost, n_real_cells, n_real_vertices;



};

} // namespace CGT
