// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2004 Janek Kozicki <cosurgi@berlios.de>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
// © 2006 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>

#include "ScGeom.hpp"
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>


YADE_PLUGIN((ScGeom));

// At least one virtual method must be in the .cpp file (!!!)
ScGeom::~ScGeom(){};

Vector3r& ScGeom::rotate(Vector3r& shearForce) const {
	// approximated rotations
	shearForce -= shearForce.cross(orthonormal_axis);
	shearForce -= shearForce.cross(twist_axis);
	//NOTE : make sure it is in the tangent plane? It's never been done before. Is it not adding rounding errors at the same time in fact?...
	//shearForce -= normal.dot(shearForce)*normal;
	return shearForce;
}

//!Precompute data needed for rotating tangent vectors attached to the interaction
void ScGeom::precompute(const State& rbp1, const State& rbp2, const Scene* scene, const shared_ptr<Interaction>& c, bool avoidGranularRatcheting){
	orthonormal_axis = prevNormal.cross(normal);
	Real angle = scene->dt*0.5*normal.dot(rbp1.angVel + rbp2.angVel);
	twist_axis = angle*normal;
	//The previous normal could be updated here after being used, them remove it from Ig2's? 
 	//prevNormal=normal;
	//Precompute shear increment
	Vector3r& x = contactPoint;
	Vector3r c1x, c2x;
	if(avoidGranularRatcheting){
		//FIXME : put the long comment on the wiki and keep only a small abstract and link here.
		/* B.C. Comment : The following definition of c1x and c2x is to avoid "granular ratcheting". This phenomenon has been introduced to me by S. McNamara in a seminar held in Paris, 2004 (GDR MiDi). The concept is also mentionned in many McNamara, Hermann, Lüding, and co-workers papers (see online : "Discrete element methods for the micro-mechanical investigation of granular ratcheting", R. García-Rojo, S. McNamara, H. J. Herrmann, Proceedings ECCOMAS 2004, @ http://www.ica1.uni-stuttgart.de/publications/2004/GMH04/), where it refers to an accumulation of strain in cyclic loadings.

	        Unfortunately, published papers tend to focus on the "good" ratcheting, i.e. irreversible deformations due to the intrinsic nature of frictional granular materials, while the talk of McNamara in Paris clearly mentioned a possible "bad" ratcheting, purely linked with the formulation of the contact laws in what he called "molecular dynamics" (i.e. Cundall's model, as opposed to "contact dynamics" from Moreau and Jean).

		Giving a short explanation :
		The bad ratcheting is best understood considering this small elastic cycle at a contact between two grains : assuming b1 is fixed, impose this displacement to b2 :
		1. translation "dx" in the normal direction
		2. rotation "a"
		3. translation "-dx" (back to initial position)
		4. rotation "-a" (back to initial orientation)

		If the branch vector used to define the relative shear in rotation*branch is not constant (typically if it is defined from the vector center->contactPoint like in the "else" below), then the shear displacement at the end of this cycle is not null : rotations a and -a are multiplied by branches of different lengths.
		It results in a finite contact force at the end of the cycle even though the positions and orientations are unchanged, in total contradiction with the elastic nature of the problem. It could also be seen as an inconsistent energy creation or loss. It is BAD! And given the fact that DEM simulations tend to generate oscillations around equilibrium (damped mass-spring), it can have a significant impact on the evolution of the packings, resulting for instance in slow creep in iterations under constant load.
		The solution to avoid that is quite simple : use a constant branch vector, like here radius_i*normal.
		 */
		// FIXME: For sphere-facet contact this will give an erroneous value of relative velocity...
		c1x =   radius1*normal;
		c2x =  -radius2*normal;
	} else {
		// FIXME: It is correct for sphere-sphere and sphere-facet contact
		c1x = (x - rbp1.pos);
		c2x = (x - rbp2.pos);
	}
	Vector3r relativeVelocity = (rbp2.vel+rbp2.angVel.cross(c2x)) - (rbp1.vel+rbp1.angVel.cross(c1x));
	if (scene->isPeriodic) relativeVelocity += scene->cell->velGrad*scene->cell->Hsize*c->cellDist.cast<Real>();
	//keep the shear part only
	relativeVelocity = relativeVelocity-normal.dot(relativeVelocity)*normal;
	shearInc = relativeVelocity*scene->dt;
}


//Do we need a function for such simple operation? BTW, the original was BAD duplicated code. ;-)
Vector3r ScGeom::updateShear(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting){rotate(shear); shear -= shearInc;}

