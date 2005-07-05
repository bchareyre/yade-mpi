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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BODYASSOCVEC_HPP__
#define __BODYASSOCVEC_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/BodyContainer.hpp>
#include <yade/yade-lib-loki/AssocVector.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <list>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyAssocVector : public BodyContainer
{
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> > bodies;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator temporaryBii;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator biiEnd;
	private	: std::list<Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator> iteratorList;

	public	: BodyAssocVector();
	public	: virtual ~BodyAssocVector();

	public	: virtual unsigned int insert(shared_ptr<Body>&);
	public	: virtual unsigned int insert(shared_ptr<Body>& , unsigned int);
	public	: virtual void clear();
	public	: virtual bool erase(unsigned int);
	public	: virtual bool find(unsigned int , shared_ptr<Body>&) const;
	public	: virtual shared_ptr<Body>& operator[](unsigned int);
	public	: virtual const shared_ptr<Body>& operator[](unsigned int) const;
	public	: virtual void pushIterator();
	public	: virtual void popIterator();

	public	: virtual void gotoFirst();
	public	: virtual bool notAtEnd();
	public	: virtual void gotoNext();
	public	: virtual shared_ptr<Body>& getCurrent();

	public	: virtual unsigned int size();

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyAssocVector);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BodyAssocVector,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYASSOCVEC_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
