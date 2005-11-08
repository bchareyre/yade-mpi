/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "StandAloneSimulator.hpp"


StandAloneSimulator::StandAloneSimulator() : Serializable()
{
	record = false;
	interval = 1;
	outputDirectory=".";
	outputBaseName="simulation_";
	paddle=4;
}


void StandAloneSimulator::setRecording(bool record) 
{
	this->record = record;
}

void StandAloneSimulator::setRecordingProperties(int interval, const string& outputDirectory,const string& outputBaseName,int paddle) 
{
	this->interval=interval;
	this->outputDirectory=outputDirectory;
	this->outputBaseName=outputBaseName;
	this->paddle=paddle;
}

