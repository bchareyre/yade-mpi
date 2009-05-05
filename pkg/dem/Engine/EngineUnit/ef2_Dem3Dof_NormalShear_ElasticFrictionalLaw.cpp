// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include"ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw.hpp"
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
YADE_PLUGIN("ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw");

void ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(_geom.get());
	NormalShearInteraction* phys=static_cast<NormalShearInteraction*>(_phys.get());

	// if (geom->displacementN()>0) return; // non-cohesive behavior
	phys->normalForce=(geom->displacementN()*phys->kn)*geom->normal;
	phys->shearForce=geom->displacementT()*phys->ks;

	applyForceAtContactPoint(phys->normalForce+phys->shearForce, geom->contactPoint, I->getId1(), geom->se31.position, I->getId2(), geom->se32.position, rootBody);
}

