// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"Brefcom.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/lib-QGLViewer/qglviewer.h>

/*
#include<boost/foreach.hpp>
#include<boost/range.hpp>
namespace boost {
	template<> struct range_iterator<InteractionContainer>{typedef shared_ptr<Interaction> type;};
	template<> struct range_const_iterator<InteractionContainer>{typedef shared_ptr<Interaction> type;};
	template<> struct range_size<InteractionContainer>{typedef unsigned int type;};
}
inline shared_ptr<Interaction> boost_range_begin(InteractionContainer& c){return *(c.begin());}
//inline shared_ptr<Interaction> boost_range_begin(const InteractionContainer& c){return *(c.begin());}
inline shared_ptr<Interaction> boost_range_end(InteractionContainer& c){return *(c.end());}
//inline shared_ptr<Interaction> boost_range_end(const InteractionContainer& c){return *(c.end());}
inline boost::range_size<InteractionContainer>::type boost_range_size(InteractionContainer& c){return c.size();}
*/


YADE_PLUGIN("BrefcomMakeContact","BrefcomContact","BrefcomLaw","GLDrawBrefcomContact","BrefcomDamageColorizer" /* ,"BrefcomStiffnessCounter"*/ );

/********************** BrefcomMakeContact ****************************/
CREATE_LOGGER(BrefcomMakeContact);


//! @todo Formulas in the following should be verified
void BrefcomMakeContact::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){

	const shared_ptr<SpheresContactGeometry>& contGeom=dynamic_pointer_cast<SpheresContactGeometry>(interaction->interactionGeometry);
	assert(contGeom); // for now, don't handle anything other than SpheresContactGeometry

	if(!interaction->isNew && interaction->interactionPhysics){
		const shared_ptr<BrefcomContact> contPhys=dynamic_pointer_cast<BrefcomContact>(interaction->interactionPhysics);
		assert(contPhys);
		contPhys->prevNormal=contGeom->normal;
	} else {
		interaction->isNew; // just in case
		//TRACE;

		const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
		const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

		Real E12=2*elast1->young*elast2->young/(elast1->young+elast2->young); // harmonic Young's modulus average
		Real nu12=2*elast1->poisson*elast2->poisson/(elast1->poisson+elast2->poisson); // dtto for Poisson ratio 
		Real S12=Mathr::PI*pow(min(contGeom->radius1,contGeom->radius2),2); // "surface" of interaction
		//Real d0=contGeom->radius1 + contGeom->radius2; // equilibrium distace is "just touching"
		Real d0=(elast1->se3.position-elast2->se3.position).Length(); // equilibrium distance is the initial contact distance
		Real E=(E12 /* was here for Kn:  *S12/d0  */)*((1+alpha)/(beta*(1+nu12)+gamma*(1-alpha*nu12)));

		/* recommend default values for parameters
		 * propose ways to determine them exactly
		 * */
		shared_ptr<BrefcomContact> contPhys(new BrefcomContact(
			/* E */ E,
			/* G */ E*(1-alpha*nu12)/(1+nu12),
			/* tanFrictionAngle */ tan(.5*(elast1->frictionAngle+elast2->frictionAngle)),
			/* undamagedCohesion */ S12*sigmaT,
			/* equilibriumDist */ d0,
			/* crossSection */ S12,
			/* epsCracking */ sigmaT/E12,
			/* epsFracture */ 5*sigmaT/E12,
			/* expBending */ expBending,
			/* xiShear*/ 0
			));
		contPhys->prevNormal=contGeom->normal;
		if(cohesiveThresholdIter<0 || Omega::instance().getCurrentIteration()<cohesiveThresholdIter) contPhys->isCohesive=true;
		else contPhys->isCohesive=false;

		interaction->interactionPhysics=contPhys;
	}
}




/********************** BrefcomContact ****************************/
CREATE_LOGGER(BrefcomContact);



/********************** BrefcomLaw ****************************/
CREATE_LOGGER(BrefcomLaw);


