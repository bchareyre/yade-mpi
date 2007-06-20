// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include"Clump.hpp"
#include<algorithm>

char* yadePluginClasses[]={
	"Clump",
	"ClumpMemberMover",
	"ClumpTestGen",
	NULL /*sentinel*/
};

CREATE_LOGGER(Clump);
CREATE_LOGGER(ClumpMemberMover);
CREATE_LOGGER(ClumpTestGen);

/**************************************************************************************
 ************************************* ClumpMemberMover ******************************
 **************************************************************************************/

// Constructor must be in the .cpp file (?)
ClumpMemberMover::ClumpMemberMover(){/*createIndex();*/ }

/*! We only call clump's method, since it belongs there logically. It makes encapsulation of private members nicer, too.
 * @param pp passed by the dispatcher
 * @param clump passed by the dispatcher
 */
void ClumpMemberMover::applyCondition(Body* _rootBody){
	MetaBody* rootBody = YADE_CAST<MetaBody*>(_rootBody);
	for(BodyContainer::iterator I=rootBody->bodies->begin(); I!=rootBody->bodies->end(); ++I){
		shared_ptr<Body> b = *I;
		if(b->isClump()){
			//LOG_TRACE("Applying movement to clump #"<<b->getId());
			static_cast<Clump*>(b.get())->moveMembers();
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
	interactingGeometry->diffuseColor=Vector3r(0,0,0);

	geometricalModel=shared_ptr<GeometricalModel>(new GeometricalModel);
	geometricalModel->diffuseColor=Vector3r(0,0,0); geometricalModel->wire=false; geometricalModel->visible=false; geometricalModel->shadowCaster=false;
#endif

}

/*! @pre Body must be dynamic.
 * @pre Body must not be part or this clump already.
 * @pre Body must have valid (non-NULL) Body::physicalParameters
 * @todo se3 calculation is not tested yet
 */
void Clump::add(body_id_t subId){
	shared_ptr<Body> subBody=Body::byId(subId);

	// preconditions
	assert(subBody->isDynamic);
	assert(physicalParameters);
	assert(members.count(subId)==0);

	// begin actual setup
	subBody->clumpId=getId();
	subBody->isDynamic=false;
	// for now, push just unitialized se3; will be updated by updateProperties
	members[subId]=Se3r();

	clumpId=getId(); // identifies a clump

	LOG_DEBUG("Added body #"<<subId<<" to clump #"<<getId());
}

/*! @pre Body with given id must be in the clump.
 */
void Clump::del(body_id_t subId){
	// erase the subBody; removing body that is not part of the clump is error
	assert(members.erase(subId)==1);
	// restore body's internal parameters;
	shared_ptr<Body> subBody=Body::byId(subId);
	subBody->clumpId=Body::ID_NONE;
	subBody->isDynamic=true;
	LOG_DEBUG("Removed body #"<<subId<<" from clump #"<<getId());
}

/*! @brief Calculate positions and orientations of members based on Clump's Se3; resets acceleration and angularAccelration to zero.
 *
 * This method is called by the ClumpMemberMover engine after each timestep.
 * @note Velocities of members are not updated, since members have isdynamic==false. It is possible, though, that someone needs to have a moving clump that is later broken apart and that liberated particle continue to move in the same way as they did within the clump. In that case, this will have to be completed.
 */
void Clump::moveMembers(){
	const Se3r& mySe3(physicalParameters->se3);
	const shared_ptr<RigidBodyParameters>& myRBP=static_pointer_cast<RigidBodyParameters>(physicalParameters);
	for(Clump::memberMap::iterator I=members.begin(); I!=members.end(); I++){
		// now, I->first is Body::id_t, I->second is Se3r of that body in the clump
		shared_ptr<Body> member=Body::byId(I->first);
		const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(member->physicalParameters));
		//LOG_TRACE("Old #"<<I->first<<"position: "<<subRBP->se3.position);
		subRBP->se3.position=mySe3.position+mySe3.orientation*I->second.position;
		subRBP->se3.orientation=mySe3.orientation*I->second.orientation;
		//LOG_TRACE("New #"<<I->first<<"position: "<<subRBP->se3.position);
		//LOG_TRACE("Clump #"<<getId()<<" moved #"<<I->first<<".");

		//! FIXME: we set velocity because of damping here; but since positions are integrated after all forces applied, these velocities will be used in the NEXT step for CundallNonViscousDamping. Does that matter?!
		subRBP->velocity=myRBP->velocity+myRBP->angularVelocity.Cross(I->second.position);
		subRBP->angularVelocity=myRBP->angularVelocity;
	}
	/* @bug Temporarily we reset acceleration and angularAcceleration of the clump here;
	 * should be a new negine that will take care of that?
	 */
	const shared_ptr<RigidBodyParameters>& clumpRBP(YADE_PTR_CAST<RigidBodyParameters>(physicalParameters));
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

/*! Clump's se3 will be updated (origin at centroid and axes coincident with principal inertia axes) and subSe3 modified in such a way that members positions in world coordinates will not change.

	The clump values that are changed are:
	-# Clump::members (holds position and orientation in clump's coordinate system)
	-# Clump::physicalParameters->mass (sum of masses of all members)
	-# Clump::physicalParameters->inertia (inertia of the aggregate - in clump coordinate system)
	-# Clump::physicalParameters->se3 (position and orientation of the clump; it is such that absolute positions and orientation of members will not chage)

	The algorithm is as follows:
	-# Clump::members values and Clump::physicalParameters::se3 are invalid from this point
	-# M=0; S=vector3r(0,0,0); I=zero tensor; (ALL calculations are in world coordinates!)
	-# loop over Clump::members (position x_i, mass m_i, inertia at subBody's centroid I_i) [this loop will be replaced by numerical integration (rasterization) for the intersecting case; the rest will be the same]
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
	@note subBodie's velocities are not taken into account. This means that clump will be at still after being created, even if its composing particles did have some velocities. If this is concern for someone, it needs to be completed in the code below. See Clump::moveMembers for complementary issue.
	@todo Needs to be tested for physical correctness
	@param intersecting if true, evaluate mass and inertia numerically; otherwise, use analytical methods (parallel axes theorem) which disregard any intersections, but are much faster. */
void Clump::updateProperties(bool intersecting){
	LOG_DEBUG("Updating clump #"<<getId()<<" parameters");
	assert(members.size()>0);

	/* quantities suffixed by
		g: global (world) coordinates
		s: local subBody's coordinates
		c: local clump coordinates */
	double M=0; // mass
	Vector3r Sg(0,0,0); // static moment
	Matrix3r Ig(true /* fill with zeros */ ), Ic(true); // tensors of inertia; is upper triangular, zeros instead of symmetric elements
	Se3r& mySe3(physicalParameters->se3);
	const shared_ptr<RigidBodyParameters>& clumpRBP(YADE_PTR_CAST<RigidBodyParameters>(physicalParameters));

	if(members.size()==1){
		LOG_DEBUG("Clump of size one will be treated specially.")
		memberMap::iterator I=members.begin();
		shared_ptr<Body> subBody=Body::byId(I->first);
		const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		// se3 of the clump as whole is the same as the member's se3
		mySe3.position=subRBP->se3.position;
		mySe3.orientation=subRBP->se3.orientation;
		// relative member's se3 is identity
		I->second.position=Vector3r::ZERO; I->second.orientation=Quaternionr::IDENTITY;
		clumpRBP->inertia=subRBP->inertia;
		clumpRBP->mass=subRBP->mass;
		clumpRBP->velocity=Vector3r::ZERO;
		clumpRBP->angularVelocity=Vector3r::ZERO;
		return;
	}

	if(intersecting){
		LOG_WARN("Self-intersecting clumps not yet implemented, intersections will be ignored.");
		intersecting=false;}

	// begin non-intersecting loop here
	if(!intersecting){
		for(memberMap::iterator I=members.begin(); I!=members.end(); I++){
			// now, I->first is Body::id_t, I->second is Se3r of that body
			shared_ptr<Body> subBody=Body::byId(I->first);
			const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
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
	}
	TRVAR1(M);
	TRWM3MAT(Ig);
	TRWM3VEC(Sg);

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
	clumpRBP->inertia=Vector3r(Ic(0,0),Ic(1,1),Ic(2,2));
	clumpRBP->mass=M;


	// this block will be removed once EigenDecomposition works for diagonal matrices
	#if 1
		if(isnan(R_g2c(0,0))||isnan(R_g2c(0,1))||isnan(R_g2c(0,2))||isnan(R_g2c(1,0))||isnan(R_g2c(1,1))||isnan(R_g2c(1,2))||isnan(R_g2c(2,0))||isnan(R_g2c(2,1))||isnan(R_g2c(2,2))){
			LOG_FATAL("EigenDecomposition gave some NaNs, we will use imaginary values for clump inertia and orientation instead. I thought this may happen only for 1-member clumps which are now treated specially. Something is broken!");
			//FIXME: since EigenDecomposition is broken, use inertia of the first body instead;
			//!!!!! note that this is HIGHLY incorrect for all non-single clumps !!!!!
			memberMap::iterator I=members.begin();
			shared_ptr<Body> subBody=Body::byId(I->first);
			const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
			clumpRBP->inertia=subRBP->inertia*10.; // 10 is arbitrary; just to have inertia of clump bigger
			// orientation of the clump is broken as well, since is result of EigenDecomposition as well (rotation matrix)
			mySe3.orientation.FromRotationMatrix(Matrix3r(1,0,0,0,1,0,0,0,1));
		}
	#endif
	TRWM3VEC(clumpRBP->inertia);

	// TODO: these might be calculated from members... but complicated... - someone needs that?!
	clumpRBP->velocity=Vector3r(0,0,0);
	clumpRBP->angularVelocity=Vector3r(0,0,0);

	// update subBodySe3s; subtract clump orientation (=apply its inverse first) to subBody's orientation
	// Conjugate is equivalent to Inverse for normalized quaternions
	for(memberMap::iterator I=members.begin(); I!=members.end(); I++){
		// now, I->first is Body::id_t, I->second is Se3r of that body
		shared_ptr<Body> subBody=Body::byId(I->first);
		const shared_ptr<RigidBodyParameters>& subRBP(YADE_PTR_CAST<RigidBodyParameters>(subBody->physicalParameters));
		I->second.orientation=mySe3.orientation.Conjugate()*subRBP->se3.orientation;
		I->second.position=mySe3.orientation.Conjugate()*(subRBP->se3.position-mySe3.position);
	}

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

#include<yade/core/MetaBody.hpp>
#include<yade/extra/Shop.hpp>

bool ClumpTestGen::generate()
{
	//Shop::setDefault("param_pythonRunExpr",string("if S.i%50==0 and S.i<1000 and S.i>500:\n\tprint S.i,len(S.sel),B[1].x, B[1].E"));

	rootBody=Shop::rootBody();
	Shop::rootBodyActors(rootBody);
	// clumps do not need to subscribe currently (that will most likely change, though)
	rootBody->engines.push_back(shared_ptr<ClumpMemberMover>(new ClumpMemberMover));
	

	shared_ptr<MetaBody> oldRootBody=Omega::instance().getRootBody();
	Omega::instance().setRootBody(rootBody);

	shared_ptr<Body> ground=Shop::box(Vector3r(0,0,-1),Vector3r(3,3,.2));
	ground->isDynamic=false;
	// revert random colors for this single case...
	ground->geometricalModel->diffuseColor=Vector3r(.6,.6,.6);
	ground->interactingGeometry->diffuseColor=Vector3r(.3,.3,.3);
	rootBody->bodies->insert(ground);

	vector<Vector3r> relPos; vector<Real> radii; Vector3r clumpPos;

	// standalone (non-clump!) sphere as well
	shared_ptr<Body> sphere=Shop::sphere(Vector3r(0,0,0),.5);
	rootBody->bodies->insert(sphere);
	// one-sphere clump
	clumpPos=Vector3r(-2,0,0);
	relPos.push_back(Vector3r(0,0,0)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();
	// two-sphere clump
	clumpPos=Vector3r(2,0,0);
	relPos.push_back(Vector3r(0,-.5,.5)); radii.push_back(.5);
	relPos.push_back(Vector3r(0,.5,0)); radii.push_back(.5);
	createOneClump(rootBody,clumpPos,relPos,radii);
	relPos.clear(); radii.clear();
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

	// restore Omega
	Omega::instance().setRootBody(oldRootBody);
	
	message="OK";
	return true;
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
	shared_ptr<Body> clumpAsBody(static_pointer_cast<Body>(clump));
	rootBody->bodies->insert(clumpAsBody);

	clump->isDynamic=true;
	// if subscribedBodies work some day: clumpMover->subscribedBodies.push_back(clump->getId());
	
	for(size_t i=0; i<relPos.size(); i++){
		shared_ptr<Body> sphere=Shop::sphere(clumpPos+relPos[i],radii[i]);
		body_id_t lastId=(body_id_t)rootBody->bodies->insert(sphere);
		clump->add(lastId);
		LOG_TRACE("Generated clumped sphere #"<<lastId);
	}
	clump->updateProperties(false);
}

