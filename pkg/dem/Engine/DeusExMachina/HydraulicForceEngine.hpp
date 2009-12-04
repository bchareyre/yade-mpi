/*************************************************************************
*  Copyright (C) 2004 by Andrea Cortis & Bruno Chareyre                  *
*  acortis@lbl.gov,   bruno.chareyre@hmg.inpg.fr                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>

class HydraulicForceEngine : public DeusExMachina 
{
	public :
		Vector3r gravity;
		bool isActivated;
		bool dummyParameter;
		bool HFinverted;
		bool savePositions;
		string outputFile;
		string inputFile;
		int HFinversion_counter;
		Real forceFactor;
	
		HydraulicForceEngine();
		virtual ~HydraulicForceEngine();
		virtual void applyCondition(Scene*);
	
	REGISTER_ATTRIBUTES(DeusExMachina,(gravity)(isActivated)(dummyParameter)(HFinverted)(savePositions)(outputFile)(inputFile)(HFinversion_counter)(forceFactor));
	REGISTER_CLASS_NAME(HydraulicForceEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};

REGISTER_SERIALIZABLE(HydraulicForceEngine);


