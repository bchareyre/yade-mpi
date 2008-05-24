#include<yade/gui-py/pyAttrUtils.hpp>
#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<boost/python.hpp>
using namespace boost::python;

YadeQtMainWindow* ensuredMainWindow(){if(!YadeQtMainWindow::self) throw runtime_error("No instance of YadeQtMainWindow"); return YadeQtMainWindow::self; }
#define STANDALONE_MAINWINDOW_FUNC(func) void func(void){ensuredMainWindow()->func();}
STANDALONE_MAINWINDOW_FUNC(createView);
STANDALONE_MAINWINDOW_FUNC(centerViews);
STANDALONE_MAINWINDOW_FUNC(createSimulationController);
STANDALONE_MAINWINDOW_FUNC(Quit);
//void closeMainWindow(void){ exit(0); /* this is perhaps too violent? */ }


BOOST_PYTHON_MODULE(qt){
	def("View",createView);
	def("center",centerViews);
	def("Controller",createSimulationController);
	def("close",Quit);
}
