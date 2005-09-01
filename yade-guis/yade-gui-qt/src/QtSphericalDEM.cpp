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

#include "QtSphericalDEM.hpp"
#include "FileDialog.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-factory/ClassFactory.hpp>
#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-core/Omega.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtSphericalDEM::QtSphericalDEM ( QWidget * parent , const char * name) : QtGeneratedSphericalDEMSimulator(parent,name)
{
	shared_ptr<Factorable> f = ClassFactory::instance().createShared("SphericalDEMSimulator");
	simulator = static_pointer_cast<StandAloneSimulator>(f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtSphericalDEM::~QtSphericalDEM()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSphericalDEM::pbPathClicked()
{
	string selectedFilter;
	string fileName = FileDialog::getOpenFileName(".", "XML Yade File (*.xml)", "Choose a file to open", parentWidget()->parentWidget(), selectedFilter );
		
	if (fileName.size()!=0 && selectedFilter == "XML Yade File (*.xml)" )
		leConfigurationFile->setText(fileName.c_str());

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSphericalDEM::pbLoadClicked()
{
	simulator->loadConfigurationFile(leConfigurationFile->text().data());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

///FIXME : use thread here !
void QtSphericalDEM::pbStartClicked()
{
	maxIteration = sbNbIterations->value();
	currentIteration = 0;
	stop = false;
	chron.start();
/*	while (currentIteration!=maxIteration && !stop)
	{
		currentIteration++;
		tlIteration->setText(lexical_cast<string>(currentIteration).c_str());
		simulator->doOneIteration();
		tlDurationValue->setText(lexical_cast<string>(chron.getTime()).c_str());
		repaint();
	}*/

	simulator->run(maxIteration,record,sbInterval->value(),leOutputDirectory->text(),leOutputBaseName->text(),sbPaddle->value());
	tlDurationValue->setText(lexical_cast<string>(chron.stop()).c_str());
	tlIteration->setText(lexical_cast<string>(maxIteration).c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSphericalDEM::pbOutputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where to save the data", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leOutputDirectory->setText(directory.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSphericalDEM::pbStopClicked()
{
	stop = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtSphericalDEM::bgTimeStepClicked(int i)
{
	if (i==0)
	{
		Real dt = lexical_cast<Real>(leTimeStep->text().data());
		simulator->setTimeStep(dt);
	}
	else
		simulator->setTimeStep(-1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


void QtSphericalDEM::closeEvent(QCloseEvent *evt)
{
	close();
	QtGeneratedSphericalDEMSimulator::closeEvent(evt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
