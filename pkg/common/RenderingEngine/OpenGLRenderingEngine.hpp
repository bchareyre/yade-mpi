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

		void setBodiesRefSe3(const shared_ptr<Scene>& scene);

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
	
	protected :
		void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Serializable,(scaleDisplacements)(displacementScale)(scaleRotations)(rotationScale)(Light_position)(Background_color)(Body_wire)(Show_DOF)(Show_ID)(Body_state)(Body_bounding_volume)(Body_interacting_geom)
		(Interaction_wire)(Interaction_geometry)(Interaction_physics)(Draw_mask)(Draw_inside)(clipPlaneSe3)(clipPlaneActive)(selectBodyLimit)(intrAllWire));
	REGISTER_CLASS_AND_BASE(OpenGLRenderingEngine,Serializable);
};
REGISTER_SERIALIZABLE(OpenGLRenderingEngine);



