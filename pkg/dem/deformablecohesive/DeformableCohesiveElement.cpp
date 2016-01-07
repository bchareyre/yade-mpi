/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <algorithm>
#include <core/Scene.hpp>
#include <pkg/dem/deformablecohesive/DeformableCohesiveElement.hpp>
#include <core/BodyContainer.hpp>
#include <core/State.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/dem/deformablecohesive/Node.hpp>
#include <lib/base/Math.hpp>

DeformableCohesiveElement::~DeformableCohesiveElement(){

}
YADE_PLUGIN((DeformableCohesiveElement));
CREATE_LOGGER(DeformableCohesiveElement);
void DeformableCohesiveElement::addPair(const shared_ptr<Body>& nodeBody1,const shared_ptr<Body>& nodeBody2)
{


	const shared_ptr<Node> node1=YADE_PTR_CAST<Node>(nodeBody1->shape);//Should be checked dynamically otherwise it will always cast: every shape is castable to Node shape
	const shared_ptr<Node> node2=YADE_PTR_CAST<Node>(nodeBody2->shape);//Should be checked dynamically otherwise it will always cast: every shape is castable to Node shape

	Body::id_t subId1=nodeBody1->getId();
	Body::id_t subId2=nodeBody2->getId();

	if(node1||node2){} else{ throw std::invalid_argument(("One of the nodes that is given is not a Node therefore cannot be added to the cohesive deformable element "));}

	if(subId1<0||subId2<0){throw std::invalid_argument(("One of the node that is given is not a member of the scene therefore it has no state, not adding exiting"));}

	if(this->localmap.count(nodeBody1)!=0) throw std::invalid_argument(("Node that has Body id #"+boost::lexical_cast<string>(subId1)+" is already part of this cohesive deformable element"));
	if(this->localmap.count(nodeBody2)!=0) throw std::invalid_argument(("Node that has Body id #"+boost::lexical_cast<string>(subId2)+" is already part of this cohesive deformable element"));

	nodepair pair;
	pair.node1=nodeBody1;
	pair.node2=nodeBody2;
	this->nodepairs[pair]=Se3r();
	// Add body to localmap
	this->nodepairs[pair].position=nodeBody1->state->pos-nodeBody2->state->pos;// Initial difference on positions

	return;

}
void DeformableCohesiveElement::delPair(const shared_ptr<Body>& node1,const shared_ptr<Body>& node2)
{
	// erase the subBody; removing body that is not part of the element throws
//	if(this->localmap.erase(subBody)!=1) throw std::invalid_argument(("Node #"+boost::lexical_cast<string>(subBody->id)+" not a part of the deformable element, not removing...").c_str());
//	LOG_DEBUG("Removed node #"<<subBody->id);
}
