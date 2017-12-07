/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <algorithm>
#include <core/Scene.hpp>
#include <core/BodyContainer.hpp>
#include <core/State.hpp>
#include <pkg/common/Sphere.hpp>
#include <pkg/dem/deformablecohesive/Node.hpp>
#include <string>
#include <boost/lexical_cast.hpp>

DeformableElement::~DeformableElement(){

}
YADE_PLUGIN((DeformableElement));

boost::python::dict DeformableElement::localmap_get(){
	boost::python::dict ret;
	FOREACH(NodeMap::value_type& b, localmap){
		ret[b.first]=boost::python::make_tuple(b.second.position,b.second.orientation);
	}
	return ret;
}
void DeformableElement::addFace(Vector3r& indexes){

	faces.push_back(indexes);

}
//std::vector<Vector3r> DeformableElement::getDisplacements(void)
//{
//
//}
shared_ptr<Body>  DeformableElement::getNode(int id){

	   NodeMap::iterator i1(this->localmap.begin());
	   std::advance(i1,id);
	   return i1->first;

}

void DeformableElement::removeLastFace(void){

	faces.pop_back();

}

void DeformableElement::addNode(const shared_ptr<Body>& nodeBody){


	if(this->localmap.size()==maxNodeCount)
	{
		std::string errormessage ="This element cannot hold more than"+ boost::lexical_cast<std::string>(maxNodeCount);
		throw std::out_of_range(errormessage);
		return;
	}
	const shared_ptr<Node> node=YADE_PTR_CAST<Node>(nodeBody->shape);//Should be checked dynamically otherwise it will always cast: every shape is castable to Node shape

	Body::id_t subId=nodeBody->getId();

	if(node){} else{ throw std::invalid_argument(("The body that is given #"+boost::lexical_cast<string>(subId)+" is not a Node therefore cannot be added to the deformable element "));}
	
	if(subId<0){throw std::invalid_argument(("The Node that is given is not a member of the scene therefore it has no state, not adding exiting"));}

	if(this->localmap.count(nodeBody)!=0) throw std::invalid_argument(("Node that has Body id #"+boost::lexical_cast<string>(subId)+" is already part of this deformable element"));

	// Add body to localmap
	this->localmap[nodeBody]=Se3r();// meaningful values will be put in by DeformableElement::updateProperties

	// Get first node
	//Scene* scene(Omega::instance().getScene().get());	// get scene
	//const shared_ptr<Body>& member=localmap.begin()->first;
	
	//Substract from the current node, therefore find the local value,
	localmap[nodeBody].position=nodeBody->state->pos;


	//cout<<"Local map of "<<subId<<" is ="<<localmap[subId].position;
	//localmap[subId].orientation=nodeBody->state->ori;

	//this->setBounded(false); // disallow collisions with the element itself

	//If we have more than three nodes define a local triad that is clumped on the first node and initially parallel to the global frame
	if(this->localmap.size()>=3)
	{	
		//define a local triad if needed		
	}		

	return;
}

void DeformableElement::delNode(const shared_ptr<Body>& subBody){
	// erase the subBody; removing body that is not part of the element throws
	if(this->localmap.erase(subBody)!=1) throw std::invalid_argument(("Node #"+boost::lexical_cast<string>(subBody->id)+" not a part of the deformable element, not removing...").c_str());
	LOG_DEBUG("Removed node #"<<subBody->id);
}


