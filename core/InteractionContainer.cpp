/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionContainer.hpp"
#include "Interaction.hpp"



void InteractionContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		interaction.clear();
	}
	else
	{
		interaction.clear();
		InteractionContainer::iterator i    = this->begin();
		InteractionContainer::iterator iEnd = this->end();
		for( ; i!=iEnd ; ++i )
			interaction.push_back(*i);
	}
}


void InteractionContainer::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		this->clear();
		vector<shared_ptr<Interaction> >::iterator it    = interaction.begin();
		vector<shared_ptr<Interaction> >::iterator itEnd = interaction.end();
		for( ; it != itEnd ; ++it)
			this->insert(*it);
		interaction.clear();
	}
	else
	{
		interaction.clear();
	}
}



