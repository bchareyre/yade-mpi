/*************************************************************************
*    Copyright (C) 2009 Anton Gladkyy gladky.anton@gmail.com             *
*                                                                        *
*    This program is free software: you can redistribute it and/or modify*
*    it under the terms of the GNU General Public License as published by*
*    the Free Software Foundation, either version 3 of the License, or   *
*    (at your option) any later version.                                 *
*                                                                        *
*    This program is distributed in the hope that it will be useful,     *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*    GNU General Public License for more details.                        *
*                                                                        *
*    You should have received a copy of the GNU General Public License   *
*    along with this program.  If not, see <http://www.gnu.org/licenses/>*
**************************************************************************/

#pragma once
#include"TranslationEngine.hpp"
#include<yade/core/DeusExMachina.hpp>

class PressTestEngine: public TranslationEngine{
	/*
	 * This class simulates the simple press work
	 * When the press "cracks" the solid brittle material,
	 * it returns back to the initial position 
	 * and stops the simulation loop.
	 */ 
	public:
		enum motionDirection {forward, backward, stop};
		motionDirection curentDirection;
		Real currentVerticalForce, maxVerticalForce, minimalForce;
		long int numberIterationAfterDestruction, currentIterationAfterDestruction;
		int pressVelocityForw2Back;
		PressTestEngine(): curentDirection(forward), currentVerticalForce(0), maxVerticalForce(0), currentIterationAfterDestruction(0), pressVelocityForw2Back(25) {};
		virtual ~PressTestEngine(){};
		virtual void applyCondition(MetaBody*);
	REGISTER_CLASS_AND_BASE(PressTestEngine,TranslationEngine);
	REGISTER_ATTRIBUTES(TranslationEngine, (numberIterationAfterDestruction));
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PressTestEngine);
