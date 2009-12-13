/**

=== HIGH LEVEL OVERVIEW OF RockPM ===

Rock Particle Model (RockPM) is a set of classes for modelling
mechanical behavior of mining rocks.
*/

#pragma once

#include<yade/pkg-common/InteractionPhysicsFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-common/PeriodicEngines.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-common/ElasticMat.hpp>


class Law2_Dem3DofGeom_RockPMPhys_Rpm: public LawFunctor{
	public:
		virtual void go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody);
		FUNCTOR2D(Dem3DofGeom,RpmPhys);
		REGISTER_CLASS_AND_BASE(Law2_Dem3DofGeom_RockPMPhys_Rpm,LawFunctor);
		REGISTER_ATTRIBUTES(LawFunctor,/*nothing here*/);
		DECLARE_LOGGER;	
};
REGISTER_SERIALIZABLE(Law2_Dem3DofGeom_RockPMPhys_Rpm);

/** This class holds information associated with each body */
class RpmMat: public GranularMat {
	public:
		int exampleNumber; ///<Number of "stone"
		bool initCohesive, isDamaged;
		Real stressCompressMax, Brittleness, G_over_E; ///<Parameters for damaging
		
		RpmMat(): 
			exampleNumber(0.), 
			initCohesive(false), 
			isDamaged(false), 
			stressCompressMax(0), 
			Brittleness(0), 
			G_over_E(1) {createIndex();};
		REGISTER_ATTRIBUTES(GranularMat, 
			(exampleNumber)
			(initCohesive)
			(isDamaged)
			(stressCompressMax)
			(Brittleness)
			(G_over_E));
		REGISTER_CLASS_AND_BASE(RpmMat,GranularMat);
		REGISTER_CLASS_INDEX(RpmMat,GranularMat);
};
REGISTER_SERIALIZABLE(RpmMat);


class Ip2_RpmMat_RpmMat_RpmPhys: public InteractionPhysicsFunctor{
	private:
	public:
		Real initDistance;

		Ip2_RpmMat_RpmMat_RpmPhys(){
			initDistance = 0;
		}

		virtual void go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction);
		REGISTER_ATTRIBUTES(InteractionPhysicsFunctor,
			(initDistance)
		);

		FUNCTOR2D(RpmMat,RpmMat);
		REGISTER_CLASS_AND_BASE(Ip2_RpmMat_RpmMat_RpmPhys,InteractionPhysicsFunctor);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Ip2_RpmMat_RpmMat_RpmPhys);


class RpmPhys: public NormalShearInteraction {
	private:
	public:
		Real crossSection,E,G,tanFrictionAngle,lengthMaxCompression,lengthMaxTension;
		Real omega, Fn, sigmaN, epsN;
		Vector3r epsT, sigmaT, Fs;
		 

		bool isCohesive;

		RpmPhys(): NormalShearInteraction(),
			crossSection(0),
			E(0),
			G(0), 
			tanFrictionAngle(0), 
			lengthMaxCompression(0), 
			lengthMaxTension(0), 
			isCohesive(false) { createIndex(); epsT=Vector3r::ZERO; omega=0; Fn=0; Fs=Vector3r::ZERO;}
		virtual ~RpmPhys();

		REGISTER_ATTRIBUTES(NormalShearInteraction,
			(E)
			(G)
			(tanFrictionAngle)
			(isCohesive)
			(lengthMaxCompression)
			(lengthMaxTension)
		);
	REGISTER_CLASS_AND_BASE(RpmPhys,NormalShearInteraction);
	REGISTER_CLASS_INDEX(RpmPhys,NormalShearInteraction);
};
REGISTER_SERIALIZABLE(RpmPhys);
