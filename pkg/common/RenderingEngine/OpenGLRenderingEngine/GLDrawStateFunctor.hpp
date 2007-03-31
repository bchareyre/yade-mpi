/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_STATE_FUNCTOR_HPP
#define GLDRAW_STATE_FUNCTOR_HPP

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/PhysicalParameters.hpp>
#include<yade/core/EngineUnit1D.hpp>


#define RENDERS(name) 							\
	public : virtual string renders() const { return #name; };

class GLDrawStateFunctor
	: public EngineUnit1D
	<
		void ,
		TYPELIST_1(const shared_ptr<PhysicalParameters>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */) 
	>
{
	public : 
		virtual ~GLDrawStateFunctor() {};
		virtual string renders() const { std::cerr << "unregistered gldraw class.\n"; throw; }; // FIXME - replace that with a nice exception
	
	REGISTER_CLASS_NAME(GLDrawStateFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(GLDrawStateFunctor,false);

#endif //  GLDRAWGEOMETRICALMODELFUNCTOR_HPP


