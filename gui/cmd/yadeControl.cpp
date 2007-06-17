#include<yade/core/Omega.hpp>

#include<boost/python.hpp>
#include<boost/bind.hpp>
#include<boost/thread/thread.hpp>
#include<boost/filesystem/operations.hpp>

//using namespace boost::python;

#include<yade/gui-qt3/GLViewer.hpp>
#include<qapplication.h>

#include<unistd.h>

#include"attrUtils.cpp"

class RenderingEngine;

class pyOmega{
	public:
	#define OMEGA Omega::instance()
	pyOmega(){};

	long iter(){ return OMEGA.getCurrentIteration();}
	double simulationTime(){return OMEGA.getSimulationTime();}
	// long realTime(){return OMEGA(get...);}
	double dt_get(){return OMEGA.getTimeStep();}
	void dt_set(double dt){OMEGA.setTimeStep(dt);}

	void run(){OMEGA.startSimulationLoop(); cerr<<"RUN!"<<endl;}
	// must stop, then reload
	//void _stop(){OMEGA(resetSimulationLoop());}
	void pause(){OMEGA.stopSimulationLoop(); cerr<<"PAUSE!"<<endl;}
	void step() {OMEGA.spawnSingleSimulationLoop(); cerr<<"STEP!"<<endl;}

	void load(std::string fileName) {
		OMEGA.finishSimulationLoop();
		OMEGA.joinSimulationLoop();
		OMEGA.setSimulationFileName(fileName);
		OMEGA.loadSimulation();
		OMEGA.createSimulationLoop();
		cerr<<"LOAD!"<<endl;
	}
};



class pyGLViewer{
	//DECLARE_LOGGER;
	shared_ptr<RenderingEngine> renderer;
	GLViewer* viewer;
	static vector<pyGLViewer*> instances;
	QApplication* app;
	void redrawAlarm(void){
		while(instances.size()>0){ // this means for eternity really...
			for(vector<pyGLViewer*>::iterator I=instances.begin(); I!=instances.end(); I++) (*I)->viewer->updateGL();
			usleep(50000);
		}
	}
public:
	//void appExec(){app->exec();}
	pyGLViewer(){
		shared_ptr<Factorable> _renderer=ClassFactory::instance().createShared("OpenGLRenderingEngine");
		renderer=static_pointer_cast<RenderingEngine>(_renderer);

		/* this is broken:
			Type of instance is: 15RenderingEngine
			RuntimeError: Cannot determine type with findType()
		*/
		#if 0
			if(renderer){// TODO: handle exceptions
				filesystem::path rendererConfig=filesystem::path(Omega::instance().yadeConfigPath+"/OpenGLRendererPref.xml");
				if(filesystem::exists(rendererConfig)) IOFormatManager::loadFromFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);
			} else {cerr<<"Cannot create renderer!!!"<<endl; abort();}
		#endif

		// maximum: one app
		int viewId=(int)instances.size();
		instances.push_back(this);

		if(viewId==0){ int _argc=0; char* _argv[]={"foo"}; app=new QApplication(_argc,_argv); }

		QGLFormat format;	QGLFormat::setDefaultFormat(format); format.setStencil(TRUE); format.setAlpha(TRUE);
		viewer=new GLViewer(viewId,renderer,format,0,viewId==0?0:instances[0]->viewer);
		viewer->notMoving();
		showAttrs(renderer);

		AttrAccess acc(renderer);
		cerr<<acc.dumpAttrs();
		cerr<<"=========== will change these values: ========="<<endl;
		cerr<<acc.dumpAttr("Background_color")<<endl<<acc.dumpAttr("Draw_inside")<<endl;
		acc.setAttrStr("Background_color","{ .8 .8 .8 }");
		acc.setAttr("Draw_inside",false);
		cerr<<"=========== new values are: ========="<<endl;
		cerr<<acc.dumpAttr("Background_color")<<endl<<acc.dumpAttr("Draw_inside")<<endl;

		if(viewId==0){
			app->setMainWidget(viewer);
			boost::thread redrawThread(boost::bind(&pyGLViewer::redrawAlarm,this));
		}
		boost::thread appThread(boost::bind(&QApplication::exec,app));
	}
	~pyGLViewer(){
		// remove this instance from the list
		for(vector<pyGLViewer*>::iterator I=instances.begin(); I!=instances.end(); I++){if((*I)==this) {instances.erase(I); break;}}
	}
};

vector<pyGLViewer*> pyGLViewer::instances;
//CREATE_LOGGER(pyGLViewer);


BOOST_PYTHON_MODULE(yadeControl)
{
	boost::python::class_<pyOmega>("Omega")
		.add_property("iter",&pyOmega::iter)
		.add_property("time",&pyOmega::simulationTime)
		// TODO: return computation time
		.add_property("dt",&pyOmega::dt_get,&pyOmega::dt_set)
		//.add_property("timeStepper",&timeStepper_get,&timeStepper_set)
		.def("load",&pyOmega::load)
		.def("run",&pyOmega::run)
		.def("pause",&pyOmega::pause)
		.def("step",&pyOmega::step);

	boost::python::class_<pyGLViewer>("View");

	//		.def("greet", &World::greet)
	//		.def("set", &World::set) 
	
	//scope().attr("iter")=OMEGA(getCurrentIteration());
	//scope().attr("time")=realTime(getSimulationTime());
	//scope().attr("verbose")=;
	//scope().attr("verbose") = verbose;

		#if 0
			.def("oneStep",&oneStep)
			.def("newView", &newView)
			.def("centerScene", &centerScene)

		.def("greet", &World::greet)
		.def("set", &World::set) 
		;
		#endif
}

