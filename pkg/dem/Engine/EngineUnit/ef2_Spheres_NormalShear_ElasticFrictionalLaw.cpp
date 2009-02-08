// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include"ef2_Spheres_NormalShear_ElasticFrictionalLaw.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
YADE_PLUGIN("ef2_Spheres_NormalShear_ElasticFrictionalLaw");

void ef2_Spheres_NormalShear_ElasticFrictionalLaw::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody){
	SpheresContactGeometry* geom=static_cast<SpheresContactGeometry*>(_geom.get());
	NormalShearInteraction* phys=static_cast<NormalShearInteraction*>(_phys.get());

	// if (geom->displacementN()>0) return; // non-cohesive behavior

	phys->normalForce=(geom->displacementN()*phys->kn)*geom->normal;
	phys->shearForce=geom->displacementT()*phys->ks;

	bodyForce (I->getId1(),rootBody)+=phys->normalForce;
	bodyForce (I->getId2(),rootBody)-=phys->normalForce;
	bodyTorque(I->getId1(),rootBody)+=(geom->contPt()-geom->pos1).Cross(phys->shearForce);
	bodyTorque(I->getId2(),rootBody)-=(geom->contPt()-geom->pos2).Cross(phys->shearForce);
}

