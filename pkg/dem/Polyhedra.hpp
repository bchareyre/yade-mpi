// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a


#pragma once

#ifdef YADE_CGAL

#include<vector>
#include<yade/core/Shape.hpp>
#include<yade/core/IGeom.hpp>
#include<yade/core/GlobalEngine.hpp>
#include<yade/core/Material.hpp>
#include<yade/pkg/common/Aabb.hpp>
#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>
#include<yade/pkg/common/Wall.hpp>
#include<yade/pkg/common/Facet.hpp>
#include<yade/lib/base/openmp-accu.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_data_structure_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h> 
#include <CGAL/convex_hull_3.h>
#include <CGAL/Tetrahedron_3.h>
#include <CGAL/linear_least_squares_fitting_3.h>

#include<time.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

//CGAL definitions - does not work with another kernel!! Why???
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>	Polyhedron;
typedef CGAL::Delaunay_triangulation_3<K> Triangulation;
typedef K::Point_3 CGALpoint;
typedef K::Vector_3 CGALvector;
typedef CGAL::Aff_transformation_3<K> Transformation;
typedef K::Segment_3 Segment;
typedef CGAL::Triangle_3<K> Triangle;
typedef CGAL::Plane_3<K> Plane;
typedef CGAL::Line_3<K> Line;
typedef CGAL::Origin CGAL_ORIGIN;

//**********************************************************************************
class Polyhedra: public Shape{
	public:
		//constructor from Vertices
		Polyhedra(std::vector<Vector3r> V) { createIndex(); v.resize(V.size()); for(int i=0;i<(int) V.size();i++) v[i]=V[i]; Initialize();} 	//contructor of "random" polyhedra
		Polyhedra(Vector3r xsize, int xseed) { createIndex(); seed=xseed; size=xsize; v.clear(); Initialize();} 
		virtual ~Polyhedra();
		Vector3r GetCentroid(){Initialize(); return centroid;}
		Vector3r GetInertia(){Initialize(); return inertia;}
		vector<int> GetSurfaceTriangulation(){Initialize(); return faceTri;}
		void Initialize();		
		bool IsInitialized(){return init;}
		std::vector<Vector3r> GetOriginalVertices();
		double GetVolume(){Initialize(); return volume;}
		Quaternionr GetOri(){Initialize(); return orientation;}
		Polyhedron GetPolyhedron(){return P;};
		void Clear(){v.clear(); P.clear(); init = 0; size = Vector3r(1.,1.,1.); faceTri.clear();};

	protected:	
		//triangulation of facets for plotting
		vector<int> faceTri;
		//centroid = (0,0,0) for random Polyhedra
		Vector3r centroid;
		//CGAL structure Polyhedron
		Polyhedron P;
		//sign of performed initialization
		bool init;
		//centroid Volume
		double volume;
		//centroid inerta - diagonal of the tensor
		Vector3r inertia;
		//orientation, that provides diagonal inertia tensor
		Quaternionr orientation;
		void GenerateRandomGeometry();
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(Polyhedra,Shape,"Polyhedral (convex) geometry.",
			((std::vector<Vector3r>,v,,,"Tetrahedron vertices in global coordinate system."))
			((int,seed, time(NULL),,"Seed for random generator."))
			((Vector3r, size, Vector3r(1.,1.,1.),,"Size of the grain in meters - x,y,z - before random rotation")),
			/*init*/,
			/*ctor*/
			createIndex();
			init = 0,
			.def("Initialize",&Polyhedra::Initialize,"Initialization")
			.def("GetVolume",&Polyhedra::GetVolume,"return polyhedra's volume")
			.def("GetInertia",&Polyhedra::GetInertia,"return polyhedra's inertia tensor")
			.def("GetOri",&Polyhedra::GetOri,"return polyhedra's orientation")
			.def("GetCentroid",&Polyhedra::GetCentroid,"return polyhedra's centroid")
			.def("GetSurfaceTriangulation",&Polyhedra::GetSurfaceTriangulation,"triangulation of facets (for plotting)")
		);		
		REGISTER_CLASS_INDEX(Polyhedra,Shape);
};
REGISTER_SERIALIZABLE(Polyhedra);


//***************************************************************************
/*! Collision configuration for Polyhedra and something.
 * This is expressed as penetration volume properties: centroid, volume, depth ...
 *
 * Self-contained. */
