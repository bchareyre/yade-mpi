// © 2007 Václav Šmilauer <eudoxos@arcig.cz>

#pragma once

#include<vector>

#include<yade/core/Shape.hpp>
#include<yade/core/InteractionGeometry.hpp>
#include<yade/core/GlobalEngine.hpp>

#include<yade/pkg-common/Aabb.hpp>
#include<yade/pkg-common/Dispatching.hpp>


/* Our mold of tetrahedron: just 4 vertices.
 *
 * Self-contained. */
class Tetra: public Shape{
	public:
		Tetra(Vector3r v0, Vector3r v1, Vector3r v2, Vector3r v3) { createIndex(); v.resize(4); v[0]=v0; v[1]=v1; v[2]=v2; v[3]=v3; } 
		virtual ~Tetra();
	protected:
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(Tetra,Shape,"Tetrahedron geometry.",
			((std::vector<Vector3r>,v,std::vector<Vector3r>(4),"Tetrahedron vertices in global coordinate system.")),
			/*ctor*/createIndex();
		);
		REGISTER_CLASS_INDEX(Tetra,Shape);
};
REGISTER_SERIALIZABLE(Tetra);


/*! Collision configuration for Tetra and something.
 * This is expressed as penetration volume properties: centroid, volume, orientation of principal axes, inertia.
 *
 * Self-contained. */
class TTetraGeom: public InteractionGeometry{
	public:
		virtual ~TTetraGeom();
	protected:
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(TTetraGeom,InteractionGeometry,"Geometry of interaction between 2 :yref:`tetrahedra<Tetra>`, including volumetric characteristics",
			((Real,penetrationVolume,NaN,"Volume of overlap [m³]"))
			((Real,equivalentCrossSection,NaN,"Cross-section of the overlap (perpendicular to the axis of least inertia"))
			((Real,maxPenetrationDepthA,NaN,"??"))
			((Real,maxPenetrationDepthB,NaN,"??"))
			((Real,equivalentPenetrationDepth,NaN,"??"))
			((Vector3r,contactPoint,,"Contact point (global coords)"))
			((Vector3r,normal,,"Normal of the interaction, directed in the sense of least inertia of the overlap volume")),
			createIndex();
		);
		FUNCTOR2D(Tetra,Tetra);
		REGISTER_CLASS_INDEX(TTetraGeom,InteractionGeometry);
};
REGISTER_SERIALIZABLE(TTetraGeom);

/*! Creates Aabb from Tetra. 
 *
 * Self-contained. */
class Bo1_Tetra_Aabb: public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*){
			Tetra* t=static_cast<Tetra*>(ig.get());
			Aabb* aabb=static_cast<Aabb*>(bv.get());
			Quaternionr invRot=se3.orientation.conjugate();
			Vector3r v_g[4]; for(int i=0; i<4; i++) v_g[i]=se3.orientation*t->v[i]; // vertices in global coordinates
			#define __VOP(op,ix) op(v_g[0][ix],op(v_g[1][ix],op(v_g[2][ix],v_g[3][ix])))
				aabb->min=se3.position+Vector3r(__VOP(std::min,0),__VOP(std::min,1),__VOP(std::min,2));
				aabb->max=se3.position+Vector3r(__VOP(std::max,0),__VOP(std::max,1),__VOP(std::max,2));
			#undef __VOP
		}
		virtual ~Bo1_Tetra_Aabb();
		FUNCTOR2D(Tetra,Aabb);
	YADE_CLASS_BASE_DOC(Bo1_Tetra_Aabb,BoundFunctor,"Create/update :yref:`Aabb` of a :yref:`Tetra`");
};
REGISTER_SERIALIZABLE(Bo1_Tetra_Aabb);

#ifdef YADE_OPENGL
	#include<yade/pkg-common/GLDrawFunctors.hpp>
	/*! Draw Tetra using OpenGL */
	class Gl1_Tetra: public GlShapeFunctor{	
		public:
			virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
		YADE_CLASS_BASE_DOC(Gl1_Tetra,GlShapeFunctor,"Renders :yref:`Tetra` object");
		RENDERS(Tetra);
	};
	REGISTER_SERIALIZABLE(Gl1_Tetra);
#endif

/*! Calculate physical response based on penetration configuration given by TTetraGeom. */

class TetraVolumetricLaw: public GlobalEngine {
	public:
		void action();
	DECLARE_LOGGER;
	YADE_CLASS_BASE_DOC(TetraVolumetricLaw,GlobalEngine,"Calculate physical response of 2 :yref:`tetrahedra<Tetra>` in interaction, based on penetration configuration given by :yref:`TTetraGeom`.");
};
REGISTER_SERIALIZABLE(TetraVolumetricLaw);



/*! @fixme implement Tetra2BoxBang by representing box as 6 tetrahedra. */

/*! Create TTetraGeom (collision geometry) from colliding Tetra's. */
class Ig2_Tetra_Tetra_TTetraGeom: public InteractionGeometryFunctor
{
	public:
		virtual bool go(const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<Shape>& cm1, const shared_ptr<Shape>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const bool& force, const shared_ptr<Interaction>& c){ throw std::logic_error("Ig2_Tetra_Tetra_TTetraGeom::goReverse called, but the functor is symmetric."); }
		FUNCTOR2D(Tetra,Tetra);
		DEFINE_FUNCTOR_ORDER_2D(Tetra,Tetra);
		YADE_CLASS_BASE_DOC(Ig2_Tetra_Tetra_TTetraGeom,InteractionGeometryFunctor,"Create/update geometry of collision between 2 :yref:`tetrahedra<Tetra>` (:yref:`TTetraGeom` instance)");
		DECLARE_LOGGER;
	private:
		list<Tetra> Tetra2TetraIntersection(const Tetra& A, const Tetra& B);
		list<Tetra> TetraClipByPlane(const Tetra& T, const Vector3r& P, const Vector3r& n);
		//! Intersection of line given by points A, B and plane given by P and its normal.
		Vector3r PtPtPlaneIntr(const Vector3r& A, const Vector3r& B, const Vector3r& P, const Vector3r& normal){const double t=(P-A).dot(normal) / (B-A).dot(normal); /* TRWM3VEC(A); TRWM3VEC(B); TRWM3VEC(P); TRWM3VEC(normal); LOG_TRACE("t="<<t); TRWM3VEC((A+t*(B-A))); */ return A+t*(B-A); }
};

REGISTER_SERIALIZABLE(Ig2_Tetra_Tetra_TTetraGeom);

// Miscillaneous functions
//! Tetrahedron's volume.
/// http://en.wikipedia.org/wiki/Tetrahedron#Surface_area_and_volume
Real TetrahedronVolume(const Vector3r v[4]);
Real TetrahedronVolume(const vector<Vector3r>& v);
Matrix3r TetrahedronInertiaTensor(const vector<Vector3r>& v);
//Matrix3r TetrahedronInertiaTensor(const Vector3r v[4]);
Matrix3r TetrahedronCentralInertiaTensor(const vector<Vector3r>& v);
//Matrix3r TetrahedronCentralInertiaTensor(const Vector3r v[4]);
Quaternionr TetrahedronWithLocalAxesPrincipal(shared_ptr<Body>& tetraBody);


