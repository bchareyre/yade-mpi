/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ElasticBodySimpleRelationship.hpp"
#include<yade/pkg-common/SimpleElasticInteraction.hpp>
#include<yade/pkg-common/ElasticBodyParameters.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


void ElasticBodySimpleRelationship::go(	  const shared_ptr<PhysicalParameters>& b1
					, const shared_ptr<PhysicalParameters>& b2
					, const shared_ptr<Interaction>& interaction)
{
	ElasticBodyParameters* s1 = static_cast<ElasticBodyParameters*>(b1.get());
	ElasticBodyParameters* s2 = static_cast<ElasticBodyParameters*>(b2.get());
	
	// the need to calculate this is only when the interaction is new
	if( interaction->isNew)
	{
		boost::shared_ptr<SimpleElasticInteraction> sei(new SimpleElasticInteraction);
		sei->kn = (s1->young + s2->young)*0.5;
		interaction->interactionPhysics=sei;
	}	
};

void ElasticBodySimpleRelationship::registerAttributes()
{
};

YADE_PLUGIN();
