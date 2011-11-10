/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ip2_FrictMat_FrictMat_MindlinCapillaryPhys.hpp"
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/pkg/dem/HertzMindlin.hpp>
#include<yade/pkg/common/ElastMat.hpp>
#include<yade/pkg/common/Dispatching.hpp>

#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN(
            (MindlinCapillaryPhys)
            (Ip2_FrictMat_FrictMat_MindlinCapillaryPhys)
);

MindlinCapillaryPhys::~MindlinCapillaryPhys(){};// destructor

void Ip2_FrictMat_FrictMat_MindlinCapillaryPhys::go( const shared_ptr<Material>& b1 //FrictMat
					, const shared_ptr<Material>& b2 // FrictMat
					, const shared_ptr<Interaction>& interaction)
{
	if(interaction->phys) return; // no updates of an already existing contact necessary

	shared_ptr<MindlinCapillaryPhys> contactPhysics(new MindlinCapillaryPhys());
	interaction->phys = contactPhysics;

	FrictMat* mat1 = YADE_CAST<FrictMat*>(b1.get());
	FrictMat* mat2 = YADE_CAST<FrictMat*>(b2.get());
	
	/* from interaction physics */
	Real Ea = mat1->young;
	Real Eb = mat2->young;
	Real Va = mat1->poisson;
	Real Vb = mat2->poisson;
	Real fa = mat1->frictionAngle;
	Real fb = mat2->frictionAngle;

	/* from interaction geometry */
	GenericSpheresContact* scg = YADE_CAST<GenericSpheresContact*>(interaction->geom.get());		
	Real Da = scg->refR1>0 ? scg->refR1 : scg->refR2; 
	Real Db = scg->refR2; 
	Vector3r normal=scg->normal; 

	/* calculate stiffness coefficients */
	Real Ga = Ea/(2*(1+Va));
	Real Gb = Eb/(2*(1+Vb));
	Real G = (Ga+Gb)/2; // average of shear modulus
	Real V = (Va+Vb)/2; // average of poisson's ratio
	Real E = Ea*Eb/((1.-std::pow(Va,2))*Eb+(1.-std::pow(Vb,2))*Ea); // Young modulus
	Real R = Da*Db/(Da+Db); // equivalent radius
	Real Rmean = (Da+Db)/2.; // mean radius
	Real Kno = 4./3.*E*sqrt(R); // coefficient for normal stiffness
	Real Kso = 2*sqrt(4*R)*G/(2-V); // coefficient for shear stiffness
	Real frictionAngle = std::min(fa,fb);

	Real Adhesion = 4.*Mathr::PI*R*gamma; // calculate adhesion force as predicted by DMT theory

	/* pass values calculated from above to MindlinCapillaryPhys */
	contactPhysics->tangensOfFrictionAngle = std::tan(frictionAngle); 
	//mindlinPhys->prevNormal = scg->normal; // used to compute relative rotation
	contactPhysics->kno = Kno; // this is just a coeff
	contactPhysics->kso = Kso; // this is just a coeff
	contactPhysics->adhesionForce = Adhesion;
	
	contactPhysics->kr = krot;
	contactPhysics->ktw = ktwist;
	contactPhysics->maxBendPl = eta*Rmean; // does this make sense? why do we take Rmean?

	/* compute viscous coefficients */
	if(en && betan) throw std::invalid_argument("Ip2_FrictMat_FrictMat_MindlinCapillaryPhys: only one of en, betan can be specified.");
	if(es && betas) throw std::invalid_argument("Ip2_FrictMat_FrictMat_MindlinCapillaryPhys: only one of es, betas can be specified.");

	// en or es specified, just compute alpha, otherwise alpha remains 0
	if(en || es){
		Real logE = log((*en)(mat1->id,mat2->id));
		contactPhysics->alpha = -sqrt(5/6.)*2*logE/sqrt(pow(logE,2)+pow(Mathr::PI,2))*sqrt(2*E*sqrt(R)); // (see Tsuji, 1992)
	}
	
	// betan specified, use that value directly; otherwise give zero
	else{	
		contactPhysics->betan=betan ? (*betan)(mat1->id,mat2->id) : 0; 
		contactPhysics->betas=betas ? (*betas)(mat1->id,mat2->id) : contactPhysics->betan;
	}
};




