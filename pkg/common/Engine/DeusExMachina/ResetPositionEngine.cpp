/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ResetPositionEngine.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/ParticleParameters.hpp>
#include<boost/foreach.hpp>

ResetPositionEngine::ResetPositionEngine(){
		Y_min=-1;
		interval=0;
		initial_positions.clear();
}

void ResetPositionEngine::postProcessAttributes(bool deserializing){}

void ResetPositionEngine::registerAttributes(){
	DeusExMachina::registerAttributes(); // for subscribedBodies
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(Y_min);
	REGISTER_ATTRIBUTE(initial_positions);
}


void ResetPositionEngine::applyCondition(MetaBody * ncb){
		if (initial_positions.size()==0) 
		{
				initialize(ncb);
				return;
		}

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	for(int i=0,e=subscribedBodies.size(); i<e; ++i)
	{
			ParticleParameters* pp = YADE_CAST<ParticleParameters*>((*bodies)[subscribedBodies[i]]->physicalParameters.get());
			if (pp->se3.position[1]<Y_min)
			{
					pp->se3.position = initial_positions[i];
					pp->velocity = Vector3r(0,0,0);
			}
	}
}

void ResetPositionEngine::initialize(MetaBody * ncb){
	initial_positions.resize(subscribedBodies.size());
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	for(int i=0,e=subscribedBodies.size(); i<e; ++i)
		initial_positions[i]=(*bodies)[subscribedBodies[i]]->physicalParameters->se3.position;
}
YADE_PLUGIN();
