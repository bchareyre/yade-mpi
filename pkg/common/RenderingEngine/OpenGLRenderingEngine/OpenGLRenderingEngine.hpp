// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/RenderingEngine.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/MetaEngine1D.hpp>
#include<yade/core/Body.hpp>

#include<yade/pkg-common/GLDrawFunctors.hpp>

class OpenGLRenderingEngine : public RenderingEngine
{	
	public :
		Vector3r Light_position,Background_color;
		bool Body_state,Body_bounding_volume,Body_interacting_geom,Body_geometrical_model,Cast_shadows,Shadow_volumes,Fast_shadow_volume,Body_wire,Interaction_wire,Draw_inside,Interaction_geometry,Interaction_physics;
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
		void render(const shared_ptr<MetaBody>& body, body_id_t selection = body_id_t(-1));
		virtual void renderWithNames(const shared_ptr<MetaBody>& );
	
	private :
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
		void registerAttributes();
	REGISTER_CLASS_NAME(OpenGLRenderingEngine);
	REGISTER_BASE_CLASS_NAME(RenderingEngine);
};
REGISTER_SERIALIZABLE(OpenGLRenderingEngine);
