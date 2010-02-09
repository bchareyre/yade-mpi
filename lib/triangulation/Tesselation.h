/*
D�finit la classe Tesselation :
- comme donn�e membre une triangulation r�guli�re (dont les cellules doivent pouvoir contenir
une info "Point" accessible par la fonction c.info() )
- comme fonctions membres de quoi calculer la tessalisation duale

// N.B : Calcule() doit �tre ex�cut�e au moins une fois
*/

#pragma once


#include "def_types.h"
#include "Operations.h"
#include "RegularTriangulation.h"
#include "stdafx.h"

namespace CGT {

// Classe Tesselation, contient les fonctions permettant de calculer la Tessalisation
// d'une RTriangulation et de stocker les centres dans chacune de ses cellules
class Tesselation
{
public:
	typedef std::vector<Vertex_handle>	Vector_Vertex;
	typedef std::vector<Cell_handle>	Vector_Cell;
	typedef std::list<Point>		List_Point;
	typedef Vector_Cell::iterator		VCell_iterator;
	int max_id;
	//Donn�es

private:
	RTriangulation* Tri;
	RTriangulation* Tes; //=NULL ou Tri selon le constructeur appell�, permet au destructeur
						 //de savoir s'il faut d�truire la Triangulation ou non
	bool computed;
public:
	Real TotalFiniteVoronoiVolume;
	Real TotalInternalVoronoiVolume;
	Real TotalInternalVoronoiPorosity;
	Vector_Vertex vertexHandles;//This is a redirection vector to get vertex pointers by spheres id
	bool redirected;//is vertexHandles filled with current vertex pointers? 	


	//Fonctions

public:
	Tesselation(void);
	Tesselation(RTriangulation &T);// : Tri(&T) { Calcule(); }
	~Tesselation(void);
	
	///Insert a sphere
	//bool insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false);
	Vertex_handle insert(Real x, Real y, Real z, Real rad, unsigned int id, bool isFictious = false);
	/// move a spheres
	Vertex_handle move (Real x, Real y, Real z, Real rad, unsigned int id);
	///Fill a vector with vertexHandles[i] = handle of vertex with id=i for fast access
	bool redirect (void);
	///Remove a sphere
	bool remove (unsigned int id); 
	unsigned int Max_id (void) {return max_id;}
	
	void	Compute ();	//Calcule le centres de Voronoi pour chaque cellule
	void	Invalidate () {computed=false;}  //Set the tesselation as "not computed" (computed=false), this will launch 						//tesselation internaly when using functions like computeVolumes())
	// N.B : Calcule() doit �tre ex�cut�e au moins une fois avant
	// toute autre fonction ci-dessous, elle est ex�cut�e dans le constructeur
	void	Clear(void);

	static Point	Dual	(const Cell_handle &cell);	
	static Plan	Dual	(Vertex_handle S1, Vertex_handle S2);
	static Segment  Dual	(Finite_facets_iterator &facet);	//G�n�re le segment dual d'une facette finie
	static Real	Volume	(Finite_cells_iterator cell);
	inline void 		AssignPartialVolume	(Finite_edges_iterator& ed_it);
	void		ResetVCellVolumes	(void);
	void		ComputeVolumes		(void);//Compute volume each voronoi cell
	void		ComputePorosity		(void);//Compute volume and porosity of each voronoi cell
	inline Real&	Volume (unsigned int id) { return vertexHandles[id]->info().v(); }

	
	Finite_cells_iterator finite_cells_begin(void);// {return Tri->finite_cells_begin();}
	Finite_cells_iterator finite_cells_end(void);// {return Tri->finite_cells_end();}
	void Voisins (Vertex_handle v, Vector_Vertex& Output_vector);// {Tri->incident_vertices(v, back_inserter(Output_vector));}
	RTriangulation& Triangulation (void);// {return *Tri;}



	bool Computed (void) {return computed;}

	bool is_short ( Finite_facets_iterator f_it );
	inline bool is_internal ( Finite_facets_iterator &facet );//

	long New_liste_edges	( Real** Coordonnes );	//G�n�re la liste des segments de Voronoi
	long New_liste_short_edges	( Real** Coordonnes );	//G�n�re la version tronqu�e (partie int�rieure)
															//du graph de Voronoi
	long New_liste_short_edges2	( Real** Coordonnes );
	long New_liste_adjacent_edges ( Vertex_handle vertex0, Real** Coordonnes );
	
};


} // namespace CGT

