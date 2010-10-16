
#include<yade/pkg-dem/L3Geom.hpp>
#include<yade/pkg-common/Sphere.hpp>

YADE_PLUGIN((L3Geom)(Ig2_Sphere_Sphere_L3Geom_Inc)(Law2_L3Geom_FrictPhys_ElPerfPl)(Law2_L6Geom_FrictPhys_Linear));

L3Geom::~L3Geom(){}
void L3Geom::applyLocalForceTorque(const Vector3r& localF, const Vector3r& localT, const Interaction* I, Scene* scene, NormShearPhys* nsp) const {
	Vector3r globF=trsf.transpose()*localF; // trsf is orthonormal, therefore inverse==transpose
	Vector3r x1c(normal*(refR1+.5*u[0])), x2c(normal*(refR1+.5*u[0]));
	if(nsp){ nsp->normalForce=normal*globF.dot(normal); nsp->shearForce=globF-nsp->normalForce; }
	Vector3r globT=Vector3r::Zero();
	// add torque, if any
	if(localT!=Vector3r::Zero()){	globT=trsf.transpose()*localT; }
	// apply force and torque
	scene->forces.addForce(I->getId1(), globF); scene->forces.addTorque(I->getId1(),x1c.cross( globF)+globT);
	scene->forces.addForce(I->getId2(),-globF); scene->forces.addTorque(I->getId2(),x2c.cross(-globF)-globT);
}

void L3Geom::applyLocalForce(const Vector3r& localF, const Interaction* I, Scene* scene, NormShearPhys* nsp) const {
	applyLocalForceTorque(localF,Vector3r::Zero(),I,scene,nsp);
}


L6Geom::~L6Geom(){}

bool Ig2_Sphere_Sphere_L3Geom_Inc::go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){
	return genericGo(/*is6Dof*/false,s1,s2,state1,state2,shift2,force,I);
};

bool Ig2_Sphere_Sphere_L6Geom_Inc::go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){
	return genericGo(/*is6Dof*/true,s1,s2,state1,state2,shift2,force,I);
};


