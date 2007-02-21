// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz>
 
/*	To compile this class, you MUST
		1. use recent version of scons
		2. say extraModules=yade-extra/clump (this will cause SConscript in this directory to be processed)
	Any further documentation is in doxygen comments.
*/

#ifndef CLUMP_HPP
#define CLUMP_HPP

#include<vector>
#include<map>
#include<yade/yade-core/Body.hpp>
#include<yade/yade-core/MetaBody.hpp>
#include<yade/yade-core/FileGenerator.hpp>
#include<yade/yade-core/DeusExMachina.hpp>
#include<yade/yade-lib-factory/Factorable.hpp>
#include<boost/shared_ptr.hpp>
#include<yade/yade-package-common/PhysicalParametersEngineUnit.hpp>
#include<yade/yade-package-common/RigidBodyParameters.hpp>
#include<yade/yade-package-common/AABB.hpp>
#include<yade/yade-lib-base/Logging.hpp>
#include<yade/yade-lib-base/yadeWm3Extra.hpp>


#ifndef HIGHLEVEL_CLUMPS
	#error HIGHLEVEL_CLUMPS must be defined if clumps are to work!
#endif

/*! Body representing clump (rigid aggregate) composed by other existing bodies.

	Clump is one of bodies that reside in rootBody->bodies.
	When an existing body is added to ::Clump, it's ::Body::isDynamic flag is set to false
	(it is still subscribed to all its engines, to make it possible to remove it from the clump again).
	All forces acting on Clump::subBodies are made to act on the clump itself, which will ensure that they
	influence all Clump::subBodies as if the clump were a rigid particle.
 
	What are clump requirements so that they function?
	-# Given any body, tell
		- if it is a clump member: Body::isClumpMember()
	 	- if it is a clump: Body:: isClump(). (Correct result is assured at each call to Clump::add).
		 (we could use RTTI instead? Would that be more reliable?)
		- if it is a standalone Body: Body::isStandalone()
		- what is it's clump id (Body::clumpId)
	-# given the root body, tell
		- what clumps it contains (enumerate all bodies and filter clumps, see above)
	-#	given a clump, tell
		- what bodies it contains (keys of ::Clump::subBodies)
		- what are se3 of these bodies (values of ::Clump::subBodies)
	-# add/delete bodies from/to clump (::Clump::add, ::Clump::del)
		- This includes saving se3 of the subBody: it \em must be in clump's local coordinates so that it is constant. The transformation from global to local is given by clump's se3 at the moment of addition. Clump's se3 is initially (origin,identity)
	-# Update clump's physical properties (Clump::updateProperties)
		- This \em must reposition subBodies so that they have the same se3 globally
	-# Apply forces acting on subBodies to the clump instead (done in NewtonsForceLaw, NewtonsMomentumLaw) - uses world coordinates to calculate effect on the clump's centroid
	-# Integrate position and orientation of the clump
		- LeapFrogPositionIntegrator and LeapFrogOrientationIntegrator move clump as whole
			- clump members are skipped, since they have Body::isDynamic==false. 
		- ClumpSubBodyMover is an engine that updates positions of the clump memebers in each timestep (calls Clump::moveSubBodies internally)

	Some more information can be found http://beta.arcig.cz/~eudoxos/phd/index.cgi/YaDe/HighLevelClumps

	For an example how to generate a clump, see ClumpTestGen::createOneClump.

	@todo GravityEngine should be applied to members, not to clump as such?! Still not sure. Perhaps Clumps should have mass and inertia set to zeros so that engines unaware of clumps do not act on it. It would have some private mass and insertia that would be used in NewtonsForceLaw etc for clumps specially...

	@note PersistentSAPCollider bypass Clumps explicitly. This no longer depends on the absence of boundingVolume.
	@note Clump relies on its id being assigned (as well as id of its components); therefore, only bodies that have already been inserted to the container may be added to Clump which has been itself already added to the container.
 
 */

class Clump: public Body {
		//! mapping of body IDs to their relative positions; replaces subBodies and subSe3s;
		typedef std::map<Body::id_t,Se3r> clumpMap;
		clumpMap subBodies;
	public:
		Clump();
		virtual ~Clump(){};
		//! \brief add Body to the Clump
		void add(Body::id_t);
		//! \brief remove Body from the Clump
		void del(Body::id_t);
		//! Recalculate physical properties of Clump.
		void updateProperties(bool intersecting);
		//! Calculate positions and orientations of subBodies based on my own Se3.
		void moveSubBodies();
	private: // may be made public, but once properly tested...
		//! Recalculates inertia tensor of a body after translation away from (default) or towards its centroid.
		static Matrix3r inertiaTensorTranslate(const Matrix3r& I,const Real m, const Vector3r& off);
		//! Recalculate body's inertia tensor in rotated coordinates.
		static Matrix3r inertiaTensorRotate(const Matrix3r& I, const Matrix3r& T);
		//! Recalculate body's inertia tensor in rotated coordinates.
		static Matrix3r inertiaTensorRotate(const Matrix3r& I, const Quaternionr& rot);

	void registerAttributes(){Body::registerAttributes(); REGISTER_ATTRIBUTE(subBodies);}
	REGISTER_CLASS_NAME(Clump);
	REGISTER_BASE_CLASS_NAME(Body);
	// REGISTER_CLASS_INDEX(Clump,Body);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(Clump,false);

/*! Update ::Clump::subBodies positions so that the Clump behaves as a rigid body.
 *
 *
*/
class ClumpSubBodyMover: public DeusExMachina {
	public:
		//! Interates over rootBody->bodies and calls Clump::moveSubBodies() for clumps.
		virtual void applyCondition(Body* rootBody);
		ClumpSubBodyMover();
		virtual ~ClumpSubBodyMover(){};

	REGISTER_CLASS_NAME(ClumpSubBodyMover);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
	// REGISTER_CLASS_INDEX(ClumpSubBodyMover,PhysicalParametersEngineUnit);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(ClumpSubBodyMover,false);

/*! \brief Test some basic clump functionality; show how to use clumps as well. */
class ClumpTestGen : public FileGenerator {
	DECLARE_LOGGER;
	private :
		//Vector3r	nbTetrahedrons,groundSize,gravity;
		//Real	minSize,density,maxSize,dampingForce,disorder,dampingMomentum,youngModulus;
		//int		 timeStepUpdateInterval;
		//bool		 rotationBlocked;
		//void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
		void createOneClump(shared_ptr<MetaBody>& rootBody, Vector3r clumpPos, vector<Vector3r> relPos, vector<Real> radii);
		shared_ptr<Body> createOneSphere(Vector3r position, Real radius);
		void createActors(shared_ptr<MetaBody>& rootBody);
		//void positionRootBody(shared_ptr<MetaBody>& rootBody);
		//void calculatePropertiesAndReposition(const shared_ptr<SlumShape>& slum, shared_ptr<ElasticBodyParameters>& rbp, Real density);
		//void makeTet(shared_ptr<Tetrahedron>& tet, Real radius);
		shared_ptr<ClumpSubBodyMover> clumpMover;
	public :
		ClumpTestGen (){};
		~ClumpTestGen (){};
		string generate();
	protected :
		virtual void postProcessAttributes(bool deserializing){};
		void registerAttributes(){};
	REGISTER_CLASS_NAME(ClumpTestGen);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(ClumpTestGen,false);





#endif /* CLUMP_HPP */
