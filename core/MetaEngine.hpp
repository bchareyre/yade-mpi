/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METAENGINE_HPP
#define METAENGINE_HPP

#include "Engine.hpp"

class MetaEngine : public Engine
{
	public :
		MetaEngine() {};
		virtual ~MetaEngine() {};

		virtual string getEngineUnitType() { throw; };

	REGISTER_CLASS_NAME(MetaEngine);	
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(MetaEngine,false);

#endif // METAENGINE_HPP

