/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef BODYREDIRECTIONVECTOR_HPP
#define BODYREDIRECTIONVECTOR_HPP


#include <yade/yade-core/BodyContainer.hpp>

#include <vector>

class Body;

using namespace boost;

class BodyRedirectionVector : public BodyContainer
{
	private	:
		std::vector< shared_ptr<Body> > bodies;
		std::vector< long int > indexes;

	public :
		BodyRedirectionVector();
		virtual ~BodyRedirectionVector();
	
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

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyRedirectionVector);
	REGISTER_BASE_CLASS_NAME(BodyContainer);
};

REGISTER_SERIALIZABLE(BodyRedirectionVector,false);

#endif // BODYREDIRECTIONVECTOR_HPP

