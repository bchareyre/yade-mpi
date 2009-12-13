#pragma once


#include "stdafx.h"
#include "def_types.h"
#include <cassert>

namespace CGT {

typedef Vertex_Info				vertex_info;		//d�fini le type des infos li�es � chaque sommet
typedef	Cell_Info				cell_info;		//d�fini le type des infos li�es � chaque cellule
typedef CGAL::Triangulation_vertex_base_with_info_3<vertex_info, Traits>	Vb_info;
typedef CGAL::Triangulation_cell_base_with_info_3<cell_info, Traits>		Cb_info;
typedef CGAL::Triangulation_data_structure_3<Vb_info, Cb_info>			Tds;

typedef CGAL::Triangulation_3<K>						Triangulation;	//Triangulation de points
typedef CGAL::Regular_triangulation_3<Traits, Tds>				RTriangulation; //Triangulation de sph�res

typedef RTriangulation::Vertex_iterator                    			Vertex_iterator;
typedef RTriangulation::Vertex_handle                      			Vertex_handle;
typedef RTriangulation::Finite_vertices_iterator                    		Finite_vertices_iterator;
//typedef RTriangulation::Finite_vertex_handle                      		Finite_vertices_handle;

typedef RTriangulation::Cell_iterator						Cell_iterator;
typedef RTriangulation::Finite_cells_iterator					Finite_cells_iterator;
typedef RTriangulation::Cell_circulator						Cell_circulator;
typedef RTriangulation::Cell_handle						Cell_handle;

typedef RTriangulation::Facet							Facet;
typedef RTriangulation::Facet_iterator						Facet_iterator;
typedef RTriangulation::Facet_circulator					Facet_circulator;
typedef RTriangulation::Finite_facets_iterator					Finite_facets_iterator;
typedef RTriangulation::Locate_type						Locate_type;

typedef RTriangulation::Edge_iterator						Edge_iterator;
typedef RTriangulation::Finite_edges_iterator					Finite_edges_iterator;
//typedef RTriangulation::Segment								Segment;

typedef CGAL::To_double<double>							W_TO_DOUBLE; // foncteur Weight to Real 

//  Alloue l'espace pour une liste de segments et le rempli avec les
//  "Edges" d'une triangulation 

long New_liste_edges	( RTriangulation &T, Real** Coordonnes );
long New_liste_edges	( RTriangulation &T, Real** Coordonnes );
long New_liste_edges	( Triangulation &T, Real** Coordonnes );
long New_liste_edges	( Triangulation &T, Real** Coordonnes );


long New_segments		( Real** ppCoordonnes );

int	Regular_Triangule( );
void Delete_liste_edges (Real** ppCoordonnes, long N_edges=0);

} // namespace CGT