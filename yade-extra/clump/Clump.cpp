// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include"Clump.hpp"

#include<algorithm>

char* yadePluginClasses[]={
	"Clump",
	"ClumpSubBodyMover",
	"ClumpTestGen",
	NULL /*sentinel*/
};

CREATE_LOGGER(Clump);
CREATE_LOGGER(ClumpSubBodyMover);
CREATE_LOGGER(ClumpTestGen);

/**************************************************************************************
 ************************************* ClumpSubBodyMover ******************************
 **************************************************************************************/

// Constructor must be in the .cpp file (?)
ClumpSubBodyMover::ClumpSubBodyMover(){/*createIndex();*/ }

/*! We only call clump's method, since it belongs there logically. It makes encapsulation of private members nicer, too.
 * @param pp passed by the dispatcher
 * @param clump passed by the dispatcher
 */
void ClumpSubBodyMover::applyCondition(Body* _rootBody){
	MetaBody* rootBody = dynamic_cast<MetaBody*>(_rootBody);
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=rootBody->bodies->end(); ++I){
		shared_ptr<Body> b = *I;
		// is this a clump?
		if(b->getId()==b->clumpId){
			//LOG_TRACE("Applying movement to clump #"<<b->getId());
			dynamic_pointer_cast<Clump>(b)->moveSubBodies();
		}
	}
	//if(!clump->isDynamic) return; // perhaps clump that has been desactivated?!
}

/**************************************************************************************
 ******************************************** Clump ***********************************
 **************************************************************************************/

/*! Create zero'ed RigidBodyParameters; they should not be manipulated directly, since they are all calculated in Clump::update.
 * @todo do we need to do the same for GeomtricalModel, InteractingGeometry and BoundingVolume? They will never be used. Sort that out for sure.
 * @bug setting Clump::isDynamic in constructor is not enough (as if it were modified somewhere), must be set explicitly by the user after construction (why?)
 */
Clump::Clump(): Body(){
	isDynamic=true;
	physicalParameters=shared_ptr<RigidBodyParameters>(new RigidBodyParameters);

	// these will not be defined for the moment...
#if 0
	boundingVolume=shared_ptr<AABB>(new AABB);
	boundingVolume->diffuseColor=Vector3r(1,0,0);

	interactingGeometry=shared_ptr<InteractingGeometry>(new InteractingGeometry);
	interactingGeometry->diffuseColor=Vector3f(0,0,0);

	geometricalModel=shared_ptr<GeometricalModel>(new GeometricalModel);
	geometricalModel->diffuseColor=Vector3f(0,0,0); geometricalModel->wire=false; geometricalModel->visible=false; geometricalModel->shadowCaster=false;
#endif

}

/*! @pre Body must be dynamic.
 * @pre Body must not be part or this clump already.
 * @pre Body must have valid (non-NULL) Body::physicalParameters
 * @todo se3 calculation is not tested yet
 */
void Clump::add(Body::id_t subId){
	shared_ptr<Body> subBody=Body::byId(subId);

	// preconditions
	assert(subBody->isDynamic);
	assert(physicalParameters);
	assert(subBodies.count(subId)==0);

	// begin actual setup
	subBody->clumpId=getId();
	subBody->isDynamic=false;
	// for now, push just unitialized se3; will be updated by updateProperties
	subBodies[subId]=Se3r();

	clumpId=getId(); // identifies a clump

	LOG_DEBUG("Added body #"<<subId<<" to clump #"<<getId());
}

/*! @pre Body with given id must be in the clump.
 */
void Clump::del(Body::id_t subId){
	// erase the subBody; removing body that is not part of the clump is error
	assert(subBodies.erase(subId)==1);
	// restore body's internal parameters;
	shared_ptr<Body> subBody=Body::byId(subId);
	subBody->clumpId=Body::ID_NONE;
	subBody->isDynamic=true;
	LOG_DEBUG("Removed body #"<<subId<<" from clump #"<<getId());
}

/*! @brief Calculate positions and orientations of subBodies based on Clump's Se3.
 *
 * This method is called by the ClumpSubBodyMover engine after each timestep.
 * @note Velocities of subBodies are not updated, since subBodies have isdynamic==false. It is possible, though, that someone needs to have a moving clump that is later broken apart and that liberated particle continue to move in the same way as theydid within the clump. In that case, this will have to be completed.
 */
