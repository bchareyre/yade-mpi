/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtSphericalDEM.hpp"
#include "FileDialog.hpp"
#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <yade/yade-lib-factory/ClassFactory.hpp>
#include <yade/yade-core/FileGenerator.hpp>
#include <yade/yade-core/Omega.hpp>


QtSphericalDEM::QtSphericalDEM ( QWidget * parent , const char * name) : QtGeneratedSphericalDEMSimulator(parent,name)
{
	shared_ptr<Factorable> f = ClassFactory::instance().createShared("SphericalDEMSimulator");
	simulator = static_pointer_cast<StandAloneSimulator>(f);
}


QtSphericalDEM::~QtSphericalDEM()
{

}


void QtSphericalDEM::pbPathClicked()
{
	string selectedFilter;
	std::vector<string> filters;
	filters.push_back("XML Yade File (*.xml)");
	string fileName = FileDialog::getOpenFileName(".", filters, "Choose a file to open", parentWidget()->parentWidget(), selectedFilter );
		
	if (fileName.size()!=0 && selectedFilter == "XML Yade File (*.xml)" )
		leConfigurationFile->setText(fileName.c_str());

}


void QtSphericalDEM::pbLoadClicked()
{
	simulator->loadConfigurationFile(leConfigurationFile->text().data());
}


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

	simulator->setRecording(record);
	simulator->setRecordingProperties(sbInterval->value(),leOutputDirectory->text(),leOutputBaseName->text(),sbPaddle->value());
	simulator->run(maxIteration);
	tlDurationValue->setText(lexical_cast<string>(chron.stop()).c_str());
	tlIteration->setText(lexical_cast<string>(maxIteration).c_str());
}


void QtSphericalDEM::pbOutputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where to save the data", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leOutputDirectory->setText(directory.c_str());
}


void QtSphericalDEM::pbStopClicked()
{
	stop = true;
}


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



void QtSphericalDEM::closeEvent(QCloseEvent *evt)
{
	close();
	QtGeneratedSphericalDEMSimulator::closeEvent(evt);
}

