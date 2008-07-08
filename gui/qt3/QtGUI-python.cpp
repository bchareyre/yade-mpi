#include<yade/gui-py/pyAttrUtils.hpp>
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<boost/python.hpp>
#include<yade/pkg-common/OpenGLRenderingEngine.hpp>

using namespace boost::python;

BASIC_PY_PROXY_HEAD(pyOpenGLRenderingEngine,OpenGLRenderingEngine)
	void setRefSe3(){ proxee->setRefSe3(Omega::instance().getRootBody()); }
BASIC_PY_PROXY_TAIL;

YadeQtMainWindow* ensuredMainWindow(){if(!YadeQtMainWindow::self) throw runtime_error("No instance of YadeQtMainWindow"); return YadeQtMainWindow::self; }
#define STANDALONE_MAINWINDOW_FUNC(func) void func(void){ensuredMainWindow()->func();}
STANDALONE_MAINWINDOW_FUNC(createView);
STANDALONE_MAINWINDOW_FUNC(centerViews);
STANDALONE_MAINWINDOW_FUNC(createSimulationController);
//STANDALONE_MAINWINDOW_FUNC(Quit);
void Quit(void){ if(YadeQtMainWindow::self) YadeQtMainWindow::self->Quit(); }
pyOpenGLRenderingEngine getRenderer(){return pyOpenGLRenderingEngine(ensuredMainWindow()->renderer);}


BOOST_PYTHON_MODULE(qt){
	def("View",createView);
	def("center",centerViews);
	def("Controller",createSimulationController);
	def("close",Quit);
	def("Renderer",getRenderer);

	BASIC_PY_PROXY_WRAPPER(pyOpenGLRenderingEngine,"GLRenderer")
		.def("setRefSe3",&pyOpenGLRenderingEngine::setRefSe3);

}