bool Ig2_Sphere_Sphere_L3Geom_Inc::genericGo(bool is6Dof, const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){

	const Se3r& se31=state1.se3; const Se3r& se32=state2.se3;
	const Real& r1(static_pointer_cast<Sphere>(s1)->radius); const Real& r2(static_pointer_cast<Sphere>(s2)->radius);
	Vector3r relPos=(se32.position+shift2)-se31.position;
	Real unDistSq=relPos.squaredNorm()-pow(distFactor*(r1+r2),2);
	if (unDistSq>0 && !I->isReal() && !force) return false;

	// contact exists, go ahead

	Real dist=relPos.norm();
	Real uN=dist-(r1+r2);
	Vector3r normal=relPos/dist;
	Vector3r contPt=se31.position+(r1+0.5*uN)*normal;

	// create geometry
	if(!I->geom){
		if(is6Dof) I->geom=shared_ptr<L6Geom>(new L6Geom);
		else       I->geom=shared_ptr<L3Geom>(new L3Geom);
		const shared_ptr<L3Geom>& g(static_pointer_cast<L3Geom>(I->geom));
		g->contactPoint=contPt;
		g->refR1=r1; g->refR2=r1;
		g->normal=normal; const Vector3r& locX(g->normal);
		// initial local y-axis orientation, in the xz or xy plane, depending on which component is larger to avoid singularities
		Vector3r locY=g->normal.cross(g->normal[1]>g->normal[2]?Vector3r::UnitY():Vector3r::UnitZ());
		Vector3r locZ=g->normal.cross(locY);
		g->trsf.col(0)=locX; g->trsf.col(1)=locY; g->trsf.col(2)=locZ;
		g->u=Vector3r(uN,0,0); // zero shear displacement
		// L6Geom::phi is initialized to Vector3r::Zero() automatically
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
	Vector3r avgNormal=(approxMask|APPROX_NO_MID_NORMAL) ? prevNormal : .5*(prevNormal+currNormal);
	if(!(approxMask|APPROX_NO_RENORM_MID_NORMAL) && !(approxMask|APPROX_NO_MID_NORMAL)) avgNormal.normalize(); // normalize only if used and if requested via approxMask
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
	if(!(approxMask | APPROX_NO_RENORM_TRSF)){ /* renormalizing quternion is faster*/ currTrsf=Matrix3r(Quaternionr(currTrsf).normalized()); }

	/* Previous local trsf u'⁻ must be updated to current u'⁰. We have transformation T⁻ and T⁰,
		δ(a) denotes increment of a as defined above.  Two possibilities:

		1. convert to global, update, convert back: T⁰(T⁻*(u'⁻)+δ(T⁻*(u'⁻))). Quite heavy.
		2. update u'⁻ straight, using relShearVel in local coords; since relShearVel is computed 
			at (t-Δt/2), we would have to find intermediary transformation (same axis, half angle;
			the same as slerp at t=.5 between the two).

			This could be perhaps simplified by using T⁰ or T⁻ since they will not differ much,
			but it would have to be verified somehow.
	*/
	// if requested via approxMask, just use prevTrsf
	Quaternionr midTrsf=(approxMask|APPROX_NO_MID_TRSF) ? Quaternionr(prevTrsf) : Quaternionr(prevTrsf).slerp(.5,Quaternionr(currTrsf));
	
	// updates of geom here

	// midTrsf*relShearVel should have the 0-th component (approximately) zero -- to be checked
	g->u+=midTrsf*relShearDu;
	g->u[0]=uN; // this does not have to be computed incrementally
	g->trsf=currTrsf;

	// GenericSpheresContact
	g->refR1=r1; g->refR2=r2;
	g->normal=currNormal;
	g->contactPoint=contPt;

	if(is6Dof){
		const shared_ptr<L6Geom> g6=static_pointer_cast<L6Geom>(g);
		// update phi, from the difference of angular velocities
		// the difference is transformed to local coord using the midTrsf transformation
		g6->phi+=midTrsf*(scene->dt*(state2.angVel-state1.angVel));
	}

	return true;
};


void Law2_L3Geom_FrictPhys_ElPerfPl::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* I){
	L3Geom* geom=static_cast<L3Geom*>(ig.get()); FrictPhys* phys=static_cast<FrictPhys*>(ip.get());

	// compute force
	Vector3r localF=geom->relU().cwise()*Vector3r(phys->kn,phys->ks,phys->ks);
	// break if necessary
	if(localF[0]>0 && !noBreak){ scene->interactions->requestErase(I->getId1(),I->getId2()); }

	if(!noSlip){
		// plastic slip, if necessary
		Real maxFs=localF[0]*phys->tangensOfFrictionAngle; Vector2r Fs=Vector2r::Map(&localF[1]);
		if(Fs.squaredNorm()>maxFs*maxFs){
			Real ratio=sqrt(maxFs*maxFs/Fs.squaredNorm());
			geom->u0+=(1-ratio)*Vector3r(0,geom->relU()[1],geom->relU()[2]); // increment plastic displacement
			Fs*=ratio; // decrement shear force value;
		}
	}
	// apply force: this converts the force to global space, updates NormShearPhys::{normal,shear}Force, applies to particles
	geom->applyLocalForce(localF,I,scene,phys);
}

void Law2_L6Geom_FrictPhys_Linear::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* I){
	L6Geom* geom=static_cast<L6Geom*>(ig.get()); FrictPhys* phys=static_cast<FrictPhys*>(ip.get());

	// simple linear relationships
	Vector3r localF=geom->relU().cwise()*Vector3r(phys->kn,phys->ks,phys->ks);
	Vector3r localT=charLen*(geom->relPhi().cwise()*Vector3r(phys->kn,phys->ks,phys->ks));

	geom->applyLocalForceTorque(localF,localT,I,scene,phys);
}
