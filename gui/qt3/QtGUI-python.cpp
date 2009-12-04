#include<yade/gui-qt3/YadeQtMainWindow.hpp>
#include<boost/python.hpp>
#include<yade/pkg-common/OpenGLRenderingEngine.hpp>

#include<qapplication.h>
#include<qcolor.h>

#include<cstdio>

#ifdef YADE_LOG4CXX
log4cxx::LoggerPtr logger=log4cxx::Logger::getLogger("yade.QtGUI-python");
#endif

using namespace boost::python;

#include<yade/gui-qt3/QtGUI.hpp>
bool qtGuiIsActive(){return (bool)YadeQtMainWindow::self; }
void qtGuiActivate(){
	if(qtGuiIsActive()) return;
	QtGUI* gui=new QtGUI();
	if(!gui->checkDisplay(/* quiet */ false) || !gui->runNaked()){
		PyErr_SetString(PyExc_ImportError,"Qt3 GUI could not be activated.");
		python::throw_error_already_set();
	}
}

YadeQtMainWindow* ensuredMainWindow(){if(!qtGuiIsActive()){qtGuiActivate(); while(!YadeQtMainWindow::self) usleep(50000); } /* throw runtime_error("No instance of YadeQtMainWindow");*/ return YadeQtMainWindow::self; }

void centerViews(void){ensuredMainWindow()->centerViews();}
void Quit(void){ if(YadeQtMainWindow::self) YadeQtMainWindow::self->Quit(); }
shared_ptr<OpenGLRenderingEngine> ensuredRenderer(){ensuredMainWindow()->ensureRenderer(); return ensuredMainWindow()->renderer;}
void OpenGLRenderingEngine_setBodiesRefSe3(const shared_ptr<OpenGLRenderingEngine>& self){ self->setBodiesRefSe3(Omega::instance().getScene()); }

#define POST_SYNTH_EVENT(EVT,checker) void evt##EVT(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_##EVT)); bool wait=true; if(wait){while(!(bool)(ensuredMainWindow()->checker)) usleep(50000);} }
POST_SYNTH_EVENT(CONTROLLER,controller);
POST_SYNTH_EVENT(GENERATOR,generator);
#undef POST_SYNT_EVENT

// event associated data will be deleted in the event handler
void restoreGLViewerState_str(string str){string* s=new string(str); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESTORE_GLVIEWER_STR,(void*)s));}
void restoreGLViewerState_num(int dispStateNo){int* i=new int(dispStateNo); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESTORE_GLVIEWER_NUM,(void*)i));}


qglviewer::Vec tuple2vec(python::tuple t){ qglviewer::Vec ret; for(int i=0;i<3;i++){python::extract<Real> e(t[i]); if(!e.check()) throw invalid_argument("Element #"+lexical_cast<string>(i)+" is not a number"); ret[i]=e();} return ret;};
python::tuple vec2tuple(qglviewer::Vec v){return python::make_tuple(v[0],v[1],v[2]);};

