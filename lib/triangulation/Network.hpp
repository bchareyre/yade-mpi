/*************************************************************************
*  Copyright (C) 2010 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

#pragma once

#include "Tesselation.h"
#include "Timer.h"
#include "basicVTKwritter.hpp"

/**
Defines class Network. Which contains the geometrical representation of a pore network on the basis of regular triangulation (using CGAL lib)
The class is the base of the pore-flow model. It has basic functions to compute quantities like void volumes and solid surfaces in the triangulation's elements.

The same data structure is used with different template parameters for periodic and aperiodic boundary conditions. The network is bounded by infinite planes represented in the triangulation by very large spheres (so that their surface looks flat at the scale of the network).

Two triangulations are in fact contained in the network, so that a simulation can switch between them and pass data from one to the other. Otherwise, some info would be lost when the problem is retriangulated.
*/

namespace CGT {
/// Representation of a boundary condition along an axis aligned plane.
struct Boundary
{
	Point p;//position
	Vecteur normal;//orientation
	Vector3r velocity;//motion
	int coordinate;//the axis perpendicular to the boundary
	bool flowCondition;//flowCondition=0, pressure is imposed // flowCondition=1, flow is imposed
	Real value;// value of imposed pressure
	bool useMaxMin;// tells if this boundary was placed following the particles (using min/max of them) or with user defined position
};


template<class Tesselation>
class Network
{
	public:
		DECLARE_TESSELATION_TYPES(Tesselation) //see Tesselation.h
		
		virtual ~Network();
		Network();
		
		Tesselation T [2];
		bool currentTes;
		double x_min, x_max, y_min, y_max, z_min, z_max, Rmoy, SectionArea, Height, Vtotale;
		bool DEBUG_OUT;
		int nOfSpheres;
		int x_min_id, x_max_id, y_min_id, y_max_id, z_min_id, z_max_id;
		int* boundsIds [6];
		vector<Cell_handle> boundingCells [6];
		Point Corner_min;
		Point Corner_max;
		Real Vsolid_tot, Vtotalissimo, Vporale, Ssolid_tot, V_porale_porosity, V_totale_porosity;
		Boundary boundaries [6];
		Boundary& boundary (int b) {return boundaries[b-id_offset];}
		short id_offset;
		int vtk_infinite_vertices, vtk_infinite_cells, num_particles;

		void AddBoundingPlanes();
		void AddBoundingPlane (Vecteur Normal, int id_wall);
		void AddBoundingPlane (Real center[3], double thickness, Vecteur Normal, int id_wall );

		void Define_fictious_cells( );
		int detectFacetFictiousVertices (Cell_handle& cell, int& j);
		double volumeSolidPore (const Cell_handle& cell);
		double volume_single_fictious_pore(const Vertex_handle& SV1, const Vertex_handle& SV2, const Vertex_handle& SV3, const Point& PV1,  const Point& PV2, Vecteur& facetSurface);
		double volume_double_fictious_pore(const Vertex_handle& SV1, const Vertex_handle& SV2, const Vertex_handle& SV3, const Point& PV1, const Point& PV2, Vecteur& facetSurface);
		double spherical_triangle_volume(const Sphere& ST1, const Point& PT1, const Point& PT2, const Point& PT3);
		
		double fast_spherical_triangle_area(const Sphere& STA1, const Point& STA2, const Point& STA3, const Point& PTA1);
		Real fast_solid_angle(const Point& STA1, const Point& PTA1, const Point& PTA2, const Point& PTA3);
		double volume_double_fictious_pore(Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1);
		double volume_single_fictious_pore(Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1);
		double Volume_Pore_VoronoiFraction ( Cell_handle& cell, int& j, bool reuseFacetData=false);
		double Surface_Solid_Pore( Cell_handle cell, int j, bool SLIP_ON_LATERALS, bool reuseFacetData=false);
		double spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 );
		
		Vecteur surface_double_fictious_facet(Vertex_handle fSV1, Vertex_handle fSV2, Vertex_handle SV3);
		Vecteur surface_single_fictious_facet(Vertex_handle fSV1, Vertex_handle SV2, Vertex_handle SV3);
		double surface_solid_double_fictious_facet(Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3);
		double surface_solid_facet(Sphere ST1, Sphere ST2, Sphere ST3);

		int facetF1, facetF2, facetRe1, facetRe2, facetRe3;
		int F1, F2, Re1, Re2;
		int facetNFictious;
		int real_vertex;
		double FAR;
		static const double ONE_THIRD;
		static const int facetVertices [4][3];
		static const int permut3 [3][3];
		static const int permut4 [4][4];
};

} //namespaceCGT

#include "Network.ipp"



#endif //FLOW_ENGINE
