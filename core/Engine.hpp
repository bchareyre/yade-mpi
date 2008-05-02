/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include "Omega.hpp"

class Body;

class Engine : public Serializable
{
	public :
		Engine() {};
		virtual ~Engine() {};
	
		virtual bool isActivated() { return true; };
		virtual void action(MetaBody*) { throw; };
		/* returns all BodyExternalVariable's (Bex; formerly PhysicalActions) that this engine needs */
		virtual list<string> getNeededBex(){return list<string>();}

	REGISTER_CLASS_NAME(Engine);
	REGISTER_BASE_CLASS_NAME(Serializable);

};

REGISTER_SERIALIZABLE(Engine,false);

#endif // ENGINE_HPP


