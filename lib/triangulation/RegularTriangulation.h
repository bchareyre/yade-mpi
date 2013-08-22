/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include "def_types.h"
#include <cassert>

namespace CGT {
	
	
template<class vertex_info, class cell_info>
class TriangulationTypes {

public:
typedef vertex_info								Vertex_Info;
typedef cell_info								Cell_Info;
typedef CGAL::Triangulation_vertex_base_with_info_3<Vertex_Info, Traits>	Vb_info;
typedef CGAL::Triangulation_cell_base_with_info_3<Cell_Info, Traits>		Cb_info;
typedef CGAL::Triangulation_data_structure_3<Vb_info, Cb_info>			Tds;

typedef CGAL::Triangulation_3<K>						Triangulation;
typedef CGAL::Regular_triangulation_3<Traits, Tds>				RTriangulation;

typedef typename RTriangulation::Vertex_iterator                    		Vertex_iterator;
typedef typename RTriangulation::Vertex_handle                      		Vertex_handle;
typedef typename RTriangulation::Finite_vertices_iterator                    	Finite_vertices_iterator;
typedef typename RTriangulation::Cell_iterator					Cell_iterator;
typedef typename RTriangulation::Finite_cells_iterator				Finite_cells_iterator;
typedef typename RTriangulation::Cell_circulator				Cell_circulator;
typedef typename RTriangulation::Cell_handle					Cell_handle;

typedef typename RTriangulation::Facet						Facet;
typedef typename RTriangulation::Facet_iterator					Facet_iterator;
typedef typename RTriangulation::Facet_circulator				Facet_circulator;
typedef typename RTriangulation::Finite_facets_iterator				Finite_facets_iterator;
typedef typename RTriangulation::Locate_type					Locate_type;

typedef typename RTriangulation::Edge_iterator					Edge_iterator;
typedef typename RTriangulation::Finite_edges_iterator				Finite_edges_iterator;
};

typedef CGAL::To_double<double>							W_TO_DOUBLE; // foncteur Weight to Real 
typedef TriangulationTypes<SimpleVertexInfo,SimpleCellInfo>			SimpleTriangulationTypes;
typedef TriangulationTypes<FlowVertexInfo,FlowCellInfo>				FlowTriangulationTypes;
typedef TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo>			PeriFlowTriangulationTypes;

} // namespace CGT