void Clump::moveSubBodies(){
	const Se3r& mySe3(physicalParameters->se3);
	for(clumpMap::iterator I=subBodies.begin(); I!=subBodies.end(); I++){
		// now, I->first is Body::id_t, I->second is Se3r of that body in the clump
		shared_ptr<Body> subBody=Body::byId(I->first);
		shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
		//LOG_TRACE("Old #"<<I->first<<"position: "<<subRBP->se3.position);
		subRBP->se3.position=mySe3.position+mySe3.orientation*I->second.position;
		subRBP->se3.orientation=mySe3.orientation*I->second.orientation;
		//LOG_TRACE("New #"<<I->first<<"position: "<<subRBP->se3.position);
		//LOG_TRACE("Clump #"<<getId()<<" moved #"<<I->first<<".");
	}
	/* @bug Temporarily we reset acceleration and angularAcceleration of the clump here;
	 * should be a new negine that will take care of that.
	 */
	shared_ptr<RigidBodyParameters> clumpRBP=dynamic_pointer_cast<RigidBodyParameters>(physicalParameters);
	#if 0
		if(Omega::instance().getCurrentIteration()%50==0){
			Real Erot=.5*clumpRBP->inertia[0]*pow(clumpRBP->angularVelocity[0],2)+.5*clumpRBP->inertia[1]*pow(clumpRBP->angularVelocity[1],2)+.5*clumpRBP->inertia[2]*pow(clumpRBP->angularVelocity[2],2);
			Real Etrans=.5*clumpRBP->mass*pow(clumpRBP->velocity.Length(),2);
			// (0,0,1) is gravity acceleration
			Real Epot=clumpRBP->se3.position.Dot(Vector3r(0,0,1))*clumpRBP->mass;
			LOG_TRACE("##"<<clumpId<<" energy "<<Erot+Etrans+Epot<<"\tv "<<Etrans<<"\tw "<<Erot<<"\tp "<<Epot);
		}
	#endif

	clumpRBP->acceleration=Vector3r(0,0,0);
	clumpRBP->angularAcceleration=Vector3r(0,0,0);
	
}

/*! Clump's se3 will be updated (origin at centroid and axes coincident with principal inertia axes) and subSe3 modified in such a way that subBodies positions in world coordinates will not change.

	The algorithm is as follows:
	-# Clump::subBodies values and Clump::physicalParameters::se3 are invalid from this point
	-# M=0; S=vector3r(0,0,0); I=zero tensor; (ALL calculations are in world coordinates!)
	-# loop over Clump::subBodies (position x_i, mass m_i, inertia at subBody's centroid I_i) [this loop will be replaced by numerical integration (rasterization) for the intersecting case; the rest will be the same]
		- M+=m_i
		- S+=m_i*x_i (local static moments are zero (centroid)
		- get inertia tensor of subBody in world coordinates, by rotating the principal (local) tensor against subBody->se3->orientation; then translate it to world origin (parallel axes theorem), then I+=I_i_world
	-# clumpPos=S/M
	-# translate aggregate's inertia tensor; parallel axes on I (R=clumpPos): I^c_jk=I'_jk-M*(delta_jk R.R - R_j*R_k) [http://en.wikipedia.org/wiki/Moments_of_inertia#Parallel_axes_theorem]
	-# eigen decomposition of I, get principal inertia and rotation matrix of the clump
	-# se3->orientation=quaternion(rotation_matrix); se3->position=clumpPos
	-#	update subSe3s

	@todo I \em think that the order of transformation of inertia is:
		- from local to global: first rotate, then translate;
		- from global to local: first translate, then rotate,
	since rotation must be done with origin at the centroid... This needs to be verified, though.
	@todo all the rest of this routine needs to be verified!
	@todo implement the loop for intersecting bodies (may cut'n'paste from slum code, but that will work for spheres only!)

	@note User is responsible for calling this function when appropriate (after adding/removing bodies and before any subsequent simulation). This function can be rather slow by virtue of numerical integration.
	@note subBodie's velocities are not taken into account. This means that clump will be at still after being created, even if its composing particles did have some velocities. If this is concern for someone, it needs to be completed in the code below. See Clump::moveSubBodies for complementary issue.
	@todo Needs to be tested for physical correctness
	@param intersecting if true, evaluate mass and inertia numerically; otherwise, use analytical methods (parallel axes theorem) which disregard any intersections, but are much faster. */
