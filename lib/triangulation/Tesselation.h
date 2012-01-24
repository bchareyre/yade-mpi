/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include "RegularTriangulation.h"

namespace CGT {
	
//Since template inheritance does not automatically give access to the members of the base class, this macro can be used to declare all members at once. 
#define DECLARE_TESSELATION_TYPES(baseType)\
		typedef typename baseType::RTriangulation		 		RTriangulation;\
		typedef typename baseType::Vertex_Info					Vertex_Info;\
		typedef typename baseType::Cell_Info					Cell_Info;\
		typedef typename baseType::Vertex_iterator		 		Vertex_iterator;\
		typedef typename baseType::Vertex_handle                      		Vertex_handle;\
		typedef typename baseType::Finite_vertices_iterator                    	Finite_vertices_iterator;\
		typedef typename baseType::Cell_iterator				Cell_iterator;\
		typedef typename baseType::Finite_cells_iterator			Finite_cells_iterator;\
		typedef typename baseType::Cell_circulator				Cell_circulator;\
		typedef typename baseType::Cell_handle					Cell_handle;\
		typedef typename baseType::Facet					Facet;\
		typedef typename baseType::Facet_iterator				Facet_iterator;\
		typedef typename baseType::Facet_circulator				Facet_circulator;\
		typedef typename baseType::Finite_facets_iterator			Finite_facets_iterator;\
		typedef typename baseType::Locate_type					Locate_type;\
		typedef typename baseType::Edge_iterator				Edge_iterator;\
		typedef typename baseType::Finite_edges_iterator			Finite_edges_iterator;\
		typedef typename baseType::Vector_Vertex				Vector_Vertex;\
		typedef typename baseType::Vector_Cell					Vector_Cell;\
		typedef typename baseType::List_Point					List_Point;\
		typedef typename baseType::VCell_iterator				VCell_iterator;	

// Classe Tesselation, contient les fonctions permettant de calculer la Tessalisation
// d'une RTriangulation et de stocker les centres dans chacune de ses cellules

//TT is of model TriangulationTypes 
template<class TT>
class _Tesselation
{
public:
	typedef typename TT::RTriangulation							RTriangulation;
	typedef typename TT::Vertex_Info							Vertex_Info;
	typedef typename TT::Cell_Info								Cell_Info;
	typedef typename RTriangulation::Vertex_iterator		 			Vertex_iterator;
	typedef typename RTriangulation::Vertex_handle                      			Vertex_handle;
	typedef typename RTriangulation::Finite_vertices_iterator                    		Finite_vertices_iterator;
	typedef typename RTriangulation::Cell_iterator						Cell_iterator;
	typedef typename RTriangulation::Finite_cells_iterator					Finite_cells_iterator;
	typedef typename RTriangulation::Cell_circulator					Cell_circulator;
	typedef typename RTriangulation::Cell_handle						Cell_handle;
	typedef typename RTriangulation::Facet							Facet;
	typedef typename RTriangulation::Facet_iterator						Facet_iterator;
	typedef typename RTriangulation::Facet_circulator					Facet_circulator;
	typedef typename RTriangulation::Finite_facets_iterator					Finite_facets_iterator;
	typedef typename RTriangulation::Locate_type						Locate_type;
	typedef typename RTriangulation::Edge_iterator						Edge_iterator;
	typedef typename RTriangulation::Finite_edges_iterator					Finite_edges_iterator;	
	
	typedef std::vector<Vertex_handle>							Vector_Vertex;
	typedef std::vector<Cell_handle>							Vector_Cell;
	typedef std::list<Point>								List_Point;
	typedef typename Vector_Cell::iterator							VCell_iterator;
	int max_id;

protected:
	RTriangulation* Tri;
	RTriangulation* Tes; //=NULL or Tri depending on the constructor used.
	bool computed;
public:
	Real TotalFiniteVoronoiVolume;
	Real area; 
	Real TotalInternalVoronoiVolume;
	Real TotalInternalVoronoiPorosity;
	Vector_Vertex vertexHandles;//This is a redirection vector to get vertex pointers by spheres id
	bool redirected;//is vertexHandles filled with current vertex pointers? 	

public:
		_Tesselation(void);
	_Tesselation(RTriangulation &T);// : Tri(&T) { Calcule(); }
	~_Tesselation(void);
	
