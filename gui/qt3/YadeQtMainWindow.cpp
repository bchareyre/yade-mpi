/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include"YadeQtMainWindow.hpp"
#include"YadeCamera.hpp"
#include"GLViewer.hpp"
#include<yade/lib-serialization/IOFormatManager.hpp>
#include<yade/lib-factory/ClassFactory.hpp>
#include <qvbox.h>
#include <qfiledialog.h>
#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <yade/pkg-common/GravityEngines.hpp>
#include<yade/gui-qt3/QtGUI.hpp>
#include<qapplication.h>


using namespace std;

CREATE_LOGGER(YadeQtMainWindow);

YadeQtMainWindow* YadeQtMainWindow::self=NULL;
bool YadeQtMainWindow::guiMayDisappear=false;

YadeQtMainWindow::YadeQtMainWindow() : YadeQtGeneratedMainWindow()
{
	self=this;

	QGLFormat format;
	format.setStencil(TRUE);
	format.setAlpha(TRUE);
	QGLFormat::setDefaultFormat(format);


	preferences = shared_ptr<QtGUIPreferences>(new QtGUIPreferences);
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);

	if ( !filesystem::exists( yadeQtGUIPrefPath ) )
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		preferences->mainWindowSizeX		= 150; preferences->mainWindowSizeY		= 150;
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	try
	{
		LOG_INFO("Loading configuration file: "<<yadeQtGUIPrefPath.string()<<".")
		IOFormatManager::loadFromFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}
	catch(SerializableError&)
	{
		preferences->mainWindowPositionX	= 50;
		preferences->mainWindowPositionY	= 50;
		preferences->mainWindowSizeX		= 150; preferences->mainWindowSizeY		= 150;
		IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	}

	resize(preferences->mainWindowSizeX,preferences->mainWindowSizeY);
	move(preferences->mainWindowPositionX,preferences->mainWindowPositionY);

	// HACK
	// if(!guiMayDisappear && Omega::instance().getSimulationFileName()!="") createController();
	if(!guiMayDisappear) createController();


	// updates GL views, may also hide/show this window as needed
	startTimer(100);
}

void YadeQtMainWindow::timerEvent(QTimerEvent* evt){
	#if 1
	//shared_ptr<World> rb=Omega::instance().getWorld();
		//if((rb && rb->bodies->size()>0) ||
		if(controller || generator) {this->hide();}
		else { if(!guiMayDisappear) this->show(); }
	#endif
	// update GL views (if any)
	redrawAll(/*force=*/false);
	if(guiMayDisappear) { QtGUI::app->processEvents(); }
}

void YadeQtMainWindow::redrawAll(bool force){
	// controller has its own timer -- and will update instead of us periodically
	if((renderer && glViews.size()>0) && (force /* || (controller  && !controller->syncRunning ) */ || (!controller))){
		FOREACH(const shared_ptr<GLViewer>& glv,glViews){ if(glv) glv->updateGL(); }
	}
}

void YadeQtMainWindow::loadSimulation(std::string file){createController(); while(!(bool)(controller)) usleep(50000); controller->loadSimulationFromFileName(file); lookDown(glViews[0]);}
void YadeQtMainWindow::centerViews(){FOREACH(const shared_ptr<GLViewer>& glv,glViews){ if(glv){ glv->centerScene();}}}


YadeQtMainWindow::~YadeQtMainWindow()
{
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);
	preferences->mainWindowPositionX	= pos().x();
	preferences->mainWindowPositionY	= pos().y();
	preferences->mainWindowSizeX		= size().width();
	preferences->mainWindowSizeY		= size().height();
	IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
	self=NULL;
}

void YadeQtMainWindow::Quit(){ emit close(); }

void YadeQtMainWindow::closeEvent(QCloseEvent *e){ renderer=shared_ptr<OpenGLRenderingEngine>(); closeAllChilds(); YadeQtGeneratedMainWindow::closeEvent(e);  }

void YadeQtMainWindow::ensureRenderer(){
	if(!renderer){
		LOG_DEBUG("Creating OpenGLRenderingEngine instance.");
		shared_ptr<Factorable> tmpRenderer=ClassFactory::instance().createShared("OpenGLRenderingEngine");
		renderer=static_pointer_cast<OpenGLRenderingEngine>(tmpRenderer);
		renderer->init();
	}

	if(!renderer) throw runtime_error("Renderer could not be created, why?");

	filesystem::path rendererConfig = filesystem::path( Omega::instance().yadeConfigPath + "/OpenGLRendererPref.xml", filesystem::native);
	if(filesystem::exists(rendererConfig)){
		try{
			LOG_DEBUG("Loading renderer config from: " << rendererConfig.string());
			IOFormatManager::loadFromFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);
		}
		catch(SerializableError& e) {
			LOG_ERROR("Unable to load existing renderer configuration file `"<<rendererConfig.string()<<"' :"<<e.what());
		}
	}
}

void YadeQtMainWindow::saveRendererConfig(){
	filesystem::path rendererConfig = filesystem::path( Omega::instance().yadeConfigPath + "/OpenGLRendererPref.xml", filesystem::native);
	LOG_DEBUG("Saving renderer config to: " << rendererConfig.string());
	IOFormatManager::saveToFile("XMLFormatManager",rendererConfig.string(),"renderer",renderer);
}