void Clump::updateProperties(bool intersecting){
	LOG_DEBUG("Updating clump #"<<getId()<<" parameters");
	assert(subBodies.size()>0);
	// maybe handle the case of only one clump subBody specially here?!
	if(intersecting){
		LOG_WARN("Self-intersecting clumps not yet implemented, intersections will be ignored.");
		intersecting=false;}

	/* quantities suffixed by
		g: global (world) coordinates
		s: local subBody's coordinates
		c: local clump coordinates */
	double M=0;
	Vector3r Sg(0,0,0); // static moment
	Matrix3r Ig(true /* fill with zeros */ ), Ic(true); // tensor of inertia; is upper triangular, zeros instead of symmetric elements
	Se3r& mySe3(physicalParameters->se3);

	// begin non-intersecting loop here
	if(!intersecting){
		for(clumpMap::iterator I=subBodies.begin(); I!=subBodies.end(); I++){
			// now, I->first is Body::id_t, I->second is Se3r of that body
			shared_ptr<Body> subBody=Body::byId(I->first);
			shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
			M+=subRBP->mass;
			Sg+=subRBP->mass*subRBP->se3.position;
			// transform from local to global coords
			// FIXME: verify this!
			Quaternionr subRBP_orientation_conjugate=subRBP->se3.orientation.Conjugate();
			Matrix3r Imatrix(subRBP->inertia[0],subRBP->inertia[1],subRBP->inertia[2]);
			// TRWM3MAT(Imatrix); TRWM3QUAT(subRBP_orientation_conjugate);
			Ig+=Clump::inertiaTensorTranslate(Clump::inertiaTensorRotate(Imatrix,subRBP_orientation_conjugate),subRBP->mass,-1.*subRBP->se3.position);

			//TRWM3MAT(Clump::inertiaTensorRotate(Matrix3r(subRBP->inertia),subRBP_orientation_conjugate));
		}
	}else{LOG_ERROR("Self-intersecting clumps not implemented. Results will be very wrong.");}
	TRVAR1(M);
	TRWM3MAT(Ig);
	TRWM3VEC(Sg);

	/*! @bug incorrect results; these are vars traced for one and only sphere:
	 *
\verbatim
DEBUG yade.Clump yade-extra/clump/Clump.cpp:153 updateProperties: Updating clump #1 parameters
DEBUG yade.Clump yade-extra/clump/Clump.cpp:182 updateProperties: subRBP->inertia=(104.72 104.72 104.72)
DEBUG yade.Clump yade-extra/clump/Clump.cpp:183 updateProperties: Clump::inertiaTensorRotate(Matrix3r(subRBP->inertia),subRBP_orientation_conjugate)=((0 0 0)(0 0 0)(0 0 0))
DEBUG yade.Clump yade-extra/clump/Clump.cpp:186 updateProperties: M=1047.2; 
DEBUG yade.Clump yade-extra/clump/Clump.cpp:187 updateProperties: Ig=((0 0 0)(0 0 0)(0 0 0))
DEBUG yade.Clump yade-extra/clump/Clump.cpp:188 updateProperties: Sg=(0 0 0)
DEBUG yade.Clump yade-extra/clump/Clump.cpp:200 updateProperties: Ic=((0 0 0)(0 0 0)(0 0 0))
DEBUG yade.Clump yade-extra/clump/Clump.cpp:201 updateProperties: Clump::inertiaTensorRotate(Ic_orientG,R_g2c)=((0 nan nan)(nan nan nan)(nan nan nan))
\endverbatim

*/


	mySe3.position=Sg/M; // clump's centroid
	// this will calculate translation only, since rotation is zero
	Matrix3r Ic_orientG=Clump::inertiaTensorTranslate(Ig, -M /* negative mass means towards centroid */, mySe3.position); // inertia at clump's centroid but with world orientation
	TRWM3MAT(Ic_orientG);

	Matrix3r R_g2c(true); //rotation matrix
	Ic_orientG(1,0)=Ic_orientG(0,1); Ic_orientG(2,0)=Ic_orientG(0,2); Ic_orientG(2,1)=Ic_orientG(1,2); // symmetrize
	//TRWM3MAT(Ic_orientG);
	Ic_orientG.EigenDecomposition(R_g2c,Ic);
	/*! @bug: eigendecomposition is wrong. see http://article.gmane.org/gmane.science.physics.yade.devel/99 for message. */
	// has NaNs for identity matrix!
	TRWM3MAT(R_g2c);

	// these two should give the same result!
	//TRWM3MAT(Ic);
	//TRWM3MAT(Clump::inertiaTensorRotate(Ic_orientG,R_g2c));

	// set quaternion from rotation matrix
	mySe3.orientation.FromRotationMatrix(R_g2c);
	// now Ic is diagonal
	shared_ptr<RigidBodyParameters> clumpRBP=dynamic_pointer_cast<RigidBodyParameters>(physicalParameters);
	clumpRBP->inertia=Vector3r(Ic(0,0),Ic(1,1),Ic(2,2));
	clumpRBP->mass=M;


	// this block will be removed once EigenDecomposition works for diagonal matrices
	#if 1
		if(isnan(R_g2c(0,0))||isnan(R_g2c(0,1))||isnan(R_g2c(0,2))||isnan(R_g2c(1,0))||isnan(R_g2c(1,1))||isnan(R_g2c(1,2))||isnan(R_g2c(2,0))||isnan(R_g2c(2,1))||isnan(R_g2c(2,2))){
			LOG_WARN("WigenDecomposition gave some NaNs, we will use imaginary values for clump inertia and orientation instead!");
			//FIXME: since EigenDecomposition is broken, use inertia of the first body instead;
			//!!!!! note that this is HIGHLY incorrect for all non-single clumps !!!!!
			clumpMap::iterator I=subBodies.begin();
			shared_ptr<Body> subBody=Body::byId(I->first);
			shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
			clumpRBP->inertia=subRBP->inertia*10.; // 10 is arbitrary; just to have inertia of clump bigger
			// orientation of the clump is broken as well, since is result of EigenDecomposition as well (rotation matrix)
			mySe3.orientation.FromRotationMatrix(Matrix3r(1,0,0,0,1,0,0,0,1));
		}
	#endif
	TRWM3VEC(clumpRBP->inertia);

	// these might be calculated from subBodies... but complicated
	clumpRBP->velocity=Vector3r(0,0,0);
	clumpRBP->angularVelocity=Vector3r(0,0,0);

	// update subBodySe3s; subtract clump orientation (apply its inverse first) to subBody's orientation
	// Conjugate is equivalent to Inverse for normalized quaternions
	for(clumpMap::iterator I=subBodies.begin(); I!=subBodies.end(); I++){
		// now, I->first is Body::id_t, I->second is Se3r of that body
		shared_ptr<Body> subBody=Body::byId(I->first);
		shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
		I->second.orientation=mySe3.orientation.Conjugate()*subRBP->se3.orientation;
		I->second.position=mySe3.orientation.Conjugate()*(subRBP->se3.position-mySe3.position);
	}

	// clump as such has no bounding box, since it is probably never needed. The following may be safely removed unless someone needs it.
	#if 0
		// update bounding box; we could have done this in previous loops, but this is cleaner
		Vector3r min(0,0,0),max(0,0,0);
		for(clumpMap::iterator I=subBodies.begin(); I!=subBodies.end(); I++){
			shared_ptr<Body> subBody=Body::byId(I->first);
			shared_ptr<AABB> subAabb=dynamic_pointer_cast<AABB>(subBody->boundingVolume);
			if (I!=subBodies.begin()){min=componentMinVector(min,subAabb->center-subAabb->halfSize);	max=componentMinVector(max,subAabb->center+subAabb->halfSize);}
			else {min=subAabb->center-subAabb->halfSize; max=subAabb->center+subAabb->halfSize;}
		}
		shared_ptr<AABB> aabb=dynamic_pointer_cast<AABB>(boundingVolume);
		aabb->center=(min+max)*.5; aabb->halfSize=(max-min)*.5;
	#endif
}

