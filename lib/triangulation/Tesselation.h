/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
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
		typedef typename baseType::RTriangulation		 	RTriangulation;\
		typedef typename baseType::AlphaShape		 		AlphaShape;\
		typedef typename baseType::VertexInfo				VertexInfo;\
		typedef typename baseType::CellInfo				CellInfo;\
		typedef typename baseType::VertexIterator			VertexIterator;\
		typedef typename baseType::VertexHandle				VertexHandle;\
		typedef typename baseType::FiniteVerticesIterator		FiniteVerticesIterator;\
		typedef typename baseType::CellIterator				CellIterator;\
		typedef typename baseType::FiniteCellsIterator			FiniteCellsIterator;\
		typedef typename baseType::CellCirculator			CellCirculator;\
		typedef typename baseType::CellHandle				CellHandle;\
		typedef typename baseType::Facet				Facet;\
		typedef typename baseType::FacetIterator			FacetIterator;\
		typedef typename baseType::FacetCirculator			FacetCirculator;\
		typedef typename baseType::FiniteFacetsIterator			FiniteFacetsIterator;\
		typedef typename baseType::LocateType				LocateType;\
		typedef typename baseType::EdgeIterator				EdgeIterator;\
		typedef typename RTriangulation::Edge				Edge;\
		typedef typename baseType::FiniteEdgesIterator			FiniteEdgesIterator;\
		typedef typename baseType::VectorVertex				VectorVertex;\
		typedef typename baseType::VectorCell				VectorCell;\
		typedef typename baseType::ListPoint				ListPoint;\
		typedef typename baseType::VCellIterator			VCellIterator;

// Classe Tesselation, contient les fonctions permettant de calculer la Tessalisation
// d'une RTriangulation et de stocker les centres dans chacune de ses cellules

//TT is of model TriangulationTypes 
template<class TT>
class _Tesselation
{
public:
	typedef typename TT::RTriangulation							RTriangulation;
	typedef typename TT::AlphaShape						 		AlphaShape;
	typedef typename TT::Vertex_Info							VertexInfo;
	typedef typename TT::Cell_Info								CellInfo;
	typedef typename RTriangulation::Vertex_iterator		 			VertexIterator;
	typedef typename RTriangulation::Vertex_handle                      			VertexHandle;
	typedef typename RTriangulation::Finite_vertices_iterator                    		FiniteVerticesIterator;
	typedef typename RTriangulation::Cell_iterator						CellIterator;
	typedef typename RTriangulation::Finite_cells_iterator					FiniteCellsIterator;
	typedef typename RTriangulation::Cell_circulator					CellCirculator;
	typedef typename RTriangulation::Cell_handle						CellHandle;
	typedef typename RTriangulation::Facet							Facet;
	typedef typename RTriangulation::Facet_iterator						FacetIterator;
	typedef typename RTriangulation::Facet_circulator					FacetCirculator;
	typedef typename RTriangulation::Finite_facets_iterator					FiniteFacetsIterator;
	typedef typename RTriangulation::Locate_type						LocateType;
	typedef typename RTriangulation::Edge_iterator						EdgeIterator;
	typedef typename RTriangulation::Edge							Edge;
	typedef typename RTriangulation::Finite_edges_iterator					FiniteEdgesIterator;	
	
	typedef std::vector<VertexHandle>							VectorVertex;
	typedef std::vector<CellHandle>								VectorCell;
	typedef std::list<Point>								ListPoint;
	typedef typename VectorCell::iterator							VCellIterator;
	int maxId;

protected:
	RTriangulation* Tri;
	RTriangulation* Tes; //=NULL or Tri depending on the constructor used.

public:
	Real TotalFiniteVoronoiVolume;
	Real area; 
	Real TotalInternalVoronoiVolume;
	Real TotalInternalVoronoiPorosity;
	VectorVertex vertexHandles;//This is a redirection vector to get vertex pointers by spheres id
	VectorCell cellHandles;//for speedup of global loops, iterating on this vector is faster than cellIterator++
	bool redirected;//is vertexHandles filled with current vertex pointers? 
	bool computed;

	_Tesselation(void);
	_Tesselation(RTriangulation &T);// : Tri(&T) { Calcule(); }
	~_Tesselation(void);
	
