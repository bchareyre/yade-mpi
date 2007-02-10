// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz>
 
/*	To compile this class, you MUST
		1. use recent version of scons
		2. say extraModules=yade-extra/clump (this will cause SConscript in this directory to be processed)
	Any further documentation is in doxygen comments.
*/

#ifndef CLUMP_HPP
#define CLUMP_HPP

#include<vector>
#include<yade/yade-core/Body.hpp>
#include<yade/yade-lib-factory/Factorable.hpp>
#include<boost/shared_ptr.hpp>
#include<yade/yade-package-common/PhysicalParametersEngineUnit.hpp>
#include<yade/yade-package-common/RigidBodyParameters.hpp>
#include<yade/yade-lib-base/Logging.hpp>
#include<yade/yade-lib-base/yadeWm3Extra.hpp>


#ifndef HIGHLEVEL_CLUMPS
	#error HIGHLEVEL_CLUMPS must be defined if clumps are to work!
#endif

/*! \brief Body representing clump (aggregate) composed by other existing bodies.

	When an existing body is added to ::Clump, it's ::Body::isDynamic flag is set to false
	(it is still subscribed to all its engines, to make it possible to remove it from the clump again).
	All forces acting on |subBodies| are made to act on the clump itself, which will ensure that they
	 influence all |subBodies| as if the clump were a rigid particle.
 
	What are clump requirements so that they function?
	-# Given any body, tell
		- if it is a clump member \code Body::clumpId!=Body::ID_NONE \endcode
		- what is it's clump id (Body::clumpId)
	 	- if it is a clump (is instance of ::Clump)
		- could be `Body::clumpId==Body::id`, but we can't guarantee consistency here; `id`s are assigned automatically by `BodyContainer` and it would have to be conscient of the body being clump... or not? This will probably be used not very often (engines use templates, not RTTI (?))
	-# given the root body, tell
		- what clumps it contains (enumerate all bodies and filter clumps, see above)
	-#	given a clump, tell
		- what bodies it contains (::Clump::subBodies)
		- what are se3 of these bodies (::Clump::subSe3s)
	-# add/delete bodies from/to clump (::Clump::add, ::Clump::del)
		- This includes saving se3 of the subBody: it \em must be in clump's local coordinates so that it is constant. The transformation from global to local is given by clump's se3 at the moment of addition. Clump's se3 is initially (origin,identity)
	-# Update clump's physical properties (Clump::update: will be copied from the Slum code)
		- This \em must reposition subBodies so that they have the same se3 globally
	-# Apply forces acting on subBodies to the clump instead (done in NewtonsForceLaw, NewtonsMomentumLaw) - uses world coordinates to calculate effect on the clump's centroid
	-# Integrate position and orientation of the clump (ClumpLeapFrogPositionAndOrientationIntegrator: copy code from LeapFrogPositionIntegrator, LeapFrogOrientationIntegrator)
		- LeapFrogPositionIntegrator and LeapFrogOrientationIntegrator skip clump subBodies automatically, since they have Body::isDynamic==false.
	-# Recalculate ::subBodies position and orientation (same class as previously will handle that automatically)

	Some more information can be found http://beta.arcig.cz/~eudoxos/phd/index.cgi/YaDe/HighLevelClumps
 
 */

class Clump: public Body {
		//! ID's of bodies that make part of this clump.
		std::vector<Body::id_t> subBodies;
		//! se3 of respective subBodies in local coordinates
		std::vector<Se3r> subSe3s;
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

	void registerAttributes(){Body::registerAttributes(); REGISTER_ATTRIBUTE(subBodies); REGISTER_ATTRIBUTE(subSe3s);}
	REGISTER_CLASS_NAME(Clump);
	REGISTER_BASE_CLASS_NAME(Body);
	// REGISTER_CLASS_INDEX(Clump,Body);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(Clump,false);

/*! \brief  Update ::Clump::subBodies positions so that the Clump behaves as a rigid body.
*/
class ClumpSubBodyMover: public PhysicalParametersEngineUnit {
	public:
		//! This is the engine's working part
		virtual void go(const shared_ptr<PhysicalParameters>& pp, Body* clump);
		ClumpSubBodyMover();
		virtual ~ClumpSubBodyMover(){};

	REGISTER_CLASS_NAME(ClumpSubBodyMover);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);
	// REGISTER_CLASS_INDEX(ClumpSubBodyMover,PhysicalParametersEngineUnit);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(ClumpSubBodyMover,false);


#endif /* CLUMP_HPP */