/*! @brief Recalculates inertia tensor of a body after translation away from (default) or towards its centroid.
 *
 * @oaram I inertia tensor in the original coordinates; it is assumed to be upper-triangular (elements below the diagonal are ignored).
 * @param m mass of the body; if positive, translation is away from the centroid; if negative, towards centroid.
 * @param off offset of the new origin from the original origin
 * @return inertia tensor in the new coordinate system; the matrix is symmetric.
 */
Matrix3r Clump::inertiaTensorTranslate(const Matrix3r& I,const Real m, const Vector3r& off){
	Real ooff=off.Dot(off);
	Matrix3r I2=I;
	//TRWM3VEC(off); TRVAR2(ooff,m); TRWM3MAT(I);
	// translation away from centroid
	/* I^c_jk=I'_jk-M*(delta_jk R.R - R_j*R_k) [http://en.wikipedia.org/wiki/Moments_of_inertia#Parallel_axes_theorem] */
	I2+=m*Matrix3r(/* dIxx */ ooff-off[0]*off[0], /* dIxy */ -off[0]*off[1], /* dIxz */ -off[0]*off[2],
		/* sym */ 0, /* dIyy */ ooff-off[1]*off[1], /* dIyz */ -off[1]*off[2],
		/* sym */ 0, /* sym */ 0, /* dIzz */ ooff-off[2]*off[2]);
	I2(1,0)=I2(0,1); I2(2,0)=I2(0,2); I2(2,1)=I2(1,2);
	//TRWM3MAT(I2);
	return I2;
}

