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



void InteractionContainer::requestErase(body_id_t id1, body_id_t id2){ find(id1,id2)->reset(); bodyIdPair v(0,2); v.push_back(id1); v.push_back(id2); pendingErase.push_back(v); }

void InteractionContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		interaction.clear();
	}
	else
	{
		interaction.clear(); interaction.reserve(this->size());
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