class PolyhedraGeom: public IGeom{
	public:
		virtual ~PolyhedraGeom();
		//precompute data for shear evaluation
		void precompute(const State& rbp1, const State& rbp2, const Scene* scene, const shared_ptr<Interaction>& c, const Vector3r& currentNormal, bool isNew, const Vector3r& shift2);
		Vector3r& rotate(Vector3r& shearForce) const;
		//sep_plane is a code storing plane, that previously separated two polyhedras. It is used for faster detection of non-overlap.
		std::vector<int> sep_plane;
		bool isShearNew;
	protected:
		YADE_CLASS_BASE_DOC_ATTRS_CTOR(PolyhedraGeom,IGeom,"Geometry of interaction between 2 :yref:`vector<Polyhedra>`, including volumetric characteristics",
			((Real,penetrationVolume,NaN,,"Volume of overlap [m³]"))
			((Real,equivalentCrossSection,NaN,,"Cross-section area of the overlap (perpendicular to the normal) - not used"))
			((Real,equivalentPenetrationDepth,NaN,,"volume / equivalentCrossSection - not used"))
			((Vector3r,contactPoint,Vector3r::Zero(),,"Contact point (global coords), centriod of the overlapping polyhedron"))
			((Vector3r,shearInc,Vector3r::Zero(),,"Shear displacement increment in the last step"))
			((Vector3r,normal,Vector3r::Zero(),,"Normal direction of the interaction"))
			((Vector3r,twist_axis,Vector3r::Zero(),,""))
			((Vector3r,orthonormal_axis,Vector3r::Zero(),,"")),
			createIndex();
			sep_plane.assign(3,0);
		);
		//FUNCTOR2D(Tetra,Tetra);
		REGISTER_CLASS_INDEX(PolyhedraGeom,IGeom);
};
REGISTER_SERIALIZABLE(PolyhedraGeom);

//***************************************************************************
/*! Creates Aabb from Polyhedra. 
 *
 * Self-contained. */
class Bo1_Polyhedra_Aabb: public BoundFunctor{
	public:
		void go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*);
		FUNCTOR1D(Polyhedra);
		YADE_CLASS_BASE_DOC(Bo1_Polyhedra_Aabb,BoundFunctor,"Create/update :yref:`Aabb` of a :yref:`Polyhedra`");
};
REGISTER_SERIALIZABLE(Bo1_Polyhedra_Aabb);

//***************************************************************************
#ifdef YADE_OPENGL
	#include<yade/pkg/common/GLDrawFunctors.hpp>
	/*! Draw Polyhedra using OpenGL */
	class Gl1_Polyhedra: public GlShapeFunctor{	
		public:
			virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
			YADE_CLASS_BASE_DOC(Gl1_Polyhedra,GlShapeFunctor,"Renders :yref:`Polyhedra` object");
			RENDERS(Polyhedra);
	};
	REGISTER_SERIALIZABLE(Gl1_Polyhedra);
#endif

//***************************************************************************
/*! Elastic material */
class PolyhedraMat: public Material{
	public:
		 PolyhedraMat(double N, double S, double F){Kn=N; Ks=S; frictionAngle=F;};
		 double GetStrength(){return strength;};
	virtual ~PolyhedraMat(){};
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(PolyhedraMat,Material,"Elastic material with Coulomb friction.",
		((Real,Kn,1e8,,"Normal volumetric 'stiffness' (N/m3)."))
		((Real,Ks,1e5,,"Shear stiffness (N/m)."))
		((Real,frictionAngle,.5,,"Contact friction angle (in radians)."))
		((bool,IsSplitable,0,,"To be splitted ... or not"))
		((double,strength,100,,"Stress at whis polyhedra of volume 4/3*pi [mm] breaks.")),
		/*ctor*/ createIndex();
	);
	REGISTER_CLASS_INDEX(PolyhedraMat,Material);
};
REGISTER_SERIALIZABLE(PolyhedraMat);

//***************************************************************************
class PolyhedraPhys: public IPhys{
	public:
	virtual ~PolyhedraPhys(){};
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(PolyhedraPhys,IPhys,"Simple elastic material with friction for volumetric constitutive laws",
		((Real,kn,0,,"Normal stiffness"))
		((Vector3r,normalForce,Vector3r::Zero(),,"Normal force after previous step (in global coordinates)."))
		((Real,ks,0,,"Shear stiffness"))
		((Vector3r,shearForce,Vector3r::Zero(),,"Shear force after previous step (in global coordinates)."))	
		((Real,tangensOfFrictionAngle,NaN,,"tangens of angle of internal friction")),
		/*ctor*/ createIndex();	
	);
	REGISTER_CLASS_INDEX(PolyhedraPhys,IPhys);
};
REGISTER_SERIALIZABLE(PolyhedraPhys);

