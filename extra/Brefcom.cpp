// 2008 © Václav Šmilauer <eudoxos@arcig.cz> 
#include"Brefcom.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
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


YADE_PLUGIN("BrefcomMakeContact","BrefcomContact","BrefcomLaw","GLDrawBrefcomContact");

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
		Real d0=contGeom->radius1 + contGeom->radius2; // equilibrium distace is "just touching"
		//Real d0=(elast1->se3.position-elast2->se3.position).Length();
		Real Kn=(E12*S12/d0)*((1+alpha)/(beta*(1+nu12)+gamma*(1-alpha*nu12)));

		/* recommend default values for parameters
		 * propose ways to determine them exactly
		 * */
		shared_ptr<BrefcomContact> contPhys(new BrefcomContact(
			/* Kn */ Kn,
			/* Kt */ Kn*(1-alpha*nu12)/(1+nu12),
			/* frictionAngle */ .5*(elast1->frictionAngle+elast2->frictionAngle),
			/* undamagedCohesion */ S12*sigmaT,
			/* equilibriumDist */ d0,
			/* crossSection */ S12,
			/* epsCracking */ sigmaT/E12,
			/* epsFracture */ calibratedEpsFracture,
			/* expBending */ expBending
			));
		contPhys->prevNormal=contGeom->normal;
		contPhys->isCohesive=(Omega::instance().getCurrentIteration()>5 ? false : true);

		interaction->interactionPhysics=contPhys;
	}
}




/********************** BrefcomContact ****************************/
CREATE_LOGGER(BrefcomContact);



/********************** BrefcomLaw ****************************/
CREATE_LOGGER(BrefcomLaw);

void BrefcomLaw::applyForce(const Vector3r force){

	static_pointer_cast<Force>(rootBody->physicalActions->find(id1,ForceClassIndex))->force+=force;
	static_pointer_cast<Force>(rootBody->physicalActions->find(id2,ForceClassIndex))->force-=force;
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id1,MomentumClassIndex))->momentum+=(contGeom->contactPoint-rbp1->se3.position).Cross(force);
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id2,MomentumClassIndex))->momentum-=(contGeom->contactPoint-rbp2->se3.position).Cross(force);
}

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


void BrefcomLaw::action(Body* body){
	rootBody=YADE_CAST<MetaBody*>(body);
	
	//BOOST_FOREACH(shared_ptr<Interaction> II,*(rootBody->transientInteractions)){}
	
	list<shared_ptr<Interaction> > interactionsToBeDeleted;
	
	InteractionContainer::iterator transientEnd=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=transientEnd; ++I){
		if(!(*I)->isReal) continue;
		// TRACE;
		// initialize temporaries
		id1=(*I)->getId1(); id2=(*I)->getId2();
		body1=Body::byId(id1); body2=Body::byId(id2);
		assert(body1); assert(body2);
		BC=YADE_PTR_CAST<BrefcomContact>((*I)->interactionPhysics);
		contGeom=YADE_PTR_CAST<SpheresContactGeometry>((*I)->interactionGeometry);
		rbp1=YADE_PTR_CAST<RigidBodyParameters>(body1->physicalParameters);
		rbp2=YADE_PTR_CAST<RigidBodyParameters>(body2->physicalParameters);
		assert(BC); assert(contGeom); assert(rbp1); assert(rbp2);

		// don't disregard links with omegaPl=1, they can still be compressed !!
		// if(BC->omegaPl >= 1) continue; // we want to keep broken links formally alive, for statistics of material damage
		
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
		//BREFREC2(BC->omega,"ω");

		// shorthands
		Vector3r& epsT(BC->epsT); Real& kappaD(BC->kappaD); const Real& equilibriumDist(BC->equilibriumDist); const Real& xiShear(BC->xiShear); const Real& Kn(BC->Kn); const Real& undamagedCohesion(BC->undamagedCohesion); const Real& frictionAngle(BC->frictionAngle); const Real& Kt(BC->Kt); const Real& crossSection(BC->crossSection); Real& omega(BC->omega);

		Real dist=(rbp1->se3.position-rbp2->se3.position).Length();

		/* deleting interaction here would (maybe) break the iterator; do it once this loop is done */
		if(!BC->isCohesive && dist>0){ /* delete this interaction later */ interactionsToBeDeleted.push_back(*I); continue; }

		/* rotate epsT to the new contact plane */
			Real dt=Omega::instance().getTimeStep();
			// rotation of the contact normal
			epsT+=BC->epsT.Cross(BC->prevNormal.Cross(contGeom->normal));
			// mutual rotation
			Real angle=dt*.5*contGeom->normal.Dot(rbp1->angularVelocity+rbp2->angularVelocity); /* broken, assumes equal radii */
			epsT+=epsT.Cross(angle*contGeom->normal);

		/* calculate tangential strain increment */
		Vector3r relVelocity=
			rbp2->velocity+rbp2->angularVelocity.Cross(contGeom->contactPoint-rbp2->se3.position)
			-rbp1->velocity+rbp1->angularVelocity.Cross(contGeom->contactPoint-rbp1->se3.position);
		Vector3r tangentialDisplacement=dt*(relVelocity-contGeom->normal.Dot(relVelocity)*contGeom->normal);
		epsT+=tangentialDisplacement/dist;
		
		/* normal strain */
		Real epsN=(dist-equilibriumDist)/dist;

		Real sigmaN; Vector3r sigmaT;

		#ifdef BREFCOM_MATERIAL_MODEL
			BREFCOM_MATERIAL_MODEL
		#else
			sigmaN=Kn*epsN;
			sigmaT=Kt*epsT;
		#endif

		// TODO: store Fn and Fs inside BC (new attributes), for use with GlobalStiffnessCounter 
		applyForce(crossSection*(contGeom->normal*sigmaN + sigmaT));

		for(size_t i=0; i<recValues.size(); i++) recStream<<recLabels[i]<<": "<<recValues[i]<<" ";
		recStream<<endl;
	}
	// delete interactions that were requested to be deleted.
	for(list<shared_ptr<Interaction> >::iterator I=interactionsToBeDeleted.begin(); I!=interactionsToBeDeleted.end(); I++){ rootBody->transientInteractions->erase((*I)->getId1(),(*I)->getId2()); }
}


