/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef NONLOCALINITIALIZER_HPP
#define NONLOCALINITIALIZER_HPP

#include <yade/yade-core/StandAloneEngine.hpp>
#include "LatticeSetParameters.hpp"

class Body;
class BodyContainer;
class InteractionContainer; 

class NonLocalInitializer : public StandAloneEngine
{
	private:
		//void calcNonLocal(Body* body1, Body* body2, BodyContainer* bodies,InteractionContainer* ints);
		//bool calcNonLocal(Body* body1, Body* body2, BodyContainer* bodies,std::vector<std::list<LatticeSetParameters::NonLocalInteraction , std::__malloc_alloc_template<sizeof(LatticeSetParameters::NonLocalInteraction)> > >& nonl);
		bool calcNonLocal(Body* body1, Body* body2, BodyContainer* bodies,void* nonl);

	public :
		Real 	range;
		
		NonLocalInitializer ();
		virtual ~NonLocalInitializer ();
		virtual void action(Body* b);

	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(NonLocalInitializer);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(NonLocalInitializer,false);

#endif // NONLOCALINITIALIZER_HPP

