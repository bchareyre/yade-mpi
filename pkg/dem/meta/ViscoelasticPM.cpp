// 2009 © Sergei Dorofeenko <sega@users.berlios.de>
#include"ViscoelasticPM.hpp"
#include<yade/core/State.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

YADE_PLUGIN((SimpleViscoelasticMat)(SimpleViscoelasticPhys)(Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys)(Law2_Spheres_Viscoelastic_SimpleViscoelastic));

/* SimpleViscoelasticMat */
SimpleViscoelasticMat::~SimpleViscoelasticMat(){}

/* SimpleViscoelasticPhys */
SimpleViscoelasticPhys::~SimpleViscoelasticPhys(){}

/* Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys */
void Ip2_SimleViscoelasticMat_SimpleViscoelasticMat_SimpleViscoelasticPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction) {
	if(interaction->interactionPhysics) return;
	SimpleViscoelasticMat* sdec1 = static_cast<SimpleViscoelasticMat*>(b1.get());
	SimpleViscoelasticMat* sdec2 = static_cast<SimpleViscoelasticMat*>(b2.get());
	SimpleViscoelasticPhys* phys = new SimpleViscoelasticPhys();
	phys->kn = sdec1->kn * sdec2->kn / (sdec1->kn + sdec2->kn);
	phys->ks = sdec1->ks * sdec2->ks / (sdec1->ks + sdec2->ks);
	phys->cn = ( (sdec1->cn==0) ? 0 : 1/sdec1->cn ) + ( (sdec2->cn==0) ? 0 : 1/sdec2->cn );
	phys->cs = ( (sdec1->cs==0) ? 0 : 1/sdec1->cs ) + ( (sdec2->cs==0) ? 0 : 1/sdec2->cs );
	if (phys->cn) phys->cn = 1/phys->cn;
	if (phys->cs) phys->cs = 1/phys->cs;
	phys->tangensOfFrictionAngle = std::tan(std::min(sdec1->frictionAngle, sdec2->frictionAngle)); 
	phys->shearForce = Vector3r(0,0,0);
	phys->prevNormal = Vector3r(0,0,0);
	interaction->interactionPhysics = shared_ptr<SimpleViscoelasticPhys>(phys);
}

/* Law2_Spheres_Viscoelastic_SimpleViscoelastic */
void Law2_Spheres_Viscoelastic_SimpleViscoelastic::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, Scene* rootBody){

	const ScGeom& geom=*static_cast<ScGeom*>(_geom.get());
	SimpleViscoelasticPhys& phys=*static_cast<SimpleViscoelasticPhys*>(_phys.get());

	int id1 = I->getId1();
	int id2 = I->getId2();
	
	if (geom.penetrationDepth<0) {
		rootBody->interactions->requestErase(id1,id2);
		return;
	}

	const BodyContainer& bodies = *rootBody->bodies;

	const State& de1 = *static_cast<State*>(bodies[id1]->state.get());
	const State& de2 = *static_cast<State*>(bodies[id2]->state.get());

	Vector3r& shearForce = phys.shearForce;

	if (I->isFresh(rootBody)) shearForce=Vector3r(0,0,0);

	Real dt = Omega::instance().getTimeStep();

	Vector3r axis = phys.prevNormal.Cross(geom.normal);
	shearForce -= shearForce.Cross(axis);
	Real angle = dt*0.5*geom.normal.Dot(de1.angVel + de2.angVel);
	axis = angle*geom.normal;
	shearForce -= shearForce.Cross(axis);

	Vector3r c1x = (geom.contactPoint - de1.pos);
	Vector3r c2x = (geom.contactPoint - de2.pos);
	/// The following definition of c1x and c2x is to avoid "granular ratcheting" 
	///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
	///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
	///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
	//Vector3r _c1x_	=  geom->radius1*geom->normal;
	//Vector3r _c2x_	= -geom->radius2*geom->normal;
	//Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(_c2x_)) - (de1->velocity+de1->angularVelocity.Cross(_c1x_));
	Vector3r relativeVelocity = (de2.vel+de2.angVel.Cross(c2x)) - (de1.vel+de1.angVel.Cross(c1x));
	Real normalVelocity	= geom.normal.Dot(relativeVelocity);
	Vector3r shearVelocity	= relativeVelocity-normalVelocity*geom.normal;
	shearForce -= (phys.ks*dt+phys.cs)*shearVelocity;

	phys.normalForce = ( phys.kn * geom.penetrationDepth - phys.cn * normalVelocity ) * geom.normal;
	phys.prevNormal = geom.normal;

	Real maxFs = phys.normalForce.SquaredLength() * std::pow(phys.tangensOfFrictionAngle,2);
	if( shearForce.SquaredLength() > maxFs )
	{
		maxFs = Mathr::Sqrt(maxFs) / shearForce.Length();
		shearForce *= maxFs;
	}

	Vector3r f = phys.normalForce + shearForce;
	addForce (id1,-f,rootBody);
	addForce (id2, f,rootBody);
	addTorque(id1,-c1x.Cross(f),rootBody);
	addTorque(id2, c2x.Cross(f),rootBody);
}

