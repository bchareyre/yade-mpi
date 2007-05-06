// © 2007 Václav Šmilauer <eudoxos@arcig.cz>

#ifndef TETRA_HPP
#define TETRA_HPP

#include<vector>

#include<yade/core/InteractingGeometry.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/InteractionSolver.hpp>
#include<yade/core/PhysicalAction.hpp>

#include<yade/pkg-common/Tetrahedron.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/pkg-common/BoundingVolumeEngineUnit.hpp>
#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>
#include<yade/pkg-common/GLDrawInteractingGeometryFunctor.hpp>
#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>

#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>

#include<Wm3Math.h>
#include<Wm3Vector3.h>


/* Our mold of tetrahedron: just 4 vertices.
 *
 * Self-contained. */
class TetraMold: public InteractingGeometry{
	public:
		//! vertices of the tetrahedron. All constructors _must_ create exactly 4 elements of the vector.
		vector<Vector3r> v;
		TetraMold(){createIndex(); for(size_t i=0; i<4; i++) v.push_back(Vector3r(0,0,0)); }
		TetraMold(Vector3r v1, Vector3r v2, Vector3r v3, Vector3r v4){createIndex(); v.clear(); v.push_back(v1); v.push_back(v2); v.push_back(v3); v.push_back(v4);}
		virtual ~TetraMold (){};
	protected:
		void registerAttributes(){InteractingGeometry::registerAttributes(); REGISTER_ATTRIBUTE(v);}
		REGISTER_CLASS_NAME(TetraMold);
		REGISTER_BASE_CLASS_NAME(InteractingGeometry);
		REGISTER_CLASS_INDEX(TetraMold,InteractingGeometry);
};
REGISTER_SERIALIZABLE(TetraMold,false);


/*! Collision configuration for Tetra and something.
 * This is expressed as penetration volume properties: centroid, volume, orientation of principal axes, inertia.
 *
 * Self-contained. */

class TetraBang: public InteractionGeometry{
	public:
		Real volume;
		Se3r overlapSe3;
		Vector3r inertia;
		
		TetraBang(): InteractionGeometry(){};
		virtual ~TetraBang(){};
	protected:
		REGISTER_CLASS_NAME(TetraBang);
		REGISTER_BASE_CLASS_NAME(InteractionGeometry);
};
REGISTER_SERIALIZABLE(TetraBang,false);

/*! Creates TetraMold from Tetrahedron.
 *
 * Self-contained. */

class Tetrahedron2TetraMold: public InteractingGeometryEngineUnit
{
	public:
		void go(const shared_ptr<GeometricalModel>& gm,shared_ptr<InteractingGeometry>& ig,const Se3r& se3,const Body*){
			Tetrahedron* tet=static_cast<Tetrahedron*>(gm.get());
			//! @fixme this seems superfluous?!: if(!ig)
			ig=boost::shared_ptr<InteractingGeometry>(new TetraMold(tet->v[0],tet->v[1],tet->v[2],tet->v[3]));
		}
	REGISTER_CLASS_NAME(Tetrahedron2TetraMold);
	REGISTER_BASE_CLASS_NAME(InteractingGeometryEngineUnit);
	DEFINE_FUNCTOR_ORDER_2D(Tetrahedron,TetraMold);
};
REGISTER_SERIALIZABLE(Tetrahedron2TetraMold,false);

/*! Creates AABB from TetraMold. 
 *
 * Self-contained. */

class TetraAABB: public BoundingVolumeEngineUnit
{
	public:
		void go(const shared_ptr<InteractingGeometry>& ig, shared_ptr<BoundingVolume>& bv, const Se3r& se3, const Body*){
			TetraMold* t=static_cast<TetraMold*>(ig.get());
			AABB* aabb=static_cast<AABB*>(bv.get());
			#define __VOP(op,ix) op(t->v[0][ix],op(t->v[1][ix],op(t->v[2][ix],t->v[3][ix])))
				aabb->min=se3.position+Vector3r(__VOP(std::min,0),__VOP(std::min,1),__VOP(std::min,2));
				aabb->max=se3.position+Vector3r(__VOP(std::max,0),__VOP(std::max,1),__VOP(std::max,2));
			#undef __VOP
			aabb->center=(aabb->min+aabb->max)*0.5;
			aabb->halfSize=(aabb->max-aabb->min)*0.5;
		}
		REGISTER_CLASS_NAME(TetraAABB);
		REGISTER_BASE_CLASS_NAME(BoundingVolumeEngineUnit);
};
REGISTER_SERIALIZABLE(TetraAABB,false);


/*! Draw TetraMold using OpenGL */

class TetraDraw: public GLDrawInteractingGeometryFunctor
{	
	public:
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool);

		RENDERS(TetraMold);
		REGISTER_CLASS_NAME(TetraDraw);
		REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};
REGISTER_SERIALIZABLE(TetraDraw,false);


/*! Calculate physical response based on penetration configuration given by TetraBang. */

class TetraLaw: public InteractionSolver {
	public:
		//! @fixme: those two are here only because this class needs to access
		/// the ID number of Force and Momentum. Those variables are actually not used to store a value of
		/// Force and Momentum, just to get ID, although normally they are
		/// used to store this value. I already have a better solution for that.
		shared_ptr<PhysicalAction> actionForce;
		shared_ptr<PhysicalAction> actionMomentum;

		TetraLaw():InteractionSolver(),actionForce(new Force),actionMomentum(new Momentum){};

		void action(Body* body);
	protected:
		void registerAttributes(){InteractionSolver::registerAttributes(); /* … */ }
		REGISTER_CLASS_NAME(TetraLaw);
		REGISTER_BASE_CLASS_NAME(InteractionSolver);
};
REGISTER_SERIALIZABLE(TetraLaw,false);



/*! @fixme implement Tetra2BoxBang by representing box as 6 tetrahedra. */

/*! Create TetraBang (collision geometry) from colliding TetraMolds. */
class Tetra2TetraBang: public InteractionGeometryEngineUnit
{
	public:
		virtual bool go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const Se3r& se31, const Se3r& se32, const shared_ptr<Interaction>& c);

		REGISTER_CLASS_NAME(Tetra2TetraBang);
		REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);
		DEFINE_FUNCTOR_ORDER_2D(TetraMold,TetraMold);
};

REGISTER_SERIALIZABLE(Tetra2TetraBang,false);


// Miscillaneous functions
Matrix3r TetrahedronInertiaTensor(vector<Vector3r> v);


#endif
