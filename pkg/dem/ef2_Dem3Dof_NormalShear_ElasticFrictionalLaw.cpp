// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include"ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw.hpp"
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/pkg-common/NormShearPhys.hpp>
YADE_PLUGIN((ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw));
void ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(_geom.get());
	NormShearPhys* phys=static_cast<NormShearPhys*>(_phys.get());

	// if (geom->displacementN()>0) return; // non-cohesive behavior
	phys->normalForce=(geom->displacementN()*phys->kn)*geom->normal;
	phys->shearForce=geom->displacementT()*phys->ks;

	applyForceAtContactPoint(phys->normalForce+phys->shearForce, geom->contactPoint, I->getId1(), geom->se31.position, I->getId2(), geom->se32.position);
}


YADE_REQUIRE_FEATURE(PHYSPAR);

