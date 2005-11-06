/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "GLSimulationPlayerViewer.hpp"
//#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GLSimulationPlayerViewer::GLSimulationPlayerViewer(QWidget * parent,char* name) : QGLViewer(parent,name)
{
	shared_ptr<Factorable> tmpRenderer = ClassFactory::instance().createShared("OpenGLRenderingEngine");
	renderer = static_pointer_cast<RenderingEngine>(tmpRenderer);

	setSceneRadius(2000);
	showEntireScene();
	resize(720, 576);
	setAnimationPeriod(1);
	saveSnapShots = false;
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

GLSimulationPlayerViewer::~GLSimulationPlayerViewer()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::initializeGL()
{
	QGLViewer::initializeGL();
	renderer->init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::draw()
{
	if (rootBody)
	{
		//renderer->drawBoundingVolume = false;
		//renderer->drawGeometricalModel = true;
		renderer->render(rootBody);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::fastDraw()
{
	if (rootBody)
	{
		//renderer->drawBoundingVolume = true;
		//renderer->drawGeometricalModel = false;
		renderer->render(rootBody);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::animate()
{
	if (!loadPositionOrientationFile())
	{
		frameNumber=0;
		stopAnimation();
	}
	else
	{
		shared_ptr<BodyContainer>& bodies = rootBody->bodies;
	
		int i=0;
	
		BodyContainer::iterator bci = bodies->begin();
		BodyContainer::iterator bciEnd = bodies->end();
	
		for( ; bci!=bciEnd ; ++bci , ++i )
			(*bci)->physicalParameters->se3 = Se3r(Vector3r(se3s[i][0],se3s[i][1],se3s[i][2]),Quaternionr(se3s[i][3], se3s[i][4], se3s[i][5], se3s[i][6]));
	
		if (saveSnapShots)
		{
			setSnapshotFilename(outputBaseDirectory+"/"+outputBaseName);
			setSnapshotFormat("BMP");
			saveSnapshot(true);
		}

		frameNumber++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::load(const string& fileName)
{
	IOFormatManager::loadFromFile("XMLFormatManager",fileName,"rootBody",rootBody);
	updateGL();
	frameNumber=0;
	setSnapshotCounter(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::doOneStep()
{
	if (!loadPositionOrientationFile())
	{
		frameNumber=0;
		stopAnimation();
	}
	else
		frameNumber++;
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::reset()
{
	frameNumber = 0;
	setSnapshotCounter(0);
	loadPositionOrientationFile();
	frameNumber++;
	updateGL();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool GLSimulationPlayerViewer::loadPositionOrientationFile()
{

	fileName = inputBaseDirectory+"/"+inputBaseName;

	string num = lexical_cast<string>(frameNumber);
	while (num.size()<inputPaddle)
		num.insert(num.begin(),'0');

	fileName += num;

	//system(("cp "+fileName+lexical_cast<string>(frameNumber)+".bz2 tmp.bz2").c_str());
	//system("bzip2 -d tmp.bz2");
	if (filesystem::exists( fileName ))
	{	
		ifstream f(fileName.c_str());
	
		int nbElements;
	
		f >> nbElements;
	
		int oldSize = se3s.size();
		if (oldSize!=nbElements)
		{
			se3s.resize(nbElements);
			for(int i=oldSize;i<nbElements;i++)
				se3s[i].resize(7);
		}
	
		for(int i=0;i<nbElements;i++)
			f >> se3s[i][0] >>se3s[i][1] >>se3s[i][2] >>se3s[i][3] >>se3s[i][4] >>se3s[i][5] >>se3s[i][6];
	
		f.close();
		//system("rm tmp");
		return true;
	}
		return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

