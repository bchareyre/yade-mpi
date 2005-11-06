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

#ifndef __SIMULATIONVIEWER_HPP__
#define __SIMULATIONVIEWER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QGLViewer/qglviewer.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/RenderingEngine.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLSimulationPlayerViewer : public QGLViewer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes										      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : shared_ptr<RenderingEngine> renderer;
	private : shared_ptr<MetaBody> rootBody;
	private : vector<vector<float> > se3s;
	
	private : string fileName;

	private : string inputBaseName;
	private : string inputBaseDirectory;
	private : int inputPaddle;

	private : string outputBaseName;
	private : string outputBaseDirectory;
	
	private : bool saveSnapShots;

	private : int frameNumber;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor								      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : GLSimulationPlayerViewer(QWidget * parent=0,char*name=0);

	/*! Destructor */
	public : virtual ~GLSimulationPlayerViewer();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods										      ///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : void setRootBody(shared_ptr<MetaBody> rb) { rootBody = rb;};
	public : void load(const string& fileName);

	private : bool loadPositionOrientationFile();

	protected : virtual void draw();
	protected : virtual void fastDraw();
	protected : virtual void animate();
	protected : virtual void initializeGL();

	public : void setInputPaddle(int p) { inputPaddle = p;};
	public : void setInputBaseName(const string& baseName) { inputBaseName = baseName;};
	public : void setInputDirectory(const string& baseDirectory) { inputBaseDirectory = baseDirectory;};
	
	public : void setOutputBaseName(const string& baseName) { outputBaseName = baseName;};
	public : void setOutputDirectory(const string& baseDirectory) { outputBaseDirectory = baseDirectory;};
	public : void setSaveSnapShots(bool b) { saveSnapShots = b;};
	
	public : void doOneStep();
	public : void reset();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// CALL REGISTER_SERIALIZABLE OR REGISTER_FACTORABLE MACRO HERE

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SIMULATIONVIEWER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