/*! Damage evolution law (static version: all parameters are explicitly passed).
 *
 * This function is zero for any eps<=epsCrackOnset (no damage before the material starts cracking),
 * therefore it will be linear-elastic. Between epsCrackOnset and espFracture (complete damage), it
 * is a non-decreasing function (an exp curve in this case, controlled by a single parameter expBending).
 * For any exp>=expFracture, return 1 (complete damage).
 *
 * @param kappaD maximum positive strain so far
 * @param expBending determines whether the function is bent up or down (and how much)
 * @param epsCrackOnsert strain at which material begins to behave non-linearly
 * @param epsFracture strain at which material is fully damaged
 */
Real BrefcomLaw::damageEvolutionLaw_static(Real kappaD, Real expBending, Real epsCrackOnset, Real epsFracture){
	//double g(double x, double c, double eps0, double eps1){
	if(kappaD<epsCrackOnset)return 0;
	if(kappaD>epsFracture)return 1;
	return (1/(1-exp(-expBending)))*(1-exp(-expBending*(kappaD-epsCrackOnset)/(epsFracture-epsCrackOnset)));
}

/*! Compute fracture energy by numerically integrating damageEvolutionLaw_static, using unitary stiffness.
 *
 * The value returned must be multiplied by E to obtain real fracture energy: Fracture energy
 *    Gf=integral(E*(1-damage(eps))*eps)=E*integral(...) and we compute just the integral(...) part.
 * The integration uses trapezoidal rule. All parameters except steps have the same
 * meaning as for damageEvolutionLaw_static.
 *
 * @param steps Number of subdivision intervals for integration.
 */

Real BrefcomLaw::unitFractureEnergy(double expBending, double epsCrackOnset, double epsFracture, int steps /*=50*/){
	assert(steps>=10);
	const double lo=0, hi=epsFracture;
	double sum=0,stepSize=(hi-lo)/steps,x1,x2;
	for(int i=0; i<steps; i++){
		x1=lo+i*stepSize; x2=x1+stepSize;
		sum+=((1-damageEvolutionLaw_static(x1,expBending,epsCrackOnset,epsFracture))*x1+(1-damageEvolutionLaw_static(x2,expBending,epsCrackOnset,epsFracture))*x2)*.5*stepSize; /* trapezoid rule */
	}
	return sum;
}

/*! Calibrate epsFracture so that the desired fracture energy is obtained, while other parameters are keps constant.
 *
 * The iteration relies on the fact that fractureEnergy is increasing for increasing epsFracture;
 * first we find some strain value eps at which fractureEnergy(eps,...)<Gf<fractureEnergy(2*eps,...),
 * then the interval is bisected until fractureEnergy matches Gf with desired precision.
 *
 * @param Gf the desired fracture energy.
 * @param relEpsilon relative (with regards to Gf) result precision.
 * @param maxIter number of iterations after which we throw exception, since for a reason we don't converge or converge too slowly.
 *
 * Other params have the same meaning as for damageEvolutionLaw_static.
 */
Real BrefcomLaw::calibrateEpsFracture(double Gf, double E, double expBending, double epsCrackOnset, double epsFractureInit /*=1e-3*/, double relEpsilon /*=1e-3*/, int maxIter /*=1000*/){
	double E1,E2,Emid,epsLo=epsFractureInit,epsHi=epsFractureInit*2,epsMid,iter=0,Gf_div_E=Gf/E;
	bool goUp=unitFractureEnergy(expBending,epsCrackOnset,epsHi)<Gf_div_E; // do we double up or down when finding margins?
	do { epsLo*=goUp?2:.5; epsHi=2*epsLo;
		E1=unitFractureEnergy(expBending,epsCrackOnset,epsLo); E2=unitFractureEnergy(expBending,epsCrackOnset,epsHi);
		if((iter++)>maxIter) throw runtime_error("Convergence problem when finding margin values for bisection.");
	} while(!(E1<Gf_div_E && E2>=Gf_div_E));
	// now E(epsLo)<Gf_div_E<=E(epsHi); go ahead using interval bisection
	do {
		epsMid=.5*(epsLo+epsHi); Emid=unitFractureEnergy(expBending,epsCrackOnset,epsMid);
		if(Emid<Gf_div_E)epsLo=.5*(epsLo+epsHi);
		else if(Emid>Gf_div_E)epsHi=.5*(epsLo+epsHi);
		if((iter++)>maxIter) throw runtime_error("Convergence problem during bisection (relEpsilon too low?).");
	} while (abs(Emid-Gf_div_E)>relEpsilon*Gf_div_E);
	return epsMid;
}

