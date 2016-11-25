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
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <lib/base/Logging.hpp>
#include <lib/base/Math.hpp>

//#include <yade/trunk/pkg/dem/deformablecohesive/Node.hpp> //Node shape

/* Before starting the implementation of the deformable element, I am really dissappointed that deformableelement's algorithm relies on its shape.
Shape means "shape" and I think it should not contain any other physical meaning or anything else.

With respect to this view; The deformable element class is derived from the body and knows the information of 
its members that are node shaped bodies.

*/

//class NewtonIntegrator;

class DeformableCohesiveElement: public DeformableElement {
	public:

		struct nodepair:public Serializable{
				public:

				YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(nodepair,Serializable,"Geometry of a body",
						((shared_ptr<Body>,node1,,,"Node1 of node pair"))
						((shared_ptr<Body>,node2,,,"Node2 of node pair")),
						/*ctor*/,
						/*py*/
					);

				// Comparison operator for table sorting.
				bool operator<(const nodepair& param) const
				{
				    if (node1.get() < param.node1.get()) return true;
				    if (node1.get() > param.node1.get()) return false;
				    if (node2.get() < param.node2.get()) return true;
				    if (node2.get() > param.node2.get()) return false;
				    LOG_ERROR("Incomplete 'if' sequence");
				    return false;
				}
			};

		typedef std::map<nodepair,Se3r> NodePairsMap;//Initial node differences
		typedef std::map<Vector3r,Se3r> localTriad;//Updated on every step
		unsigned int max_pair;
		Matrix calculateStiffness(Real, Real ,Vector3r,Vector3r,Vector3r,Vector3r);
		Matrix calculateMassMatrix(Real, Real);
		virtual ~DeformableCohesiveElement();
		void initialize(void){max_pair=3;}

		void addPair(const shared_ptr<Body>& node1,const shared_ptr<Body>& node2);
		void delPair(const shared_ptr<Body>& node1,const shared_ptr<Body>& node2);

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DeformableCohesiveElement,DeformableElement,"Tetrahedral Deformable Element Composed of Nodes",
		((NodePairsMap,nodepairs,,,"Ids and relative position+orientation difference of members of the cohesive deformable element in the inital condition (should not be accessed directly)"))
		,
		,
		createIndex(); /*ctor*/
		initialize();
		,
		/*py*/
		.def("addPair",&DeformableCohesiveElement::addPair,"Add a node shared_pt<:yref:'Body'>& as into the element")
		.def("removePair",&DeformableCohesiveElement::delPair,"Add a node shared_pt<:yref:'Body'>& as into the element")
	);
		DECLARE_LOGGER;

		REGISTER_CLASS_INDEX(DeformableCohesiveElement,DeformableElement);

};

// necessary
using namespace yade;
REGISTER_SERIALIZABLE(DeformableCohesiveElement);
