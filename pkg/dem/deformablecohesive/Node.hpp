/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef NODE_HPP_
#define NODE_HPP_
#include <core/Shape.hpp>
//TODO: Look at Sphere hack to work around problem
// HACK to work around https://bugs.launchpad.net/yade/+bug/528509
// see comments there for explanation
namespace yade{

class Node: public Shape{
	public:
		Node(Real _radius): radius(_radius){ createIndex();}
		virtual ~Node ();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(Node,Shape,"Geometry of node particle.",
		((Real,radius,0.1,,"Radius [m]")),
		createIndex(); /*ctor*/
	);
	REGISTER_CLASS_INDEX(Node,Shape);

};

}
// necessary
using namespace yade; 

// must be outside yade namespace
REGISTER_SERIALIZABLE(Node);
#endif
