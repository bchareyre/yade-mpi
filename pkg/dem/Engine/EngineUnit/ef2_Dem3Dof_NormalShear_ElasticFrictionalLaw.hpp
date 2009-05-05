// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#include<yade/pkg-common/ConstitutiveLaw.hpp>
/* Experimental constitutive law using the ConstitutiveLawDispatcher.
 * Has only purely elastic normal and shear components. */
class ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw: public ConstitutiveLaw {
	virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, MetaBody*);
	FUNCTOR2D(Dem3DofGeom,NormalShearInteraction);
	REGISTER_CLASS_AND_BASE(ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw,ConstitutiveLaw);
};
REGISTER_SERIALIZABLE(ef2_Dem3Dof_NormalShear_ElasticFrictionalLaw);
