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
	void setRefSe3(){ proxee->setRefSe3(Omega::instance().getRootBody()); }
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
void evtVIEW(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_VIEW)); }


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
 *  qt.runPlayer('/tmp/aa.sqlite','/tmp/__g','/tmp/qglviewerState.xml',20)
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

BOOST_PYTHON_MODULE(qt){
	def("Generator",evtGENERATOR,"Start simulation generator");
	def("Controller",evtCONTROLLER,"Start simulation controller");
	def("Player",evtPLAYER,"Start simulation player");
	def("View",evtVIEW,"Create new 3d view");
	def("center",centerViews,"Center all existing views.");
	def("Renderer",ensuredRenderer,"Return wrapped OpenGLRenderingEngine; the renderer is constructed if necessary.");
	def("close",Quit);
	def("isActive",qtGuiIsActive,"Whether the Qt GUI is being used.");
	def("runPlayerSession",runPlayerSession,runPlayerSession_overloads(args("savedQGLState","dispParamsNo","stride","postLoadHook")));

	BASIC_PY_PROXY_WRAPPER(pyOpenGLRenderingEngine,"GLRenderer")
		.def("setRefSe3",&pyOpenGLRenderingEngine::setRefSe3,"Make current positions and orientation reference for scaleDisplacements and scaleRotations.");

}
