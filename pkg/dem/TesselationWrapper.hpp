/*************************************************************************
*  Copyright (C) 2008 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL

#pragma once
#include<core/GlobalEngine.hpp>
#include<pkg/common/Sphere.hpp>
#include<core/Omega.hpp>
#include<core/Scene.hpp>
#include<lib/triangulation/Tesselation.h>
#include<pkg/dem/MicroMacroAnalyser.hpp>

/*! \class TesselationWrapper
 * \brief Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities : currently volume and porosity of each Voronoï sphere.
 * More accessors in course of implementation. Feel free to suggest new ones.
 *
 * Example usage script :
 *
 *tt=TriaxialTest()
 *tt.generate("test.yade")
 *O.load("test.yade")
 *O.run(100,True) 
 *TW=TesselationWrapper()
 *TW.triangulate() #compute regular Delaunay triangulation, don't construct tesselation
 *TW.computeVolumes() #will silently tesselate the packing
 *TW.volume(10) #get volume associated to sphere of id 10
 *
 */


class TesselationWrapper : public GlobalEngine{
public:
	typedef CGT::_Tesselation<CGT::SimpleTriangulationTypes> 			Tesselation;
	typedef Tesselation::RTriangulation						RTriangulation;
	typedef Tesselation::VertexInfo							VertexInfo;
	typedef Tesselation::CellInfo							CellInfo;
	typedef RTriangulation::Finite_edges_iterator					FiniteEdgesIterator;
	typedef Tesselation::AlphaFace							AlphaFace;
	typedef Tesselation::AlphaCap                                                   AlphaCap;
	
	
	
	Tesselation* Tes;
	double mean_radius, inf;
	bool rad_divided;
	bool bounded;
	CGT::Point Pmin;
	CGT::Point Pmax;

	~TesselationWrapper();

    	/// Insert a sphere, "id" will be used by some getters to retrieve spheres
    	bool insert(double x, double y, double z, double rad, unsigned int id);
	/// A faster version of insert, inserting all spheres in scene (first erasing current triangulation  if reset=true)
	void insertSceneSpheres(bool reset=true);
	/// Move one sphere to the new position (x,y,z) and maintain triangulation (invalidates the tesselation)
	bool move (double x, double y, double z, double rad, unsigned int id);

    	void checkMinMax(double x, double y, double z, double rad);//for experimentation purpose
	/// Reset the triangulation
    	void clear(void);
    	void clear2(void);

	/// Add axis aligned bounding planes (modelised as spheres with (almost) infinite radius)
  	void 	addBoundingPlanes (void);
	/// Force boudaries at positions not equal to precomputed ones
 	void	addBoundingPlanes(double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz);
	void 	RemoveBoundingPlanes (void);
	///compute voronoi centers then stop (don't compute anything else)
 	void	computeTesselation (void);
 	void	computeTesselation( double pminx, double pmaxx, double pminy, double pmaxy, double pminz, double pmaxz);
	
	void	testAlphaShape(double alpha) {Tes->testAlphaShape(alpha);}
	boost::python::list getAlphaFaces(double alpha);
	boost::python::list getAlphaCaps(double alpha, double shrinkedAlpha, bool fixedAlpha);
	boost::python::list getAlphaVertices(double alpha);
        boost::python::list getAlphaGraph(double alpha, double shrinkedAlpha, bool fixedAlpha);
	void applyAlphaForces(Matrix3r stress, double alpha, double shrinkedAlpha, bool fixedAlpha);

	///compute Voronoi vertices + volumes of all cells
	///use computeTesselation to force update, e.g. after spheres positions have been updated
	void	computeVolumes	(void);
	void	computeDeformations (void) {mma.analyser->computeParticlesDeformation();}
	///Get volume of the sphere inserted with indentifier "id""
	double	Volume	(unsigned int id);
	double	deformation (unsigned int id,unsigned int i,unsigned int j) {
		if (!mma.analyser->ParticleDeformation.size()) {LOG_ERROR("compute deformations first"); return 0;}
		if (mma.analyser->ParticleDeformation.size()<id) {LOG_ERROR("id out of bounds"); return 0;}
		if (i<1 || i>3 || j<1 || j>3) {LOG_ERROR("tensor index must be between 1 and 3"); return 0;}
		return mma.analyser->ParticleDeformation[id](i,j);}