/*! @brief Recalculate body's inertia tensor in rotated coordinates.
 *
 * @param I inertia tensor in old coordinates
 * @param T rotation matrix from old to new coordinates
 * @return inertia tensor in new coordinates
 */
Matrix3r Clump::inertiaTensorRotate(const Matrix3r& I,const Matrix3r& T){
	/* [http://www.kwon3d.com/theory/moi/triten.html] */
	//TRWM3MAT(I); TRWM3MAT(T);
	return T.Transpose()*I*T;
}

/*! @brief Recalculate body's inertia tensor in rotated coordinates.
 *
 * @param I inertia tensor in old coordinates
 * @param rot quaternion that describes rotation from old to new coordinates
 * @return inertia tensor in new coordinates
 */
Matrix3r Clump::inertiaTensorRotate(const Matrix3r& I, const Quaternionr& rot){
	Matrix3r T;
	rot.ToRotationMatrix(T);
	return inertiaTensorRotate(I,T);
}



/**************************************************************************************
 ********************* ClumpTestGen ***************************************************
 **************************************************************************************/

#include <yade/yade-package-common/MetaInteractingGeometry2AABB.hpp>
#include <yade/yade-package-common/MetaInteractingGeometry.hpp>
#include <yade/yade-package-common/Box.hpp>
#include <yade/yade-package-common/AABB.hpp>
#include <yade/yade-package-common/Sphere.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/PersistentSAPCollider.hpp>

#include <yade/yade-package-common/BodyRedirectionVector.hpp>
#include <yade/yade-package-common/InteractionVecSet.hpp>
#include <yade/yade-package-common/PhysicalActionVectorVector.hpp>

#include <yade/yade-package-common/InteractingBox.hpp>
#include <yade/yade-package-common/InteractingSphere.hpp>


#include <yade/yade-package-common/PhysicalActionContainerReseter.hpp>
#include <yade/yade-package-common/PhysicalActionContainerInitializer.hpp>
#include <yade/yade-package-common/PhysicalParametersMetaEngine.hpp>

#include <yade/yade-package-common/InteractionGeometryMetaEngine.hpp>
#include <yade/yade-package-common/InteractionPhysicsMetaEngine.hpp>
#include <yade/yade-package-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-package-common/PhysicalActionDamper.hpp>
#include <yade/yade-package-common/PhysicalActionApplier.hpp>
#include <yade/yade-package-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-package-common/CundallNonViscousMomentumDamping.hpp>
#include <yade/yade-package-common/GravityEngine.hpp>



#include <yade/yade-package-dem/BodyMacroParameters.hpp>
#include <yade/yade-package-dem/ElasticCriterionTimeStepper.hpp>
#include <yade/yade-package-dem/ElasticContactLaw.hpp>

#include<yade/yade-extra/PythonRecorder.hpp>
//#include "ElasticCohesiveLaw.hpp"
//#include "MacroMicroElasticRelationships.hpp"
//#include "BodyMacroParameters.hpp"


