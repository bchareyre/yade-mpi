/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "BcpConnection.hpp"


BcpConnection::BcpConnection() : SimpleViscoelasticBodyParameters()
{
	createIndex();
        id1 = id2 = 0;
}

BcpConnection::~BcpConnection()
{
}


void BcpConnection::registerAttributes()
{
	SimpleViscoelasticBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
}

YADE_PLUGIN("BcpConnection");
