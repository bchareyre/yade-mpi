
#include<yade/pkg-dem/L3Geom.hpp>
#include<yade/pkg-common/Sphere.hpp>

YADE_PLUGIN((L3Geom)(Ig2_Sphere_Sphere_L3Geom_Inc));

L3Geom::~L3Geom(){}

bool Ig2_Sphere_Sphere_L3Geom_Inc::go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){
	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;
	const Real& r1(static_pointer_cast<Sphere>(s1)->radius); const Real& r2(static_pointer_cast<Sphere>(s2)->radius);
	Vector3r relPos=(se32.position+shift2)-se31.position;
	Real unDistSq=relPos.squaredNorm()-pow(distFactor*(r1+r2),2);
	if (unDistSq>0 && !I->isReal() && !force) return false;

	// contact exists, go ahead

	Real uN=relPos.norm()-(r1+r2);
	Vector3r normal=relPos.normalized();
	Vector3r contPt=se31.position+(r1+0.5*uN)*normal;

	// create geometry
	if(!I->geom){ 
		I->geom=shared_ptr<L3Geom>(new L3Geom); const shared_ptr<L3Geom>& g(static_pointer_cast<L3Geom>(I->geom));
		g->contactPoint=contPt;
		g->refR1=r1; g->refR2=r1;
		g->normal=normal; const Vector3r& locX(g->normal);
		// initial local y-axis orientation, in the xz or xy plane, depending on which component is larger to avoid singularities
		Vector3r locY=g->normal.cross(g->normal[1]>g->normal[2]?Vector3r::UnitY():Vector3r::UnitZ());
		Vector3r locZ=g->normal.cross(locY);
		g->trsf.col(0)=locX; g->trsf.col(1)=locY; g->trsf.col(2)=locZ;
		g->u=Vector3r(uN,0,0); // zero shear displacement
		return true;
	}
	
	// update geometry

	/* motion of the conctact consists in rigid motion (normRotVec, normTwistVec) and mutual motion (relShearDu);
	   they are used to update trsf and u
	*/

	const shared_ptr<L3Geom>& g(static_pointer_cast<L3Geom>(I->geom));
	const Vector3r& currNormal(normal); const Vector3r& prevNormal(g->normal);
	// normal rotation vector, between last steps
	Vector3r normRotVec=prevNormal.cross(currNormal);
	// contrary to what ScGeom::precompute does now (r2486), we take average normal, i.e. .5*(prevNormal+currNormal),
	// so that all terms in the equation are in the previous mid-step
	// the re-normalization might not be necessary for very small increments, but better do it
	Vector3r avgNormal=.5*(prevNormal+currNormal); avgNormal.normalize();
	// twist vector of the normal from the last step
	Vector3r normTwistVec=avgNormal*scene->dt*.5*avgNormal.dot(state1.angVel+state2.angVel);
	// compute relative velocity
	// noRatch: take radius or current distance as the branch vector; see discussion in ScGeom::precompute (avoidGranularRatcheting)
	Vector3r c1x=(noRatch ? ( r1*normal).eval() : (contPt-state1.pos).eval());
	Vector3r c2x=(noRatch ? (-r2*normal).eval() : (contPt-state2.pos).eval());
	Vector3r relShearVel=(state2.vel+state2.angVel.cross(c2x))-(state1.vel+state1.angVel.cross(c1x));
	// account for relative velocity of particles in different cell periods
	if(scene->isPeriodic) relShearVel+=scene->cell->velGrad*scene->cell->Hsize*I->cellDist.cast<Real>();
	// separate the tangential component (this is likely to be not stricly necessary, since u[0] is set directly later)
	relShearVel-=avgNormal.dot(relShearVel)*avgNormal;
	Vector3r relShearDu=relShearVel*scene->dt;

	/* Update of quantities in global coords consists in adding 3 increments we have computed; in global coords (a is any vector)

		1. +relShearVel*scene->dt;      // mutual motion of the contact
		2. -a.cross(normRotVec);   // rigid rotation perpendicular to the normal
		3. -a.cross(normTwistVec); // rigid rotation parallel to the normal

	*/

	// compute current transformation, by updating previous axes
	// the X axis can be prescribed directly (copy of normal)
	// the mutual motion on the contact does not change transformation
	const Matrix3r& prevTrsf(g->trsf); Matrix3r currTrsf; currTrsf.col(0)=currNormal;
	for(int i=1; i<3; i++){
		currTrsf.col(i)=prevTrsf.col(i)-prevTrsf.col(i).cross(normRotVec)-prevTrsf.col(i).cross(normTwistVec);
	}

	/* Previous local trsf u'⁻ must be updated to current u'⁰. We have transformation T⁻ and T⁰,
		δ(a) denotes increment of a as defined above.  Two possibilities:

		1. convert to global, update, convert back: T⁰(T⁻*(u'⁻)+δ(T⁻*(u'⁻))). Quite heavy.
		2. update u'⁻ straight, using relShearVel in local coords; since relShearVel is computed 
			at (t-Δt/2), we would have to find intermediary transformation (same axis, half angle;
			the same as slerp at t=.5 between the two).

			This could be perhaps simplified by using T⁰ or T⁻ since they will not differ much,
			but it would have to be verified somehow.
	*/
	Quaternionr midTrsf=Quaternionr(prevTrsf).slerp(.5,Quaternionr(currTrsf));
	
	// updates of geom here

	// midTrsf*relShearVel should have the 0-th component (approximately) zero -- to be checked
	g->u+=midTrsf*relShearDu;
	g->u[0]=uN; // this does not have to be computed incrementally
	g->trsf=currTrsf;

	// GenericSpheresContact
	g->refR1=r1; g->refR2=r2;
	g->normal=currNormal;
	g->contactPoint=contPt;

	return true;
};