// generate either random spheres, or (if not defined) regular one sphere and {1,2,3,4}-clumps
//#define CLUMP_COMPLICATED


string ClumpTestGen::generate()
{
	{ // root body
		rootBody = shared_ptr<MetaBody>(new MetaBody); rootBody->isDynamic=false;
		shared_ptr<ParticleParameters> physics(new ParticleParameters); Quaternionr q; q.FromAxisAngle(Vector3r(0,0,1),0); physics->se3=Se3r(Vector3r(0,0,0),q); physics->mass=0; physics->velocity=Vector3r::ZERO; physics->acceleration=Vector3r::ZERO;
		rootBody->physicalParameters=physics;
		shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());	set->diffuseColor=Vector3f(0,0,1);
		rootBody->interactingGeometry=dynamic_pointer_cast<InteractingGeometry>(set);	
		shared_ptr<AABB> aabb(new AABB); aabb->diffuseColor=Vector3r(0,0,1);
		rootBody->boundingVolume=dynamic_pointer_cast<BoundingVolume>(aabb);
		createActors(rootBody);
		// Containers
		rootBody->persistentInteractions=shared_ptr<InteractionContainer>(new InteractionVecSet);
		rootBody->transientInteractions=shared_ptr<InteractionContainer>(new InteractionVecSet);
		rootBody->physicalActions=shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
		rootBody->bodies=shared_ptr<BodyContainer>(new BodyRedirectionVector);
	}

	// FIXME: this is to make Body::byId work; otherwise crash will occur. Save rootBody, restore after generation is completed
	// FIXME: will not be restored if generation crashes.
	// FIXME: is it ever needed to restore rootBody?
	shared_ptr<MetaBody> oldRootBody=Omega::instance().getRootBody();
	Omega::instance().setRootBody(rootBody);
	
	{ // ground
		Vector3r extents(3,3,.2);
		shared_ptr<Body> ground=shared_ptr<Body>(new Body(0,55)); ground->isDynamic=false;
		shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters); Quaternionr q; q.FromAxisAngle(Vector3r(0,0,1),0); physics->angularVelocity=Vector3r(0,0,0); physics->velocity=Vector3r(0,0,0); physics->mass=1; physics->inertia=Vector3r(1,1,1); physics->se3=Se3r(Vector3r(0,0,-1),q); physics->young=1e7; physics->poisson=.3; physics->frictionAngle=30*Mathr::PI/180.0;
		ground->physicalParameters=physics;
		shared_ptr<AABB> aabb(new AABB);aabb->diffuseColor=Vector3r(1,0,0);
		ground->boundingVolume=aabb;
		shared_ptr<Box> gBox(new Box);gBox->extents=extents; gBox->diffuseColor=Vector3f(1,1,1); gBox->wire=false; gBox->visible=true; gBox->shadowCaster=true;
		ground->geometricalModel=gBox;
		shared_ptr<InteractingBox> iBox(new InteractingBox); iBox->extents=extents; iBox->diffuseColor=Vector3f(1,1,1);
		ground->interactingGeometry=iBox;
		rootBody->bodies->insert(ground);
	}

	setMessage("Elements...");

	vector<Vector3r> relPos; vector<Real> radii; Vector3r clumpPos;
	#ifdef CLUMP_COMPLICATED
		for(int i=0;i<2;i++) for(int j=0;j<2;j++) for(int k=0;k<2;k++){
			relPos.clear(); radii.clear();
			clumpPos=Vector3r(i,j,k)+Vector3r(.2*Mathr::SymmetricRandom(),.2*Mathr::SymmetricRandom(),.2*Mathr::SymmetricRandom());
			int nSpheres=(int)(Mathr::UnitRandom()*4+1);
			for(int l=0; l<nSpheres; l++) {
				relPos.push_back(.3*Vector3r(Mathr::SymmetricRandom(),Mathr::SymmetricRandom(),Mathr::SymmetricRandom()));
				radii.push_back(Mathr::IntervalRandom(.1,.3));
			}
			createOneClump(rootBody,clumpPos,relPos,radii);
		}
	#else
		#if 1
		// standalone (non-clump!) sphere as well
		shared_ptr<Body> sphere=createOneSphere(Vector3r(0,0,0),.5);
		rootBody->bodies->insert(sphere);

		// one-sphere clump
		clumpPos=Vector3r(-2,0,0);
		relPos.push_back(Vector3r(0,0,0)); radii.push_back(.5);
		createOneClump(rootBody,clumpPos,relPos,radii);
		relPos.clear(); radii.clear();
		#endif
		
		// two-sphere clump
		clumpPos=Vector3r(2,0,0);
		relPos.push_back(Vector3r(0,-.5,.5)); radii.push_back(.5);
		relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
		createOneClump(rootBody,clumpPos,relPos,radii);
		relPos.clear(); radii.clear();

		#if 1
		// three-sphere slump
		clumpPos=Vector3r(0,2,0);
		relPos.push_back(Vector3r(0,-.5,.5)); radii.push_back(.5);
		relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
		relPos.push_back(Vector3r(.5,0,0)); radii.push_back(.5);
		createOneClump(rootBody,clumpPos,relPos,radii);
		relPos.clear(); radii.clear();

		// four-sphere slump
		clumpPos=Vector3r(0,-2,0);
		relPos.push_back(Vector3r(0,0,0)); radii.push_back(.5);
		relPos.push_back(Vector3r(.5,0,0)); radii.push_back(.5);
		relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
		relPos.push_back(Vector3r(0,0,.5)); radii.push_back(.5);
		createOneClump(rootBody,clumpPos,relPos,radii);
		relPos.clear(); radii.clear();
		#endif
	#endif

	// restore Omega
	Omega::instance().setRootBody(oldRootBody);
	

	return "Garbage generated, since your IQ is <200...";
}


