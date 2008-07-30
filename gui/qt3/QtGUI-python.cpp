#include<yade/gui-py/pyAttrUtils.hpp>
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<boost/python.hpp>
#include<yade/pkg-common/OpenGLRenderingEngine.hpp>

#include"GLSimulationPlayerViewer.hpp"
#include"QtSimulationPlayer.hpp"

#include<qapplication.h>

#include<cstdio>

#ifdef LOG4CXX
log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.QtGUI-python");
#endif

using namespace boost::python;

BASIC_PY_PROXY_HEAD(pyOpenGLRenderingEngine,OpenGLRenderingEngine)
	void setRefSe3(){ proxee->setBodiesRefSe3(Omega::instance().getRootBody()); }
BASIC_PY_PROXY_TAIL;

YadeQtMainWindow* ensuredMainWindow(){if(!YadeQtMainWindow::self) throw runtime_error("No instance of YadeQtMainWindow"); return YadeQtMainWindow::self; }

void centerViews(void){ensuredMainWindow()->centerViews();}
void Quit(void){ if(YadeQtMainWindow::self) YadeQtMainWindow::self->Quit(); }
pyOpenGLRenderingEngine ensuredRenderer(){ensuredMainWindow()->ensureRenderer(); return pyOpenGLRenderingEngine(ensuredMainWindow()->renderer);}

#define POST_SYNTH_EVENT(EVT,checker) void evt##EVT(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_##EVT)); bool wait=true; if(wait){while(!(bool)(ensuredMainWindow()->checker)) usleep(50000);} }
POST_SYNTH_EVENT(PLAYER,player);
POST_SYNTH_EVENT(CONTROLLER,controller);
POST_SYNTH_EVENT(GENERATOR,generator);
// BOOST_PYTHON_FUNCTION_OVERLOADS(evtPLAYER_overloads,evtPLAYER,0,1); BOOST_PYTHON_FUNCTION_OVERLOADS(evtCONTROLLER_overloads,evtCONTROLLER,0,1); BOOST_PYTHON_FUNCTION_OVERLOADS(evtGENERATOR_overloads,evtGENERATOR,0,1);
#undef POST_SYNT_EVENT
void evtVIEW(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_VIEW)); size_t origViewNo=ensuredMainWindow()->glViews.size(); while(ensuredMainWindow()->glViews.size()!=origViewNo+1) usleep(50000); }

// event associated data will be deleted in the event handler
void restoreGLViewerState_str(string str){string* s=new string(str); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESTORE_GLVIEWER_STR,(void*)s));}
void restoreGLViewerState_num(int dispStateNo){int* i=new int(dispStateNo); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESTORE_GLVIEWER_NUM,(void*)i));}


/* This function adds the ability to non-interactively run the player. 
 *
 * @param savedSim is simulation saved by either SQLiteRecorder (preferrably) or 
 * is a simulation XML if using PositionOrientationRecorder (beware: must be run from the
 * same dir if the base xyz snapshot dir is relative.
 *
 * @param savedQGLState is file with QGLViewer state (camera position, orientation, display 
 * dimensions etc) saved when running the GLViewer (either in player or during simulation) and
 * pressing 'S'; this saves state to /tmp/qglviewerState.xml.
 *
 * @param dispParamsNo reloads n-th display parameters (GLViewer and OpenGLRenderingEngine)
 * from states saved in MetaBody::dispParams.
 *
 * @param stride is that every stride-th simulation state will be taken

 * @param snapBase is basename for .png snapshots. If ommited, tmpnam generates unique basename.
 *
 * @param postLoadHook is string with python command(s) that will be interpreted after loading if non-empty.
 * 	Can be used e.g. for adjusting wire display of bodies after loading the simulation.
 *
 * This function may be especially useful for offsreen rendering using Xvfb
 * (install the xvfb package first). Then run
 *
 * $ Xvfb :1 -ac -screen 0 1024x768x16 # :1 is display number, last is width x height x depth
 * 
 * and on another console something like
 *
 * yade-trunk /tmp/tmpFile.py
 *
 * with /tmp/tmpFile.py containing something like:
 *
 *  from yade import qt
 *  qt.runPlayerSession('/tmp/aa.sqlite','/tmp/__g','/tmp/qglviewerState.xml',20)
 *  quit()
 *
 * @returns tuple of (wildcard,[snap0,snap1,snap2,...]), where the list contains filename of snapshots.
 */
