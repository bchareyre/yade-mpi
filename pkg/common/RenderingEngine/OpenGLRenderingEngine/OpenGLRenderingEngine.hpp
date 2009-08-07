// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/RenderingEngine.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/MetaEngine1D.hpp>
#include<yade/core/Body.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

#include<yade/pkg-common/GLDrawFunctors.hpp>

class OpenGLRenderingEngine : public RenderingEngine
{	
	public :
		Vector3r Light_position,Background_color;
		bool Show_DOF,Show_ID,Body_state,Body_bounding_volume,Body_interacting_geom,Body_geometrical_model,Cast_shadows,Shadow_volumes,Fast_shadow_volume,Body_wire,Interaction_wire,Draw_inside,Interaction_geometry,Interaction_physics;
		body_id_t current_selection;
		int Draw_mask;

		vector<Se3r> clipPlaneSe3;
		vector<int> clipPlaneActive; // should be bool, but serialization doesn't handle vector<bool>
		const int clipPlaneNum;
		//vector<body_id_t> bodiesByDepth; // camera::cameraCoordinatesOf[2]
		bool scaleDisplacements,scaleRotations;
		Vector3r displacementScale; Real rotationScale;

		bool pointClipped(const Vector3r& p);
		vector<Vector3r> clipPlaneNormals;
		void setBodiesRefSe3(const shared_ptr<MetaBody>& rootBody);
		void setBodiesDispSe3(const shared_ptr<MetaBody>& rootBody);
		long numBodiesWhenRefSe3LastSet,numIterWhenRefSe3LastSet;
		static bool glutInitDone;
		static size_t selectBodyLimit;
		Vector3r viewDirection; // updated from GLViewer regularly
		Vector3r highlightEmission0;
		Vector3r highlightEmission1;
		// normalized saw signal with given periodicity, with values ∈ 〈0,1〉 */
		Real normSaw(Real t, Real period){ Real xi=(t-period*((int)(t/period)))/period; /* normalized value, (0-1〉 */ return (xi<.5?2*xi:2-2*xi); }
		Real normSquare(Real t, Real period){ Real xi=(t-period*((int)(t/period)))/period; /* normalized value, (0-1〉 */ return (xi<.5?0:1); }

		//! wrap number to interval x0…x1
		Real wrapCell(const Real x, const Real x0, const Real x1);
		//! wrap point to inside MetaBody's cell (identity if !MetaBody::isPeriodic)
		Vector3r wrapCellPt(const Vector3r& pt, MetaBody* rb);
		void drawPeriodicCell(MetaBody*);


	private :
		DynLibDispatcher< InteractionGeometry , GLDrawInteractionGeometryFunctor, void , TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>& , const shared_ptr<Body>&, const shared_ptr<Body>&, bool) > interactionGeometryDispatcher;
		DynLibDispatcher< InteractionPhysics  , GLDrawInteractionPhysicsFunctor,  void , TYPELIST_5(const shared_ptr<InteractionPhysics>& , const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool) > interactionPhysicsDispatcher;

		DynLibDispatcher< PhysicalParameters  , GLDrawStateFunctor,               void , TYPELIST_1(const shared_ptr<PhysicalParameters>&) > stateDispatcher;
		DynLibDispatcher< BoundingVolume      , GLDrawBoundingVolumeFunctor,      void , TYPELIST_1(const shared_ptr<BoundingVolume>&) > boundingVolumeDispatcher;
		DynLibDispatcher< InteractingGeometry , GLDrawInteractingGeometryFunctor, void , TYPELIST_3(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool) > interactingGeometryDispatcher;
		// FIXME - in fact it is a 1D dispatcher
		DynLibDispatcher< GeometricalModel    , GLDrawGeometricalModelFunctor,    void , TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, bool) > geometricalModelDispatcher;
		DynLibDispatcher< GeometricalModel    , GLDrawShadowVolumeFunctor,        void , TYPELIST_3(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&, const Vector3r& ) > shadowVolumeDispatcher;

		vector<vector<string> >
			stateFunctorNames,
			boundingVolumeFunctorNames,
			interactingGeometryFunctorNames, 
			geometricalModelFunctorNames,
			shadowVolumeFunctorNames,
			interactionGeometryFunctorNames,
			interactionPhysicsFunctorNames;

		DECLARE_LOGGER;

	public :
		void addStateFunctor(const string& str);
		void addBoundingVolumeFunctor(const string& str);
		void addInteractingGeometryFunctor(const string& str);
		void addGeometricalModelFunctor(const string& str);
		void addShadowVolumeFunctor(const string& str);
		void addInteractionGeometryFunctor(const string& str);
		void addInteractionPhysicsFunctor(const string& str);
			
		OpenGLRenderingEngine();
		virtual ~OpenGLRenderingEngine();
	
		void init();
		void initgl();
		void render(const shared_ptr<MetaBody>& body, body_id_t selection = body_id_t(-1));
		virtual void renderWithNames(const shared_ptr<MetaBody>& );
	
	private :
		void renderDOF_ID(const shared_ptr<MetaBody>& rootBody);
		void renderGeometricalModel(const shared_ptr<MetaBody>& rootBody);
		void renderInteractionPhysics(const shared_ptr<MetaBody>& rootBody);
		void renderInteractionGeometry(const shared_ptr<MetaBody>& rootBody);
		void renderState(const shared_ptr<MetaBody>& rootBody);
		void renderBoundingVolume(const shared_ptr<MetaBody>& rootBody);
		void renderInteractingGeometry(const shared_ptr<MetaBody>& rootBody);
		void renderShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position);
		void renderSceneUsingShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position);
		void renderSceneUsingFastShadowVolumes(const shared_ptr<MetaBody>& rootBody,Vector3r Light_position);
	
	protected :
		void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Serializable,(scaleDisplacements)(displacementScale)(scaleRotations)(rotationScale)(Light_position)(Background_color)(Body_wire)(Show_DOF)(Show_ID)(Body_state)(Body_bounding_volume)(Body_interacting_geom)(Body_geometrical_model)(Interaction_wire)(Interaction_geometry)(Interaction_physics)(Draw_mask)(Draw_inside)(Cast_shadows)(Shadow_volumes)(Fast_shadow_volume)(clipPlaneSe3)(clipPlaneActive)(selectBodyLimit));
	REGISTER_CLASS_AND_BASE(OpenGLRenderingEngine,RenderingEngine);
};
REGISTER_SERIALIZABLE(OpenGLRenderingEngine);



