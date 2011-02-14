
#include<yade/pkg/dem/SpheresFactory.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>


//YADE_PLUGIN((SpheresFactory)(DragForceApplier));
YADE_PLUGIN((SpheresFactory)(CircularFactory)(BoxFactory)(DragForceApplier));
CREATE_LOGGER(SpheresFactory);
CREATE_LOGGER(CircularFactory);
CREATE_LOGGER(BoxFactory);

void DragForceApplier::action(){
	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if(!b) continue;
		Sphere* sphere=dynamic_cast<Sphere*>(b->shape.get());
		if(!sphere) continue;
		Vector3r dragForce=-b->state->vel.normalized()*(1/2.)*density*b->state->vel.squaredNorm()*.47*Mathr::PI*pow(sphere->radius,2);
		scene->forces.addForce(b->id,dragForce);
	}
}

// initialize random number generator with time seed
static boost::minstd_rand randGen(TimingInfo::getNow(/* get the number even if timing is disabled globally */ true));
static boost::variate_generator<boost::minstd_rand&, boost::uniform_real<Real> > randomUnit(randGen, boost::uniform_real<Real>(0,1));

void SpheresFactory::pickRandomPosition(Vector3r&,Real){
	LOG_FATAL("Engine "<<getClassName()<<" calling virtual method SpheresFactory::pickRandomPosition(), but had to call derived class. This could occur if you use SpheresFactory directly instead derived engines. If not, please submit bug report at http://bugs.launchpad.net/yade.");
	throw std::logic_error("SpheresFactory::pickRandomPosition() called.");
}

void SpheresFactory::action(){

	if(!collider){
		FOREACH(const shared_ptr<Engine>& e, scene->engines){ collider=dynamic_pointer_cast<Collider>(e); if(collider) break; }
		if(!collider) throw runtime_error("SpheresFactory: No Collider instance found in engines (needed for collision detection).");
	}
	goalMass+=massFlowRate*scene->dt; // totalMass that we want to attain in the current step

	normal.normalize();

	LOG_TRACE("totalMass="<<totalMass<<", goalMass="<<goalMass);

	while(totalMass<goalMass && (maxParticles<0 || numParticles<maxParticles)){
		// pick random radius (later: based on the particle size distribution)
		Real r=rMin+randomUnit()*(rMax-rMin);
		LOG_TRACE("Radius "<<r);
		Vector3r c=Vector3r::Zero();
		// until there is no overlap, pick a random position for the new particle
		int attempt;
		for(attempt=0; attempt<maxAttempt; attempt++){
			pickRandomPosition(c,r);
			LOG_TRACE("Center "<<c);
			Bound b; b.min=c-Vector3r(r,r,r); b.max=c+Vector3r(r,r,r);
			vector<Body::id_t> collidingParticles=collider->probeBoundingVolume(b);
			if(collidingParticles.size()==0) break;
			#ifdef YADE_DEBUG
				FOREACH(const Body::id_t& id, collidingParticles) LOG_TRACE(scene->iter<<":"<<attempt<<": collision with #" <<id);
			#endif
		}
		if(attempt==maxAttempt) {
			if (silent) {massFlowRate=0; goalMass=totalMass; LOG_INFO("Unable to place new sphere after "<<maxAttempt<<" attempts, SpheresFactory disabled.");} 
			else {LOG_WARN("Unable to place new sphere after "<<maxAttempt<<" attempts, giving up.");}
			return;
		}
		// pick random initial velocity (normal with some variation)
		// preliminary version that randomizes valocity magnitude but always makes initVel exactly aligned with normal
		Vector3r initVel=normal*(vMin+randomUnit()*(vMax-vMin)); // TODO: compute from vMin, vMax, vAngle, normal;
		//Vector3r initVel= normal*100;

		// create particle
		int mId=(materialId>=0 ? materialId : scene->materials.size()+materialId);
		if(mId<0 || (size_t) mId>=scene->materials.size()) throw std::invalid_argument(("SpheresFactory: invalid material id "+lexical_cast<string>(materialId)).c_str());
		const shared_ptr<Material>& material=scene->materials[mId];
		shared_ptr<Body> b(new Body);
		shared_ptr<Sphere> sphere(new Sphere); 
		shared_ptr<State> state(material->newAssocState());
		sphere->radius=r;  
		state->pos=state->refPos=c; 

		state->vel=initVel; 
		Real vol=(4/3.)*Mathr::PI*pow(r,3);
		state->mass=vol*material->density;
		state->inertia=(2./5.)*vol*r*r*material->density*Vector3r::Ones();
		state->blockedDOFs_vec_set(blockedDOFs);

		b->shape=sphere; 
		b->state=state; 
		b->material=material;
		// insert particle in the simulation
		scene->bodies->insert(b);
		// increment total mass we've spit out
		totalMass+=state->mass;
		numParticles++;
		
	} 
	//std::cout<<"mass flow rate: "<<totalMass<<endl;totalMass =0.0;
};

void CircularFactory::pickRandomPosition(Vector3r& c, Real r){
	const Quaternionr q(Quaternionr().setFromTwoVectors(Vector3r::UnitZ(),normal));
	Real angle=randomUnit()*2*Mathr::PI, rr=randomUnit()*(radius-r); // random polar coordinate inside the circle
	Real l=(randomUnit()-0.5)*length;
	c = center+q*Vector3r(cos(angle)*rr,sin(angle)*rr,0)+normal*l;
}

void BoxFactory::pickRandomPosition(Vector3r& c, Real r){
	const Quaternionr q(Quaternionr().setFromTwoVectors(Vector3r::UnitZ(),normal));
	//c=center+q*Vector3r((randomUnit()-.5)*2*(extents[0]-r),(randomUnit()-.5)*2*(extents[1]-r),(randomUnit()-.5)*2*(extents[2]-r));
	c=center+q*Vector3r((randomUnit()-.5)*2*(extents[0]),(randomUnit()-.5)*2*(extents[1]),(randomUnit()-.5)*2*(extents[2]));
}
