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
#include"Omega.hpp"
#include<yade/core/Timing.hpp>

class Body;

class Engine : public Serializable
{
	public :
		string label; /* user-definable label, to convenienty retrieve this particular engine instance even if multiple engines of the same type exist */
		TimingInfo timingInfo; // hihg-level profiling information; not serializable
		shared_ptr<TimingDeltas> timingDeltas;
		Engine() {};
		virtual ~Engine() {};
	
		virtual bool isActivated(MetaBody*) { return true; };
		virtual void action(MetaBody*) { throw; };
		/* returns all BodyExternalVariable's (Bex; formerly PhysicalActions) that this engine needs */
		virtual list<string> getNeededBex(){return list<string>();}
	REGISTER_ATTRIBUTES(/*no base*/,(label));
	REGISTER_CLASS_AND_BASE(Engine,Serializable);
};
REGISTER_SERIALIZABLE(Engine);



