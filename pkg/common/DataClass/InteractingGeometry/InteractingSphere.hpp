/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONSPHERE_HPP
#define INTERACTIONSPHERE_HPP

#include<yade/core/InteractingGeometry.hpp>

class InteractingSphere : public InteractingGeometry
{
	public :
		Real radius;

		InteractingSphere ();
		virtual ~InteractingSphere ();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(InteractingSphere);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

/// Indexable
	REGISTER_CLASS_INDEX(InteractingSphere,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingSphere,false);

#endif // INTERACTIONSPHERE_HPP

