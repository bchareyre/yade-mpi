 /***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ErrorTolerantDynamicEngine.hpp"
#include "RigidBody.hpp"
#include "NonConnexBody.hpp"
#include "ErrorTolerantContactModel.hpp"
#include "Contact.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ErrorTolerantDynamicEngine::ErrorTolerantDynamicEngine() : DynamicEngine()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ErrorTolerantDynamicEngine::~ErrorTolerantDynamicEngine()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void ErrorTolerantDynamicEngine::processAttributes()
{
	DynamicEngine::processAttributes();
	// PROCESS DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantDynamicEngine::registerAttributes()
{
	DynamicEngine::registerAttributes();
	// REGISTER DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void ErrorTolerantDynamicEngine::respondToCollisions(Body* body)
{

	NonConnexBody * ncb = dynamic_cast<NonConnexBody*>(body);
	vector<shared_ptr<Body> >& bodies = ncb->bodies;


	// Build inverse of masses matrix and store it into a vector
	ublas::vector<float> invM(6*bodies.size());
	for(unsigned int i=0;i<bodies.size();i++)
	{
		int offset = 6*i;
		shared_ptr<RigidBody> rb = dynamic_pointer_cast<RigidBody>(bodies[i]);
		invM(offset++) = rb->invMass;
		invM(offset++) = rb->invMass;
		invM(offset++) = rb->invMass;
		invM(offset++) = rb->invInertia[0];
		invM(offset++) = rb->invInertia[1];
		invM(offset++) = rb->invInertia[2];
	}

	// Build the jacobian and transpose of jacobian
	ublas::sparse_matrix<float> J (6*bodies.size(), body->interactions.size(), 6*bodies.size()*body->interactions.size());
	ublas::sparse_matrix<float> Jt (body->interactions.size(), 6*bodies.size(), 6*bodies.size()*body->interactions.size());

	Vector3 n,o1p1,o2p2;
	Vector3 o1p1CrossN,o2p2CrossN;
	int id1,id2,offset1,offset2;
	
	list<shared_ptr<Interaction> >::const_iterator cti = ncb->interactions.begin();
	list<shared_ptr<Interaction> >::const_iterator ctiEnd = ncb->interactions.end();
	for(int i=0 ; cti!=ctiEnd ; ++cti,i++)
	{
		shared_ptr<Contact> contact = static_pointer_cast<Contact>(*cti);
		shared_ptr<ErrorTolerantContactModel> cm = dynamic_pointer_cast<ErrorTolerantContactModel>((*cti)->interactionGeometry);

		id1 		= contact->id1;
		id2 		= contact->id2;
		o1p1 		= cm->o1p1;
		o2p2 		= cm->o2p2;
		n 		= cm->normal;
		o1p1CrossN 	= o1p1.cross(n);
		o2p2CrossN 	= o2p2.cross(n);

		offset1		= 6*id1;
		offset2		= 6*id2;
		
		J(offset1++,i)	= n[0];
		J(offset1++,i)	= n[1];
		J(offset1++,i)	= n[2];
		J(offset1++,i)	= o1p1CrossN[0];
		J(offset1++,i)	= o1p1CrossN[1];
		J(offset1,i)	= o1p1CrossN[2];
		J(offset2++,i)	= -n[0];
		J(offset2++,i)	= -n[1];
		J(offset2++,i)	= -n[2];
		J(offset2++,i)	= -o2p2CrossN[0];
		J(offset2++,i)	= -o2p2CrossN[1];
		J(offset2,i)	= -o2p2CrossN[2];

		offset1		= 6*id1;
		offset2		= 6*id2;

		Jt(i,offset1++)	= n[0];
		Jt(i,offset1++)	= n[1];
		Jt(i,offset1++)	= n[2];
		Jt(i,offset1++)	= o1p1CrossN[0];
		Jt(i,offset1++)	= o1p1CrossN[1];
		Jt(i,offset1)	= o1p1CrossN[2];
		Jt(i,offset2++)	= -n[0];
		Jt(i,offset2++)	= -n[1];
		Jt(i,offset2++)	= -n[2];
		Jt(i,offset2++)	= -o2p2CrossN[0];
		Jt(i,offset2++)	= -o2p2CrossN[1];
		Jt(i,offset2)	= -o2p2CrossN[2];
		
	}

	ublas::vector<float> constantTerm;
	ublas::vector<float> res;
	
	BCGSolve(J,invM,Jt,constantTerm,res);





	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void ErrorTolerantDynamicEngine::BCGSolve(	ublas::sparse_matrix<float>& J	,
						ublas::vector<float>& invM	,
						ublas::sparse_matrix<float>& Jt	,
						ublas::vector<float>& constantTerm,
						ublas::vector<float>& res)
{

	bool modif=true;
	float residu,oldResidu;
	int size = J.size2(); //J.size1(); ??
	int nbRestart;
	float ak,akden,bk,bknum,bkden=1;
	bool restart,solve;
	int iter;
	
	
	static ublas::vector<float> a, r, rr, p, pp, z, zz, tmpVec;

	solve		= false;
	iter		= 0;
	nbRestart	= 0;
	restart		= true;
	
	a.resize(3*size); 
	r.resize(3*size); 
	rr.resize(3*size); 
	p.resize(3*size); 
	pp.resize(3*size); 
	z.resize(3*size); 
	zz.resize(3*size); 
	tmpVec.resize(3*size);

	res.resize(3*size);


// 	initConstraintForces(false,res);
// 	contactStates.resize(size);
// 	nbReactivations.resize(size);
// 	invDiag.resize(size);
// 	
// 	std::fill( contactStates.begin(), contactStates.end(), CLAMPED);
// 	std::fill( nbReactivations.begin(), nbReactivations.end(), 0);
// 	
// 	if (precond)
// 		buildInvDiag(jacobian,invMass) ;
// 
// 	multA(res, a,jacobian,invMass);				// a = Af
// 
// 	animal::v_meq (a, constantTerm);			// a = Af - b
// 
// 	animal::v_eq_minus (a, a);                              // a = b - Af
// 
// 	if (modif)
// 	  filter(a,r);						
// 	else
// 	  r = a;
// 
// 	if (solved(r,threshold))
// 		solve = true;	
// 	else 
// 	{
// 	
// 		if (precond)
// 			leftMultByInvDiag(z,r);
// 		else
// 			z = r;
// 			
// 		multA(r,rr,jacobian,invMass);
// 		if (modif)
// 		{
// 		  filter(rr,tmpVec);
// 		  rr = tmpVec;
// 		}
// 	}
// 
// 	nbRestart = 0;
// 	
// 	oldResidu = 0;
// 	residu = residuNorm(r);
// 
// 	while (iter < maxIterations && !solve) {	                      
// 				
// 		iter ++;
// 		
// 		if (precond)		
// 			leftMultByInvDiagTrans(zz,rr);
// 		else
// 			zz = rr;
// 
// 		bknum = animal::vv_dot ( z , rr );
// 	
// 		if (restart) {	
// 			p       = z;
// 			pp      = zz;
// 			restart = false;  
// 		} 
// 		else {		
// 			bk = bknum/bkden;
// 			animal::vv_eq ( p, z, bk, p );				//p  = bk * p  + z;		
// 			animal::vv_eq ( pp, zz, bk, pp );			//pp = bk * pp + zz
// 		}
// 
// 		bkden = bknum;
// 	
// 		multA(p,z,jacobian,invMass);
// 		if (modif)
// 		  filter(z,tmpVec);
// 		else
// 		  tmpVec=z;
// 
// 		akden =  animal::vv_dot ( tmpVec  , pp );
// 		
// 		if (akden!=0)		
// 			ak = bknum/akden;
// 		else 
// 			ak = 0;
// 
// 		animal::vv_peq ( res, ak, p );		//x = x + ak * p;
// 
// 		t_VecVec3::iterator ri  = res.begin();	
// 		t_VecVec3::iterator riEnd  = res.end();	
// 		
// 		for( ; ri != riEnd ; ++ri)
// 		{
// 		  // if (fabs((*ri)[0])<1e-3) (*ri)[0]=0;
// 		  if (fabs((*ri)[1])<1e-3) (*ri)[1]=0;
// 		  if (fabs((*ri)[2])<1e-3) (*ri)[2]=0;
// 		}
// 
// 		if (_isFilterFrictionForcesActive)
// 			filterFrictionForces(res,closestFeatures);
// 		 
// 		//filter(z,tmpVec);						//r = r - ak * filter(z);
// 		animal::vv_peq ( r, -ak, tmpVec );
// 	
// 		animal::vv_peq ( a, -ak, z );					//a = a - ak * z;
// 	
// 		if (modif)
// 		  restart = wrong(res,a,closestFeatures) ;
// 		else
// 		  restart = false;
// 
// 		if (restart) {
// 			nbRestart++;
// 			multA(res,a,jacobian,invMass);			// a = Af
// 			animal::v_meq (a, constantTerm);	//a = Af - b; 		
// 			animal::v_eq_minus (a, a);
// 			filter(a, r);		
// 		}
// 
// 		if (solved(r,threshold)) 
// 			solve = true;
// 		else {
// 		
// 			if (precond)			
// 				leftMultByInvDiag(z,r);
// 			else
// 				z = r;
// 	
// 			if (restart) {
// 				multA(r,rr,jacobian,invMass);
// 				filter(rr,tmpVec);
// 				rr = tmpVec;
// 			}
// 			else {
// 				multA(pp,zz,jacobian,invMass);
// 				if (modif)
// 				  filter(zz,tmpVec);				
// 				else
// 				  tmpVec = zz;
// 
// 				animal::vv_peq (rr, -ak, tmpVec );	// rr = rr - ak*filter(zz);
// 			}
// 		}	
// 		
// 		oldResidu = residu;
// 		residu = residuNorm(r);
// 
// 	}
}

