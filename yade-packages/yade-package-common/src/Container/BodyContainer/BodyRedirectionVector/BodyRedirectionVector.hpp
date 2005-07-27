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

#ifndef __BODYVECTOR_HPP__
#define __BODYVECTOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/BodyContainer.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyRedirectionVector : public BodyContainer
{
	private	: std::vector< shared_ptr<Body> > bodies;
	private	: std::vector< long int > indexes;
//	private	: std::vector< shared_ptr<Body> >::iterator vii;
//	private	: std::vector< shared_ptr<Body> >::iterator temporaryVii;
//	private	: std::vector< shared_ptr<Body> >::iterator viiEnd;
//	private	: std::list< std::vector< shared_ptr<Body> >::iterator > iteratorList;

	public	: BodyRedirectionVector();
	public	: virtual ~BodyRedirectionVector();

	public	: virtual unsigned int insert(shared_ptr<Body>&);
	public	: virtual unsigned int insert(shared_ptr<Body>& , unsigned int);
	public	: virtual void clear();
	public	: virtual bool erase(unsigned int);
	public	: virtual bool find(unsigned int , shared_ptr<Body>&) const;
	public	: virtual shared_ptr<Body>& operator[](unsigned int);
	public	: virtual const shared_ptr<Body>& operator[](unsigned int) const;

//	public	: virtual void pushIterator();
//	public	: virtual void popIterator();
//	public	: virtual void gotoFirst();
//	public	: virtual bool notAtEnd();
//	public	: virtual void gotoNext();
//	public	: virtual shared_ptr<Body>& getCurrent();

	public    : virtual BodyContainer::iterator begin();
        public    : virtual BodyContainer::iterator end();


	public	: virtual unsigned int size();

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyRedirectionVector);
	REGISTER_BASE_CLASS_NAME(BodyContainer);

};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BodyRedirectionVector,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYVECTOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
