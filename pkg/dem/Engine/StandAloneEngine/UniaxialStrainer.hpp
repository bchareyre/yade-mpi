// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<fstream>
#include<limits>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/StandAloneEngine.hpp>

#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif

/*! Axial displacing two groups of bodies in the opposite direction with given strain rate.
 *
 * Takes two groups of body IDs (in posIds and negIds) and displaces them at each timestep in the direction given by axis∈{0,1,2} (for axes x,y,z respectively). These bodies automatically have Body::isDynamic==false.
 *
 * This engine should be run once forces on particles have been computed.
 */
class UniaxialStrainer: public StandAloneEngine {
	private:
		MetaBody* rootBody;
		bool needsInit;

		void computeAxialForce();
		Real& axisCoord(body_id_t id){ return Body::byId(id,rootBody)->physicalParameters->se3.position[axis]; };
		void init();
	public:
		virtual bool isActivated(MetaBody*){return active;}
		//! strain rate, starting at 0, linearly raising to strainRate
		Real strainRate,currentStrainRate;
		//! alternatively, absolute speed of boundary motion can be specified; this is effective only at the beginning and if strainRate is not set; changing absSpeed directly during simulation wil have no effect.
		Real absSpeed;
		//! strain at which we will pause simulation; inactive (nan) by default; must be reached from below (in absolute value)
		Real stopStrain;
		//! distance of reference bodies in the direction of axis before straining started
		Real originalLength;
		//! invert the sense of straining (sharply, without transition) one this value of strain is reached. Not effective if 0.
		Real limitStrain;
		//! Flag whether the sense of straining has already been reversed
		bool notYetReversed;
		Real sumPosForces,sumNegForces;
		//! crossSection perpendicular to he strained axis, computed from AABB of MetaBody
		Real crossSectionArea;		//! Apply strain along x (0), y (1) or z(2) axis
		//! The axis which is strained (0,1,2 for x,y,z)
		int axis;
		//! If 0, straining is symmetric for negIds and posIds; for 1 (or -1), only posIds are strained and negIds don't move (or vice versa)
		int asymmetry;
		//! Whether displacement of boundary bodies perpendicular to the strained axis are blocked of are free
		bool blockDisplacements;
		//! Whether rotations of boundary bodies are blocked.
		bool blockRotations;
		//! Are we activated?
		bool active;
		//! Number of iterations that will pass without straining activity after stopStrain has been reached (default: 0)
		long idleIterations;
		//! Time for strain reaching the requested value (linear interpolation). If negative, the time is dt*(-initAccelTime), where dt is  the timestep at the first iteration.
		Real initAccelTime, initAccelTime_s /* value always in s, computed from initAccelTime */;
		//! should we set speeds at the beginning directly, instead of increasing strain rate progressively?
		bool setSpeeds;
		//! how often to update forces (initialized automatically)
		int stressUpdateInterval;

		/** bodies on which straining will be applied (on the positive and negative side of axis) */
		vector<body_id_t> posIds, negIds;
		/** coordinates of pos/neg bodies in the direction of axis */
		vector<Real> posCoords,negCoords;
		//! Auxiliary vars (serializable, for recording)
		Real strain, avgStress;

		virtual void action(MetaBody*);
		UniaxialStrainer(){axis=2; asymmetry=0; currentStrainRate=0; originalLength=-1; limitStrain=0; notYetReversed=true; crossSectionArea=-1; needsInit=true; strain=avgStress=0; blockRotations=false; blockDisplacements=false; setSpeeds=false; strainRate=absSpeed=stopStrain=numeric_limits<Real>::quiet_NaN(); active=true; idleIterations=0; initAccelTime=-200;};
		virtual ~UniaxialStrainer(){};
		REGISTER_ATTRIBUTES(StandAloneEngine,
				(strainRate) 
				(absSpeed)
				(initAccelTime)
				(stopStrain) 
				(active)
				(idleIterations)
				(currentStrainRate) 
				(axis) 
				(asymmetry) 
				(posIds) 
				(negIds) 
				(originalLength) 
				(limitStrain) 
				(notYetReversed) 
				(crossSectionArea) 
				(strain) 
				(avgStress) 
				(blockDisplacements) 
				(blockRotations) 
				(setSpeeds)
		);
	REGISTER_CLASS_AND_BASE(UniaxialStrainer,StandAloneEngine);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainer);


