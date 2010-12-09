/* Klaus Thoeni 2010  */

#include"WirePM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>

YADE_PLUGIN((WireMat)(WireState)(WirePhys)(Ip2_WireMat_WireMat_WirePhys)(Law2_ScGeom_WirePhys_WirePM));


/********************** WireMat ****************************/
CREATE_LOGGER(WireMat);

void WireMat::postLoad(WireMat&){
	if(StrainStressValues.size() < 2)
		throw invalid_argument("WireMat.StrainStressValues: at least two points must be given.");
	if(StrainStressValues[0](0) == 0. && StrainStressValues[0](1) == 0.)
		throw invalid_argument("WireMat.StrainStressValues: Definition must start with values greather then zero (strain>0,stress>0)");
	// compute cross-sectin area
	As = pow(diameter*0.5,2)*Mathr::PI;

	// debug printout
// 	Vector2r vbegin = *StrainStressValues.begin();
// // 	Vector2r vbegin = StrainStressValues.front();
// 	std::cerr << vbegin(0) << endl;
// 	std::cerr << vbegin(1) << endl;
// 	std::cerr << endl;
// 
// 	Real v0 = (*(StrainStressValues.begin()))(0);
// 	Real v1 = (*StrainStressValues.begin())(1);
// 	std::cerr << v0 << endl;
// 	std::cerr << v1 << endl;

	//BUG ????? postLoad is called twice,
}


/********************** Law2_ScGeom_WirePhys_WirePM ****************************/
CREATE_LOGGER(Law2_ScGeom_WirePhys_WirePM);

void Law2_ScGeom_WirePhys_WirePM::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	WirePhys* phys = static_cast<WirePhys*>(ip.get());
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	Body* b1 = Body::byId(id1,scene).get();
	Body* b2 = Body::byId(id2,scene).get();
	
	Real displN = geom->penetrationDepth; // NOTE: ScGeom->penetrationDepth>0 when spheres interpenetrate, and therefore, for wire always negative

	/* get reference to values since values are updated for unloading */
	vector<Vector2r> &DFValues = phys->DisplForceValues;
	vector<Real> &kValues = phys->StiffnessValues;

	Real D = displN - phys->initD; // interparticular distance is computed depending on the equilibrium distance

	/* check whether the particles are linked or not */
	if ( !phys->isLinked ) { // destroy the interaction before calculation
		scene->interactions->requestErase(contact->getId1(),contact->getId2());
		return;
	}
	if ( (phys->isLinked) && (D < DFValues.back()(0)) ) { // spheres are linked but failure because of reaching maximal admissible displacement 
		phys->isLinked=false; 
		// update body state with the number of broken links
		WireState* st1=dynamic_cast<WireState*>(b1->state.get());
		WireState* st2=dynamic_cast<WireState*>(b2->state.get());
		st1->numBrokenLinks+=1;
		st2->numBrokenLinks+=1;
		scene->interactions->requestErase(contact->getId1(),contact->getId2());
		return;
	}
	
	/* compute normal force Fn */
	Real Fn = 0.;
	if ( D > DFValues[0](0) )
		Fn = kValues[0] * (D-phys->Dplast);
	else {
		bool isDone = false;
		unsigned int i = 0;
		while (!isDone && i < DFValues.size()) { 
			i++;
			if ( D > DFValues[i](0) ) {
				Fn = DFValues[i-1](1) + (D-DFValues[i-1](0))*kValues[i];
				phys->Dplast = D - Fn/kValues[0];
				isDone = true;
				// update values for unloading
				DFValues[0](0) = D;
				DFValues[0](1) = Fn;
			}
		}
	}
	
	TRVAR3( displN, D, Fn );

	/* compression forces cannot be applied to wires */
	if (Fn > 0.) Fn = 0.;
	
	phys->normalForce = Fn*geom->normal; // NOTE: normal is position2-position1 - It is directed from particle1 to particle2
	        
	State* st1 = Body::byId(id1,scene)->state.get();
	State* st2 = Body::byId(id2,scene)->state.get();
	
	/* apply forces */
	Vector3r f = phys->normalForce;
	// these lines to adapt to periodic boundary conditions
	if ( !scene->isPeriodic )  
		applyForceAtContactPoint(f , geom->contactPoint , id2, st2->se3.position, id1, st1->se3.position);
	else { // in scg we do not wrap particles positions, hence "applyForceAtContactPoint" cannot be used when scene is periodic
		scene->forces.addForce(id1,-f);
		scene->forces.addForce(id2,f);
	}

}