//***************************************************************************
class Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& b1,
			const shared_ptr<Material>& b2,
			const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(PolyhedraMat,PolyhedraMat);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys,IPhysFunctor,"",		
	);
};
REGISTER_SERIALIZABLE(Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys);

//***************************************************************************
/*! Calculate physical response based on penetration configuration given by TTetraGeom. */

class PolyhedraVolumetricLaw: public LawFunctor{
	OpenMPAccumulator<Real> plasticDissipation;
	virtual void go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*);
	Real elasticEnergy ();
	Real getPlasticDissipation();
	void initPlasticDissipation(Real initVal=0);
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(PolyhedraVolumetricLaw,LawFunctor,"Calculate physical response of 2 :yref:`vector<Polyhedra>` in interaction, based on penetration configuration given by :yref:`PolyhedraGeom`.",
	((Vector3r,shearForce,Vector3r::Zero(),,"Shear force from last step"))
	((bool,traceEnergy,false,,"Define the total energy dissipated in plastic slips at all contacts. This will trace only plastic energy in this law, see O.trackEnergy for a more complete energies tracing"))
	((int,plastDissipIx,-1,(Attr::hidden|Attr::noSave),"Index for plastic dissipation (with O.trackEnergy)"))
	((int,elastPotentialIx,-1,(Attr::hidden|Attr::noSave),"Index for elastic potential energy (with O.trackEnergy)"))
	,,
	.def("elasticEnergy",&PolyhedraVolumetricLaw::elasticEnergy,"Compute and return the total elastic energy in all \"FrictPhys\" contacts")
	.def("plasticDissipation",&PolyhedraVolumetricLaw::getPlasticDissipation,"Total energy dissipated in plastic slips at all FrictPhys contacts. Computed only if :yref:`Law2_ScGeom_FrictPhys_CundallStrack::traceEnergy` is true.")
	.def("initPlasticDissipation",&PolyhedraVolumetricLaw::initPlasticDissipation,"Initialize cummulated plastic dissipation to a value (0 by default).")
	);
	FUNCTOR2D(PolyhedraGeom,PolyhedraPhys);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PolyhedraVolumetricLaw);


//***************************************************************************
//compute plane equation from three points on the facet
struct Plane_equation {
    template <class Facet>
    typename Facet::Plane_3 operator()( Facet& f) {
        typename Facet::Halfedge_handle h = f.halfedge();
        typedef typename Facet::Plane_3  Plane;
        return Plane( h->vertex()->point(),
                      h->next()->vertex()->point(),
                      h->next()->next()->vertex()->point());
    }
};
//get Tetrahedron inertia
Matrix3r TetraInertiaTensor(Vector3r av,Vector3r bv,Vector3r cv,Vector3r dv);
//return intersection of two polyhedrons 
Polyhedron Polyhedron_Polyhedron_intersection(Polyhedron A, Polyhedron B, CGALpoint X, CGALpoint centroidA, CGALpoint centroidB,  std::vector<int> &code);
//return intersection of plane & polyhedron 
Polyhedron Polyhedron_Plane_intersection(Polyhedron A, Plane B, CGALpoint centroid, CGALpoint X);
//return approximate intersection of sphere & polyhedron 
bool Sphere_Polyhedron_intersection(Polyhedron A, double r, CGALpoint C, CGALpoint centroid,  double volume, CGALvector normal, double area);
//return volume and centroid of polyhedra
bool P_volume_centroid(Polyhedron P, double * volume, Vector3r * centroid);
//CGAL - miniEigen communication
Vector3r FromCGALPoint(CGALpoint A);
Vector3r FromCGALVector(CGALvector A);
CGALpoint ToCGALPoint(Vector3r A);
CGALvector ToCGALVector(Vector3r A);
//determination of intersection of two polyhedras
bool do_intersect(Polyhedron A, Polyhedron B);
bool do_intersect(Polyhedron A, Polyhedron B, std::vector<int> &sep_plane);
//connect triagular facets if possible
Polyhedron Simplify(Polyhedron P, double lim);
//list of facets and edges
void PrintPolyhedron(Polyhedron P);
//normal by least square fitting of separating segments
Vector3r FindNormal(Polyhedron Int, Polyhedron PA, Polyhedron PB);
//calculate area of projection of polyhedron into the plane
double CalculateProjectionArea(Polyhedron Int, CGALvector CGALnormal);
//split polyhedron
void SplitPolyhedra(const shared_ptr<Body>& body, Vector3r direction);
//new polyhedra
shared_ptr<Body> NewPolyhedra(vector<Vector3r> v, shared_ptr<Material> mat);

#endif // YADE_CGAL