void BrefcomLaw::applyForce(const Vector3r force){
	static_pointer_cast<Force>(rootBody->physicalActions->find(id1,ForceClassIndex))->force+=force;
	static_pointer_cast<Force>(rootBody->physicalActions->find(id2,ForceClassIndex))->force-=force;
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id1,MomentumClassIndex))->momentum+=(contGeom->contactPoint-rbp1->se3.position).Cross(force);
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id2,MomentumClassIndex))->momentum-=(contGeom->contactPoint-rbp2->se3.position).Cross(force);
}

void BrefcomLaw::action(MetaBody* _rootBody){
	rootBody=_rootBody;
	
	//BOOST_FOREACH(shared_ptr<Interaction> II,*(rootBody->transientInteractions)){}
	
	list<shared_ptr<Interaction> > interactionsToBeDeleted;
	
	InteractionContainer::iterator transientEnd=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=transientEnd; ++I){
		if(!(*I)->isReal) { /*LOG_DEBUG("Skipped unreal interaction of "<<(*I)->getId1()<<"+"<<(*I)->getId2());*/ continue; }
		//TRACE;
		// initialize temporaries
		id1=(*I)->getId1(); id2=(*I)->getId2();
		body1=Body::byId(id1); body2=Body::byId(id2);
		assert(body1); assert(body2);
		BC=YADE_PTR_CAST<BrefcomContact>((*I)->interactionPhysics);
		contGeom=YADE_PTR_CAST<SpheresContactGeometry>((*I)->interactionGeometry);
		rbp1=YADE_PTR_CAST<RigidBodyParameters>(body1->physicalParameters);
		rbp2=YADE_PTR_CAST<RigidBodyParameters>(body2->physicalParameters);
		assert(BC); assert(contGeom); assert(rbp1); assert(rbp2);
		
		#ifdef BREFCOM_REC
			recValues.clear(); recLabels.clear();
			BREFREC2(Omega::instance().getCurrentIteration(),"iter");
			BREFREC(id1);
			BREFREC(id2);
			BREFREC2(rbp1->se3.position[0],"x1");
			BREFREC2(rbp1->se3.position[1],"y1");
			BREFREC2(rbp1->se3.position[2],"z1");
			BREFREC2(rbp2->se3.position[0],"x2");
			BREFREC2(rbp2->se3.position[1],"y2");
			BREFREC2(rbp2->se3.position[2],"z2");
			BREFREC2(BC->omega,"ω_old");
		#endif

		// shorthands
		Real& epsN(BC->epsN); Vector3r& epsT(BC->epsT); Real& kappaD(BC->kappaD); const Real& equilibriumDist(BC->equilibriumDist); const Real& xiShear(BC->xiShear); const Real& E(BC->E); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& tanFrictionAngle(BC->tanFrictionAngle); const Real& G(BC->G); const Real& crossSection(BC->crossSection); Real& omega(BC->omega); Real& sigmaN(BC->sigmaN); 

		Real dist=(rbp1->se3.position-rbp2->se3.position).Length();

		/* FIXME dist>0 doesn't take into account interaction radius!!! */
		// /* TODO: recover non-cohesive contact deletion: */
		if(!BC->isCohesive && dist>0){ /* delete this interaction later */ /* (*I)->isReal=false; */ continue; }

		/* rotate epsT to the new contact plane */
			const Real& dt=Omega::instance().getTimeStep();
			// rotation of the contact normal
			epsT+=BC->epsT.Cross(BC->prevNormal.Cross(contGeom->normal));
			// mutual rotation
			Real angle=dt*.5*contGeom->normal.Dot(rbp1->angularVelocity+rbp2->angularVelocity); /* broken, assumes equal radii */
			epsT+=epsT.Cross(angle*contGeom->normal);

		/* calculate tangential strain increment */
			Vector3r relVelocity /* at the contact point */ = 
				rbp1->velocity-rbp2->velocity
				+rbp1->angularVelocity.Cross(contGeom->contactPoint-rbp1->se3.position)
				-rbp2->angularVelocity.Cross(contGeom->contactPoint-rbp2->se3.position);
			Vector3r tangentialDisplacement=dt*(relVelocity-contGeom->normal.Dot(relVelocity)*contGeom->normal);
		epsT+=tangentialDisplacement/dist;
		
		/* normal strain */
		epsN=(dist-equilibriumDist)/equilibriumDist;

		Vector3r sigmaT;

		#ifdef BREFCOM_MATERIAL_MODEL
			BREFCOM_MATERIAL_MODEL
		#else
			sigmaN=E*epsN;
			sigmaT=G*epsT;
		#endif

		//sigmaT=Vector3r::ZERO;
		//if(BC->omega==1){TRVAR5(equilibriumDist,dist,epsN,kappaD,BC->epsFracture);}

		// store Fn (and Fs?), for use with BrefcomStiffnessCounter
		Fn=sigmaN*crossSection;
		applyForce(crossSection*(contGeom->normal*sigmaN + sigmaT)); /* this is the force applied on the _first_ body */

		#ifdef BREFCOM_REC
			/*BREFREC(epsN); BREFREC(epsT[0]);BREFREC(epsT[1]);BREFREC(epsT[2]);*/
			BREFREC(epsN); BREFREC(Fn);
			BREFREC2(BC->omega,"ω_new");
			for(size_t i=0; i<recValues.size(); i++) recStream<<recLabels[i]<<": "<<recValues[i]<<" ";
			recStream<<endl;
		#endif
	}
	// delete interactions that were requested to be deleted.
	//for(list<shared_ptr<Interaction> >::iterator I=interactionsToBeDeleted.begin(); I!=interactionsToBeDeleted.end(); I++){ rootBody->transientInteractions->erase((*I)->getId1(),(*I)->getId2()); }
}


