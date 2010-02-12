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
		body_id_t current_selection;

		static const int numClipPlanes=3;

		bool pointClipped(const Vector3r& p);
		vector<Vector3r> clipPlaneNormals;
		void setBodiesDispInfo();
		static bool initDone;
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

		DynLibDispatcher<InteractionGeometry, GlInteractionGeometryFunctor, void, TYPELIST_5(const shared_ptr<InteractionGeometry>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)> interactionGeometryDispatcher;
		DynLibDispatcher<InteractionPhysics, GlInteractionPhysicsFunctor, void, TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool)> interactionPhysicsDispatcher;

		DynLibDispatcher<State, GlStateFunctor, void, TYPELIST_1(const shared_ptr<State>&)> stateDispatcher;
		DynLibDispatcher<Bound, GlBoundFunctor, void, TYPELIST_2(const shared_ptr<Bound>&, Scene*)> boundDispatcher;
		DynLibDispatcher<Shape, GlShapeFunctor, void, TYPELIST_4(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&)> shapeDispatcher;

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

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(OpenGLRenderingEngine,Serializable,"Class responsible for rendering scene on OpenGL devices.",
		((bool,scaleDisplacements,false,"Whether to scale (artificially enlarge) displacements, so that they become better visible"))
		((Vector3r,displacementScale,Vector3r::ONE,"Relative stretch of current body position from its reference position (independently in 3 dimensions), if scaleDisplacements is set."))
		((bool,scaleRotations,false,"Whether to scale (artificially enlarge) rotations, so that they become better visible"))
		((Real,rotationScale,1,"Relative growth of current body rotation from its reference orientation (independently in 3 dimensions), if scaleRotations is set."))
		((Vector3r,Light_position,Vector3r(75,130,0),"Position of OpenGL light source in the scene."))
		((Vector3r,Background_color,Vector3r(.2,.2,.2),"Color of the backgroud canvas (RGB)"))
		((bool,Body_wire,false,"Render all bodies with wire only (faster)"))
		((bool,Show_DOF,false,"Show which degrees of freedom are blocked for each body"))
		((bool,Show_ID,false,"Show body id's"))
		((bool,Body_state,false,"Render body state [deprecated]"))
		((bool,Body_bounding_volume,false,"Render body bound"))
		((bool,Body_interacting_geom,true,"Render body shape"))
		((bool,Interaction_wire,false,"??"))
		((bool,Interaction_geometry,false,"Render :yref:`geometry of interaction<Interaction::interactionGeometry>`"))
		((bool,Interaction_physics,false,"Render :yref:`Interaction::interactionPhysics`"))
		((int,Draw_mask,((void)"draw everything",~0),"Bitmask for showing only bodies where ((Draw_mask & Body::groupMask)!=0)"))
		((bool,Draw_inside,true,"??"))
		((vector<Se3r>,clipPlaneSe3,vector<Se3r>(numClipPlanes,Se3r(Vector3r::ZERO,Quaternionr::IDENTITY)),"Position and orientation of clipping planes"))
		((vector<int>,clipPlaneActive,vector<int>(numClipPlanes,0),"Activate/deactivate respective clipping planes"))
		((size_t,selectBodyLimit,1000,"Limit number of bodies to allow picking body with mouse (performance reasons)"))
		((bool,intrAllWire,false,"Draw wire for all interactions, blue for potential and green for real ones (mostly for debugging)")),
		/*ctor*/ ,
		/*py*/
		.def("setRefSe3",&OpenGLRenderingEngine::setBodiesRefSe3,"Make current positions and orientation reference for scaleDisplacements and scaleRotations.");
	);
};
REGISTER_SERIALIZABLE(OpenGLRenderingEngine);



