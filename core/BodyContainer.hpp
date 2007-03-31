/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef BODYCONTAINER_HPP
#define BODYCONTAINER_HPP


#include<yade/lib-serialization/Serializable.hpp>


#include "BodyContainerIteratorPointer.hpp"


class Body;


using namespace boost;


class BodyContainer : public Serializable
{
	public :
		BodyContainer();
		virtual ~BodyContainer();

	
		virtual unsigned int insert(shared_ptr<Body>&)			{throw;}; //FIXME : use const shared_ptr<Body>&
		virtual unsigned int insert(shared_ptr<Body>& , unsigned int)	{throw;};
		virtual void clear() 						{throw;};
		virtual bool erase(unsigned int) 				{throw;};
		virtual bool find(unsigned int , shared_ptr<Body>&) const	{throw;};
		virtual bool exists(unsigned int id) const 			{throw;};
		virtual shared_ptr<Body>& operator[](unsigned int)		{throw;};
		virtual const shared_ptr<Body>& operator[](unsigned int) const	{throw;};
		
		typedef BodyContainerIteratorPointer iterator;
		virtual BodyContainer::iterator begin()				{throw;};
		virtual BodyContainer::iterator end()				{throw;};
		
		virtual unsigned int size() 					{throw;};

	protected :
		void setId(shared_ptr<Body>& , unsigned int);

	REGISTER_CLASS_NAME(BodyContainer);
	REGISTER_BASE_CLASS_NAME(Serializable);

	// local storage for uniform serialization of all possible container concrete implementations.
	private :
		vector<shared_ptr<Body> > body; 
	public :
		virtual void registerAttributes();
	protected :
		virtual void preProcessAttributes(bool deserializing);
		virtual void postProcessAttributes(bool deserializing);
};

REGISTER_SERIALIZABLE(BodyContainer,false);

#endif // __BODYCONTAINER_HPP__

