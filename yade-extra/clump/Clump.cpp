// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#include"Clump.hpp"

#include<algorithm>

char* yadePluginClasses[]={
	"Clump",
	"ClumpSubBodyMover",
	NULL /*sentinel*/
};

CREATE_LOGGER(Clump);
CREATE_LOGGER(ClumpSubBodyMover);

//! Constructor must be in the .cpp file (?)
ClumpSubBodyMover::ClumpSubBodyMover(): PhysicalParametersEngineUnit() {/*createIndex();*/ }

/*! We only call clump's method, since it belongs there logically. It makes encapsulation of private members nicer, too.
 * @param pp passed by the dispatcher
 * @param clump passed by the dispatcher
 */
void ClumpSubBodyMover::go(const shared_ptr<PhysicalParameters>& pp, Body* clump){
	if(!clump->isDynamic) return; // perhaps clump that has been desactivated?!
	dynamic_cast<Clump*>(clump)->moveSubBodies();
}


/*! Create zero'ed RigidBodyParameters; they should not be manipulated directly, since they are all calculated in Clump::update.
 * @todo do we need to do the same for GeomtricalModel, InteractingGeometry and BoundingVolume? They will never be used. Sort that out for sure.
 */
Clump::Clump(): Body(){
	isDynamic=false;
	physicalParameters=shared_ptr<RigidBodyParameters>(new RigidBodyParameters);
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
	assert(find(subBodies.begin(),subBodies.end(),subId)==subBodies.end());

	// begin actual setup
	subBody->clumpId=getId();
	subBody->isDynamic=false;
	subBodies.push_back(subId);
	// for now, push just unitialized se3; will be updated by updateProperties
	subSe3s.push_back(Se3r());
	LOG_DEBUG("Added body #"<<subId<<" to clump #"<<getId());
}

/*! @pre Body with given id must be in the clump.
 */
void Clump::del(Body::id_t subId){
	// find position of the body in vector
	std::vector<Body::id_t>::iterator subBodiesIter=find(subBodies.begin(),subBodies.end(),subId);
	// removing body that is not part of the clump is error
	assert(subBodiesIter!=subBodies.end());
	// se3 is at the same position
	size_t idx=distance(subBodies.begin(),subBodiesIter);
	std::vector<Se3r>::iterator subSe3sIter=subSe3s.begin()+idx;
	// erase these elements;
	subBodies.erase(subBodiesIter);
	subSe3s.erase(subSe3sIter);
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
	assert(subBodies.size()==subSe3s.size());
	const Se3r& mySe3(physicalParameters->se3);
	for(size_t i=0; i<subBodies.size(); i++){
		shared_ptr<Body> subBody=Body::byId(subBodies[i]);
		shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
		subRBP->se3.position=mySe3.position+mySe3.orientation*subSe3s[i].position;
		subRBP->se3.orientation=mySe3.orientation*subSe3s[i].orientation;
	}
}

/*! Clump's se3 will be updated (origin at centroid and axes coincident with principal inertia axes) and subSe3 modified in such a way that subBodies positions in world coordinates will not change.

	The algorithm is as follows:
	-# Clump::subSe3s and Clump::physicalParameters::se3 are invalid from this point
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
	@todo Needs to be implemented and tested.
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
		for(size_t i=0; i<subBodies.size(); i++){
			shared_ptr<Body> subBody=Body::byId(subBodies[i]);
			shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
			M+=subRBP->mass;
			Sg+=subRBP->mass*subRBP->se3.position;
			// transform from local to global coords
			// FIXME: verify this!
			Quaternionr subRBP_orientation_conjugate=subRBP->se3.orientation.Conjugate();
			Ig+=Clump::inertiaTensorTranslate(Clump::inertiaTensorRotate(Matrix3r(subRBP->inertia),subRBP_orientation_conjugate),-subRBP->mass,-1.*subRBP->se3.position);
		}
	}else{LOG_ERROR("Self-intersecting clumps not implemented.");}

	mySe3.position=Sg/M; // clump's centroid
	// this will calculate translation only, since rotation is zero
	Matrix3r Ic_orientG=Clump::inertiaTensorTranslate(Ig, -M /* negative mass means towards centroid */, mySe3.position); // inertia at clump's centroid but with world orientation

	Matrix3r R_g2c; //rotation matrix
	Ic_orientG(1,0)=Ic_orientG(0,1); Ic_orientG(2,0)=Ic_orientG(0,2); Ic_orientG(2,1)=Ic_orientG(1,2); // symmetrize
	Ic_orientG.EigenDecomposition(R_g2c,Ic);

	// these two should give the same result!
	TRWM3MAT(Ic);
	TRWM3MAT(Clump::inertiaTensorRotate(Ic_orientG,R_g2c));

	// set quaternion from rotation matrix
	mySe3.orientation.FromRotationMatrix(R_g2c);
	// now Ic is diagonal
	dynamic_pointer_cast<RigidBodyParameters>(physicalParameters)->inertia=Vector3r(Ic(0,0),Ic(1,1),Ic(2,2));

	// update subBodySe3s; subtract clump orientation (apply its inverse first) to subBody's orientation
	// Conjugate is equivalent to Inverse for normalized quaternions
	for(size_t i=0; i<subBodies.size(); i++){
		shared_ptr<Body> subBody=Body::byId(subBodies[i]);
		shared_ptr<RigidBodyParameters> subRBP=dynamic_pointer_cast<RigidBodyParameters>(subBody->physicalParameters);
		subSe3s[i].orientation=mySe3.orientation.Conjugate()*subRBP->se3.orientation;
		subSe3s[i].position=mySe3.orientation.Conjugate()*(subRBP->se3.position-mySe3.position);
	}
}

