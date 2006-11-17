/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Quadrilateral.hpp"
#include <yade/yade-core/MetaBody.hpp>

Quadrilateral::Quadrilateral() : GeometricalModel()
{
	createIndex();
}

Quadrilateral::Quadrilateral(int p1,int p2,int p3,int p4,MetaBody* mb) : GeometricalModel()
{
	createIndex();
	n1 = p1;
	n2 = p2;
	n3 = p3;
	n4 = p4;
	#define Q(x) ( (*(mb->bodies))[x]->physicalParameters->se3.position )
	i1 = Q(n1);
	i2 = Q(n2);
	i3 = Q(n3);
	i4 = Q(n4);
	#undef Q
}

Quadrilateral::~Quadrilateral()
{
}

void Quadrilateral::registerAttributes()
{
	GeometricalModel::registerAttributes();
	REGISTER_ATTRIBUTE(n1);
	REGISTER_ATTRIBUTE(n2);
	REGISTER_ATTRIBUTE(n3);
	REGISTER_ATTRIBUTE(n4);
	
	REGISTER_ATTRIBUTE(i1);
	REGISTER_ATTRIBUTE(i2);
	REGISTER_ATTRIBUTE(i3);
	REGISTER_ATTRIBUTE(i4);
}

