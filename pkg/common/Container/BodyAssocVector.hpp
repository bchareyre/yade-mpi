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

#include<yade/core/BodyContainer.hpp>
#include<yade/lib-loki/AssocVector.hpp>

#include<list>
#include<vector>

using namespace std;
using namespace boost;

class Body;

class BodyAssocVectorIterator : public BodyContainerIterator 
{
	public :
		Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii; // FIXME - private maybe?

		BodyAssocVectorIterator();
		~BodyAssocVectorIterator();
		
		virtual bool isDifferent(const BodyContainerIterator& i);
		virtual void affect(const BodyContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Body> getValue();
		virtual shared_ptr<BodyContainerIterator> createPtr();

};



class BodyAssocVector : public BodyContainer
{
	private	:
		Loki::AssocVector<unsigned int , shared_ptr<Body> > bodies;

	public	:
		BodyAssocVector();
		virtual ~BodyAssocVector();

		virtual unsigned int insert(shared_ptr<Body>&);
		virtual unsigned int insert(shared_ptr<Body>& , unsigned int);
		virtual void clear();
		virtual bool erase(unsigned int);
		virtual bool find(unsigned int , shared_ptr<Body>&) const;
		virtual bool exists(unsigned int id) const;
		virtual shared_ptr<Body>& operator[](unsigned int);
		virtual const shared_ptr<Body>& operator[](unsigned int) const;

		virtual BodyContainer::iterator begin();
        	virtual BodyContainer::iterator end();

		virtual unsigned int size();

	REGISTER_CLASS_NAME(BodyAssocVector);
	REGISTER_BASE_CLASS_NAME(BodyContainer);
};

REGISTER_SERIALIZABLE(BodyAssocVector,false);

