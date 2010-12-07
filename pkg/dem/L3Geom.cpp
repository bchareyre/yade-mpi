
#include<yade/pkg/dem/L3Geom.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Wall.hpp>

#ifdef YADE_OPENGL
	#include<yade/lib/opengl/OpenGLWrapper.hpp>
	#include<yade/lib/opengl/GLUtils.hpp>
	#include<GL/glu.h>
#endif

YADE_PLUGIN((L3Geom)(L6Geom)(Ig2_Sphere_Sphere_L3Geom_Inc)(Ig2_Wall_Sphere_L3Geom_Inc)(Ig2_Sphere_Sphere_L6Geom_Inc)(Law2_L3Geom_FrictPhys_ElPerfPl)(Law2_L6Geom_FrictPhys_Linear)
	#ifdef YADE_OPENGL
		(Gl1_L3Geom)(Gl1_L6Geom)
	#endif
);

L3Geom::~L3Geom(){}
void L3Geom::applyLocalForceTorque(const Vector3r& localF, const Vector3r& localT, const Interaction* I, Scene* scene, NormShearPhys* nsp) const {

	Vector2r foo; // avoid undefined ~Vector2r with clang?

	Vector3r globF=trsf.transpose()*localF; // trsf is orthonormal, therefore inverse==transpose
	Vector3r x1c(normal*(refR1+.5*u[0])), x2c(-normal*(refR2+.5*u[0]));
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


CREATE_LOGGER(Ig2_Sphere_Sphere_L3Geom_Inc);

bool Ig2_Sphere_Sphere_L3Geom_Inc::genericGo(bool is6Dof, const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){
	// temporary hack only, to not have elastic potential energy in rigid packings with overlapping spheres
	//if(state1.blockedDOFs==State::DOF_ALL && state2.blockedDOFs==State::DOF_ALL) return false;

	const Real& r1=s1->cast<Sphere>().radius; const Real& r2=s2->cast<Sphere>().radius;
	Vector3r relPos=state2.pos+shift2-state1.pos;
	Real unDistSq=relPos.squaredNorm()-pow(distFactor*(r1+r2),2);
	if (unDistSq>0 && !I->isReal() && !force) return false;

	// contact exists, go ahead

	Real dist=relPos.norm();
	Real uN=dist-(r1+r2);
	Vector3r normal=relPos/dist;
	Vector3r contPt=state1.pos+(r1+0.5*uN)*normal;

	// create geometry
	if(!I->geom){
		if(is6Dof) I->geom=shared_ptr<L6Geom>(new L6Geom);
		else       I->geom=shared_ptr<L3Geom>(new L3Geom);
		L3Geom& g(I->geom->cast<L3Geom>());
		g.contactPoint=contPt;
		g.refR1=r1; g.refR2=r1;
		g.normal=normal; const Vector3r& locX(g.normal);
		// initial local y-axis orientation, in the xz or xy plane, depending on which component is larger to avoid singularities
		Vector3r locY=normal.cross(normal[1]<normal[2]?Vector3r::UnitY():Vector3r::UnitZ());
		Vector3r locZ=normal.cross(locY);
		g.trsf.row(0)=locX; g.trsf.row(1)=locY; g.trsf.row(2)=locZ;
		g.u=Vector3r(uN,0,0); // zero shear displacement
		// L6Geom::phi is initialized to Vector3r::Zero() automatically
		//cerr<<"Init trsf=\n"<<g.trsf<<endl<<"locX="<<locX<<", locY="<<locY<<", locZ="<<locZ<<endl;
		return true;
	}
	
	// update geometry

	/* motion of the conctact consists in rigid motion (normRotVec, normTwistVec) and mutual motion (relShearDu);
	   they are used to update trsf and u
	*/

	L3Geom& g(I->geom->cast<L3Geom>());
	const Vector3r& currNormal(normal); const Vector3r& prevNormal(g.normal);
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
	Vector3r c2x=(noRatch ? (-r2*normal).eval() : (contPt-state2.pos+shift2).eval());
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
	const Matrix3r& prevTrsf(g.trsf); Matrix3r currTrsf; currTrsf.row(0)=currNormal;
	for(int i=1; i<3; i++){
		currTrsf.row(i)=prevTrsf.row(i)-prevTrsf.row(i).cross(normRotVec)-prevTrsf.row(i).cross(normTwistVec);
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
	//cerr<<"prevTrsf=\n"<<prevTrsf<<", currTrsf=\n"<<currTrsf<<", midTrsf=\n"<<Matrix3r(midTrsf)<<endl;
	
	// updates of geom here

	// midTrsf*relShearVel should have the 0-th component (approximately) zero -- to be checked
	g.u+=midTrsf*relShearDu;
	//cerr<<"midTrsf=\n"<<midTrsf<<",relShearDu="<<relShearDu<<", transformed "<<midTrsf*relShearDu<<endl;
	g.u[0]=uN; // this does not have to be computed incrementally
	g.trsf=currTrsf;

	// GenericSpheresContact
	g.refR1=r1; g.refR2=r2;
	g.normal=currNormal;
	g.contactPoint=contPt;

	if(is6Dof){
		// update phi, from the difference of angular velocities
		// the difference is transformed to local coord using the midTrsf transformation
		// perhaps not consistent when spheres have different radii (depends how bending moment is computed)
		I->geom->cast<L6Geom>().phi+=midTrsf*(scene->dt*(state2.angVel-state1.angVel));
	}
	return true;
};

bool Ig2_Wall_Sphere_L3Geom_Inc::go(const shared_ptr<Shape>& s1, const shared_ptr<Shape>& s2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& I){
	if(scene->isPeriodic) throw std::logic_error("Ig2_Wall_Sphere_L3Geom_Inc does not handle periodic boundary conditions.");
	const Real& radius=s2->cast<Sphere>().radius; const int& ax(s1->cast<Wall>().axis); const int& sense(s1->cast<Wall>().sense);
	Real dist=state2.pos[ax]+shift2[ax]-state1.pos[ax]; // signed "distance" between centers
	if(!I->isReal() && abs(dist)>radius && !force) { return false; }// wall and sphere too far from each other
	// contact point is sphere center projected onto the wall
	Vector3r contPt=state2.pos+shift2; contPt[ax]=state1.pos[ax];
	Vector3r normal=Vector3r::Zero();
	// wall interacting from both sides: normal depends on sphere's position
	assert(sense==-1 || sense==0 || sense==1);
	if(sense==0) normal[ax]=dist>0?1.:-1.;
	else normal[ax]=sense==1?1.:-1;
	Real uN=normal[ax]*dist-radius; // takes in account sense, radius and distance

	if(!I->geom){
		/* if(is6Dof) I->geom=shared_ptr<L6Geom>(new L6Geom);
		else */       I->geom=shared_ptr<L3Geom>(new L3Geom);
		L3Geom& g=I->geom->cast<L3Geom>();
		g.contactPoint=contPt;
		g.refR1=0; g.refR2=radius;
		g.normal=normal;
		g.trsf=Matrix3r::Zero();
		g.trsf.row(0)=normal; g.trsf.row(1)[(ax+1)%3]=1; g.trsf.row(2)[(ax+2)%3]=1;
		g.u=Vector3r(uN,0,0); // zero shear displacement
		// L6Geom::phi is initialized to Vector3r::Zero() automatically
		return true;
	}
	L3Geom& g=I->geom->cast<L3Geom>();
	if(g.normal!=normal) throw std::logic_error(("Ig2_Wall_Sphere_L3Geom_Inc: normal changed in Wall+Sphere ##"+lexical_cast<string>(I->getId1())+"+"+lexical_cast<string>(I->getId2())+" (Wall.sense≠0?)").c_str());
	Vector3r normTwistVec=normal*scene->dt*.5*normal.dot(state1.angVel+state2.angVel);

	/* TODO: below is a lot of code common with Sphere+Sphere; identify, factor out in a separate func */
	/* needed params: states, geom (trsf, contact point, normal), branch vectors. That's it? */

	Vector3r c1x=contPt-state1.pos; // is not aligned with normal; Wall could rotate in a plate-like manner
	Vector3r c2x=(noRatch ? (-radius*normal).eval() : (contPt-state2.pos+shift2).eval());
	Vector3r relShearVel=(state2.vel+state2.angVel.cross(c2x))-(state1.vel+state1.angVel.cross(c1x));
	// perhaps not necessary, see in Ig2_Sphere_Sphere_L3Geom_Inc
	relShearVel-=normal.dot(relShearVel)*normal;
	Vector3r relShearDu=relShearVel*scene->dt;
	// normal did not change, no need to re-assign row(0); no normRotVec either, since the normal orientation never changes
	Matrix3r prevTrsf(g.trsf); // copy
	for(int i=1; i<3; i++){	g.trsf.row(i)=g.trsf.row(i)-g.trsf.row(i).cross(normTwistVec); }
	if(!(approxMask | APPROX_NO_RENORM_TRSF)){ /* renormalizing quternion is faster*/ g.trsf=Matrix3r(Quaternionr(g.trsf).normalized()); }
	Quaternionr midTrsf=(approxMask|APPROX_NO_MID_TRSF) ? Quaternionr(prevTrsf) : Quaternionr(prevTrsf).slerp(.5,Quaternionr(g.trsf));
	g.u+=midTrsf*relShearDu;
	g.u[0]=uN;
	// GenericSpheresContact
	g.refR1=0; g.refR2=radius;
	g.contactPoint=contPt;
	/* if(is6Dof){ } */
	return true;
};

void Law2_L3Geom_FrictPhys_ElPerfPl::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* I){
	L3Geom* geom=static_cast<L3Geom*>(ig.get()); FrictPhys* phys=static_cast<FrictPhys*>(ip.get());

	// compute force
	Vector3r& localF(geom->F);
	localF=geom->relU().cwise()*Vector3r(phys->kn,phys->ks,phys->ks);
	// break if necessary
	if(localF[0]>0 && !noBreak){ scene->interactions->requestErase(I->getId1(),I->getId2()); }

	if(!noSlip){
		// plastic slip, if necessary; non-zero elastic limit only for compression
		Real maxFs=-min(0.,localF[0]*phys->tangensOfFrictionAngle); Eigen::Map<Vector2r> Fs(&localF[1]);
		//cerr<<"u="<<geom->relU()<<", maxFs="<<maxFs<<", Fn="<<localF[0]<<", |Fs|="<<Fs.norm()<<", Fs="<<Fs<<endl;
		if(Fs.squaredNorm()>maxFs*maxFs){
			Real ratio=sqrt(maxFs*maxFs/Fs.squaredNorm());
			Vector3r u0slip=(1-ratio)*Vector3r(/*no slip in the normal sense*/0,geom->relU()[1],geom->relU()[2]);
			geom->u0+=u0slip; // increment plastic displacement
			Fs*=ratio; // decrement shear force value;
			//cerr<<"SLIP: Fs="<<Fs<<", ratio="<<ratio<<", u0slip="<<u0slip<<", localF="<<localF<<endl;
			if(unlikely(scene->trackEnergy)){ Real dissip=Fs.norm()*u0slip.norm(); if(dissip>0) scene->energy->add(dissip,"plastDissip",plastDissipIx,/*reset*/false); }
		}
	}
	if(unlikely(scene->trackEnergy)){ scene->energy->add(0.5*(pow(geom->relU()[0],2)*phys->kn+(pow(geom->relU()[1],2)+pow(geom->relU()[2],2))*phys->ks),"elastPotential",elastPotentialIx,/*reset at every timestep*/true); }
	// apply force: this converts the force to global space, updates NormShearPhys::{normal,shear}Force, applies to particles
	geom->applyLocalForce(localF,I,scene,phys);
}

void Law2_L6Geom_FrictPhys_Linear::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* I){
	L6Geom& geom=ig->cast<L6Geom>(); FrictPhys& phys=ip->cast<FrictPhys>();

	// simple linear relationships
	Vector3r localF=geom.relU().cwise()*Vector3r(phys.kn,phys.ks,phys.ks);
	Vector3r localT=charLen*(geom.relPhi().cwise()*Vector3r(phys.kn,phys.ks,phys.ks));

	geom->applyLocalForceTorque(localF,localT,I,scene,phys);
}

