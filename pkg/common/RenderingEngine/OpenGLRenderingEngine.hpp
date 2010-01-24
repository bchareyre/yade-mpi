// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Dispatcher.hpp>
#include<yade/core/Body.hpp>
#include<yade/lib-opengl/OpenGLWrapper.hpp>

#include<yade/pkg-common/GLDrawFunctors.hpp>

class OpenGLRenderingEngine : public Serializable
{	
	public:
		Vector3r Light_position,Background_color;
		bool Show_DOF,Show_ID,Body_state,Body_bounding_volume,Body_interacting_geom,
			Body_wire,Interaction_wire,Draw_inside,Interaction_geometry,Interaction_physics, intrAllWire;
		body_id_t current_selection;
		int Draw_mask;

		vector<Se3r> clipPlaneSe3;
		vector<int> clipPlaneActive; // should be bool, but serialization doesn't handle vector<bool>
		const int clipPlaneNum;
		bool scaleDisplacements,scaleRotations;
		Vector3r displacementScale; Real rotationScale;

		bool pointClipped(const Vector3r& p);
		vector<Vector3r> clipPlaneNormals;
		void setBodiesDispInfo();
		static bool glutInitDone;
		static size_t selectBodyLimit;
		Vector3r viewDirection; // updated from GLViewer regularly
		GLViewInfo viewInfo; // update from GLView regularly
		Vector3r highlightEmission0;
		Vector3r highlightEmission1;

		// normalized saw signal with given periodicity, with values ∈ 〈0,1〉 */
		Real normSaw(Real t, Real period){ Real xi=(t-period*((int)(t/period)))/period; /* normalized value, (0-1〉 */ return (xi<.5?2*xi:2-2*xi); }
		Real normSquare(Real t, Real period){ Real xi=(t-period*((int)(t/period)))/period; /* normalized value, (0-1〉 */ return (xi<.5?0:1); }

		void drawPeriodicCell();

		void setBodiesRefSe3();

		struct BodyDisp{
			Vector3r pos;
			Quaternionr ori;
			bool isDisplayed;
		};
		//! display data for individual bodies
		vector<BodyDisp> bodyDisp;


	private:
		// updated after every call to render
		shared_ptr<Scene> scene; 

		DynLibDispatcher< InteractionGeometry , GlInteractionGeometryFunctor, void , TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>& , const shared_ptr<Body>&, const shared_ptr<Body>&, bool) > interactionGeometryDispatcher;
		DynLibDispatcher< InteractionPhysics  , GlInteractionPhysicsFunctor,  void , TYPELIST_5(const shared_ptr<InteractionPhysics>& , const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool) > interactionPhysicsDispatcher;

		DynLibDispatcher< State  , GlStateFunctor,               void , TYPELIST_1(const shared_ptr<State>&) > stateDispatcher;
		DynLibDispatcher< Bound      , GlBoundFunctor,      void , TYPELIST_2(const shared_ptr<Bound>&, Scene*) > boundDispatcher;
		DynLibDispatcher< Shape , GlShapeFunctor, void , TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&) > shapeDispatcher;

		vector<vector<string> >
			stateFunctorNames,
			boundFunctorNames,
			shapeFunctorNames, 
			interactionGeometryFunctorNames,
			interactionPhysicsFunctorNames;

		DECLARE_LOGGER;

	public :
		void addStateFunctor(const string& str);
		void addBoundingVolumeFunctor(const string& str);
		void addInteractingGeometryFunctor(const string& str);
		void addInteractionGeometryFunctor(const string& str);
		void addInteractionPhysicsFunctor(const string& str);
			
		OpenGLRenderingEngine();
		virtual ~OpenGLRenderingEngine();
	
		void init();
		void initgl();
		void render(const shared_ptr<Scene>& scene, body_id_t selection = body_id_t(-1));
		void renderWithNames(const shared_ptr<Scene>& );
	
	private :
		void renderDOF_ID();
		void renderInteractionPhysics();
		void renderInteractionGeometry();
		void renderBoundingVolume();
		void renderShape();

		void renderAllInteractionsWire();
	
	public: void postProcessAttributes(bool deserializing);

	YADE_CLASS_BASE_DOC_ATTRS_PY(OpenGLRenderingEngine,Serializable,"Class responsible for rendering scene on OpenGL devices.",
		((scaleDisplacements,"Whether to scale (artificially enlarge) displacements, so that they become better visible"))
		((displacementScale,"Relative stretch of current body position from its reference position (independently in 3 dimensions), if scaleDisplacements is set."))
		((scaleRotations,"Whether to scale (artificially enlarge) rotations, so that they become better visible"))
		((rotationScale,"Relative growth of current body rotation from its reference orientation (independently in 3 dimensions), if scaleRotations is set."))
		((Light_position,"Position of OpenGL light source in the scene."))
		((Background_color,"Color of the backgroud canvas (RGB)"))
		((Body_wire,"Render all bodies with wire only (faster)"))
		((Show_DOF,"Show which degrees of freedom are blocked for each body"))
		((Show_ID,"Show body id's"))
		((Body_state,"Render body state [deprecated]"))
		((Body_bounding_volume,"Render body bound"))
		((Body_interacting_geom,"Render body shape"))
		((Interaction_wire,"??"))
		((Interaction_geometry,"Render geometry of interaction"))
		((Interaction_physics,"Render Interaction::interactionPhysics"))
		((Draw_mask,"Bitmask for showing only bodies where ((Draw_mask & Body::groupMask)!=0)"))
		((Draw_inside,"??"))
		((clipPlaneSe3,"Position and orientation of clipping planes"))
		((clipPlaneActive,"Activate/deactivate respective clipping planes"))
		((selectBodyLimit,"Limit number of bodies to allow picking body with mouse (performance reasons)"))
		((intrAllWire,"Draw wire for all interactions, blue for potential and green for real ones (mostly for debugging)")),
		.def("setRefSe3",&OpenGLRenderingEngine::setBodiesRefSe3,"Make current positions and orientation reference for scaleDisplacements and scaleRotations.");
	);
};
REGISTER_SERIALIZABLE(OpenGLRenderingEngine);