/********************** GLDrawBrefcomContact ****************************/

#include<yade/lib-opengl/OpenGLWrapper.hpp>

void GLDrawBrefcomContact::go(const shared_ptr<InteractionPhysics>& ip, const shared_ptr<Interaction>&, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool wireFrame){
	const shared_ptr<BrefcomContact>& BC=static_pointer_cast<BrefcomContact>(ip);

	/* shared_ptr<Sphere> s1(dynamic_pointer_cast<Sphere>(b1->geometricalModel)), s2(dynamic_pointer_cast<Sphere>(b2->geometricalModel));
	if(s1 && s2){
		Real radius=min(s1->radius,s2->radius), len=(b1->physicalParameters->se3.position-b2->physicalParameters->se3.position).Length();
		glScale(len,radius,radius);
		glutSolidCube(1);
	} */

	glColor3(BC->omega,1-BC->omega,0.0); /* damaged links red, undamaged green */

	glBegin(GL_LINES);
		glVertex3v(b1->physicalParameters->se3.position);
		glVertex3v(b2->physicalParameters->se3.position);
	glEnd();

	Vector3r mid=0.5*(b1->physicalParameters->se3.position+b2->physicalParameters->se3.position);
	glTranslatev(mid);
	glPushMatrix();
		glRasterPos2i(0,0);
		ostringstream oss; oss<<setprecision(3)<< /* "w="<< */ (float)BC->omega;
		//std::string str=std::string("omegaPl=")+boost::lexical_cast<std::string>((float)(BC->omegaPl));
		for(unsigned int i=0;i<oss.str().length();i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,oss.str()[i]);
	glPopMatrix();
}

/********************** BrefcomDamageColorizer ****************************/
void BrefcomDamageColorizer::action(Body* b){
	MetaBody* rootBody=static_cast<MetaBody*>(b);
	vector<pair<short,Real> > bodyDamage; /* number of cohesive interactions per body; cummulative damage of interactions */
	bodyDamage.resize(rootBody->bodies->size(),pair<short,Real>(0,0));
	InteractionContainer::iterator transientEnd=rootBody->transientInteractions->end();
	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=transientEnd; ++I){
		shared_ptr<BrefcomContact> BC=YADE_PTR_CAST<BrefcomContact>((*I)->interactionPhysics);
		if(!BC->isCohesive) continue;
		const body_id_t id1=(*I)->getId1(), id2=(*I)->getId2();
		bodyDamage[id1].first++; bodyDamage[id2].first++;
		bodyDamage[id1].second+=BC->omega; bodyDamage[id2].second+=BC->omega;
	}
	BodyContainer::iterator bodiesEnd=rootBody->bodies->end();
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=bodiesEnd; ++I){
		Real normDmg=bodyDamage[(*I)->getId()].second/bodyDamage[(*I)->getId()].first;
		(*I)->geometricalModel->diffuseColor=Vector3r(normDmg,1-normDmg,.5);	
	}
}
