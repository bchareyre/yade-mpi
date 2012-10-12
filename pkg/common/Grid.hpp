#pragma once
#include "Sphere.hpp"
#include <yade/core/Body.hpp>
#include<yade/pkg/common/Dispatching.hpp>



//!##################	SHAPES   #####################
class GridConnection: public Sphere{
public:
		virtual ~GridConnection();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(GridConnection,Sphere,"Cylinder geometry of a connection between two :yref:`Node`.",
		((shared_ptr<Body> , node1 , ,,))
		((shared_ptr<Body> , node2 , ,,))
  		((Quaternionr,chainedOrientation,Quaternionr::Identity(),,"Deviation of node1 orientation from node-to-node vector"))
		,createIndex();/*ctor*/
		);
	REGISTER_CLASS_INDEX(GridConnection,Sphere);
};
REGISTER_SERIALIZABLE(GridConnection);


class GridNode: public Sphere{
	public:
		virtual ~GridNode();
		void addConnection(shared_ptr<Body> GC);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(GridNode,Sphere,"GridNode component of a grid",
		((vector<shared_ptr<Body> >,ConnList,,,"List of :yref:'GridConnection' the GridNode is connected to.")),
		/*ctor*/
		createIndex();,
		/*py*/
		.def("addConnection",&GridNode::addConnection,(python::arg("Body")),"Add a GridConnection to the GridNode")
	);
	REGISTER_CLASS_INDEX(GridNode,Sphere);
};

REGISTER_SERIALIZABLE(GridNode);