void YadeQtMainWindow::createView(){
	ensureRenderer();

	bool isFirst=glViews.empty();
	shared_ptr<GLViewer> glv=shared_ptr<GLViewer>(new GLViewer(glViews.size(),renderer,NULL,isFirst?NULL:glViews[0].get()));
	GLLock lock(glv.get());
	glv->setCamera(new YadeCamera);
	glv->camera()->frame()->setWheelSensitivity(-1.0f);
	glv->camera()->setUpVector(qglviewer::Vec(0,1,0));
	glv->camera()->setViewDirection(qglviewer::Vec(-1,-1,-1));
	lookDown(glv);
	glViews.push_back(glv);
	if(isFirst)
		renderer->initgl();
	//connect( glv, SIGNAL(closeSignal(int)), this, SLOT( closeGLViewEvent(int) ) );
}

void YadeQtMainWindow::lookDown(shared_ptr<GLViewer> glv)
{
	bool hasSimulation=(Omega::instance().getWorld() ? Omega::instance().getWorld()->bodies->size()>0 : false );
	if(hasSimulation)
	{	
		Vector3r g(0,1,0);
		FOREACH(const shared_ptr<Engine>& e,Omega::instance().getWorld()->engines){
			if(e && e->getClassName()=="GravityEngine")  
				g = -1.0*(dynamic_cast<GravityEngine*>(e.get()))->gravity;
		}
		glv->camera()->setUpVector(qglviewer::Vec(g[0],g[1],g[2]));
	} else {
		glv->camera()->setUpVector(qglviewer::Vec(0,1,0));
	}
	glv->camera()->setViewDirection(qglviewer::Vec(-1,-1,-1));
	glv->centerScene();
};
		
void YadeQtMainWindow::adjustCameraInCurrentView(qglviewer::Vec up,qglviewer::Vec dir)
{
	boost::posix_time::ptime last(boost::posix_time::second_clock::local_time()-boost::posix_time::hours(1000));
	FOREACH(const shared_ptr<GLViewer>& glv,glViews){ if(glv) last=std::max(last,glv->getLastUserEvent()); }
	FOREACH(const shared_ptr<GLViewer>& glv,glViews){ 
		if(glv && glv->getLastUserEvent()>=last)
		{
			glv->camera()->setUpVector(up);
			glv->camera()->setViewDirection(dir);
			glv->centerScene();
		}
	}
};

void YadeQtMainWindow::closeView(GLViewer* glv){
	for(size_t i=0; i<glViews.size(); i++){
		if(glViews[i].get()==glv){
			if(i>0) { // non-primary view can be closed safely
				glViews[i]=shared_ptr<GLViewer>(); // will close the window as well
				return;
			}else{ // requesting closing primary view
				bool noOtherViews=true;
				// iff all other views are empty, we can close the primary view
				for(size_t j=1; j<glViews.size(); j++){
					if(glViews[j]) noOtherViews=false;
				}
				if(noOtherViews){
					LOG_DEBUG("Deleting primary view (no other views exist).");
					glViews.clear();
					// why delete renderer if we close view? That obliterates all renderer settings.
					// renderer=shared_ptr<OpenGLRenderingEngine>();
				}else{LOG_INFO("Cannot close primary view, other views still exist.");}
				return;
			}
		}
	}
	LOG_ERROR("No such GLView?! "<<glv);
}

/* Close the last view (default); close the i-th view from the beginning (0=primary) */
void YadeQtMainWindow::closeView(int ii=-1){
	if(ii<0){ // get the last non-NULL view
		for(size_t i=glViews.size()-1; i>=0; i--){
			if(glViews[i]) { closeView(glViews[i].get()); return; }
		}
	} else {	if(glViews[ii]) closeView(glViews[ii].get()); }
}

size_t YadeQtMainWindow::viewNo(shared_ptr<GLViewer> g){
	for(size_t i=0; i<glViews.size(); i++) if(glViews[i]==g) return i;
	throw std::invalid_argument("No such view");
}

void YadeQtMainWindow::closeAllChilds(bool closeGL){
	if(closeGL){ while(glViews.size()>0 && glViews[0]!=NULL) { LOG_DEBUG("glViews.size()="<<glViews.size()<<", glViews[0]="<<glViews[0]); closeView(-1);} }
	if(controller) controller=shared_ptr<SimulationController>();
	if(generator) generator=shared_ptr<QtFileGenerator>();
}

void YadeQtMainWindow::customEvent(QCustomEvent* e){
	switch(e->type()){
		case EVENT_CONTROLLER: createController(); break;
		case EVENT_VIEW: createView(); break;
		case EVENT_GENERATOR: createGenerator(); break;
		case EVENT_RESIZE_VIEW: {
			vector<int>* d=(vector<int>*)e->data();
			if(glViews.size()<(size_t)(d->at(0)+1) || !glViews[d->at(0)]) LOG_ERROR("No view #"+lexical_cast<string>(d->at(0))+", resize request ignored");
			glViews[d->at(0)]->resize(d->at(1),d->at(2));
			delete d;
		}
		#if 0
		case EVENT_RESTORE_GLVIEWER_NUM:
			{if(glViews.size()<1 || !glViews[0]){ LOG_ERROR("No primary view to set attributes on, ignored.");} else{glViews[0]->useDisplayParameters(*(int*)(e->data())); delete (int*)e->data(); }} break;
		case EVENT_RESTORE_GLVIEWER_STR:
			{if(glViews.size()<1 || !glViews[0]){ LOG_ERROR("No primary view to set attributes on, ignored.");} else{glViews[0]->setState(*(string*)(e->data())); delete (string*)e->data();}} break;
		#endif
		default: ;
	}
}