/*! @brief Recalculates inertia tensor of a body after translation away from (default) or towards its centroid.
 *
 * @oaram I inertia tensor in the original coordinates; it is assumed to be upper-triangular (elements below the diagonal are ignored).
 * @param m mass of the body; if positive, translation is away from the centroid;
 * @param off offset of the new origin from the original origin
 * @return inertia tensor in the new coordinate system; the matrix is symmetric.
 */
Matrix3r Clump::inertiaTensorTranslate(const Matrix3r& I,const Real m, const Vector3r& off){
	Real ooff=off.Dot(off);
	Matrix3r I2=I;
	// translation away from centroid
	/* I^c_jk=I'_jk-M*(delta_jk R.R - R_j*R_k) [http://en.wikipedia.org/wiki/Moments_of_inertia#Parallel_axes_theorem] */
	I2+=m*Matrix3r(/* dIxx */ ooff-off[0]*off[0], /* dIxy */ -off[0]*off[1], /* dIxz */ -off[0]*off[2],
		/* sym */ 0, /* dIyy */ ooff-off[1]*off[1], /* dIyz */ -off[1]*off[2],
		/* sym */ 0, /* sym */ 0, /* dIzz */ ooff-off[2]*off[2]);
	I2(1,0)=I2(0,1); I2(2,0)=I2(0,2); I2(2,1)=I2(1,2);
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
 ********************* ClumpLeapFrogPositionAndOrientationIntegrator ******************
 **************************************************************************************/

/*  this WAS nonsensical duplication of LeapFrogPositionIntegrator and LeapFrogOrientationIntegrator.
 *  They will update clump's position and orientation just as for any other body. We just need to update particle's
 *  positions and orientations according to that


void ClumpLeapFrogPositionAndOrientationIntegrator::go(const shared_ptr<PhysicalParameters>& clumpPP, Body* clump){
	if(!clump->isDynamic) return; // parhaps clump that has been desactivated?!
	Body::id_t clumpId=clump->getId();

	RigidBodyParameters * clumpRBP = dynamic_cast<RigidBodyParameters*>(clumpPP.get());
	// is this a new clump we have never seen? If yes, initialize its previous values to zeros
	if(prevVelocities.find(clumpId)==prevVelocities.end()){
		// if it didn't exist in prevVelocities, it must not exist in prevAngularVelocities either
		assert(prevAngularVelocities.find(clumpId)==prevAngularVelocities.end());
		prevVelocities[clumpId]=Vector3r(0,0,0);
		prevAngularVelocities[clumpId]=Vector3r(0,0,0);
	} else { // otherwise, use previous values
		clumpRBP->velocity=prevVelocities[clumpId]+clumpRBP->acceleration*.5*dt;
		clumpRBP->angularVelocity=prevAngularVelocities[clumpId]+clumpRBP->angularAcceleration*.5*dt;

	}
	// velocity
	prevVelocities[clumpId]=clumpRBP->velocity+clumpRBP->acceleration*.5*dt;
	clumpRBP->se3.position+=prevVelocities[clumpId]*dt;

	// angular velocity
	prevAngularVelocities[clumpId]=clumpRBP->angularVelocity+clumpRBP->angularAcceleration*.5*dt;
	Vector3r axis=clumpRBP->angularVelocity;
	Real angle=axis.Normalize();
	Quaternionr q;	q.FromAxisAngle(axis,angle*dt);
	clumpRBP->se3.orientation=q*clumpRBP->se3.orientation;
	clumpRBP->se3.orientation.Normalize();
} */