#ifndef IGCACHE
Vector3r ScGeom::rotateAndGetShear(Vector3r& shearForce, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting){
	//Just pass null shift to the periodic version
	return rotateAndGetShear(shearForce,prevNormal,rbp1,rbp2,dt,Vector3r::Zero(),avoidGranularRatcheting);
}
#endif

//Removing this function will need to adapt all laws
const Vector3r& ScGeom::rotateAndGetShear(Vector3r& shearForce, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt, const Vector3r& shiftVel, bool avoidGranularRatcheting){
#ifdef IGCACHE
 	rotate(shearForce);
 	return shearInc;
 #else
	rotate(shearForce,prevNormal,rbp1,rbp2,dt);
	Vector3r& x = contactPoint;
	Vector3r c1x, c2x;
	if(avoidGranularRatcheting){
		// FIXME: For sphere-facet contact this will give an erroneous value of relative velocity...
		c1x =   radius1*normal;
		c2x =  -radius2*normal;
	} else {
		// FIXME: It is correct for sphere-sphere and sphere-facet contact
		c1x = (x - rbp1->pos);
		c2x = (x - rbp2->pos);
	}
	Vector3r relativeVelocity = (rbp2->vel+rbp2->angVel.cross(c2x)) - (rbp1->vel+rbp1->angVel.cross(c1x));
	
	//update relative velocity for interactions across periods
	relativeVelocity+=shiftVel;
	//keep the shear part only
	relativeVelocity = relativeVelocity-normal.dot(relativeVelocity)*normal;
	Vector3r shearDisplacement = relativeVelocity*dt;
	return shearDisplacement;
#endif //IGCACHE
}

Vector3r& ScGeom::rotate(Vector3r& shearForce, const Vector3r& prevNormal, const State* rbp1, const State* rbp2, Real dt){
#ifdef IGCACHE
 	return rotate(shearForce);
#else
	Vector3r axis;
	Real angle;
	// approximated rotations
		axis = prevNormal.cross(normal);
		shearForce -= shearForce.cross(axis);
		angle = dt*0.5*normal.dot(rbp1->angVel + rbp2->angVel);
		axis = angle*normal;
		shearForce -= shearForce.cross(axis);
	// exact rotations

// 		Quaternionr q;
// 		axis					= prevNormal.Cross(normal);
// 		angle					= acos(normal.Dot(prevNormal));
// 		q.FromAngleAxis(angle,axis);
// 		shearForce        = shearForce*q;
// 		angle             = dt*0.5*normal.dot(rbp1->angVel+rbp2->angVel);
// 		axis					= normal;
// 		q.FromAngleAxis(angle,axis);
// 		shearForce        = q*shearForce;

	return shearForce;
#endif
}

Vector3r ScGeom::getIncidentVel(const State* rbp1, const State* rbp2, Real dt, const Vector3r& shiftVel, bool avoidGranularRatcheting){
	Vector3r& x = contactPoint;
	Vector3r c1x, c2x;
	if(avoidGranularRatcheting){
		// FIXME: For sphere-facet contact this will give an erroneous value of relative velocity...
		c1x =   radius1*normal;
		c2x =  -radius2*normal;
	} else {
		// FIXME: It is correct for sphere-sphere and sphere-facet contact
		c1x = (x - rbp1->pos);
		c2x = (x - rbp2->pos);
	}
	Vector3r relativeVelocity = (rbp2->vel+rbp2->angVel.cross(c2x)) - (rbp1->vel+rbp1->angVel.cross(c1x));
	//update relative velocity for interactions across periods
	relativeVelocity+=shiftVel;
	return relativeVelocity;
}

Vector3r ScGeom::getIncidentVel(const State* rbp1, const State* rbp2, Real dt, bool avoidGranularRatcheting){
	//Just pass null shift to the periodic version
	return getIncidentVel(rbp1,rbp2,dt,Vector3r::Zero(),avoidGranularRatcheting);
}

/* keep this for reference; declarations in the header */
#if 0
	Vector3r ScGeom::relRotVector() const{
		Quaternionr relOri12=ori1.Conjugate()*ori2;
		Quaternionr oriDiff=initRelOri12.Conjugate()*relOri12;
			Vector3r axis; Real angle;
		oriDiff.ToAxisAngle(axis,angle);
		if(angle>Mathr::PI)angle-=Mathr::TWO_PI;
		return angle*axis;
	}

	void ScGeom::bendingTorsionAbs(Vector3r& bend, Real& tors){
		Vector3r relRot=relRotVector();
		tors=relRot.Dot(normal);
		bend=relRot-tors*normal;
	}
#endif

