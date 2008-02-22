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
			/* epsFracture */ 5*sigmaT/E12
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
#if 0
void BrefcomLaw::calculateShearForce(){
	Fs=Vector3r::ZERO; return;
	
	Fs = BC->Fs;
	Real dt=Omega::instance().getTimeStep();
	// rotation of the contact normal
	Fs += BC->Fs.Cross( BC->prevNormal.Cross(contGeom->normal) );
	//TRVAR1(BC->Fs.Cross( BC->prevNormal.Cross(contGeom->normal)));
	// mutual rotation of elements
	Real angle=dt*.5*contGeom->normal.Dot(rbp1->angularVelocity+rbp2->angularVelocity);
	Fs += BC->Fs.Cross(angle*contGeom->normal);
	//TRVAR1(BC->Fs.Cross(angle*contGeom->normal));
	// element slip
	Vector3r relVelocity=
		rbp2->velocity+rbp2->angularVelocity.Cross(contGeom->contactPoint-rbp2->se3.position)-
		rbp1->velocity+rbp1->angularVelocity.Cross(contGeom->contactPoint-rbp1->se3.position);
	Vector3r shearDisplacement=dt*(relVelocity-contGeom->normal.Dot(relVelocity)*contGeom->normal);
	Fs += BC->Ks*shearDisplacement;
	//TRVAR1(BC->Ks*shearDisplacement);
	if(isnan(Fs[0]) || isnan(Fs[1]) || isnan(Fs[2])){
		TRVAR4(BC->Fs, BC->prevNormal, contGeom->normal, BC->Fs.Cross(BC->prevNormal.Cross(contGeom->normal)));
		TRVAR4(angle, rbp1->angularVelocity, rbp2->angularVelocity,BC->Fs.Cross(angle*contGeom->normal));
		TRVAR4(rbp1->se3.position, rbp2->se3.position,rbp1->velocity,rbp2->velocity);
		TRVAR4(relVelocity,shearDisplacement,BC->Ks,BC->Ks*shearDisplacement);
		exit(0);
	}
}
#endif

void BrefcomLaw::applyForce(const Vector3r force){

	static_pointer_cast<Force>(rootBody->physicalActions->find(id1,ForceClassIndex))->force+=force;
	static_pointer_cast<Force>(rootBody->physicalActions->find(id2,ForceClassIndex))->force-=force;
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id1,MomentumClassIndex))->momentum+=(contGeom->contactPoint-rbp1->se3.position).Cross(force);
	static_pointer_cast<Momentum>(rootBody->physicalActions->find(id2,MomentumClassIndex))->momentum-=(contGeom->contactPoint-rbp2->se3.position).Cross(force);
}

Real BrefcomLaw::funcG(Real kappaD){
#ifdef BREFCOM_FUNC_G
	BREFCOM_FUNC_G
#else
	return 0;
#endif
}

Real BrefcomLaw::funcH(Real kappaD){
#ifdef BREFCOM_FUNC_H
	BREFCOM_FUNC_H
#else
	return 1;
#endif	
}	

void BrefcomLaw::action(Body* body){
	rootBody=YADE_CAST<MetaBody*>(body);
	
	//BOOST_FOREACH(shared_ptr<Interaction> II,*(rootBody->transientInteractions)){}

	for(InteractionContainer::iterator I=rootBody->transientInteractions->begin(); I!=rootBody->transientInteractions->end(); ++I){
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
		Real dist=(rbp1->se3.position-rbp2->se3.position).Length(); 
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
		ostringstream oss; oss<<setw(3)<< /* "w="<< */ (float)BC->omega;
		//std::string str=std::string("omegaPl=")+boost::lexical_cast<std::string>((float)(BC->omegaPl));
		for(unsigned int i=0;i<oss.str().length();i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,oss.str()[i]);
	glPopMatrix();
}

