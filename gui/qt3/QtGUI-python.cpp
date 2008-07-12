#include<yade/gui-py/pyAttrUtils.hpp>
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<boost/python.hpp>
#include<yade/pkg-common/OpenGLRenderingEngine.hpp>

#include"GLSimulationPlayerViewer.hpp"
#include"QtSimulationPlayer.hpp"

#include<qapplication.h>

using namespace boost::python;

BASIC_PY_PROXY_HEAD(pyOpenGLRenderingEngine,OpenGLRenderingEngine)
	void setRefSe3(){ proxee->setRefSe3(Omega::instance().getRootBody()); }
BASIC_PY_PROXY_TAIL;

YadeQtMainWindow* ensuredMainWindow(){if(!YadeQtMainWindow::self) throw runtime_error("No instance of YadeQtMainWindow"); return YadeQtMainWindow::self; }

void centerViews(void){ensuredMainWindow()->centerViews();}
void Quit(void){ if(YadeQtMainWindow::self) YadeQtMainWindow::self->Quit(); }
pyOpenGLRenderingEngine getRenderer(){return pyOpenGLRenderingEngine(ensuredMainWindow()->renderer);}

#define POST_SYNTH_EVENT(EVT) void evt##EVT(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_##EVT));}
POST_SYNTH_EVENT(PLAYER);
POST_SYNTH_EVENT(CONTROLLER);
POST_SYNTH_EVENT(VIEW);
POST_SYNTH_EVENT(GENERATOR);

/* This function adds the ability to non-interactively run the player. 
 * @param savedSim is simulation saved by either SQLiteRecorder (preferrably) or 
 * is a simulation XML if using PositionOrientationRecorder (beware: must be run from the
 * same dir if the base xyz snapshot dir is relative.
 *
 * @param snapBase is basename for snapshots saved as .png images. It contains both path
 * and first part of filename, e.g. /tmp/abc will create /tmp/abc-0001.png and so on.
 *
 * @param savedQGLState is file with QGLViewer state (camera position, orientation, display 
 * dimensions etc) saved when running the GLViewer (either in player or during simulation) and
 * pressing Alt-S; this saves state to /tmp/qglviewerState.xml.
 *
 * @param stride is that every stride-th simulation state will be taken
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
 *
 */
void runPlayer(string savedSim,string snapBase,string savedQGLState="",int stride=1){
	evtPLAYER();
	usleep(1000000); // ugly: sleep 1 secs to get the window hopefully ready
	cerr<<".";
	shared_ptr<QtSimulationPlayer> player=ensuredMainWindow()->player;
	GLSimulationPlayerViewer* glv=player->glSimulationPlayerViewer;
	glv->stride=stride;
	glv->load(savedSim);
	cerr<<":";
	glv->saveSnapShots=true;
	glv->snapshotsBase=snapBase;
	if(!savedQGLState.empty()){
		glv->setStateFileName(savedQGLState);
		glv->restoreStateFromFile();
		glv->setStateFileName(QString::null);
	}
	cerr<<"!";
	glv->startAnimation();
	cerr<<"@";
	while(glv->animationIsStarted()) { usleep(2000000); /*cerr<<"Last msg: "<<*player->messages.rbegin()<<endl;*/ }
}
BOOST_PYTHON_FUNCTION_OVERLOADS(runPlayer_overloads,runPlayer,2,4);

BOOST_PYTHON_MODULE(qt){
	def("Controller",evtCONTROLLER);
	def("Player",evtPLAYER);
	def("View",evtVIEW);
	def("center",centerViews);
	def("Renderer",getRenderer);
	def("close",Quit);
	def("runPlayer",runPlayer,runPlayer_overloads(args("viewStateFile","stride")));

	BASIC_PY_PROXY_WRAPPER(pyOpenGLRenderingEngine,"GLRenderer")
		.def("setRefSe3",&pyOpenGLRenderingEngine::setRefSe3);

}
