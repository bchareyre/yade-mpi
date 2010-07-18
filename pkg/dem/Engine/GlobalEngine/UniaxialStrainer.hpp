// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#pragma once
#include<fstream>
#include<limits>
#include<yade/core/Scene.hpp>
#include<yade/pkg-dem/Shop.hpp>
#include<yade/pkg-common/BoundaryController.hpp>

#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif

/*! Axial displacing two groups of bodies in the opposite direction with given strain rate.
 *
 * Takes two groups of body IDs (in posIds and negIds) and displaces them at each timestep in the direction given by axis∈{0,1,2} (for axes x,y,z respectively). These bodies automatically have Body::isDynamic==false.
 *
 * This engine should be run once forces on particles have been computed.
 */
class UniaxialStrainer: public BoundaryController {
	private:
		bool needsInit;
		void computeAxialForce();
		Real& axisCoord(body_id_t id){ return Body::byId(id,scene)->state->pos[axis]; };
		void init();
	public:
		virtual bool isActivated(){ return active; }
		Real sumPosForces,sumNegForces;
		Real initAccelTime_s /* value always in s, computed from initAccelTime */;
		/** coordinates of pos/neg bodies in the direction of axis */
		vector<Real> posCoords,negCoords;

		virtual void action();
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(UniaxialStrainer,BoundaryController,"Axial displacing two groups of bodies in the opposite direction with given strain rate.",
			((Real,strainRate,NaN,"Rate of strain, starting at 0, linearly raising to strainRate. [-]"))
			((Real,absSpeed,NaN,"alternatively, absolute speed of boundary motion can be specified; this is effective only at the beginning and if strainRate is not set; changing absSpeed directly during simulation wil have no effect. [ms⁻¹]"))
			((Real,initAccelTime,-200,"Time for strain reaching the requested value (linear interpolation). If negative, the time is dt*(-initAccelTime), where dt is  the timestep at the first iteration. [s]"))
			((Real,stopStrain,NaN,"Strain at which we will pause simulation; inactive (nan) by default; must be reached from below (in absolute value)"))
			((bool,active,true,"Whether this engine is activated"))
			((long,idleIterations,0,"Number of iterations that will pass without straining activity after stopStrain has been reached"))
			((Real,currentStrainRate,NaN,"Current strain rate (update automatically). |yupdate|"))
			((int,axis,2,"The axis which is strained (0,1,2 for x,y,z)"))
			((int,asymmetry,((void)"symmetric",0),"If 0, straining is symmetric for negIds and posIds; for 1 (or -1), only posIds are strained and negIds don't move (or vice versa)")) 
			((vector<body_id_t>,posIds,,"Bodies on which strain will be applied (on the positive end along the axis)"))
			((vector<body_id_t>,negIds,,"Bodies on which strain will be applied (on the negative end along the axis)"))
			((Real,originalLength,NaN,"Distance of reference bodies in the direction of axis before straining started (computed automatically) [m]"))
			((Real,limitStrain,((void)"disabled",0),"Invert the sense of straining (sharply, without transition) one this value of strain is reached. Not effective if 0."))
			((bool,notYetReversed,true,"Flag whether the sense of straining has already been reversed (only used internally).")) 
			((Real,crossSectionArea,NaN,"crossSection perpendicular to he strained axis; must be given explicitly [m²]"))
			((Real,strain,0,"Current strain value, elongation/originalLength |yupdate| [-]")) 
			((Real,avgStress,0,"Current average stress  |yupdate| [Pa]"))
			((bool,blockDisplacements,false,"Whether displacement of boundary bodies perpendicular to the strained axis are blocked of are free"))
			((bool,blockRotations,false,"Whether rotations of boundary bodies are blocked."))
			((bool,setSpeeds,false,"should we set speeds at the beginning directly, instead of increasing strain rate progressively?"))
			((int,stressUpdateInterval,10,"How often to recompute stress on supports.")),
			/*ctor*/ needsInit=true;
		);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(UniaxialStrainer);


