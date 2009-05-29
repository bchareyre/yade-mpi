/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
**************************************************************************/

#include "SpherePadder.hpp"
#include "TetraMesh.hpp"
#include <boost/python.hpp>


boost::python::list getSphereList(SpherePadder* SP)
{
    boost::python::list l;
    vector <Sphere> & spheres = SP->getSphereList();
    double xtrans = SP->getMesh()->xtrans;
    double ytrans = SP->getMesh()->ytrans;
    double ztrans = SP->getMesh()->ztrans;
    for (unsigned int i = 0 ; i < spheres.size() ; ++i)
    {
        if (spheres[i].type == VIRTUAL || spheres[i].R <= 0.0) continue;
        l.append( boost::python::make_tuple(spheres[i].x+xtrans, spheres[i].y+ytrans, spheres[i].z+ztrans, spheres[i].R) );
    }
    return l;
}


BOOST_PYTHON_MODULE(packing){
    using namespace boost::python;
    
    class_<TetraMesh>("TetraMesh")
            .def("read", &TetraMesh::read)
            .def("read_gmsh", &TetraMesh::read_gmsh)
            .def("read_inp", &TetraMesh::read_inp)
            ;

    class_<SpherePadder>("SpherePadder")

            .def("ShutUp", &SpherePadder::ShutUp)
            .def("Speak", &SpherePadder::Speak)
            .def("setRadiusRatio", &SpherePadder::setRadiusRatio)
            .def("setRadiusRange", &SpherePadder::setRadiusRange)
            .def("setMaxOverlapRate", &SpherePadder::setMaxOverlapRate)
            .def("setVirtualRadiusFactor", &SpherePadder::setVirtualRadiusFactor)
            .def("setMaxNumberOfSpheres", &SpherePadder::setMaxNumberOfSpheres)
            .def("setMaxSolidFractioninProbe", &SpherePadder::setMaxSolidFractioninProbe)
            .def("getNumberOfSpheres", &SpherePadder::getNumberOfSpheres)
            .def("getMeanSolidFraction", &SpherePadder::getMeanSolidFraction)

            .def("plugTetraMesh", &SpherePadder::plugTetraMesh)
            .def("save_mgpost", &SpherePadder::save_mgpost)
            .def("save_Rxyz", &SpherePadder::save_Rxyz)

            .def("pad_5", &SpherePadder::pad_5)
            .def("place_virtual_spheres", &SpherePadder::place_virtual_spheres)
            .def("densify", &SpherePadder::densify)
            .def("insert_sphere", &SpherePadder::insert_sphere)
            ;
    
    def("getSphereList", getSphereList);
}


    
