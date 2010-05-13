// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
#include"ViscoelasticPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((ViscElMat)(ViscElPhys)(Ip2_ViscElMat_ViscElMat_ViscElPhys)(Law2_ScGeom_ViscElPhys_Basic));

/* ViscElMat */
ViscElMat::~ViscElMat(){}

/* ViscElPhys */
ViscElPhys::~ViscElPhys(){}

/* Ip2_ViscElMat_ViscElMat_ViscElPhys */
void Ip2_ViscElMat_ViscElMat_ViscElPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
	if(interaction->interactionPhysics) return;
	ViscElMat* sdec1 = static_cast<ViscElMat*>(b1.get());
	ViscElMat* sdec2 = static_cast<ViscElMat*>(b2.get());
	ViscElPhys* phys = new ViscElPhys();
	phys->kn = sdec1->kn * sdec2->kn / (sdec1->kn + sdec2->kn);
	phys->ks = sdec1->ks * sdec2->ks / (sdec1->ks + sdec2->ks);
	phys->cn = ( (sdec1->cn==0) ? 0 : 1/sdec1->cn ) + ( (sdec2->cn==0) ? 0 : 1/sdec2->cn );
	phys->cs = ( (sdec1->cs==0) ? 0 : 1/sdec1->cs ) + ( (sdec2->cs==0) ? 0 : 1/sdec2->cs );
	if (phys->cn) phys->cn = 1/phys->cn;
	if (phys->cs) phys->cs = 1/phys->cs;
	phys->tangensOfFrictionAngle = std::tan(std::min(sdec1->frictionAngle, sdec2->frictionAngle)); 
	phys->shearForce = Vector3r(0,0,0);
	phys->prevNormal = Vector3r(0,0,0);
	interaction->interactionPhysics = shared_ptr<ViscElPhys>(phys);
}

/* Law2_ScGeom_ViscElPhys_Basic */
void Law2_ScGeom_ViscElPhys_Basic::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody){

	const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
	ViscElPhys& phys=*static_cast<ViscElPhys*>(_phys.get());

	const int id1 = I->getId1();
	const int id2 = I->getId2();
	
	if (geom.penetrationDepth<0) {
		rootBody->interactions->requestErase(id1,id2);
		return;
	}

	const BodyContainer& bodies = *rootBody->bodies;

	const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
	const State& de2 = *static_cast<State*>(bodies[id2]->state.get());

	Vector3r& shearForce = phys.shearForce;

	if (I->isFresh(rootBody)) shearForce=Vector3r(0,0,0);

	const Real& dt = scene->dt;

	Vector3r axis = phys.prevNormal.cross(geom.normal);
	shearForce -= shearForce.cross(axis);
	const Real angle = dt*0.5*geom.normal.dot(de1.angVel + de2.angVel);
	axis = angle*geom.normal;
	shearForce -= shearForce.cross(axis);

	const Vector3r c1x = (geom.contactPoint - de1.pos);
	const Vector3r c2x = (geom.contactPoint - de2.pos);
	/// The following definition of c1x and c2x is to avoid "granular ratcheting" 
	///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
	///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
	///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
	//Vector3r _c1x_	=  geom->radius1*geom->normal;
	//Vector3r _c2x_	= -geom->radius2*geom->normal;
	//Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(_c2x_)) - (de1->velocity+de1->angularVelocity.Cross(_c1x_));
	const Vector3r relativeVelocity = (de1.vel+de1.angVel.cross(c1x)) - (de2.vel+de2.angVel.cross(c2x)) ;
	const Real normalVelocity	= geom.normal.dot(relativeVelocity);
	const Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;
	// As Chiara Modenese suggest, we store the elastic part 
	// and then add the viscous part if we pass the Mohr-Coulomb criterion.
	// See http://www.mail-archive.com/yade-users@lists.launchpad.net/msg01391.html
	shearForce += phys.ks*dt*shearVelocity; // the elastic shear force have a history, but
	Vector3r shearForceVisc = Vector3r::Zero(); // the viscous shear damping haven't a history because it is a function of the instant velocity 

	phys.normalForce = ( phys.kn * geom.penetrationDepth + phys.cn * normalVelocity ) * geom.normal;
	phys.prevNormal = geom.normal;

	const Real maxFs = phys.normalForce.squaredNorm() * std::pow(phys.tangensOfFrictionAngle,2);
	if( shearForce.squaredNorm() > maxFs )
	{
		// Then Mohr-Coulomb is violated (so, we slip), 
		// we have the max value of the shear force, so 
		// we consider only friction damping.
		const Real ratio = Mathr::Sqrt(maxFs) / shearForce.norm();
		shearForce *= ratio;
	} 
	else 
	{
		// Then no slip occurs we consider friction damping + viscous damping.
		shearForceVisc = phys.cs*shearVelocity; 
	}

	const Vector3r f = phys.normalForce + shearForce + shearForceVisc;
	addForce (id1,-f,rootBody);
	addForce (id2, f,rootBody);
	addTorque(id1,-c1x.cross(f),rootBody);
	addTorque(id2, c2x.cross(f),rootBody);
}