/********************** GLDrawBrefcomContact ****************************/

#include<yade/lib-opengl/OpenGLWrapper.hpp>

CREATE_LOGGER(GLDrawBrefcomContact);

bool GLDrawBrefcomContact::contactLine=true;
bool GLDrawBrefcomContact::dmgLabel=true;
bool GLDrawBrefcomContact::epsT=false;
bool GLDrawBrefcomContact::epsTAxes=false;
bool GLDrawBrefcomContact::normal=false;


void GLDrawBrefcomContact::go(const shared_ptr<InteractionPhysics>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
	const shared_ptr<BrefcomContact>& BC=static_pointer_cast<BrefcomContact>(ip);

	/* shared_ptr<Sphere> s1(dynamic_pointer_cast<Sphere>(b1->geometricalModel)), s2(dynamic_pointer_cast<Sphere>(b2->geometricalModel));
	if(s1 && s2){
		Real radius=min(s1->radius,s2->radius), len=(b1->physicalParameters->se3.position-b2->physicalParameters->se3.position).Length();
		glScale(len,radius,radius);
		glutSolidCube(1);
	} */

	if(contactLine) Shop::GLDrawLine(b1->physicalParameters->se3.position,b2->physicalParameters->se3.position,Vector3r(BC->omega,1-BC->omega,0.0) /* damaged links red, undamaged green */ );
	if(dmgLabel){ Shop::GLDrawNum(BC->omega,0.5*(b1->physicalParameters->se3.position+b2->physicalParameters->se3.position),Vector3r(BC->omega,1-BC->omega,0.)); }
	const Vector3r& cp=static_pointer_cast<SpheresContactGeometry>(i->interactionGeometry)->contactPoint;
	if(epsT){
		Real maxShear=(BC->undamagedCohesion-BC->sigmaN*BC->tanFrictionAngle)/BC->G;
		Real relShear=BC->epsT.Length()/maxShear;
		Real scale=.5*BC->equilibriumDist;
		Vector3r dirShear=BC->epsT; dirShear.Normalize();
		if(epsTAxes){
			Shop::GLDrawLine(cp-Vector3r(scale,0,0),cp+Vector3r(scale,0,0));
			Shop::GLDrawLine(cp-Vector3r(0,scale,0),cp+Vector3r(0,scale,0));
			Shop::GLDrawLine(cp-Vector3r(0,0,scale),cp+Vector3r(0,0,scale));
		}
		Shop::GLDrawArrow(cp,cp+dirShear*relShear*scale,Vector3r(1.,0.,0.));
		Shop::GLDrawLine(cp+dirShear*relShear*scale,cp+dirShear*scale,Vector3r(.3,.3,.3));

		/* normal strain */ Shop::GLDrawArrow(cp,cp+BC->prevNormal*(BC->epsN/maxShear),Vector3r(0.,1.,0.));
	}
	//if(normal) Shop::GLDrawArrow(cp,cp+BC->prevNormal*.5*BC->equilibriumDist,Vector3r(0.,1.,0.));
}