python::tuple runPlayerSession(string savedSim,string snapBase="",string savedQGLState="",int dispParamsNo=-1,int stride=1,string postLoadHook=""){
	evtPLAYER();
	shared_ptr<QtSimulationPlayer> player=ensuredMainWindow()->player;
	GLSimulationPlayerViewer* glv=player->glSimulationPlayerViewer;
	string snapBase2(snapBase);
	if(snapBase2.empty()){ char tmpnam_str [L_tmpnam]; tmpnam(tmpnam_str); snapBase2=tmpnam_str; LOG_INFO("Using "<<snapBase2<<" as temporary basename for snapshots."); }
	glv->stride=stride;
	glv->load(savedSim);
	if(!postLoadHook.empty()){ PyGILState_STATE gstate; gstate = PyGILState_Ensure(); PyRun_SimpleString(postLoadHook.c_str()); PyGILState_Release(gstate); }
	glv->saveSnapShots=true;
	glv->snapshotsBase=snapBase2;
	if(!savedQGLState.empty()){
		glv->setStateFileName(savedQGLState);
		glv->restoreStateFromFile();
		glv->setStateFileName(QString::null);
	}
	if(dispParamsNo>=0) glv->useDisplayParameters(dispParamsNo);
	glv->startAnimation();
	while(glv->animationIsStarted()) { usleep(2000000); LOG_DEBUG("Last msg: "<<*player->messages.rbegin()); }
	python::list snaps; FOREACH(string s, glv->snapshots){snaps.append(s);}
	return python::make_tuple(snapBase2+"-%.04d.png",snaps);
}

bool qtGuiIsActive(){return (bool)YadeQtMainWindow::self;}

BOOST_PYTHON_FUNCTION_OVERLOADS(runPlayerSession_overloads,runPlayerSession,2,6);

qglviewer::Vec tuple2vec(python::tuple t){ qglviewer::Vec ret; for(int i=0;i<3;i++){python::extract<Real> e(t[i]); if(!e.check()) throw invalid_argument("Element #"+lexical_cast<string>(i)+" is not a number"); ret[i]=e();} return ret;};
python::tuple vec2tuple(qglviewer::Vec v){return python::make_tuple(v[0],v[1],v[2]);};

class pyGLViewer{
	shared_ptr<GLViewer> glv;
	void init(size_t viewNo){
		if(YadeQtMainWindow::self->glViews.size()<viewNo+1 || !YadeQtMainWindow::self->glViews[viewNo]){throw runtime_error("No view #"+lexical_cast<string>(viewNo));}
		glv=YadeQtMainWindow::self->glViews[viewNo];
	}
	public:
		pyGLViewer(){ init(0); }
		pyGLViewer(size_t viewNo){init(viewNo);}
		pyGLViewer(const shared_ptr<GLViewer>& _glv){glv=_glv;}
		python::tuple get_grid(){return python::make_tuple(glv->drawGridXYZ[0],glv->drawGridXYZ[1],glv->drawGridXYZ[2]);}
		void set_grid(python::tuple t){for(int i=0;i<3;i++)glv->drawGridXYZ[i]=python::extract<bool>(t[i])();}
		#define VEC_GET_SET(property,getter,setter) python::object get_##property(){return vec2tuple(getter());} void set_##property(python::tuple t){setter(tuple2vec(t));}
		VEC_GET_SET(upVector,glv->camera()->upVector,glv->camera()->setUpVector);
		VEC_GET_SET(lookAt,glv->camera()->position()+glv->camera()->viewDirection,glv->camera()->lookAt);
		VEC_GET_SET(viewDir,glv->camera()->viewDirection,glv->camera()->setViewDirection);
		VEC_GET_SET(eyePosition,glv->camera()->position,glv->camera()->setPosition);
		#define BOOL_GET_SET(property,getter,setter)void set_##property(bool b){setter(b);} bool get_##property(){return getter();}
		BOOL_GET_SET(axes,glv->axisIsDrawn,glv->setAxisIsDrawn);
		BOOL_GET_SET(fps,glv->FPSIsDisplayed,glv->setFPSIsDisplayed);
		bool get_scale(){return glv->drawScale;} void set_scale(bool b){glv->drawScale=b;}
		bool get_orthographic(){return glv->camera()->type()==qglviewer::Camera::ORTHOGRAPHIC;}
		void set_orthographic(bool b){return glv->camera()->setType(b ? qglviewer::Camera::ORTHOGRAPHIC : qglviewer::Camera::PERSPECTIVE);}
		#define FLOAT_GET_SET(property,getter,setter)void set_##property(Real r){setter(r);} Real get_##property(){return getter();}
		FLOAT_GET_SET(sceneRadius,glv->sceneRadius,glv->setSceneRadius);
		void fitAABB(python::tuple min, python::tuple max){glv->camera()->fitBoundingBox(tuple2vec(min),tuple2vec(max));}
		void fitSphere(python::tuple center,Real radius){glv->camera()->fitSphere(tuple2vec(center),radius);}
		void showEntireScene(){glv->camera()->showEntireScene();}
		void center(bool median=false){if(median)glv->centerMedianQuartile(); else glv->centerScene();}
		python::tuple get_screenSize(){return python::make_tuple(glv->width(),glv->height());} void set_screenSize(python::tuple t){ vector<int>* ii=new(vector<int>); ii->push_back(ensuredMainWindow()->viewNo(glv)); ii->push_back(python::extract<int>(t[0])()); ii->push_back(python::extract<int>(t[1])()); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESIZE_VIEW,(void*)ii));}
		string pyStr(){return string("<GLViewer for view #")+lexical_cast<string>(ensuredMainWindow()->viewNo(glv))+">";}
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pyGLViewer_center_overloads,center,0,1);