#ifdef YADE_OPENGL
bool Gl1_L3Geom::axesLabels;
Real Gl1_L3Geom::uScale;
Real Gl1_L6Geom::phiScale;

void Gl1_L3Geom::go(const shared_ptr<IGeom>& ig, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool){ draw(ig); }
void Gl1_L6Geom::go(const shared_ptr<IGeom>& ig, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool){ draw(ig,true,phiScale); }

void Gl1_L3Geom::draw(const shared_ptr<IGeom>& ig, bool isL6Geom, const Real& phiScale){
	const L3Geom& g(ig->cast<L3Geom>());
	glTranslatev(g.contactPoint);
	#if EIGEN_MAJOR_VERSION<30
 		glMultMatrixd(Eigen::Transform3d(g.trsf.transpose()).data());
	#else
 		glMultMatrixd(Eigen::Affine3d(g.trsf.transpose()).data());
	#endif
	Real rMin=g.refR1<=0?g.refR2:(g.refR2<=0?g.refR1:min(g.refR1,g.refR2));
	glLineWidth(2.);
	for(int i=0; i<3; i++){
		Vector3r pt=Vector3r::Zero(); pt[i]=.5*rMin; Vector3r color=.3*Vector3r::Ones(); color[i]=1;
		GLUtils::GLDrawLine(Vector3r::Zero(),pt,color);
		if(axesLabels) GLUtils::GLDrawText(string(i==0?"x":(i==1?"y":"z")),pt,color);
	}
	glLineWidth(4.);
	if(uScale!=0) GLUtils::GLDrawLine(Vector3r::Zero(),uScale*g.relU(),Vector3r(0,1,.5));
	if(isL6Geom && phiScale>0) GLUtils::GLDrawLine(Vector3r::Zero(),ig->cast<L6Geom>().relPhi()/Mathr::PI*rMin*phiScale,Vector3r(.8,0,1));
	glLineWidth(1.);
};

#endif