	///Insert a sphere
	Vertex_handle insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false);
	/// move a spheres
	Vertex_handle move (Real x, Real y, Real z, Real rad, unsigned int id);
	///Fill a vector with vertexHandles[i] = handle of vertex with id=i for fast access
	bool redirect (void);
	///Remove a sphere
	bool remove (unsigned int id); 
	int Max_id (void) {return max_id;}
	
	void	Compute ();	//Calcule le centres de Voronoi pour chaque cellule
	void	Invalidate () {computed=false;}  //Set the tesselation as "not computed" (computed=false), this will launch 						//tesselation internaly when using functions like computeVolumes())
	// N.B : Compute() must be executed before the functions below are used
	void	Clear(void);

	static Point	Dual	(const Cell_handle &cell);	
	static Plan	Dual	(Vertex_handle S1, Vertex_handle S2);
	static Segment  Dual	(Finite_facets_iterator &facet);	//G�n�re le segment dual d'une facette finie
	static Real	Volume	(Finite_cells_iterator cell);
	inline void 	AssignPartialVolume	(Finite_edges_iterator& ed_it);
// 	inline void 	ComputeVFacetArea	(Finite_edges_iterator& ed_it);
	double		ComputeVFacetArea (Finite_edges_iterator ed_it);
	void		ResetVCellVolumes	(void);
	void		ComputeVolumes		(void);//Compute volume each voronoi cell
	void		ComputePorosity		(void);//Compute volume and porosity of each voronoi cell
	inline Real&	Volume (unsigned int id) { return vertexHandles[id]->info().v(); }
	inline const Vertex_handle&	vertex (unsigned int id) const { return vertexHandles[id]; }

	
	Finite_cells_iterator finite_cells_begin(void);// {return Tri->finite_cells_begin();}
	Finite_cells_iterator finite_cells_end(void);// {return Tri->finite_cells_end();}
	void Voisins (Vertex_handle v, Vector_Vertex& Output_vector);// {Tri->incident_vertices(v, back_inserter(Output_vector));}
	RTriangulation& Triangulation (void);// {return *Tri;}

	bool Computed (void) {return computed;}

	bool is_short ( Finite_facets_iterator f_it );
	inline bool is_internal ( Finite_facets_iterator &facet );//

	long New_liste_edges	( Real** Coordonnes );	//Genere la liste des segments de Voronoi
	long New_liste_short_edges	( Real** Coordonnes );	//Genere la version tronquee (partie interieure) du graph de Voronoi
	long New_liste_short_edges2	( Real** Coordonnes );
	long New_liste_adjacent_edges ( Vertex_handle vertex0, Real** Coordonnes );
};



template<class Tesselation>
class PeriodicTesselation : public Tesselation
{
	public:
	DECLARE_TESSELATION_TYPES(Tesselation)
	using Tesselation::Tri;
	using Tesselation::vertexHandles;
	using Tesselation::max_id;
	using Tesselation::redirected;
		
	///Insert a sphere, which can be a duplicate one from the base period if duplicateOfId>=0
	Vertex_handle insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false, int duplicateOfId=-1);
	///Fill a vector with vertexHandles[i] = handle of vertex with id=i for fast access, contains only spheres from the base period
	bool redirect (void);
};

} // namespace CGT

#include "Tesselation.ipp"

//Explicit instanciation
typedef CGT::_Tesselation<CGT::SimpleTriangulationTypes>		SimpleTesselation;
typedef CGT::_Tesselation<CGT::FlowTriangulationTypes>			FlowTesselation;
typedef CGT::PeriodicTesselation<CGT::_Tesselation<CGT::PeriFlowTriangulationTypes> >	PeriFlowTesselation;


