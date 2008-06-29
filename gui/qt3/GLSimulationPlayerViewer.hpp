/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONPLAYERVIEWER_HPP
#define SIMULATIONPLAYERVIEWER_HPP

#include<yade/core/Omega.hpp>
#include<yade/core/RenderingEngine.hpp>
#include<yade/core/MetaBody.hpp>
//#include<yade/lib-QGLViewer/qglviewer.h>
#include<yade/gui-qt3/GLViewer.hpp>

#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class QtSimulationPlayer;

class GLSimulationPlayerViewer: public GLViewer {
	private :
		shared_ptr<MetaBody>		 rootBody;
		void tryFillingOutputPattern();	
	public:
		QtSimulationPlayer* simPlayer;
		boost::posix_time::ptime lastCheckPointTime;
		long lastCheckPointFrame;
		string fileName, inputBaseName, inputBaseDirectory, outputBaseName, outputBaseDirectory;
		bool saveSnapShots;
		int frameNumber;
		bool loadPositionOrientationFile();
		list<string> xyzFiles;
		list<string>::iterator xyzFilesIter;
	public :
		GLSimulationPlayerViewer(QWidget* parent,char* name);
		virtual ~GLSimulationPlayerViewer(){};
		void setRootBody(shared_ptr<MetaBody> rb) { rootBody = rb;};
		void load(const string& fileName, bool fromFile=true);
		void doOneStep();
		void reset();
	protected :
		virtual void animate();
		virtual void initializeGL();
	DECLARE_LOGGER;
};

#endif // SIMULATIONVIEWER_HPP

