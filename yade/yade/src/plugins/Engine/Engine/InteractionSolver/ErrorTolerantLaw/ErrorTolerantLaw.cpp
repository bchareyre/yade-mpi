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

#include "ErrorTolerantLaw.hpp"
#include "RigidBodyParameters.hpp"
#include "MetaBody.hpp"
#include "ErrorTolerantContactModel.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ErrorTolerantLaw::ErrorTolerantLaw() : InteractionSolver()
{
	maxReactivations = 100;
	maxIterations    = 5;
	threshold	 = 0.01;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

ErrorTolerantLaw::~ErrorTolerantLaw()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void ErrorTolerantLaw::postProcessAttributes(bool deserializing)
{
	InteractionSolver::postProcessAttributes(deserializing);
	// PROCESS DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantLaw::registerAttributes()
{
	InteractionSolver::registerAttributes();
	// REGISTER DESIRED ATTRIBUTES HERE
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void ErrorTolerantLaw::calculateForces(Body* body)
{

	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	if (ncb->volatileInteractions->size() > 0)
	{
		// Build inverse of masses matrix and store it into a vector
		ublas::banded_matrix<float> invM(6*bodies->size(),6*bodies->size(),1,1);


		shared_ptr<Body> b;
		unsigned int i=0;
// FIXME - this loop assumes that id's of all bodies start from zero and increment by one?
//		for(unsigned int i=0;i<bodies->size();i++)
		for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
		{
			b = bodies->getCurrent();

			int offset = 6*i;
			shared_ptr<RigidBodyParameters> rb = dynamic_pointer_cast<RigidBodyParameters>(b);
			invM(offset,offset++) = rb->invMass;
			invM(offset,offset++) = rb->invMass;
			invM(offset,offset++) = rb->invMass;
			invM(offset,offset++) = rb->invInertia[0];
			invM(offset,offset++) = rb->invInertia[1];
			invM(offset,offset)   = rb->invInertia[2];
		}

		// Build the jacobian and transpose of jacobian
		ublas::sparse_matrix<float> J (6*bodies->size(), ncb->volatileInteractions->size(), ncb->volatileInteractions->size()*2/*6*bodies.size()*body->volatileInteractions.size()*/);
		ublas::sparse_matrix<float> Jt (ncb->volatileInteractions->size(), 6*bodies->size(), ncb->volatileInteractions->size()*2/*6*bodies.size()*body->volatileInteractions.size()*/);

		static ublas::vector<float> penetrationDepthes;
		static ublas::vector<float> penetrationVelocities;

		penetrationDepthes.resize(bodies->size());
		penetrationVelocities.resize(bodies->size());

		Vector3r n,o1p1,o2p2;
		Vector3r o1p1CrossN,o2p2CrossN;
		int id1,id2,offset1,offset2;


		shared_ptr<Interaction> contact;
		i=0;
//		list<shared_ptr<Interaction> >::const_iterator cti = ncb->volatileInteractions.begin();
//		list<shared_ptr<Interaction> >::const_iterator ctiEnd = ncb->volatileInteractions.end();
//		for(int i=0 ; cti!=ctiEnd ; ++cti,i++)
		for( ncb->volatileInteractions->gotoFirst() ; ncb->volatileInteractions->notAtEnd() ; ncb->volatileInteractions->gotoNext() )
		{
			contact = ncb->volatileInteractions->getCurrent();

			shared_ptr<ErrorTolerantContactModel> cm = dynamic_pointer_cast<ErrorTolerantContactModel>(contact->interactionGeometry);

			id1 		= contact->getId1();
			id2 		= contact->getId2();
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

			penetrationDepthes[i] = (cm->closestPoints[0].first-cm->closestPoints[0].second).length();

			//penetrationVelocities[i] = ( ((*bodies)[id1]->velocity+o1p1.cross((*bodies)[id1]->angularVelocity)) - ((*bodies)[id2]->velocity+o2p2.cross((*bodies)[id2]->angularVelocity)) ).dot(n);

		}

		ublas::vector<float> impulses;
		ublas::vector<float> displacement;

		impulses.resize(bodies->size());
		displacement.resize(bodies->size());

		BCGSolve(J,invM,Jt,penetrationDepthes,displacement);
		BCGSolve(J,invM,Jt,penetrationVelocities,impulses);
	}


	shared_ptr<Body> b;
	unsigned int i=0;
// FIXME - this loop assumes that id's of all bodies start from zero and increment by one?
//	for(unsigned int i=0; i < bodies->size(); i++)
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
	{
		b = bodies->getCurrent();


		shared_ptr<RigidBodyParameters> rb = dynamic_pointer_cast<RigidBodyParameters>(b);
		if (rb)
		{
			//Vector3r force = Omega::instance().getGravity()*rb->mass;
			Vector3r force = Vector3r(0,-9.81,0)*rb->mass; // FIXME - use GravityCondition
			int sign;
			float f = force.length();

			for(int j=0;j<3;j++)  // FIXME - use CundallNonViscousForceDampingFunctor
			{
				if (rb->velocity[j]==0)
					sign=0;
				else if (rb->velocity[j]>0)
					sign=1;
				else
					sign=-1;
				force[j] -= 0.1*f*sign;
			}

			/*float m = moments[i].length();

			for(int j=0;j<3;j++)
			{
				if (rb->angularVelocity[j]==0)
					sign=0;
				else if (rb->angularVelocity[j]>0)
					sign=1;
				else
					sign=-1;
				moments[i][j] -= 0.1*m*sign;
			}*/

			rb->acceleration += force*rb->invMass;
			//rb->angularAcceleration += moments[i].multTerm(rb->invInertia);
		}
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantLaw::multA(	ublas::vector<float>& res		,
					ublas::sparse_matrix<float>& J		,
					ublas::banded_matrix<float>& invM	,
					ublas::sparse_matrix<float>& Jt		,
					ublas::vector<float>& v		)
{

	ublas::vector<float> Jtv = ublas::prod(Jt,v);
	ublas::vector<float> invMJtv = ublas::prod(invM,Jtv);
	res = ublas::prod(J,invMJtv);
	//res = ublas::prod(J , ublas::prod(invM , ublas::prod(Jt,v) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantLaw::initInitialGuess(ublas::vector<float>& v)
{

	ublas::vector<float>::iterator vi	= v.begin();
	ublas::vector<float>::iterator viEnd	= v.end();

	for( ; vi != viEnd ; ++vi)
	{
		//(*vi) = t_Vec3((float)random()/(float)RAND_MAX,(float)random()/(float)RAND_MAX,(float)random()/(float)RAND_MAX);
		(*vi) = 0;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantLaw::filter(const ublas::vector<float>& v, ublas::vector<float>& res)
{

	ublas::vector<float>::const_iterator vi		= v.begin();
	ublas::vector<float>::const_iterator viEnd	= v.end();
	ublas::vector<float>::iterator resi		= res.begin();
	vector<ContactState>::iterator si		= contactStates.begin();

	for( ; vi != viEnd ; ++vi , ++si , ++resi)
	{
		if ((*si) == VANISHING)
			(*resi) = 0;
		else
			(*resi) = (*vi);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool ErrorTolerantLaw::solved(const ublas::vector<float>& v, float threshold)
{
	static ublas::vector<float> tmpVec;
	tmpVec.resize(v.size());
	filter(v,tmpVec);
	return (norm(tmpVec)<threshold);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

float ErrorTolerantLaw::norm(const ublas::vector<float>& v)
{
	return ublas::norm_1(v);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool ErrorTolerantLaw::wrong(ublas::vector<float>& f, const ublas::vector<float>& a)
{
	bool err;
	vector<int>::iterator 		ri	= nbReactivations.begin();
	ublas::vector<float>::iterator	fi	= f.begin();
	ublas::vector<float>::iterator	fiEnd	= f.end();
	ublas::vector<float>::const_iterator ai	= a.begin();
	vector<ContactState>::iterator	si	= contactStates.begin();

	err = false;
	for( ; fi != fiEnd ; ++fi , ++ai , ++si,++ri)
	{
		if ( (*si) == VANISHING && (*ai) < 0 && (*ri)<maxReactivations)
		{
			(*si) = CLAMPED;
			err   = true;
			(*ri)++;
		}
		else if ( (*si) == CLAMPED && (*fi) < 0 )
		{
			(*si) 	= VANISHING;
			(*fi) 	= 0;
			err   	= true;
		}
	}

	return err;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void ErrorTolerantLaw::BCGSolve(	ublas::sparse_matrix<float>& J		,
						ublas::banded_matrix<float>& invM	,
						ublas::sparse_matrix<float>& Jt		,
						ublas::vector<float>& constantTerm	,
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

	initInitialGuess(res);

	contactStates.resize(size);
	fill( contactStates.begin(), contactStates.end(), CLAMPED);

	nbReactivations.resize(size);
	fill( nbReactivations.begin(), nbReactivations.end(), 0);
/// 	invDiag.resize(size);

/// 	if (precond)
/// 		buildInvDiag(jacobian,invMass) ;


	multA(a,J,invM,Jt,res);	// a = Af 	// multA(res, a,jacobian,invMass)

	a -= constantTerm;	// a = Af - b	// animal::v_meq (a, constantTerm);

	a = -a;			// a = b - Af	// animal::v_eq_minus (a, a);

	if (modif)
		filter(a,r);
	else
		r = a;

	if (solved(r,threshold))
		solve = true;
	else
	{
///		if (precond)
///			z = invDiag*r;	// leftMultByInvDiag(z,r);
///		else
			z = r;

		multA(rr,J,invM,Jt,r);	// multA(r,rr,jacobian,invMass);

		if (modif)
		{
			filter(rr,tmpVec);
			rr = tmpVec;
	 	}
	}

	nbRestart = 0;
	oldResidu = 0;
	residu = norm(r);

	while (iter < maxIterations && !solve)
	{
		iter ++;

///		if (precond)
///			zz = invDiag*rr;	// leftMultByInvDiag(zz,rr);
///		else
///			zz = rr;

		bknum = ublas::inner_prod(z,rr);

		if (restart)
		{
			p       = z;
			pp      = zz;
			restart = false;
		}
		else
		{
			bk = bknum/bkden;
			p = z+bk*p;		// animal::vv_eq ( p, z, bk, p );		//p  = bk * p  + z;
			pp = zz + bk*pp;	//animal::vv_eq ( pp, zz, bk, pp );	//pp = bk * pp + zz
		}

		bkden = bknum;

		multA(z,J,invM,Jt,p);	// multA(p,z,jacobian,invMass);

		if (modif)
			filter(z,tmpVec);
		else
			tmpVec=z;

		akden =  ublas::inner_prod(tmpVec,pp);

		if (akden!=0)
			ak = bknum/akden;
		else
			ak = 0;

		res += ak*p;		// x = x + ak * p;


// 		ublas::vector<float>::iterator ri    = res.begin();
// 		ublas::vector<float>::iterator riEnd = res.end();
// 		for( ; ri != riEnd ; ++ri)
// 		{
// 			if (fabs((*ri)[0])<1e-3) (*ri)[0]=0;
// 			if (fabs((*ri)[1])<1e-3) (*ri)[1]=0;
// 			if (fabs((*ri)[2])<1e-3) (*ri)[2]=0;
// 		}

		filter(z,tmpVec);	// r = r - ak * filter(z);

		r -= ak*tmpVec;		// animal::vv_peq ( r, -ak, tmpVec );
		a -= ak*z;		// a = a - ak * z;	// animal::vv_peq ( a, -ak, z );

		if (modif)
			restart = wrong(res,a);
		else
			restart = false;

		if (restart)
		{
			nbRestart++;
			multA(a,J,invM,Jt,res);	// a = Af	// multA(res,a,jacobian,invMass);
			a -= constantTerm;	//a = Af - b;	// animal::v_meq (a, constantTerm);
			a = -a;			// animal::v_eq_minus (a, a);
			filter(a, r);
		}

		if (solved(r,threshold))
			solve = true;
		else
		{
///			if (precond)
///				z = invDiag*r;	// leftMultByInvDiag(z,r);
///			else
				z = r;

			if (restart)
			{
				multA(rr,J,invM,Jt,r);	//multA(r,rr,jacobian,invMass);
				filter(rr,tmpVec);
				rr = tmpVec;
			}
			else
			{
				multA(zz,J,invM,Jt,pp);	// multA(pp,zz,jacobian,invMass);
				if (modif)
					filter(zz,tmpVec);
				else
					tmpVec = zz;
				rr -= ak*tmpVec;	// rr = rr - ak*filter(zz); //animal::vv_peq (rr, -ak, tmpVec );
			}
		}
		oldResidu = residu;
		residu = norm(r);
	}
}

