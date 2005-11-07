/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONVIEWER_HPP
#define SIMULATIONVIEWER_HPP

#include <QGLViewer/qglviewer.h>

#include <yade/yade-core/RenderingEngine.hpp>
#include <yade/yade-core/MetaBody.hpp>

class GLSimulationPlayerViewer : public QGLViewer
{
/// Attributes
	private :
		shared_ptr<RenderingEngine>	 renderer;
		shared_ptr<MetaBody>		 rootBody;
		vector<vector<float> >		 se3s;
		
		string				 fileName
						,inputBaseName
						,inputBaseDirectory
						,outputBaseName
						,outputBaseDirectory;

		int				 inputPaddle;

		bool				 saveSnapShots;

		int				 frameNumber;
		
		bool loadPositionOrientationFile();
	
	public :
		GLSimulationPlayerViewer(QWidget * parent=0,char*name=0);
		virtual ~GLSimulationPlayerViewer();

		void setRootBody(shared_ptr<MetaBody> rb) { rootBody = rb;};
		void load(const string& fileName);

		void setInputPaddle(int p) { inputPaddle = p;};
		void setInputBaseName(const string& baseName) { inputBaseName = baseName;};
		void setInputDirectory(const string& baseDirectory) { inputBaseDirectory = baseDirectory;};
		
		void setOutputBaseName(const string& baseName) { outputBaseName = baseName;};
		void setOutputDirectory(const string& baseDirectory) { outputBaseDirectory = baseDirectory;};
		void setSaveSnapShots(bool b) { saveSnapShots = b;};
		
		void doOneStep();
		void reset();

	protected :
		virtual void draw();
		virtual void fastDraw();
		virtual void animate();
		virtual void initializeGL();
};

#endif // SIMULATIONVIEWER_HPP

