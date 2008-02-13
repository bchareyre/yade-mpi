/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ENGINEUNIT_HPP
#define ENGINEUNIT_HPP

#include<yade/lib-serialization/Serializable.hpp>

class EngineUnit : public Serializable
{
	public: virtual vector<std::string> getFunctorTypes(){throw;}
	REGISTER_CLASS_NAME(EngineUnit);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(EngineUnit,false);

#endif // ENGINEUNIT_HPP