python::list getAllViews(){
	python::list ret;
	for(size_t i=0; i<YadeQtMainWindow::self->glViews.size(); i++) ret.append(YadeQtMainWindow::self->glViews[i]);
	return ret;
};

BOOST_PYTHON_MODULE(_qt){
	def("Generator",evtGENERATOR,"Start simulation generator");
	def("Controller",evtCONTROLLER,"Start simulation controller");
	def("Player",evtPLAYER,"Start simulation player");
	def("View",evtVIEW,"Create new 3d view");
	def("center",centerViews,"Center all existing views.");
	def("Renderer",ensuredRenderer,"Return wrapped OpenGLRenderingEngine; the renderer is constructed if necessary.");
	def("close",Quit);
	def("isActive",qtGuiIsActive,"Whether the Qt GUI is being used.");
	def("runPlayerSession",runPlayerSession,runPlayerSession_overloads(args("savedQGLState","dispParamsNo","stride","postLoadHook")));
	def("views",getAllViews);

	BASIC_PY_PROXY_WRAPPER(pyOpenGLRenderingEngine,"GLRenderer")
		.def("setRefSe3",&pyOpenGLRenderingEngine::setRefSe3,"Make current positions and orientation reference for scaleDisplacements and scaleRotations.");

	boost::python::class_<pyGLViewer>("GLViewer")
		.def(python::init<unsigned>())
		.add_property("upVector",&pyGLViewer::get_upVector,&pyGLViewer::set_upVector)
		.add_property("lookAt",&pyGLViewer::get_lookAt,&pyGLViewer::set_lookAt)
		.add_property("viewDir",&pyGLViewer::get_viewDir,&pyGLViewer::set_viewDir)
		.add_property("eyePosition",&pyGLViewer::get_eyePosition,&pyGLViewer::set_eyePosition)
		.add_property("grid",&pyGLViewer::get_grid,&pyGLViewer::set_grid)
		.add_property("fps",&pyGLViewer::get_fps,&pyGLViewer::set_fps)
		.add_property("axes",&pyGLViewer::get_axes,&pyGLViewer::set_axes)
		.add_property("scale",&pyGLViewer::get_scale,&pyGLViewer::set_scale)
		.add_property("sceneRadius",&pyGLViewer::get_sceneRadius,&pyGLViewer::set_sceneRadius)
		.add_property("ortho",&pyGLViewer::get_orthographic,&pyGLViewer::set_orthographic)
		.add_property("screenSize",&pyGLViewer::get_screenSize,&pyGLViewer::set_screenSize)
		.def("fitAABB",&pyGLViewer::fitAABB)
		.def("fitSphere",&pyGLViewer::fitSphere)
		.def("showEntireScene",&pyGLViewer::showEntireScene)
		.def("center",&pyGLViewer::center,pyGLViewer_center_overloads())
		.def("__repr__",&pyGLViewer::pyStr).def("__str__",&pyGLViewer::pyStr)
		;
}