/*! \brief Generate clump of spheres, the result will be inserted into rootBody.
 *
 * To create a clump, first the clump itself needs to be instantiated \em and inserted into rootBody (this will assign an Body::id).
 * In order for this to work, Omega::roootBody must have been assigned; within generators, use Omega::setRootBody for this.
 *
 * The body to add to clump must have been also created and added to the rootBody (so that it has id, again).
 *
 * Finally, call Clump::updateProperties to get physical properties physically right (inertia, position, orientation, mass, ...).
 *
 * @param clumpPos Center of the clump (not necessarily centroid); serves merely as reference for sphere positions.
 * @param relPos Relative positions of individual spheres' centers.
 * @param radii Radii of composing spheres. Must have the same length as relPos.
 */
void ClumpTestGen::createOneClump(shared_ptr<MetaBody>& rootBody, Vector3r clumpPos, vector<Vector3r> relPos, vector<Real> radii)
{
	assert(relPos.size()==radii.size());
	
	// empty clump	
	shared_ptr<Clump> clump=shared_ptr<Clump>(new Clump());
	shared_ptr<Body> clumpAsBody=dynamic_pointer_cast<Body>(clump);
	rootBody->bodies->insert(clumpAsBody);

	clump->isDynamic=true;
	// if subscribedBodies work some day: clumpMover->subscribedBodies.push_back(clump->getId());
	
	for(size_t i=0; i<relPos.size(); i++){
		shared_ptr<Body> sphere=createOneSphere(clumpPos+relPos[i],radii[i]);
		Body::id_t lastId=(Body::id_t)rootBody->bodies->insert(sphere);
		clump->add(lastId);
		LOG_TRACE("Generated clumped sphere #"<<lastId);
	}
	clump->updateProperties(false);
}

/* Create single Sphere with some sane default parameters.
 *
 * @return Sphere (as Body) that can be readily used.
 */
shared_ptr<Body> ClumpTestGen::createOneSphere(Vector3r position, Real radius){
	Real density=2000;

	// body itself
	shared_ptr<Body> body=shared_ptr<Body>(new Body(0,55));
	body->isDynamic=true;

	// physics
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	Quaternionr q; q.FromAxisAngle(Vector3r(0,0,1),0);
	physics->angularVelocity=Vector3r(0,0,0);
	physics->velocity=Vector3r(0,0,0);
	physics->mass=4.0/3.0*Mathr::PI*radius*radius*radius*density;
	physics->inertia=Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius);
	physics->se3=Se3r(position,q);
	physics->young=8e7;
	physics->poisson=.3;
	physics->frictionAngle=40*Mathr::PI/180.0;
	body->physicalParameters=physics;

	// aabb
	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor=Vector3r(0,1,0);
	body->boundingVolume=aabb;

	// mold
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	iSphere->radius=radius;
	iSphere->diffuseColor=Vector3f(Mathf::UnitRandom(),Mathf::UnitRandom(),Mathf::UnitRandom());
	body->interactingGeometry=iSphere;

	//shape
	gSphere->radius=radius;
	gSphere->diffuseColor=Vector3f(Mathf::UnitRandom(),Mathf::UnitRandom(),Mathf::UnitRandom());
	gSphere->wire=false;
	gSphere->visible=true;
	gSphere->shadowCaster=true;
	body->geometricalModel=gSphere;

	return body;
}
/*! Instantiate engines acting on bodies during simulation.
 *
 * For simplicity, physical constants are not parametrized but hardcoded.
 * Damping is not used so that energy conservation may be asserted.
 */
