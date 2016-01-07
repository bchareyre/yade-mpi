/*************************************************************************
*  Copyright (C) 2013 by Burak ER    burak.er@btu.edu.tr              	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <pkg/dem/deformablecohesive/DeformableElement.hpp>
#include <pkg/dem/deformablecohesive/If2_Lin4NodeTetra_LinIsoRayleighDampElast.hpp>
YADE_PLUGIN((If2_Lin4NodeTetra_LinIsoRayleighDampElast));
If2_Lin4NodeTetra_LinIsoRayleighDampElast::~If2_Lin4NodeTetra_LinIsoRayleighDampElast(){};


/********************************************************************
                      InternalForceDispatcher
*********************************************************************/

typedef DeformableElement::NodeMap NodeMap;
#define VECTOR12_TEMPLATE(Scalar) Eigen::Matrix<Scalar,12,1>
typedef VECTOR12_TEMPLATE(Real) Vector12r;

CREATE_LOGGER(If2_Lin4NodeTetra_LinIsoRayleighDampElast);
void If2_Lin4NodeTetra_LinIsoRayleighDampElast::go(const shared_ptr<Shape>& element,const shared_ptr<Material>& material,const shared_ptr<Body>& bdy)
{

	shared_ptr<Lin4NodeTetra> tetel=YADE_PTR_CAST<Lin4NodeTetra>(element);
	shared_ptr<LinIsoRayleighDampElastMat> mat=YADE_PTR_CAST<LinIsoRayleighDampElastMat>(material);

	   DeformableElement::NodeMap::iterator i0(tetel->localmap.begin());
	   DeformableElement::NodeMap::iterator i1(i0);
	   DeformableElement::NodeMap::iterator i2(i0);
	   DeformableElement::NodeMap::iterator i3(i0);

	   std::advance(i1,1);
	   std::advance(i2,2);
	   std::advance(i3,3);

	if(tetel->massMatrixInvProductstiffnessMatrix==0)
	{

		   Vector3r node0relpos=Vector3r(0,0,0);
		   Vector3r node1relpos=i1->second.position-i0->second.position;
		   Vector3r node2relpos=i2->second.position-i0->second.position;
		   Vector3r node3relpos=i3->second.position-i0->second.position;
		   // I dont know wheter this is optimum or not
		   tetel->massMatrixInvProductstiffnessMatrix=shared_ptr<Matrix>
		   (
				   new Matrix(
				   tetel->calculateMassMatrix(mat->density,mat->poissonratio).inverse()*
				   tetel->calculateStiffness(mat->youngmodulus,mat->poissonratio,node0relpos,node1relpos,node2relpos,node3relpos)
				   )
		   );


	}

	//apply internal forces to the tetrahedron
	//Calculate displacements
	   Vector12r displacements;
	   Vector12r displacementvelocity;

	   displacements<<i0->first->state->pos - i0->second.position,
			   	   	  i1->first->state->pos - i1->second.position,
			   	   	  i2->first->state->pos - i2->second.position,
			   	   	  i3->first->state->pos - i3->second.position;
	   displacementvelocity<<i0->first->state->vel,
			   	   	  i1->first->state->vel,
			   	   	  i2->first->state->vel,
			   	   	  i3->first->state->vel;


	//Now calculate the forces
	Vector12r forces=-(*tetel->massMatrixInvProductstiffnessMatrix)*displacements-(mat->alpha*Matrix::Identity(12,12)+(*tetel->massMatrixInvProductstiffnessMatrix)*(mat->beta))*displacementvelocity;

	scene->forces.addForce(i0->first->getId(),forces.segment(0,3));
	scene->forces.addForce(i1->first->getId(),forces.segment(3,3));
	scene->forces.addForce(i2->first->getId(),forces.segment(6,3));
	scene->forces.addForce(i3->first->getId(),forces.segment(9,3));

	return;
}