CREATE_LOGGER(Ip2_WireMat_WireMat_WirePhys);

void Ip2_WireMat_WireMat_WirePhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	
	/* avoid any updates if interactions which already exist */
	if(interaction->phys) return; 
	
	ScGeom* geom=dynamic_cast<ScGeom*>(interaction->geom.get());
	assert(geom);

	/* set equilibrium distance, e.g. initial distance between particle (stress free state) */
	shared_ptr<WirePhys> contactPhysics(new WirePhys()); 
	contactPhysics->initD = geom->penetrationDepth;
	contactPhysics->normalForce = Vector3r::Zero();

	/* get values from material */
	const shared_ptr<WireMat>& mat1 = YADE_PTR_CAST<WireMat>(b1);
	const shared_ptr<WireMat>& mat2 = YADE_PTR_CAST<WireMat>(b2);

	Real crossSection;
	vector<Vector2r> SSValues;
	
	/* ckeck properties of interaction */
	if ( mat1->id == mat2->id ) { // interaction of two bodies of the same material
		crossSection = mat1->As;
		SSValues = mat1->StrainStressValues;
		if ( (mat1->isDoubleTwist) && (abs(interaction->getId1()-interaction->getId2())==1) ) // bodies which id differs by 1 are double twisted
			contactPhysics->isDoubleTwist = true;
		else
			contactPhysics->isDoubleTwist = false;
	}
	else { // interaction of two bodies of two different materials, take weaker material and no double-twist
		contactPhysics->isDoubleTwist = false;
		if ( mat1->diameter <= mat2->diameter){
			crossSection = mat1->As;
			SSValues = mat1->StrainStressValues;
		}
		else {
			crossSection = mat2->As;
			SSValues = mat2->StrainStressValues;
		}
	}
	
	Real R1 = geom->radius1;
	Real R2 = geom->radius2;
	
	Real l0 = R1 + R2 - contactPhysics->initD; // initial lenght of the wire (can be single or double twisted)

	/* compute displscement-force values (tension negative since ScGem is used!) */
	vector<Vector2r> DFValues;
	for ( vector<Vector2r>::iterator it = SSValues.begin(); it != SSValues.end(); it++ ) {
		Vector2r values = Vector2r::Zero();
		values(0) = -(*it)(0)*l0;
		values(1) = -(*it)(1)*crossSection;
		DFValues.push_back(values);
	}

	/* compute elastic stiffness of single wire */
	vector<Real> kValues;
	Real k = DFValues[0](1) / DFValues[0](0);

	/* update values if the interaction is double twiseted */
	if ( contactPhysics->isDoubleTwist ) {
		Real alpha = atan( l0 / (3.*Mathr::PI*mat1->diameter) );
		Real kh = k * ( l0*mat1->diameter/crossSection ) / ( 48.*cos(alpha) * ( 41./9.*(1.+mat1->poisson) + 17./4.*pow(tan(alpha),2) ) );
		k = 2. * ( mat1->lambdak*kh + (1-mat1->lambdak)*k );
		Real F = k * DFValues[0](0);
		Real mappingF = F/DFValues[0](1);
		DFValues[0](1) = F;
		for (unsigned int i = 1; i<DFValues.size(); i++) {
			DFValues[i](0) *= mat1->lambdaEps;
			DFValues[i](1) *= mappingF;
		}
	}
	
	/* store displscement-force values in physics */
	contactPhysics->DisplForceValues = DFValues;

	/* compute stiffness-values of wire */
	contactPhysics->kn = k;
	kValues.push_back(k);
	for( unsigned int i = 1 ; i < DFValues.size()-1; i++ ) {
		Real deltau = -DFValues[i](0) + DFValues[i-1](0);
		Real deltaF = -DFValues[i](1) + DFValues[i-1](1);
		k = deltaF/deltau;
		kValues.push_back(k);
	}
	contactPhysics->StiffnessValues = kValues;

	/* set particles as linked */
	if ( (scene->iter < linkThresholdIteration))
		contactPhysics->isLinked=true;
	else
		contactPhysics->isLinked=false;

	interaction->phys = contactPhysics;

}

WirePhys::~WirePhys(){}
