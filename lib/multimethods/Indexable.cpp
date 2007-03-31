/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Indexable.hpp"


Indexable::Indexable () 
{
}


Indexable::~Indexable () 
{

}


void Indexable::createIndex () 
{
	int& index = getClassIndex();
	if(index == -1)				// assign new index
	{
		index = getMaxCurrentlyUsedClassIndex()+1;
		// so that other dispatchers will not fall in conflict with this index
		incrementMaxCurrentlyUsedClassIndex();
	}

}

