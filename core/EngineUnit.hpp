/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>

class TimingDeltas;

class EngineUnit : public Serializable
{
	public: virtual vector<std::string> getFunctorTypes(){throw;}
		virtual list<string> getNeededBex(){return list<string>();}
	shared_ptr<TimingDeltas> timingDeltas;
	REGISTER_CLASS_AND_BASE(EngineUnit,Serializable);
};

REGISTER_SERIALIZABLE(EngineUnit);