	///Insert a sphere
	VertexHandle insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false);
	/// move a spheres
	VertexHandle move (Real x, Real y, Real z, Real rad, unsigned int id);
	///Fill a vector with vertexHandles[i] = handle of vertex with id=i for fast access
	bool redirect (void);
	///Remove a sphere
	bool remove (unsigned int id); 
	int Max_id (void) {return maxId;}
	
	void	compute ();	//Calcule le centres de Voronoi pour chaque cellule
	Point   setCircumCenter (const CellHandle& cell, bool force=0); 
        Point	circumCenter (const Sphere& S0, const Sphere& S1, const Sphere& S2, const Sphere& S3);
	Point	circumCenter (const CellHandle& cell);
        Point	circumCenter (const CellHandle& cell, const short facet, const double wExt, bool& violate, Sphere& SAlpha, CVector& normal);
        Point	circumCenter (const CellHandle& cell, const short facet, const Sphere& sExt, bool& violate);
	void	Invalidate () {computed=false;}  //Set the tesselation as "not computed" (computed=false), this will launch tesselation internaly when using functions like computeVolumes())
	// N.B : compute() must be executed before the functions below are used
	void	Clear(void);

	static Point	Dual	(const CellHandle &cell);	
	static Plane	Dual	(VertexHandle S1, VertexHandle S2);
	static Segment  Dual	(FiniteFacetsIterator &facet);	//G�n�re le segment dual d'une facette finie
	static Real	Volume	(FiniteCellsIterator cell);
	inline void 	AssignPartialVolume	(FiniteEdgesIterator& ed_it);
	double		computeVFacetArea (FiniteEdgesIterator ed_it);
	void		ResetVCellVolumes	(void);
	void		computeVolumes		(void);//compute volume each voronoi cell
	void		computePorosity		(void);//compute volume and porosity of each voronoi cell
	inline Real&	Volume (unsigned int id) { return vertexHandles[id]->info().v(); }
	inline const VertexHandle&	vertex (unsigned int id) const { return vertexHandles[id]; }

	// Alpha Shapes
	void testAlphaShape(double alpha=0);
	struct AlphaFace {unsigned int ids[3]; CVector normal;};
        struct AlphaCap {unsigned int id; CVector normal;};
	void setAlphaFaces(std::vector<AlphaFace>& faces, double alpha=0);
        void setExtendedAlphaCaps(std::vector<AlphaCap>& caps, double alpha, double shrinkedAlpha, bool fixedAlpha);
        std::vector<Vector3r> getExtendedAlphaGraph(double alpha, double shrinkedAlpha, bool fixedAlpha);
	CVector alphaVoronoiFaceArea (const Edge& ed_it, const AlphaShape& as, const RTriangulation& Tri);
        CVector alphaVoronoiPartialCapArea (const Edge& ed_it, const AlphaShape& as,std::vector<Vector3r>& vEdges);
        CVector alphaVoronoiPartialCapArea (Facet facet, const AlphaShape& as, double shrinkedAlpha, std::vector<Vector3r>& vEdges);
	std::vector<int> getAlphaVertices(double alpha=0);
	
// 	FiniteCellsIterator finite_cells_begin(void);// {return Tri->finite_cells_begin();}
// 	FiniteCellsIterator finiteCellsEnd(void);// {return Tri->finite_cells_end();}
	void voisins (VertexHandle v, VectorVertex& Output_vector);// {Tri->incident_vertices(v, back_inserter(Output_vector));}
	RTriangulation& Triangulation (void);// {return *Tri;}

// 	bool computed (void) {return computed;}

	bool is_short ( FiniteFacetsIterator f_it );
	inline bool is_internal ( FiniteFacetsIterator &facet );//

	long newListeEdges	( Real** Coordonnes );	//Genere la liste des segments de Voronoi
	long newListeShortEdges	( Real** Coordonnes );	//Genere la version tronquee (partie interieure) du graph de Voronoi
	long newListeShortEdges2	( Real** Coordonnes );
	long New_liste_adjacent_edges ( VertexHandle vertex0, Real** Coordonnes );
};



template<class Tesselation>
class PeriodicTesselation : public Tesselation
{
	public:
	DECLARE_TESSELATION_TYPES(Tesselation)
	using Tesselation::Tri;
	using Tesselation::vertexHandles;
	using Tesselation::maxId;
	using Tesselation::redirected;
		
	///Insert a sphere, which can be a duplicate one from the base period if duplicateOfId>=0
	VertexHandle insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false, int duplicateOfId=-1);
	///Fill a vector with vertexHandles[i] = handle of vertex with id=i for fast access, contains only spheres from the base period
	bool redirect (void);
};

} // namespace CGT

#include "Tesselation.ipp"

//Explicit instanciation
typedef CGT::_Tesselation<CGT::SimpleTriangulationTypes>		SimpleTesselation;



