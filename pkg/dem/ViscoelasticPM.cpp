// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
#include"ViscoelasticPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/pkg/common/Sphere.hpp>

YADE_PLUGIN((ViscElMat)(ViscElPhys)(Ip2_ViscElMat_ViscElMat_ViscElPhys)(Law2_ScGeom_ViscElPhys_Basic));

/* ViscElMat */
ViscElMat::~ViscElMat(){}

/* ViscElPhys */
ViscElPhys::~ViscElPhys(){}

/* Ip2_ViscElMat_ViscElMat_ViscElPhys */
void Ip2_ViscElMat_ViscElMat_ViscElPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
	// no updates of an existing contact 
	if(interaction->phys) return;
	ViscElPhys * phys = Calculate_ViscElMat_ViscElMat_ViscElPhys(b1, b2, interaction);
	interaction->phys = shared_ptr<ViscElPhys>(phys);
}

/* Law2_ScGeom_ViscElPhys_Basic */
void Law2_ScGeom_ViscElPhys_Basic::go(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I) {
	Vector3r force = Vector3r::Zero();
	Vector3r torque1 = Vector3r::Zero();
	Vector3r torque2 = Vector3r::Zero();
	computeForceTorqueViscEl(_geom, _phys, I, force, torque1, torque2);
	if (I->isActive) {
		const int id1 = I->getId1();
		const int id2 = I->getId2();
		
		addForce (id1,-force,scene);
		addForce (id2, force,scene);
		addTorque(id1, torque1,scene);
		addTorque(id2, torque2,scene);
  }
}

void computeForceTorqueViscEl(shared_ptr<IGeom>& _geom, shared_ptr<IPhys>& _phys, Interaction* I, Vector3r & force, Vector3r & torque1, Vector3r & torque2) {
	const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
	Scene* scene=Omega::instance().getScene().get();
	ViscElPhys& phys=*static_cast<ViscElPhys*>(_phys.get());

	const int id1 = I->getId1();
	const int id2 = I->getId2();

	const BodyContainer& bodies = *scene->bodies;

	const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
	const State& de2 = *static_cast<State*>(bodies[id2]->state.get());

	Vector3r& shearForce = phys.shearForce;
	if (I->isFresh(scene)) shearForce=Vector3r(0,0,0);
	const Real& dt = scene->dt;
	shearForce = geom.rotate(shearForce);

	// Handle periodicity.
	const Vector3r shift2 = scene->isPeriodic ? scene->cell->intrShiftPos(I->cellDist): Vector3r::Zero(); 
	const Vector3r shiftVel = scene->isPeriodic ? scene->cell->intrShiftVel(I->cellDist): Vector3r::Zero(); 

	const Vector3r c1x = (geom.contactPoint - de1.pos);
	const Vector3r c2x = (geom.contactPoint - de2.pos - shift2);
	
	const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) + shiftVel;
	const Real normalVelocity	= geom.normal.dot(relativeVelocity);
	const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;
	
	// As Chiara Modenese suggest, we store the elastic part 
	// and then add the viscous part if we pass the Mohr-Coulomb criterion.
	// See http://www.mail-archive.com/yade-users@lists.launchpad.net/msg01391.html
	shearForce += phys.ks*dt*shearVelocity; // the elastic shear force have a history, but
	Vector3r shearForceVisc = Vector3r::Zero(); // the viscous shear damping haven't a history because it is a function of the instant velocity 


	// Prevent appearing of attraction forces due to a viscous component
	// [Radjai2011], page 3, equation [1.7]
	// [Schwager2007]
	const Real normForceReal = phys.kn * geom.penetrationDepth + phys.cn * normalVelocity;
	if (normForceReal < 0) {
		phys.normalForce = Vector3r::Zero();
	} else {
		phys.normalForce = normForceReal * geom.normal;
	}
	
	Vector3r momentResistance = Vector3r::Zero();
	if (phys.mR>0.0) {
		const Vector3r relAngVel  = de1.angVel - de2.angVel;
		relAngVel.normalized();
		
		if (phys.mRtype == 1) { 
			momentResistance = -phys.mR*phys.normalForce.norm()*relAngVel;																														// [Zhou1999536], equation (3)
		} else if (phys.mRtype == 2) { 
			momentResistance = -phys.mR*(c1x.cross(de1.angVel) - c2x.cross(de2.angVel)).norm()*phys.normalForce.norm()*relAngVel;			// [Zhou1999536], equation (4)
		}
	}
	
	const Real maxFs = phys.normalForce.squaredNorm() * std::pow(phys.tangensOfFrictionAngle,2);
	if( shearForce.squaredNorm() > maxFs )
	{
		// Then Mohr-Coulomb is violated (so, we slip), 
		// we have the max value of the shear force, so 
		// we consider only friction damping.
		const Real ratio = sqrt(maxFs) / shearForce.norm();
		shearForce *= ratio;
	} 
	else 
	{
		// Then no slip occurs we consider friction damping + viscous damping.
		shearForceVisc = phys.cs*shearVelocity; 
	}
	force = phys.normalForce + shearForce + shearForceVisc;
	torque1 = -c1x.cross(force)+momentResistance;
	torque2 =  c2x.cross(force)-momentResistance;
}

