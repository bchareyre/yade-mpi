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
	
	se3s.resize(125002);
	fileName = "/disc/yade/Compressed/bigsimu_";
	frameNumber = 0;
	for(int i=0;i<125002;i++)
		se3s[i].resize(7);

	setSnapshotFilename("/disc/yade/images/Spheres");
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
		//renderer->render(rootBody);
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
		//renderer->render(rootBody);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLSimulationPlayerViewer::animate()
{
	system(("cp "+fileName+lexical_cast<string>(frameNumber)+".bz2 tmp.bz2").c_str());
	system("bzip2 -d tmp.bz2");
		
 	ifstream f("tmp",ifstream::binary);
	
	for(int i=0;i<125002;i++)
		f >> se3s[i][0] >>se3s[i][1] >>se3s[i][2] >>se3s[i][3] >>se3s[i][4] >>se3s[i][5] >>se3s[i][6];

	shared_ptr<BodyContainer> bodies = rootBody->bodies;
	shared_ptr<Body> b;
	int i=1;

	BodyContainer::iterator bci = bodies->begin();
	BodyContainer::iterator bciEnd = bodies->end();
	++bci;
	for( ; bci!=bciEnd ; ++bci , ++i )
	{
		b = *bci;		
		b->physicalParameters->se3 = Se3r(Vector3r(se3s[i][0],se3s[i][1],se3s[i][2]),Quaternionr(se3s[i][3], se3s[i][4], se3s[i][5], se3s[i][6]));
	}

 	f.close();
	system("rm tmp");

	setSnapshotFormat("BMP");
	saveSnapshot(true);
	frameNumber++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// void SimulationPlayer::generate(QWidget * parent)
// {
// 	viewer		= new SimulationViewer(parent);
// 	controller	= new SimulationPlayerController(parent);
// 	viewer->show();
// 	controller->show();
// 	
// 	IOManager::loadFromFile("XMLManager","/disc/yade/125000Spheres.xml","rootBody",rootBody);
// 
// 	shared_ptr<BodyContainer> bodies = rootBody->bodies;
// 	shared_ptr<Body> b;
// 	int i=0;
// 	bodies->gotoFirst();
// 	bodies->gotoNext();
// 	for( ; bodies->notAtEnd() ; bodies->gotoNext() , ++i )
// 	{
// 		b = bodies->getCurrent();
// 		if (b->physicalParameters->se3.position[0]<-200)
// 			b->geometricalModel->diffuseColor = Vector3r(1,0,1);
// 		else if (b->physicalParameters->se3.position[0]<-100)
// 			b->geometricalModel->diffuseColor = Vector3r(0,1,1);
// 		else if (b->physicalParameters->se3.position[0]<0)
// 			b->geometricalModel->diffuseColor = Vector3r(1,1,0);
// 		else if (b->physicalParameters->se3.position[0]<100)
// 			b->geometricalModel->diffuseColor = Vector3r(0,1,0);
// 		else if (b->physicalParameters->se3.position[0]<200)
// 			b->geometricalModel->diffuseColor = Vector3r(1,0,0);
// 		else if (b->physicalParameters->se3.position[0]<300)
// 			b->geometricalModel->diffuseColor = Vector3r(0,0,1);
// 	}
// 	
// 	viewer->setRootBody(rootBody);
// 	
// }