	/// number of facets in the tesselation (finite branches of the triangulation)
	unsigned int NumberOfFacets(bool initIters=false);
	/// set first and last facets, set facet_it = facet_begin
	void InitIter(void);
	/// set facet = next pair (body1->id,body2->id), returns facet_it==facet_end
	bool nextFacet (std::pair<unsigned int, unsigned int>& facet);

	/// make the current state the initial (0) or final (1) configuration for the definition of displacement increments, use only state=0 if you just want to get only volmumes and porosity
	void setState (bool state=0);
	void loadState (string fileName, bool stateNumber=0, bool bz2=false);
	void saveState (string fileName, bool stateNumber=0, bool bz2=false);
	/// read two state files and write per-particle deformation to a vtk file. The second variant uses existing states.
 	void defToVtkFromStates (string inputFile1, string inputFile2, string outputFile="def.vtk", bool bz2=false);
	void defToVtkFromPositions (string inputFile1, string inputFile2, string outputFile="def.vtk", bool bz2=false);
	void defToVtk (string outputFile="def.vtk");

	/// return python array containing voronoi volumes, per-particle porosity, and optionaly per-particle deformation, if states 0 and 1 have been assigned
	boost::python::dict getVolPoroDef(bool deformation);//FIXME ; unexplained crash for now


public:
	/// edge iterators are used for returning tesselation "facets", i.e. spheres with a common branch in the triangulation, convert CGAL::edge to int pair (b1->id, b2->id)
	FiniteEdgesIterator facet_begin;
	FiniteEdgesIterator facet_end;
	FiniteEdgesIterator facet_it;
	MicroMacroAnalyser mma;

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(TesselationWrapper,GlobalEngine,"Handle the triangulation of spheres in a scene, build tesselation on request, and give access to computed quantities (see also the `dedicated section in user manual <https://yade-dem.org/doc/user.html#micro-stress-and-micro-strain>`_). The calculation of microstrain is explained in [Catalano2014a]_ \n\nSee example usage in script example/tesselationWrapper/tesselationWrapper.py.\n\nBelow is an output of the :yref:`defToVtk<TesselationWrapper::defToVtk>` function visualized with paraview (in this case Yade's TesselationWrapper was used to process experimental data obtained on sand by Edward Ando at Grenoble University, 3SR lab.)\n\n.. figure:: fig/localstrain.*",
	((unsigned int,n_spheres,0,,"|ycomp|"))
	((Real,far,10000.,,"Defines the radius of the large virtual spheres used to define nearly flat boundaries around the assembly. The radius will be the (scene's) bounding box size multiplied by 'far'. Higher values will minimize the error theoretically (since the infinite sphere really defines a plane), but it may increase numerical errors at some point. The default should give a resonable compromize."))
	,/*ctor*/
  	Tes = new Tesselation;
	clear();
	facet_begin = Tes->Triangulation().finite_edges_begin();
	facet_end = Tes->Triangulation().finite_edges_end();
	facet_it = Tes->Triangulation().finite_edges_begin();
	inf=1e10;
	mma.analyser->SetConsecutive(false);
	,/*py*/
	.def("triangulate",&TesselationWrapper::insertSceneSpheres,(boost::python::arg("reset")=true),"triangulate spheres of the packing")
 	.def("setState",&TesselationWrapper::setState,(boost::python::arg("state")=0),"Make the current state of the simulation the initial (0) or final (1) configuration for the definition of displacement increments, use only state=0 if you just want to get  volmumes and porosity.")
 	.def("loadState",&TesselationWrapper::loadState,(boost::python::arg("inputFile")="state",boost::python::arg("state")=0,boost::python::arg("bz2")=true),"Load a file with positions to define state 0 or 1.")
 	.def("saveState",&TesselationWrapper::saveState,(boost::python::arg("outputFile")="state",boost::python::arg("state")=0,boost::python::arg("bz2")=true),"Save a file with positions, can be later reloaded in order to define state 0 or 1.")
 	.def("volume",&TesselationWrapper::Volume,(boost::python::arg("id")=0),"Returns the volume of Voronoi's cell of a sphere.")
 	.def("defToVtk",&TesselationWrapper::defToVtk,(boost::python::arg("outputFile")="def.vtk"),"Write local deformations in vtk format from states 0 and 1.")
 	.def("defToVtkFromStates",&TesselationWrapper::defToVtkFromStates,(boost::python::arg("input1")="state1",boost::python::arg("input2")="state2",boost::python::arg("outputFile")="def.vtk",boost::python::arg("bz2")=true),"Write local deformations in vtk format from state files (since the file format is very special, consider using defToVtkFromPositions if the input files were not generated by TesselationWrapper).")
 	.def("defToVtkFromPositions",&TesselationWrapper::defToVtkFromPositions,(boost::python::arg("input1")="pos1",boost::python::arg("input2")="pos2",boost::python::arg("outputFile")="def.vtk",boost::python::arg("bz2")=false),"Write local deformations in vtk format from positions files (one sphere per line, with x,y,z,rad separated by spaces).")
 	.def("computeVolumes",&TesselationWrapper::computeVolumes,"compute volumes of all Voronoi's cells.")
	.def("getVolPoroDef",&TesselationWrapper::getVolPoroDef,(boost::python::arg("deformation")=false),"Return a table with per-sphere computed quantities. Include deformations on the increment defined by states 0 and 1 if deformation=True (make sure to define states 0 and 1 consistently).")
	.def("computeDeformations",&TesselationWrapper::computeDeformations,"compute per-particle deformation. Get it with :yref:`TesselationWrapper::deformation` (id,i,j).")
	.def("deformation",&TesselationWrapper::deformation,(boost::python::arg("id"),boost::python::arg("i"),boost::python::arg("j")),"Get particle deformation")
	.def("testAlphaShape",&TesselationWrapper::testAlphaShape,(boost::python::arg("alpha")=0),"transitory function, testing AlphaShape feature")
	.def("getAlphaFaces",&TesselationWrapper::getAlphaFaces,(boost::python::arg("alpha")=0),"Get the list of alpha faces for a given alpha. If alpha is not specified or null the minimum alpha resulting in a unique connected domain is used")
        .def("getAlphaCaps",&TesselationWrapper::getAlphaCaps,(boost::python::arg("alpha")=0,boost::python::arg("shrinkedAlpha")=0,boost::python::arg("fixedAlpha")=false),"Get the list of area vectors for the polyhedral caps associated to boundary particles ('extended' alpha-contour). If alpha is not specified or null the minimum alpha resulting in a unique connected domain is used. Taking a smaller 'shrinked' alpha for placing the virtual spheres moves the enveloppe outside the packing, It should be ~(alpha-refRad) typically.")
	.def("applyAlphaForces",&TesselationWrapper::applyAlphaForces,(boost::python::arg("stress"),boost::python::arg("alpha")=0,boost::python::arg("shrinkedAlpha")=0,boost::python::arg("fixedAlpha")=false),"set permanent forces based on stress using an alpha shape")
        .def("getAlphaGraph",&TesselationWrapper::getAlphaGraph,(boost::python::arg("alpha")=0,boost::python::arg("shrinkedAlpha")=0,boost::python::arg("fixedAlpha")=false),"Get the list of area vectors for the polyhedral caps associated to boundary particles ('extended' alpha-contour). If alpha is not specified or null the minimum alpha resulting in a unique connected domain is used")
	.def("getAlphaVertices",&TesselationWrapper::getAlphaVertices,(boost::python::arg("alpha")=0),"Get the list of 'alpha' bounding spheres for a given alpha. If alpha is not specified or null the minimum alpha resulting in a unique connected domain is used. This function is generating a new alpha shape for each call, not to be used intensively.")
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TesselationWrapper);
//} // namespace CGT

#endif /* YADE_CGAL */