class pyGLViewer{
	size_t viewNo;
	void init(size_t _viewNo){
		viewNo=_viewNo;
		getGlv();
	}
	GLViewer* getGlv(){ if(YadeQtMainWindow::self->glViews.size()<viewNo+1 || !YadeQtMainWindow::self->glViews[viewNo]){throw runtime_error("No view #"+lexical_cast<string>(viewNo));} return YadeQtMainWindow::self->glViews[viewNo].get(); }
	public:
		#define MUTEX GLLock _lock(glv)
		#define GLV GLViewer* glv=getGlv()
		pyGLViewer(){ init(0); }
		pyGLViewer(size_t _viewNo){init(_viewNo);}
		python::tuple get_grid(){GLV; return python::make_tuple(glv->drawGridXYZ[0],glv->drawGridXYZ[1],glv->drawGridXYZ[2]);}
		void set_grid(python::tuple t){GLV; MUTEX; for(int i=0;i<3;i++)glv->drawGridXYZ[i]=python::extract<bool>(t[i])();}
		#define VEC_GET_SET(property,getter,setter) Vector3r get_##property(){GLV; qglviewer::Vec v=getter(); return Vector3r(v[0],v[1],v[2]); } void set_##property(const Vector3r& t){GLV; MUTEX; setter(qglviewer::Vec(t[0],t[1],t[2]));}
		VEC_GET_SET(upVector,glv->camera()->upVector,glv->camera()->setUpVector);
		VEC_GET_SET(lookAt,glv->camera()->position()+glv->camera()->viewDirection,glv->camera()->lookAt);
		VEC_GET_SET(viewDir,glv->camera()->viewDirection,glv->camera()->setViewDirection);
		VEC_GET_SET(eyePosition,glv->camera()->position,glv->camera()->setPosition);
		#define BOOL_GET_SET(property,getter,setter)void set_##property(bool b){GLV; MUTEX; setter(b);} bool get_##property(){GLV; return getter();}
		BOOL_GET_SET(axes,glv->axisIsDrawn,glv->setAxisIsDrawn);
		BOOL_GET_SET(fps,glv->FPSIsDisplayed,glv->setFPSIsDisplayed);
		bool get_scale(){GLV; return glv->drawScale;} void set_scale(bool b){GLV; MUTEX; glv->drawScale=b;}
		bool get_orthographic(){GLV; return glv->camera()->type()==qglviewer::Camera::ORTHOGRAPHIC;}
		void set_orthographic(bool b){GLV; MUTEX; return glv->camera()->setType(b ? qglviewer::Camera::ORTHOGRAPHIC : qglviewer::Camera::PERSPECTIVE);}
		#define FLOAT_GET_SET(property,getter,setter)void set_##property(Real r){GLV; MUTEX; setter(r);} Real get_##property(){GLV; return getter();}
		FLOAT_GET_SET(sceneRadius,glv->sceneRadius,glv->setSceneRadius);
		void fitAABB(const Vector3r& min, const Vector3r& max){GLV; MUTEX; glv->camera()->fitBoundingBox(qglviewer::Vec(min[0],min[1],min[2]),qglviewer::Vec(max[0],max[1],max[2]));}
		void fitSphere(const Vector3r& center,Real radius){GLV; MUTEX; glv->camera()->fitSphere(qglviewer::Vec(center[0],center[1],center[2]),radius);}
		void showEntireScene(){GLV; MUTEX; glv->camera()->showEntireScene();}
		void center(bool median=false){GLV; MUTEX; if(median)glv->centerMedianQuartile(); else glv->centerScene();}
		python::tuple get_screenSize(){GLV; return python::make_tuple(glv->width(),glv->height());} void set_screenSize(python::tuple t){GLV; MUTEX; vector<int>* ii=new(vector<int>); ii->push_back(viewNo); ii->push_back(python::extract<int>(t[0])()); ii->push_back(python::extract<int>(t[1])()); QApplication::postEvent(ensuredMainWindow(),new QCustomEvent((QEvent::Type)YadeQtMainWindow::EVENT_RESIZE_VIEW,(void*)ii));}
		string pyStr(){return string("<GLViewer for view #")+lexical_cast<string>(viewNo)+">";}
		void saveDisplayParameters(size_t n){GLV; MUTEX; glv->saveDisplayParameters(n);}
		void useDisplayParameters(size_t n){GLV; MUTEX; glv->useDisplayParameters(n);}
		string get_timeDisp(){GLV; const int& m(glv->timeDispMask); string ret; if(m&GLViewer::TIME_REAL) ret+='r'; if(m&GLViewer::TIME_VIRT) ret+="v"; if(m&GLViewer::TIME_ITER) ret+="i"; return ret;}
		void set_timeDisp(string s){GLV; MUTEX; int& m(glv->timeDispMask); m=0; FOREACH(char c, s){switch(c){case 'r': m|=GLViewer::TIME_REAL; break; case 'v': m|=GLViewer::TIME_VIRT; break; case 'i': m|=GLViewer::TIME_ITER; break; default: throw invalid_argument(string("Invalid flag for timeDisp: `")+c+"'");}}}
		void set_bgColor(const Vector3r& c){ QColor cc(255*c[0],255*c[1],255*c[2]); GLV; MUTEX; glv->setBackgroundColor(cc);} Vector3r get_bgColor(){ GLV; MUTEX; QColor c(glv->backgroundColor()); return Vector3r(c.red()/255.,c.green()/255.,c.blue()/255.);}
		#undef MUTEX
		#undef GLV
};
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pyGLViewer_center_overloads,center,0,1);


pyGLViewer evtVIEW(){QApplication::postEvent(ensuredMainWindow(),new QCustomEvent(YadeQtMainWindow::EVENT_VIEW)); size_t origViewNo=ensuredMainWindow()->glViews.size(); while(ensuredMainWindow()->glViews.size()!=origViewNo+1) usleep(50000); return pyGLViewer((*ensuredMainWindow()->glViews.rbegin())->viewId);}

python::list getAllViews(){
	python::list ret;
	FOREACH(const shared_ptr<GLViewer>& glView, YadeQtMainWindow::self->glViews){ if(glView) ret.append(pyGLViewer(glView->viewId)); }
	return ret;
};

BOOST_PYTHON_MODULE(_qt){
	def("Generator",evtGENERATOR,"Start simulation generator");
	def("Controller",evtCONTROLLER,"Start simulation controller");
	def("View",evtVIEW,"Create new 3d view");
	def("center",centerViews,"Center all existing views.");
	def("Renderer",ensuredRenderer,"Return wrapped OpenGLRenderingEngine; the renderer is constructed if necessary.");
	def("close",Quit);
	def("isActive",qtGuiIsActive,"Whether the Qt GUI is being used.");
	def("activate",qtGuiActivate,"Attempt to activate the Qt GUI from within python.");
	def("views",getAllViews);

	python::class_<OpenGLRenderingEngine, shared_ptr<OpenGLRenderingEngine>, python::bases<Serializable>, noncopyable>("OpenGLRenderingEngine")
		.def("setRefSe3",&OpenGLRenderingEngine_setBodiesRefSe3,"Make current positions and orientation reference for scaleDisplacements and scaleRotations.");

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
		.add_property("timeDisp",&pyGLViewer::get_timeDisp,&pyGLViewer::set_timeDisp)
		// .add_property("bgColor",&pyGLViewer::get_bgColor,&pyGLViewer::set_bgColor) // useless: OpenGLRenderingEngine::Background_color is used via openGL directly, bypassing QGLViewer background property
		.def("fitAABB",&pyGLViewer::fitAABB)
		.def("fitSphere",&pyGLViewer::fitSphere)
		.def("showEntireScene",&pyGLViewer::showEntireScene)
		.def("center",&pyGLViewer::center,pyGLViewer_center_overloads())
		.def("saveState",&pyGLViewer::saveDisplayParameters)
		.def("loadState",&pyGLViewer::useDisplayParameters)
		.def("__repr__",&pyGLViewer::pyStr).def("__str__",&pyGLViewer::pyStr)
		;
}
