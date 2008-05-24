/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include"YadeQtMainWindow.hpp"
#include"YadeCamera.hpp"
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
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

using namespace std;

CREATE_LOGGER(YadeQtMainWindow);

YadeQtMainWindow* YadeQtMainWindow::self=NULL;


YadeQtMainWindow::YadeQtMainWindow() : YadeQtGeneratedMainWindow()
{
	self=this;

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
	
//	connect(btGenerator,SIGNAL(clicked()),this,SLOT(createGenerator()));
//	connect(btPlayer,SIGNAL(clicked()),this,SLOT(createPlayer()));
//	connect(btController,SIGNAL(clicked()),this,SLOT(createSimulationController()));

	// HACK
	if(Omega::instance().getSimulationFileName()!="") createSimulationController();

	// updates GL views, may also hide/show this window as needed
	startTimer(100);
}

void YadeQtMainWindow::timerEvent(QTimerEvent* evt){
	// if hidden, GUI operations from within ipython will not work...
	#if 0
	shared_ptr<MetaBody> rb=Omega::instance().getRootBody();
		if((rb && rb->bodies->size()>0) || simulationController) {this->hide();}
		else {this->show(); }
	#endif
	// update GL views (if any)
	FOREACH(const shared_ptr<GLViewer>& glv,glViews){ if(glv) glv->updateGL(); }
	
}

void YadeQtMainWindow::loadSimulation(string file){
	createSimulationController();
	simulationController->loadSimulationFromFileName(file);
}

void YadeQtMainWindow::centerViews(){
	FOREACH(const shared_ptr<GLViewer>& glv,glViews){ if(glv) glv->centerScene(); }
}


YadeQtMainWindow::~YadeQtMainWindow()
{
	filesystem::path yadeQtGUIPrefPath = filesystem::path( Omega::instance().yadeConfigPath + "/QtGUIPreferences.xml", filesystem::native);

	preferences->mainWindowPositionX	= pos().x();
	preferences->mainWindowPositionY	= pos().y();
	preferences->mainWindowSizeX		= size().width();
	preferences->mainWindowSizeY		= size().height();
	IOFormatManager::saveToFile("XMLFormatManager",yadeQtGUIPrefPath.string(),"preferences",preferences);
}

#if 0
void YadeQtMainWindow::addMenu(string menuName)
{
	if (!menuNameExists(menuName))
		menus.push_back(pair<string,QPopupMenu*>(menuName,new QPopupMenu(this)));

}


void YadeQtMainWindow::addItem(string menuName, string itemName,string className)
{
	if (menuNameExists(menuName))
	{
		item2ClassName[itemName] = className;
		items.push_back(new QAction(this, itemName.c_str()));
		items.back()->setText( itemName.c_str() );
		items.back()->setMenuText( itemName.c_str() );
		items.back()->setToolTip( ("Load plugin "+ClassFactory::instance().libNameToSystemName(className)).c_str() );
		items.back()->addTo(getPopupMenu(menuName));
		connect( items.back(), SIGNAL( activated() ), this, SLOT( dynamicMenuClicked() ) );
	}
}


void YadeQtMainWindow::createMenus()
{
	vector<pair<string,QPopupMenu*> >::iterator mi    = menus.begin();
	vector<pair<string,QPopupMenu*> >::iterator miEnd = menus.end();
	for(;mi!=miEnd;++mi)
		MenuBar->insertItem( QString((*mi).first), (*mi).second);

}

void YadeQtMainWindow::dynamicMenuClicked()
{
	QAction * action = (QAction*)(this->sender());
	string name = action->text();
	qtWidgets.push_back(ClassFactory::instance().createShared(item2ClassName[name]));

	shared_ptr<QWidget> widget = dynamic_pointer_cast<QWidget>(qtWidgets.back());
	if (widget) // the library is a QWidget so we set workspace as its parent
	{
		widget->show();
	}
}
#endif



void YadeQtMainWindow::closeSimulationControllerEvent() { deleteSimulationController(); }

void YadeQtMainWindow::Quit(){ emit close(); }
#define RUN_PLUGIN_AS_WIDGET(method,plugin) \
	void YadeQtMainWindow::method(){cerr<<plugin<<endl; shared_ptr<Factorable> tmp=ClassFactory::instance().createShared(plugin); qtWidgets.push_back(tmp); shared_ptr<QWidget> widget=dynamic_pointer_cast<QWidget>(tmp); if(widget) widget->show(); else LOG_FATAL("Widget of type "<<plugin" not created?"); }
RUN_PLUGIN_AS_WIDGET(createGenerator,"QtFileGenerator")
RUN_PLUGIN_AS_WIDGET(createPlayer,"QtSimulationPlayer")

void YadeQtMainWindow::closeEvent(QCloseEvent *e)
{
	simulationController=shared_ptr<SimulationController>();
	YadeQtGeneratedMainWindow::closeEvent(e);
}


	/******************************************************************************************/


void YadeQtMainWindow::ensureRenderer(){
	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	renderer = static_pointer_cast<RenderingEngine>(tmpRenderer);

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

void YadeQtMainWindow::createSimulationController(){
	if(!simulationController){
		simulationController=shared_ptr<SimulationController>(new SimulationController(NULL));
		simulationController->show();
		connect(simulationController.get(),SIGNAL(closeSignal()),this,SLOT(closeSimulationControllerEvent()));
	}
	else{
		simulationController->show(); // if hidden
		simulationController->raise(); //bring to fg
	}
}

void YadeQtMainWindow::deleteSimulationController(){
	if(simulationController) simulationController=shared_ptr<SimulationController>();
	Omega::instance().setSimulationFileName("");
}

void YadeQtMainWindow::createView(){
	ensureRenderer();

	QGLFormat format; QGLFormat::setDefaultFormat(format);
	format.setStencil(TRUE); format.setAlpha(TRUE);
	bool isFirst=glViews.empty();
	shared_ptr<GLViewer> glv=shared_ptr<GLViewer>(new GLViewer(glViews.size(),renderer,format,NULL,isFirst?NULL:glViews[0].get()));
	glv->setCamera(new YadeCamera);
	glv->camera()->frame()->setWheelSensitivity(-1.0f);
	glv->centerScene();
	glViews.push_back(glv);

	//connect( glv, SIGNAL(closeSignal(int)), this, SLOT( closeGLViewEvent(int) ) );

}

void YadeQtMainWindow::closeView(GLViewer* glv){
	for(size_t i=0; i<glViews.size(); i++){
		if(glViews[i].get()==glv){
			if(i>0) { // non-primary view can be closed safely
				glViews[i]=shared_ptr<GLViewer>(); // will close the window as well (?)
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
				}else{LOG_INFO("Cannot close primary view, other views still exist.");}
				return;
			}
		}
	}
	LOG_FATAL("No such view???");
}

/* Close the last view (default); close the i-th view from the beginning (0=primary) */
void YadeQtMainWindow::closeView(int ii=-1){
	if(ii<0){ // get the last non-NULL view
		for(size_t i=glViews.size()-1; i>=0; i--){
			if(glViews[i]) { closeView(glViews[i].get()); return; }
		}
	} else {	if(glViews[ii]) closeView(glViews[ii].get()); }
}



