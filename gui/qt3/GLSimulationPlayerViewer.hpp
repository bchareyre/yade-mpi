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
#include<yade/lib-QGLViewer/qglviewer.h>

#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>


class GLSimulationPlayerViewer : public QGLViewer
{
/// Attributes
	private :
		shared_ptr<RenderingEngine>	 renderer;
		shared_ptr<MetaBody>		 rootBody;
	public:
		
		string fileName, inputBaseName, inputBaseDirectory, outputBaseName, outputBaseDirectory;
		bool				 saveSnapShots;
		int				 frameNumber;
		bool loadPositionOrientationFile();
		list<string> xyzFiles;
		list<string>::iterator xyzFilesIter;
	public :
		GLSimulationPlayerViewer(QWidget * parent=0,char*name=0);
		virtual ~GLSimulationPlayerViewer();

		void setRootBody(shared_ptr<MetaBody> rb) { rootBody = rb;};
		void load(const string& fileName);

		
		void doOneStep();
		void reset();

	protected :
		virtual void draw();
		virtual void fastDraw();
		virtual void animate();
		virtual void initializeGL();
	DECLARE_LOGGER;
};

#endif // SIMULATIONVIEWER_HPP