/********************** BrefcomDamageColorizer ****************************/
void BrefcomDamageColorizer::action(MetaBody* rootBody){
	vector<pair<short,Real> > bodyDamage; /* number of cohesive interactions per body; cummulative damage of interactions */
	bodyDamage.resize(rootBody->bodies->size(),pair<short,Real>(0,0));
	InteractionContainer::iterator transientEnd=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=transientEnd; ++I){
		shared_ptr<BrefcomContact> BC=dynamic_pointer_cast<BrefcomContact>((*I)->interactionPhysics);
		if(!BC || !BC->isCohesive) continue;
		const body_id_t id1=(*I)->getId1(), id2=(*I)->getId2();
		bodyDamage[id1].first++; bodyDamage[id2].first++;
		bodyDamage[id1].second+=BC->omega; bodyDamage[id2].second+=BC->omega;
	}
	BodyContainer::iterator bodiesEnd=rootBody->bodies->end();
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=bodiesEnd; ++I){
		if(bodyDamage[(*I)->getId()].first==0) continue;
		Real normDmg=bodyDamage[(*I)->getId()].second/bodyDamage[(*I)->getId()].first;
		(*I)->geometricalModel->diffuseColor=Vector3r(normDmg,1-normDmg,(*I)->isDynamic?0:1);
	}
}

/* doesn't work correctly */
#if 0
void BrefcomStiffnessCounter::action(MetaBody* rootBody){
	InteractionContainer::iterator Iend=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=Iend; ++I){
		const shared_ptr<Interaction>& contact = *I;
		if(!contact->isReal) continue;
		shared_ptr<SpheresContactGeometry> cGeom=YADE_PTR_CAST<SpheresContactGeometry>(contact->interactionGeometry);
		shared_ptr<BrefcomContact> cPhys=YADE_PTR_CAST<BrefcomContact>(contact->interactionPhysics);
		if(cPhys->Fn==0) continue;
		Vector3r diag_stiffness=Vector3r(pow(cGeom->normal[0],2),pow(cGeom->normal[1],2),pow(cGeom->normal[2],2))*(cPhys->Kn-cPhys->Kt)+Vector3r(1,1,1)*cPhys->Kt;
		Vector3r diag_Rstiffness=Vector3r(pow(cGeom->normal[1],2)+pow(cGeom->normal[2],2),pow(cGeom->normal[0],2)+pow(cGeom->normal[2],2),pow(cGeom->normal[0],2)+pow(cGeom->normal[1],2))*cPhys->Kt;
		GlobalStiffness* s=static_cast<GlobalStiffness*>(rootBody->physicalActions->find(contact->getId1(),actionStiffnessIndex).get());
		s->stiffness+=diag_stiffness; s->Rstiffness+=diag_Rstiffness*pow(cGeom->radius1,2);	
		s=static_cast<GlobalStiffness*>(rootBody->physicalActions->find(contact->getId2(),actionStiffnessIndex).get());
		s->stiffness+=diag_stiffness; s->Rstiffness+=diag_Rstiffness*pow(cGeom->radius2,2);
	}
}
#endif 
