/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtSimulationPlayer.hpp"
#include "FileDialog.hpp"
#include "GLSimulationPlayerViewer.hpp"
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/FileGenerator.hpp>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

QtSimulationPlayer::QtSimulationPlayer() : QtGeneratedSimulationPlayer()
{
	
}


QtSimulationPlayer::~QtSimulationPlayer()
{
}


void QtSimulationPlayer::pbInputConfigFileClicked()
{
	string selectedFilter;
	string fileName = FileDialog::getOpenFileName("../data", "XML Yade File (*.xml)", "Choose a file to load", this->parentWidget()->parentWidget(),selectedFilter );
 
	if (!fileName.empty() && selectedFilter == "XML Yade File (*.xml)")
		leInputConfigFile->setText(fileName);
}



void QtSimulationPlayer::pbInputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where the recorded file are", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leInputDirectory->setText(directory);
}


void QtSimulationPlayer::pbLoadClicked()
{
	glSimulationPlayerViewer->load(leInputConfigFile->text());
}


void QtSimulationPlayer::pbOutputDirectoryClicked()
{
	string directory = FileDialog::getExistingDirectory ( "../data","Choose the directory where to save the sna shots", this->parentWidget()->parentWidget());
	if (!directory.empty())
		leOutputDirectory->setText(directory.c_str());

}


void QtSimulationPlayer::pbPlayClicked()
{	
	setParameters();

	glSimulationPlayerViewer->startAnimation();

}


void QtSimulationPlayer::pbPauseClicked()
{	
	glSimulationPlayerViewer->stopAnimation();
}


void QtSimulationPlayer::pbStepClicked()
{
	glSimulationPlayerViewer->stopAnimation();
	glSimulationPlayerViewer->doOneStep();
}


void QtSimulationPlayer::pbResetClicked()
{
	setParameters();
	glSimulationPlayerViewer->reset();
}


void QtSimulationPlayer::cbSaveSnapShotsToggled(bool b)
{
	glSimulationPlayerViewer->setSaveSnapShots(b);
}


void QtSimulationPlayer::closeEvent(QCloseEvent *e)
{
	QtGeneratedSimulationPlayer::closeEvent(e);
	
}


void QtSimulationPlayer::setParameters()
{
	glSimulationPlayerViewer->setInputPaddle(sbInputPaddle->value());
	glSimulationPlayerViewer->setInputBaseName(leInputBaseName->text());
	glSimulationPlayerViewer->setInputDirectory(leInputDirectory->text());
	
	glSimulationPlayerViewer->setOutputBaseName(leOutputBaseName->text());
	glSimulationPlayerViewer->setOutputDirectory(leOutputDirectory->text());
}


