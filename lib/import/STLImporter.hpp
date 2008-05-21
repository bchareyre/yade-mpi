/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef STLIMPORTER_HPP
#define STLIMPORTER_HPP
#include <yade/pkg-common/Vertex.hpp>
#include <yade/pkg-common/InteractingVertex.hpp>
#include <yade/pkg-common/GeometricalEdge.hpp>
#include <yade/pkg-common/InteractingEdge.hpp>
#include <yade/pkg-common/Facet.hpp>
#include <yade/pkg-common/InteractingFacet.hpp>
#include "STLReader.hpp"
#include<yade/core/Body.hpp>
#include<yade/core/BodyContainer.hpp>

class STLImporter {
    public:
	STLImporter();	

	/// open stl file 
	bool open(const char* filename);
	
	/// number of all imported elements 
	int number_of_all_imported() const {return number_of_imported_vertices() + number_of_imported_edges() + number_of_imported_facets();}
	/// number of imported vertices (all|corner)
	int number_of_imported_vertices() const {if (import_vertices) return (import_flat_vertices_flag) ? number_of_vertices() : number_of_corner_vertices(); else return 0;}
	/// number of imported edges (all|corner)
	int number_of_imported_edges() const {if (import_edges) return (import_flat_edges_flag) ? number_of_edges() : number_of_corner_edges(); else return 0;}
	/// number of imported facets (all|corner)
	int number_of_imported_facets() const {return (import_facets) ? number_of_facets() : 0;}
	
	/// number of all vertices
	int number_of_vertices() const { return vts.size(); }
	/// number of corner vertices
	int number_of_corner_vertices() const { return number_of_vertices()-number_of_flat_vertices(); }
	/// number of vertices lies on a flats
	int number_of_flat_vertices() const { return nbFlatVertices; }
	/// number of facets 
	int number_of_facets() const { return fcs.size(); }
	/// number of all edges
	int number_of_edges() const { return egs.size(); }
	/// number of corner edges
	int number_of_corner_edges() const { return number_of_edges()-number_of_flat_edges(); }
	/// number of edges lies on a flats
	int number_of_flat_edges() const { return nbFlatEdges; }

	/// max number of facet vertices (by default 4)
	int max_vertices_in_facet; 

	void set_imported_stuff(bool v, bool e, bool f) {import_vertices=v; import_edges=e; import_facets=f;}
	/// import vertices (by default true)
	bool import_vertices;
	/// import edges (by default true)
	bool import_edges;
	/// import facets (by default true)
	bool import_facets;

	/// (by default false)
	bool facets_wire;

	/// import edges on a flats (by default false)
	bool import_flat_edges_flag; 
	/// import vertices on a flats (by default false)
	bool import_flat_vertices_flag; 

	/// import geometry (sequence vertices, edges, facets)
	void import(shared_ptr<BodyContainer> bodies, unsigned int begin=0);

    protected:
	/// Vertex
	struct Vrtx {
	    Vector3r point;
	    vector<Vector3r> normals;
	    bool flat;
	    Vrtx(Real x, Real y, Real z) : point(x,y,z), flat(false) {}
	};

	/// Edge
	struct Edg {
	    int first, second;
	    Vector3r normal1, normal2;
	    bool both,flat;
	    Edg(int f, int s) : first(f), second(s), both(false), flat(false) {}
	};

	/// Facet
	struct Fct {
	    vector<int> vertex;
	    /// constructor (each facet have 3 vertices as minimum)
	    Fct(int f, int s, int t) { vertex.resize(3); vertex[0]=f; vertex[1]=s; vertex[2]=t; }
	    /// get vertex i
	    int& operator[] (int i) { return vertex[i]; }
	    int operator[] (int i) const { return vertex[i]; }
	    /// number of vertices
	    int size() const { return vertex.size(); }
	    /// get edge i (vertices i,i+1)
	    pair<int,int> edge(int i) const { return pair<int,int>(vertex[i], ( (i+1<size()) ? vertex[i+1] : vertex[0]) ); }
	    /// remove edge i
	    void remove_edge(int i) { vertex.erase(vertex.begin()+i); vertex.erase( vertex.begin()+( (i<size()) ? i:0) ); }
	};

	/// remove vertices of facets which lies of a flat
	void reduce_facet_vertices(Fct& f); 

	// FIXME: add  checking of a orientation (clockwise)
	/// check convexity of a facet
	bool convex_test(const Fct& f) const;

	friend ostream& operator<<(ostream& os, const Fct& f);

	/// define cut off flats for a edge
	void processEdge(int id);

	/// join facets which have a common edge and lies on a flat 
	void facetsConsolidation();
	/// define cut off flats for a vertex
	void processVertex(int id);

    private:
	int nbFlatVertices;
	int nbFlatEdges;
	vector<Vrtx> vts;
	vector<Edg> egs;
	vector<Fct> fcs;	
};

ostream& operator<<(ostream& os, const STLImporter::Fct& f);

#endif // STLIMPORTER_HPP
