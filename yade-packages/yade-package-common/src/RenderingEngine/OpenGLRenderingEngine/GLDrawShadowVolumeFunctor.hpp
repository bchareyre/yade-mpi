/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWSHADOWVOLUMEFUNCTOR_HPP
#define GLDRAWSHADOWVOLUMEFUNCTOR_HPP

#include <yade/yade-core/EngineUnit1D.hpp>
#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>

#define RENDERS(name) 							\
	public : virtual string renders() const { return #name; };

class GLDrawShadowVolumeFunctor : 	public EngineUnit1D
					<
		 				void ,
		 				TYPELIST_3(	  const shared_ptr<GeometricalModel>&
		 						, const shared_ptr<PhysicalParameters>&
								, const Vector3r&	
							  )
					>
{
	public:
		virtual string renders() const { std::cerr << "unregistered gldraw class.\n"; throw; }; // FIXME - replace that with a nice exception

	REGISTER_CLASS_NAME(GLDrawShadowVolumeFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(GLDrawShadowVolumeFunctor,false);

#endif //  GLDRAWSHADOWVOLUMEFUNCTOR_HPP

