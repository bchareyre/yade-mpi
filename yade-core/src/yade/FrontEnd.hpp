/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FRONTEND_HPP
#define FRONTEND_HPP

#include "Omega.hpp"

#include <yade/yade-lib-serialization/IOFormatManager.hpp>
#include <yade/yade-lib-factory/Factorable.hpp>

class FrontEnd : public Factorable
{	
	public :
		FrontEnd ();
		virtual ~FrontEnd ();

		virtual int run(int , char * []) { return -1;};

	REGISTER_CLASS_NAME(FrontEnd);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

REGISTER_FACTORABLE(FrontEnd);

#endif // FRONTEND_HPP

