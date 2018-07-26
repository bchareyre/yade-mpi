/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include<pkg/common/Collider.hpp>

YADE_PLUGIN((Collider));

#define YADE_MPI

int Collider::avoidSelfInteractionMask = 0 ;

bool Collider::mayCollide(const Body* b1, const Body* b2
		#ifdef YADE_MPI
		,Body::id_t subdomain
		#endif 
		) {
	return 
		#ifdef YADE_MPI //skip interactions outside subdomain, and between the subdomain and its own bodies
		((subdomain==b1->subdomain or subdomain==b2->subdomain) and not (b1->subdomain==b2->subdomain and (b1->getSubdomain() or b2->getSubdomain()))) &&
		#endif 
		// might be called with deleted bodies, i.e. NULL pointers
		(b1!=NULL && b2!=NULL) &&
		// only collide if at least one particle is standalone or they belong to different clumps
		(b1->isStandalone() || b2->isStandalone() || b1->clumpId!=b2->clumpId 
		#ifdef YADE_SPH
		// If SPH-mode enabled, we do not skip interactions between clump-members
		// to get the correct calculation of density b->rho
		|| true
		#endif
		) &&
		 // do not collide clumps, since they are just containers, never interact
		!b1->isClump() && !b2->isClump() &&
		// masks must have at least 1 bit in common
		b1->maskCompatible(b2->groupMask) &&
		// avoid contact between particles having the same mask compatible with the avoidSelfInteractionMask.
 		!( (b1->groupMask == b2->groupMask) && b1->maskCompatible(avoidSelfInteractionMask) )
	;
}

void Collider::pyHandleCustomCtorArgs(boost::python::tuple& t, boost::python::dict& d){
	if(boost::python::len(t)==0) return; // nothing to do
	if(boost::python::len(t)!=1) throw invalid_argument(("Collider optionally takes exactly one list of BoundFunctor's as non-keyword argument for constructor ("+boost::lexical_cast<string>(boost::python::len(t))+" non-keyword ards given instead)").c_str());
	typedef std::vector<shared_ptr<BoundFunctor> > vecBound;
	vecBound vb=boost::python::extract<vecBound>(t[0])();
	FOREACH(shared_ptr<BoundFunctor> bf, vb) this->boundDispatcher->add(bf);
	t=boost::python::tuple(); // empty the args
}
