/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"FixedPositionEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<boost/foreach.hpp>

FixedPositionEngine::FixedPositionEngine(){
	fixedPosition=Vector3r(0,0,0);
	mask=Vector3r(0,0,1);
}

void FixedPositionEngine::postProcessAttributes(bool deserializing){}


void FixedPositionEngine::applyCondition(MetaBody * ncb){
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	FOREACH(int id, subscribedBodies){
		if(!bodies->exists(id)) continue;
		if(mask[0]!=0) (*bodies)[id]->physicalParameters->se3.position[0]=fixedPosition[0];
		if(mask[1]!=0) (*bodies)[id]->physicalParameters->se3.position[1]=fixedPosition[1];
		if(mask[2]!=0) (*bodies)[id]->physicalParameters->se3.position[2]=fixedPosition[2];
	}
}

YADE_PLUGIN();
