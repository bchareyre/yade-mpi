/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#pragma once

#include <core/Body.hpp>
#include <core/PartialEngine.hpp>
#include <core/Shape.hpp>
#include <lib/base/Logging.hpp>
#include <lib/base/Math.hpp>

//#include <yade/trunk/pkg/dem/deformablecohesive/Node.hpp> //Node shape

/* Before starting the implementation of the deformable element, I am really dissappointed that deformableelement's algorithm relies on its shape.
Shape means "shape" and I think it should not contain any other physical meaning or anything else.

With respect to this view; The deformable element class is derived from the body and knows the information of 
its members that are node shaped bodies.

*/

class NewtonIntegrator;
class InternalForceFunctor;
namespace yade{
typedef Eigen::MatrixXd Matrix;

class DeformableElement: public Shape {
	public:

		typedef std::map<shared_ptr<Body>,Se3r> NodeMap;//Node id's with initial positions first node is selected as the reference node
		typedef std::vector<Vector3r> Triangles; // Used for drawing the element

		unsigned int maxNodeCount;//Maximum number of nodes of this element
		Se3r referenceCoord;//Reference node position in global coordinates

		virtual ~DeformableElement();
		void addNode(const shared_ptr<Body>& subBody);
		shared_ptr<Body> getNode(int id);

		void delNode(const shared_ptr<Body>& subBody);
		std::vector<Vector3r> getDisplacements(void);

		void addFace(Vector3r&);
		void removeLastFace(void);
		//! Recalculate physical properties of DeformableElement.
		//virtual void getMassMatrix()=0;

		Se3r frame_get() const
		{
			const shared_ptr<Body>& member=localmap.begin()->first;
			return member->state->se3;
		}

		void frame_set(Se3r) const
		{
			return;
		}

		boost::python::dict localmap_get();

		virtual Real getVolume(){return -1;}
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DeformableElement,Shape,"Deformable aggregate of nodes",
		((NodeMap,localmap,,,"Ids and relative positions+orientations of members of the deformable element (should not be accessed directly)"))
		((Se3r,elementframe,,,"Position and orientation of the element frame"))
		((Triangles,faces,,,"Faces of the element for drawing"))
		,
		,
		createIndex(); /*ctor*/
		//init();
		,
		/*py*/
		.add_property("elementframe",&DeformableElement::frame_get)
		.def("addNode",&DeformableElement::addNode,"Add a node shared_pt<:yref:'Body'>& as into the element")
		.def("getNode",&DeformableElement::getNode,"Get a node shared_pt<:yref:'Body'>& as into the element")
		.def("delNode",&DeformableElement::delNode,"Remove a node shared_pt<:yref:'Body'>& from the element")
		.def("addFace",&DeformableElement::addFace,"Add a face into the element")
		.def("removeLastFace",&DeformableElement::removeLastFace,"Remove a face from the element")
		.def("getVolume",&DeformableElement::getVolume,"Get volume of the element")

	);
	REGISTER_CLASS_INDEX(DeformableElement,Shape);

};
}
// necessary
using namespace yade;
REGISTER_SERIALIZABLE(DeformableElement);
