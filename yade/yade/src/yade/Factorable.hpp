/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FACTORABLE__
#define __FACTORABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_CLASS(name,isFundamental) 						\
	REGISTER_CLASS_TO_FACTORY(name,name,SERIALIZABLE,isFundamental);
	
#define REGISTER_CUSTOM_CLASS(name,sname,isFundamental) 				\
	REGISTER_CLASS_TO_FACTORY(name,sname,CUSTOM_CLASS,isFundamental);

//#define REGISTER_FUNDAMENTAL_CLASS(name) 						
//	REGISTER_CLASS_TO_FACTORY(name,name,FACTORABLE,true);

//#define REGISTER_CUSTOM_FUNDAMENTAL(name,sname) 				
//	REGISTER_CLASS_TO_FACTORY(name,sname,CUSTOM_FUNDAMENTAL,true);

//#define REGISTER_CUSTOM_POLYMORPHIC_BASE(base,derived)	
//	public : virtual any getCustomPolymorphicBase() { return #cn; };
	
//#define REGISTER_CUSTOM_TEMPLATE_CLASS(name,templateType)			
//	REGISTER_TEMPLATE_CLASS_TO_FACTORY(name,templateType, CUSTOM_CLASS);
		
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Factorable
{
	public    : Factorable();
	public    : virtual ~Factorable();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __FACTORABLE__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


