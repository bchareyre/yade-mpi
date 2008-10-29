/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BCP_CONNECTION_HPP
#define BCP_CONNECTION_HPP

#include<yade/core/PhysicalParameters.hpp>
//#include<Wm3Vector3.h>
//#include<yade/lib-base/yadeWm3.hpp>
//#include<yade/lib-base/yadeWm3Extra.hpp>

class BcpConnection : public PhysicalParameters
{
	public :
		// parameters (no parameters)

		// state
		unsigned int id1, id2;

		BcpConnection();
		virtual ~BcpConnection();

/// Serializable
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(BcpConnection);
	REGISTER_BASE_CLASS_NAME(BcpConnection);

/// Indexable
	REGISTER_CLASS_INDEX(BcpConnection,PhysicalParameters);

};

REGISTER_SERIALIZABLE(BcpConnection,false);

#endif // BCP_CONNECTION_HPP

