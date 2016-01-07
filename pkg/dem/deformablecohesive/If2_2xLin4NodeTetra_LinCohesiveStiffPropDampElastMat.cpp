/*************************************************************************
*  Copyright (C) 2013 by Burak ER    burak.er@btu.edu.tr              	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <pkg/dem/deformablecohesive/Lin4NodeTetra_Lin4NodeTetra_InteractionElement.hpp>
#include <pkg/dem/deformablecohesive/If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat.hpp>
YADE_PLUGIN((If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat));
If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat::~If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat(){};


/********************************************************************
                      InternalForceDispatcher
*********************************************************************/
typedef std::map<DeformableCohesiveElement::nodepair,Se3r> NodePairsMap;//Initial node differences

CREATE_LOGGER(If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat);
void If2_2xLin4NodeTetra_LinCohesiveStiffPropDampElastMat::go(const shared_ptr<Shape>& element,const shared_ptr<Material>& material,const shared_ptr<Body>& bdy)
{

	shared_ptr<Lin4NodeTetra_Lin4NodeTetra_InteractionElement> interactionelement=YADE_PTR_CAST<Lin4NodeTetra_Lin4NodeTetra_InteractionElement>(element);
	shared_ptr<LinCohesiveStiffPropDampElastMat> mat=YADE_PTR_CAST<LinCohesiveStiffPropDampElastMat>(material);


	NodePairsMap::iterator i1(interactionelement->nodepairs.begin());
	shared_ptr<Body> node11=i1->first.node1;shared_ptr<Body> node12=i1->first.node2;
	Vector3r initial1=i1->second.position;
	std::advance(i1,1);
	shared_ptr<Body> node21= i1->first.node1;shared_ptr<Body> node22=i1->first.node2;
	Vector3r initial2=i1->second.position;
	std::advance(i1,1);
	shared_ptr<Body> node31= i1->first.node1;shared_ptr<Body> node32=i1->first.node2;
	Vector3r initial3=i1->second.position;

	Vector3r disp1=(node11->state->pos-node12->state->pos)-initial1;
	Vector3r disp2=(node21->state->pos-node22->state->pos)-initial2;
	Vector3r disp3=(node31->state->pos-node32->state->pos)-initial3;



	Vector3r l1=node11->state->pos-node21->state->pos;
	Vector3r l2=node31->state->pos-node21->state->pos;

	Real A=0.5*fabs(l1.cross(l2).norm());

	Vector3r f1=(0.083333333)*A*mat->youngmodulus*(2*disp1+disp2+disp3);
	Vector3r f2=(0.083333333)*A*mat->youngmodulus*(disp1+2*disp2+disp3);
	Vector3r f3=(0.083333333)*A*mat->youngmodulus*(disp1+disp2+2*disp3);

	//cout<<"forces are \n"<<f1<<"\n"<<f2<<"\n"<<f3<<"\n";

	scene->forces.addForce(node11->getId(),-f1);	scene->forces.addForce(node12->getId(),f1);
	scene->forces.addForce(node21->getId(),-f2);	scene->forces.addForce(node22->getId(),f2);
	scene->forces.addForce(node31->getId(),-f3);	scene->forces.addForce(node32->getId(),f3);


	return;
}
