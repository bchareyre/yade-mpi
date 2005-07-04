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

#ifndef __BODYCONTAINER_HPP__
#define __BODYCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-serialization/Serializable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Body;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class BodyContainer : public Serializable
{
	public    : BodyContainer() { body.clear(); };
	public    : virtual ~BodyContainer() {};

	//FIXME : use const shared_ptr<Body>&
	public    : virtual unsigned int insert(shared_ptr<Body>&)			{throw;};
	public    : virtual unsigned int insert(shared_ptr<Body>& , unsigned int)	{throw;};
	public    : virtual void clear() 						{throw;};
	public    : virtual bool erase(unsigned int) 					{throw;};
	public    : virtual bool find(unsigned int , shared_ptr<Body>&) const		{throw;};
	public    : virtual shared_ptr<Body>& operator[](unsigned int)			{throw;};
	public    : virtual const shared_ptr<Body>& operator[](unsigned int) const	{throw;};

	// looping over the data
	public    : virtual void gotoFirst() 						{throw;};
	public    : virtual bool notAtEnd() 						{throw;};
	public    : virtual void gotoNext() 						{throw;};
	public    : virtual shared_ptr<Body>& getCurrent()	 			{throw;};
	public    : virtual void pushIterator()			 			{throw;};
	public    : virtual void popIterator()			 			{throw;};

	public    : virtual unsigned int size() 					{throw;};

	protected : void setId(shared_ptr<Body>& , unsigned int);

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyContainer);
	// local storage for uniform serialization of all possible container concrete implementations.
	private   : vector<shared_ptr<Body> > body; 
	
	public    : virtual void registerAttributes();

	protected : virtual void preProcessAttributes(bool deserializing);
	protected : virtual void postProcessAttributes(bool deserializing);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BodyContainer,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
