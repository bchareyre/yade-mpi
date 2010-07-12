/*************************************************************************
*  Copyright (C) 2007 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2008 by Janek Kozicki <cosurgi@berlios.de>              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ip2_2xCohFrictMat_CohFrictPhys.hpp"
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/CohFrictPhys.hpp>
#include<yade/pkg-dem/CohFrictMat.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

void Ip2_2xCohFrictMat_CohFrictPhys::go(const shared_ptr<Material>& b1    // CohFrictMat
                                        , const shared_ptr<Material>& b2 // CohFrictMat
                                        , const shared_ptr<Interaction>& interaction)
{
	CohFrictMat* sdec1 = static_cast<CohFrictMat*>(b1.get());
	CohFrictMat* sdec2 = static_cast<CohFrictMat*>(b2.get());
	ScGeom* interactionGeometry = YADE_CAST<ScGeom*>(interaction->interactionGeometry.get());

	//Create cohesive interractions only once
	if (setCohesionNow && cohesionDefinitionIteration==-1) {
		cohesionDefinitionIteration=Omega::instance().getCurrentIteration();
	}
	if (setCohesionNow && cohesionDefinitionIteration!=-1 && cohesionDefinitionIteration!=Omega::instance().getCurrentIteration()) {
		cohesionDefinitionIteration = -1;
		setCohesionNow = 0;}
		
	if (interactionGeometry)
	{
		if (!interaction->interactionPhysics)
		{
			interaction->interactionPhysics = shared_ptr<CohFrictPhys>(new CohFrictPhys());
			CohFrictPhys* contactPhysics = YADE_CAST<CohFrictPhys*>(interaction->interactionPhysics.get());
			Real Ea 	= sdec1->young;
			Real Eb 	= sdec2->young;
			Real Va 	= sdec1->poisson;
			Real Vb 	= sdec2->poisson;
			Real Da 	= interactionGeometry->radius1;
			Real Db 	= interactionGeometry->radius2;
			Real fa 	= sdec1->frictionAngle;
			Real fb 	= sdec2->frictionAngle;
			Real Kn = 2.0*Ea*Da*Eb*Db/(Ea*Da+Eb*Db);//harmonic average of two stiffnesses

			Real Ks;
			if (Va && Vb) Ks = 2.0*Ea*Da*Va*Eb*Db*Vb/(Ea*Da*Va+Eb*Db*Vb);//harmonic average of two stiffnesses with ks=V*kn for each sphere
			else Ks=0;
				
			// Jean-Patrick Plassiard, Noura Belhaine, Frederic
			// Victor Donze, "Calibration procedure for spherical
			// discrete elements using a local moemnt law".
			Real Kr = Da*Db*Ks*2.0; // just like "2.0" above - it's an arbitrary parameter
			contactPhysics->frictionAngle			= std::min(fa,fb);
			contactPhysics->tangensOfFrictionAngle		= std::tan(contactPhysics->frictionAngle);

			if ((setCohesionOnNewContacts || setCohesionNow) && sdec1->isCohesive && sdec2->isCohesive)
			{
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(Db, Da),2);
				contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(Db, Da),2);;

				contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
				contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
				contactPhysics->initialPosition1    = Body::byId(interaction->getId1())->state->pos;
				contactPhysics->initialPosition2    = Body::byId(interaction->getId2())->state->pos;
				contactPhysics->kr = Kr;
				contactPhysics->initialContactOrientation.setFromTwoVectors(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
				contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.conjugate() * contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.conjugate() * contactPhysics->initialContactOrientation;
			}
			contactPhysics->prevNormal = interactionGeometry->normal;
			contactPhysics->kn = Kn;
			contactPhysics->ks = Ks;
			contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
			contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
			contactPhysics->initialPosition1    = Body::byId(interaction->getId1())->state->pos;
			contactPhysics->initialPosition2    = Body::byId(interaction->getId2())->state->pos;
			contactPhysics->kr = Kr;
			contactPhysics->initialContactOrientation.setFromTwoVectors(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
			contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.conjugate() * contactPhysics->initialContactOrientation;
			contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.conjugate() * contactPhysics->initialContactOrientation;
			//contactPhysics->elasticRollingLimit = elasticRollingLimit;
		}
		else // !isNew, but if setCohesionNow, all contacts are initialized like if they were newly created
		{
			CohFrictPhys* contactPhysics = YADE_CAST<CohFrictPhys*>(interaction->interactionPhysics.get());
			if (setCohesionNow && sdec1->isCohesive && sdec2->isCohesive)
			{
				contactPhysics->cohesionBroken = false;
				contactPhysics->normalAdhesion			= normalCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
				contactPhysics->shearAdhesion			= shearCohesion*pow(std::min(interactionGeometry->radius2, interactionGeometry->radius1),2);
				contactPhysics->initialOrientation1	= Body::byId(interaction->getId1())->state->ori;
				contactPhysics->initialOrientation2	= Body::byId(interaction->getId2())->state->ori;
				contactPhysics->initialPosition1    = Body::byId(interaction->getId1())->state->pos;
				contactPhysics->initialPosition2    = Body::byId(interaction->getId2())->state->pos;
				contactPhysics->initialContactOrientation.setFromTwoVectors(Vector3r(1.0,0.0,0.0),interactionGeometry->normal);
				contactPhysics->currentContactOrientation = contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact1   = contactPhysics->initialOrientation1.conjugate() * contactPhysics->initialContactOrientation;
				contactPhysics->orientationToContact2	= contactPhysics->initialOrientation2.conjugate() * contactPhysics->initialContactOrientation;
			}
		}
	}
};
YADE_PLUGIN((Ip2_2xCohFrictMat_CohFrictPhys));



