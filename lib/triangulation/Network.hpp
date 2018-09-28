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
	CVector normal;//orientation
	Vector3r velocity;//motion
	int coordinate;//the axis perpendicular to the boundary
	bool flowCondition;//flowCondition=0, pressure is imposed // flowCondition=1, flow is imposed
	Real value;// value of imposed pressure
	bool useMaxMin;// tells if this boundary was placed following the particles (using min/max of them) or with user defined position
};

struct ThermalBoundary
{
	Point p;//position
	CVector normal;//orientation
	Vector3r velocity;//motion
	int coordinate;//the axis perpendicular to the boundary
	bool fluxCondition;//fluxCondition=0, temperature is imposed // fluxCondition=1, flux is imposed
	Real value;// value of imposed temperature
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
		Tesselation& tesselation() {return T[currentTes];};
		
		double xMin, xMax, yMin, yMax, zMin, zMax, Rmoy, sectionArea, Height, vTotal;
		bool debugOut;
		int nOfSpheres;
		int xMinId, xMaxId, yMinId, yMaxId, zMinId, zMaxId;
		int* boundsIds [6];
		vector<CellHandle> boundingCells [6];
		vector<CellHandle> thermalBoundingCells [6];
		vector<CellHandle> conductionBoundingCells [6];
		Point cornerMin;
		Point cornerMax;
		Real VSolidTot, Vtotalissimo, vPoral, sSolidTot, vPoralPorosity, vTotalPorosity;
		Boundary boundaries [6];
		ThermalBoundary thermalBoundaries [6];
		ThermalBoundary conductionBoundaries [6];
		Boundary& boundary (int b) {return boundaries[b-idOffset];}
		ThermalBoundary& thermalBoundary (int b) {return thermalBoundaries[b-idOffset];}
		ThermalBoundary& conductionBoundary (int b) {return conductionBoundaries[b-idOffset];}
		short idOffset;
		int vtkInfiniteVertices, vtkInfiniteCells, num_particles;

		void addBoundingPlanes();
		void addBoundingPlane (CVector Normal, int id_wall);
		void addBoundingPlane (Real center[3], double thickness, CVector Normal, int id_wall );

		void defineFictiousCells( );
		int detectFacetFictiousVertices (CellHandle& cell, int& j);
		double volumeSolidPore (const CellHandle& cell);
		double volumeSingleFictiousPore(const VertexHandle& SV1, const VertexHandle& SV2, const VertexHandle& SV3, const Point& PV1,  const Point& PV2, CVector& facetSurface);
		double volumeDoubleFictiousPore(const VertexHandle& SV1, const VertexHandle& SV2, const VertexHandle& SV3, const Point& PV1, const Point& PV2, CVector& facetSurface);
		double sphericalTriangleVolume(const Sphere& ST1, const Point& PT1, const Point& PT2, const Point& PT3);
		
		double fastSphericalTriangleArea(const Sphere& STA1, const Point& STA2, const Point& STA3, const Point& PTA1);
		Real fastSolidAngle(const Point& STA1, const Point& PTA1, const Point& PTA2, const Point& PTA3);
		double volumeDoubleFictiousPore(VertexHandle SV1, VertexHandle SV2, VertexHandle SV3, Point PV1);
		double volumeSingleFictiousPore(VertexHandle SV1, VertexHandle SV2, VertexHandle SV3, Point PV1);
		double volumePoreVoronoiFraction ( CellHandle& cell, int& j, bool reuseFacetData=false);
		double surfaceSolidThroat( CellHandle cell, int j, bool slipBoundary, bool reuseFacetData=false);
		double surfaceSolidThroatInPore( CellHandle cell, int j, bool slipBoundary, bool reuseFacetData=false);// returns the solid area in the throat, keeping only that part of the throat in cell
		double sphericalTriangleArea ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 );
		
		CVector surfaceDoubleFictiousFacet(VertexHandle fSV1, VertexHandle fSV2, VertexHandle SV3);
		CVector surfaceSingleFictiousFacet(VertexHandle fSV1, VertexHandle SV2, VertexHandle SV3);
		double surfaceSolidDoubleFictiousFacet(VertexHandle SV1, VertexHandle SV2, VertexHandle SV3);
		double surfaceSolidFacet(Sphere ST1, Sphere ST2, Sphere ST3);

		void lineSolidPore(CellHandle cell, int j);
		double lineSolidFacet(Sphere ST1, Sphere ST2, Sphere ST3);

		int facetF1, facetF2, facetRe1, facetRe2, facetRe3;
		int facetNFictious;
		double FAR;
		static const double ONE_THIRD;
		static const int facetVertices [4][3];
		static const int permut3 [3][3];
		static const int permut4 [4][4];
};

} //namespaceCGT

#include "Network.ipp"

#endif //FLOW_ENGINE
