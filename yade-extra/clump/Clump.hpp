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
#include <boost/shared_ptr.hpp>
#include<yade/yade-package-common/PhysicalParametersEngineUnit.hpp>
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
		Clump(): Body() { /*createIndex();*/ isDynamic=false; };
		virtual ~Clump(){};
		//! \brief add Body to the Clump
		void add(Body::id_t);
		//! \brief remove Body from the Clump
		void del(Body::id_t);
		//! Recalculate physical properties of clump.
		void update(bool intersecting);

	void registerAttributes(){Body::registerAttributes(); REGISTER_ATTRIBUTE(subBodies); REGISTER_ATTRIBUTE(subSe3s);}
	REGISTER_CLASS_NAME(Clump);
	REGISTER_BASE_CLASS_NAME(Body);
	// REGISTER_CLASS_INDEX(Clump,Body);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(Clump,false);

/*! \brief  Engine that calculate position and orientation of the clump in next timestep.

This engine is responsible for updating ::Clump::subBodies positions as well so that the clump behaves as a rigid body.
*/

class ClumpLeapFrogPositionAndOrientationIntegrator: public PhysicalParametersEngineUnit {
	public:
		/*! \todo Needs to be implemented */
		virtual void go(const shared_ptr<PhysicalParameters>&, Body*);
		ClumpLeapFrogPositionAndOrientationIntegrator(): PhysicalParametersEngineUnit() {/*createIndex();*/ }
		virtual ~ClumpLeapFrogPositionAndOrientationIntegrator(){};

	REGISTER_CLASS_NAME(ClumpLeapFrogPositionAndOrientationIntegrator);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);
	// REGISTER_CLASS_INDEX(ClumpLeapFrogPositionAndOrientationIntegrator,PhysicalParametersEngineUnit);
	DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(ClumpLeapFrogPositionAndOrientationIntegrator,false);


#endif /* CLUMP_HPP */