ViscElPhys* Calculate_ViscElMat_ViscElMat_ViscElPhys(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
	ViscElPhys* phys = new ViscElPhys();
	
	ViscElMat* mat1 = static_cast<ViscElMat*>(b1.get());
	ViscElMat* mat2 = static_cast<ViscElMat*>(b2.get());
	Real mass1 = 1.0;
	Real mass2 = 1.0;
	
	if (mat1->massMultiply and mat2->massMultiply) {
		mass1 = Body::byId(interaction->getId1())->state->mass;
		mass2 = Body::byId(interaction->getId2())->state->mass;
		if (mass1 == 0.0 and mass2 > 0.0) {
			mass1 = mass2;
		} else if (mass2 == 0.0 and mass1 > 0.0) {
			mass2 = mass1;
		}
	}
	
	GenericSpheresContact* sphCont=YADE_CAST<GenericSpheresContact*>(interaction->geom.get());
	Real R1=sphCont->refR1>0?sphCont->refR1:sphCont->refR2;
	Real R2=sphCont->refR2>0?sphCont->refR2:sphCont->refR1;
	
	const Real kn1 = isnan(mat1->kn)?2*mat1->young*R1:mat1->kn*mass1;
	const Real cn1 = mat1->cn*mass1;
	const Real ks1 = isnan(mat1->ks)?kn1*mat1->poisson:mat1->ks*mass1;
	const Real cs1 = mat1->cs*mass1;
	
	const Real mR1 = mat1->mR;      const Real mR2 = mat2->mR; 
	const int mRtype1 = mat1->mRtype; const int mRtype2 = mat2->mRtype;
	
	const Real kn2 = isnan(mat2->kn)?2*mat2->young*R2:mat2->kn*mass2;
	const Real cn2 = mat2->cn*mass2;
	const Real ks2 = isnan(mat2->ks)?kn2*mat2->poisson:mat2->ks*mass2;
	const Real cs2 = mat2->cs*mass2;
	

	phys->kn = contactParameterCalculation(kn1,kn2, mat1->massMultiply&&mat2->massMultiply);
	phys->ks = contactParameterCalculation(ks1,ks2, mat1->massMultiply&&mat2->massMultiply);
	phys->cn = contactParameterCalculation(cn1,cn2, mat1->massMultiply&&mat2->massMultiply);
	phys->cs = contactParameterCalculation(cs1,cs2, mat1->massMultiply&&mat2->massMultiply);

 	if ((mR1>0) or (mR2>0)) {
		phys->mR = 2.0/( ((mR1>0)?1/mR1:0) + ((mR2>0)?1/mR2:0) );
	} else {
		phys->mR = 0;
	}

	phys->tangensOfFrictionAngle = std::tan(std::min(mat1->frictionAngle, mat2->frictionAngle)); 
	phys->shearForce = Vector3r(0,0,0);
	
	if ((mRtype1 != mRtype2) or (mRtype1>2) or (mRtype2>2) or (mRtype1<1) or (mRtype2<1) ) {
		throw runtime_error("mRtype should be equal for both materials and have the values 1 or 2!");
	} else {
		phys->mRtype = mRtype1;
	}
	
	return phys;
}

/* Contact parameter calculation function */
Real contactParameterCalculation(const Real& l1, const Real& l2, const bool& massMultiply){
  if (massMultiply) {
    // If one of paramaters > 0. we DO NOT return 0
    Real a = (l1?1/l1:0) + (l2?1/l2:0);
    if (a) return 1/a;
    else return 0;
  } else { 
   // If one of paramaters > 0, we return 0
   return (l1>0 or l2>0)?l1*l2/(l1+l2):0;
  }
}

