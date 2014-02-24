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

typedef typename RTriangulation::Vertex_iterator                    		VertexIterator;
typedef typename RTriangulation::Vertex_handle                      		VertexHandle;
typedef typename RTriangulation::Finite_vertices_iterator                    	FiniteVerticesIterator;
typedef typename RTriangulation::Cell_iterator					CellIterator;
typedef typename RTriangulation::Finite_cells_iterator				FiniteCellsIterator;
typedef typename RTriangulation::Cell_circulator				CellCirculator;
typedef typename RTriangulation::Cell_handle					CellHandle;

typedef typename RTriangulation::Facet						Facet;
typedef typename RTriangulation::Facet_iterator				FacetIterator;
typedef typename RTriangulation::Facet_circulator				FacetCirculator;
typedef typename RTriangulation::Finite_facets_iterator			FiniteFacetsIterator;
typedef typename RTriangulation::Locate_type					LocateType;

typedef typename RTriangulation::Edge_iterator					EdgeIterator;
typedef typename RTriangulation::Finite_edges_iterator				FiniteEdgesIterator;
};

typedef CGAL::To_double<double>							W_TO_DOUBLE; // foncteur Weight to Real 
typedef TriangulationTypes<SimpleVertexInfo,SimpleCellInfo>			SimpleTriangulationTypes;
typedef TriangulationTypes<FlowVertexInfo,FlowCellInfo>				FlowTriangulationTypes;
typedef TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo>			PeriFlowTriangulationTypes;

} // namespace CGT
