/* C.W.Boon @ NOV 2009 */
YADE_REQUIRE_FEATURE(abcd);
#pragma once

#include<yade/pkg-dem/BodyMacroParameters.hpp> //Superclass for body properties
#include<yade/pkg-common/InteractionPhysicsFunctor.hpp> //Superclass to link body and interaction properties
#include<yade/pkg-common/NormalShearInteractions.hpp> //Superclass for interaction properties 
#include<yade/pkg-common/ConstitutiveLaw.hpp> //Superclass for contact laws

/* Contact Law */
class CundallStrackLaw: public ConstitutiveLaw{
	public:
		
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody);
		FUNCTOR2D(Dem3DofGeom,CSPhys);
		REGISTER_CLASS_AND_BASE(CundallStrackLaw,ConstitutiveLaw);
		REGISTER_ATTRIBUTES(ConstitutiveLaw,/*nothing here*/);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(CundallStrackLaw);

/* This class stores body properties */
/* Superclass:BodyMacroParameters has poisson, frictionAngle; ElasticBodyParameters has young's modulus */
class CSMat: public BodyMacroParameters {
	public:
		CSMat(){createIndex();};
		REGISTER_ATTRIBUTES(BodyMacroParameters, /*nothing here*/);
		REGISTER_CLASS_AND_BASE(CSMat,BodyMacroParameters);
		REGISTER_CLASS_INDEX(CSMat,BodyMacroParameters);
};
REGISTER_SERIALIZABLE(CSMat);

/* This class links body and interaction properties */
/* It does not store variables */
class Ip2_CSMat_CSMat_CSPhys: public InteractionPhysicsFunctor{
	public:
		Ip2_CSMat_CSMat_CSPhys(){};
		virtual void go(const shared_ptr<PhysicalParameters>& b1, const shared_ptr<PhysicalParameters>& b2, const shared_ptr<Interaction>& interaction);
		REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,/*nothing here*/);
		FUNCTOR2D(CSMat,CSMat);
		REGISTER_CLASS_AND_BASE(Ip2_CSMat_CSMat_CSPhys,InteractionPhysicsFunctor);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_CSMat_CSMat_CSPhys);


/* This class stores interaction properties */
/* Superclass:Normal NormalShearInteraction has kn, <vector> normalForce, ks, <vector> shearForce */
class CSPhys: public NormalShearInteraction {
	public:
		CSPhys(){createIndex();};

		// kn,ks,normal inherited from NormalShearInteraction
		Real 		frictionAngle 			// angle of friction, according to Coulumb criterion
				,tangensOfFrictionAngle
				;	
		Vector3r	prevNormal;			// unit normal of the contact plane.
		
	virtual ~CSPhys(){};
	REGISTER_ATTRIBUTES(NormalShearInteraction,(frictionAngle)(tangensOfFrictionAngle));
	REGISTER_CLASS_AND_BASE(CSPhys,NormalShearInteraction);
	REGISTER_CLASS_INDEX(CSPhys,NormalShearInteraction);
};
REGISTER_SERIALIZABLE(CSPhys);