void ClumpTestGen::createActors(shared_ptr<MetaBody>& rootBody)
{
	shared_ptr<PhysicalActionContainerInitializer> physicalActionInitializer(new PhysicalActionContainerInitializer);
	physicalActionInitializer->physicalActionNames.push_back("Force");
	physicalActionInitializer->physicalActionNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","InteractingSphere2InteractingSphere4SpheresContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","InteractingBox2InteractingSphere4SpheresContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");
		
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","InteractingSphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","InteractingBox2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","MetaInteractingGeometry2AABB");
		
	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity=Vector3r(0,0,-10);
	
	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = .2;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = .2;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add("Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");
	
	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
 	
	shared_ptr<ElasticCriterionTimeStepper> sdecTimeStepper(new ElasticCriterionTimeStepper);
	sdecTimeStepper->sdecGroupMask = 55;
	sdecTimeStepper->timeStepUpdateInterval = 300;
	
	shared_ptr<ElasticContactLaw> constitutiveLaw(new ElasticContactLaw);
	constitutiveLaw->sdecGroupMask = 55;
	constitutiveLaw->momentRotationLaw = true;
/*// FIXME FIXME FIXME ....	
	shared_ptr<ElasticCohesiveLaw> constitutiveLaw2(new ElasticCohesiveLaw);
	constitutiveLaw2->sdecGroupMask = 55;
	constitutiveLaw2->momentRotationLaw = momentRotationLaw;*/

	// clumps will be subscribed later, as they are generated
	clumpMover=shared_ptr<ClumpSubBodyMover>(new ClumpSubBodyMover);

	
	rootBody->engines.clear();
	rootBody->engines.push_back(sdecTimeStepper);
	rootBody->engines.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->engines.push_back(boundingVolumeDispatcher);
	rootBody->engines.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->engines.push_back(interactionGeometryDispatcher);
	rootBody->engines.push_back(interactionPhysicsDispatcher);
	rootBody->engines.push_back(constitutiveLaw);
	//rootBody->engines.push_back(constitutiveLaw2);
	rootBody->engines.push_back(gravityCondition);
	//rootBody->engines.push_back(actionDampingDispatcher);
	rootBody->engines.push_back(applyActionDispatcher);
	rootBody->engines.push_back(positionIntegrator);
	rootBody->engines.push_back(orientationIntegrator);
	rootBody->engines.push_back(clumpMover);
	#ifdef EMBED_PYTHON
		shared_ptr<PythonRecorder> pythonRecorder=shared_ptr<PythonRecorder>(new PythonRecorder);
		//pythonRecorder->expression="print 'x2=',B[2].x,'v4=',B[4].I";
		//pythonRecorder->expression="print 'x=',B[2].x,'v=',B[2].v,'m=',B[2].m,'energy=',B[2].Etrans+B[2].Erot+B[2].Epot,'(trans=',B[2].Etrans,'rot=',B[2].Erot,'pot=',B[2].Epot,')'";
		//pythonRecorder->expression="if (S.i%50==0): print 'file=',S.file,'iteration =',S.i,'t= ',S.t,'x2 =',B[2].x";
		pythonRecorder->expression="if (S.i%50==0) and len(S.sel)>=2:\n\tprint B[S.sel[0]].E\n\tprint '=========END OF OUTPUT==============='";
		rootBody->engines.push_back(pythonRecorder);
	#endif

	rootBody->initializers.clear();
	rootBody->initializers.push_back(physicalActionInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
}

