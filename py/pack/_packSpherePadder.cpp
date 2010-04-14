/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
**************************************************************************/

#include"SpherePadder/SpherePadder.hpp"
#include<yade/lib-pyutil/doc_opts.hpp>


BOOST_PYTHON_MODULE(_packSpherePadder){

	YADE_SET_DOCSTRING_OPTS;
    
	// not wrapped directly; only pass filename to SpherePadder ctor
	#if 0 
	class_<TetraMesh>("TetraMesh")
		.def("read", &TetraMesh::read)
		.def("read_gmsh", &TetraMesh::read_gmsh)
		.def("read_inp", &TetraMesh::read_inp)
		;
	#endif

    py::class_<SpherePadder>("SpherePadder","Geometrical algorithm for filling tetrahedral mesh with spheres; the algorithm was designed by Jean-François Jerier and is described in [Jerier2009]_.",py::init<std::string,std::string>((py::arg("fileName"),py::arg("meshType")=""),"Initialize using tetrahedral mesh stored in *fileName*. Type of file is determined by extension: .gmsh implies *meshType*='GMSH', .inp implies *meshType*='INP'. If the extension is different, specify *meshType* explicitly. Possible values are 'GMSH' and 'INP'."))

            .add_property("radiusRatio", &SpherePadder::getRadiusRatio,&SpherePadder::setRadiusRatio_simple)
				.def("setRadiusRatio",&SpherePadder::setRadiusRatio,"Like radiusRatio, but taking 2nd parameter.")
            .add_property("radiusRange", &SpherePadder::getRadiusRange,&SpherePadder::setRadiusRange_py)
            .add_property("maxOverlapRate", &SpherePadder::getMaxOverlapRate,&SpherePadder::setMaxOverlapRate)
            .add_property("virtualRadiusFactor", &SpherePadder::getVirtualRadiusFactor,&SpherePadder::setVirtualRadiusFactor)
            .add_property("maxNumberOfSpheres", &SpherePadder::getMaxNumberOfSpheres,&SpherePadder::setMaxNumberOfSpheres)
            .add_property("maxSolidFractioninProbe", &SpherePadder::getMaxSolidFractionInProbe,&SpherePadder::setMaxSolidFractioninProbe_py)
            .add_property("numberOfSpheres", &SpherePadder::getNumberOfSpheres)
            .def_readonly("meanSolidFraction",&SpherePadder::getMeanSolidFraction)

            // .def("plugTetraMesh", &SpherePadder::plugTetraMesh)   // takes pointer
            .def("save_mgpost", &SpherePadder::save_mgpost)
            // .def("save_Rxyz", &SpherePadder::save_Rxyz) /// is asSpherePack.save(...) instead

            .def("pad_5", &SpherePadder::pad_5)
            .def("place_virtual_spheres", &SpherePadder::place_virtual_spheres)
            .def("densify", &SpherePadder::densify)
            .def("insert_sphere", &SpherePadder::insert_sphere)

				.def("asSpherePack",&SpherePadder::getSpherePackObject);
            ;
}


    
