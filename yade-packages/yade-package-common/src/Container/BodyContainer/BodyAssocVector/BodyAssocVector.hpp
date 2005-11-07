/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef BODYASSOCVEC_HPP
#define BODYASSOCVEC_HPP

#include <yade/yade-core/BodyContainer.hpp>
#include <yade/yade-lib-loki/AssocVector.hpp>

#include <list>

class Body;

using namespace boost;

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
		virtual shared_ptr<Body>& operator[](unsigned int);
		virtual const shared_ptr<Body>& operator[](unsigned int) const;

		virtual BodyContainer::iterator begin();
        	virtual BodyContainer::iterator end();

		virtual unsigned int size();

	REGISTER_CLASS_NAME(BodyAssocVector);
	REGISTER_BASE_CLASS_NAME(BodyContainer);
};

REGISTER_SERIALIZABLE(BodyAssocVector,false);

#endif // BODYASSOCVEC_HPP

