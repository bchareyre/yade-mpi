/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ColorizedVelocityFilter.hpp"

CREATE_LOGGER(ColorizedVelocityFilter);

ColorizedVelocityFilter::ColorizedVelocityFilter() : FilterEngine() 
{
	first=true;
	autoScale=true;
	minValue=0;
	maxValue=0;
	midValue=0;
	onlyDynamic=true;
	prevPositions.clear();
	prevIteration=0;
	dt=0;
	subscrBodies.clear();
}


ColorizedVelocityFilter::~ColorizedVelocityFilter()
{
}

bool ColorizedVelocityFilter::isActivated()
{
	return FilterEngine::isActivated();
}

void ColorizedVelocityFilter::registerAttributes()
{
	FilterEngine::registerAttributes();
	REGISTER_ATTRIBUTE(autoScale);
	REGISTER_ATTRIBUTE(onlyDynamic);
	REGISTER_ATTRIBUTE(minValue);
	REGISTER_ATTRIBUTE(maxValue);
}

void ColorizedVelocityFilter::applyCondition(MetaBody* ncb)
{

	if (first) { initialize(ncb); return; }
	
	long int currIteration = Omega::instance().getCurrentIteration();
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	for (int i=0,e=subscrBodies.size(); i<e; ++i)
	{
		shared_ptr<Body> b =(*bodies)[subscrBodies[i]]; 
		Vector3r currPos = b->physicalParameters->se3.position;
		values[i] = (currPos - prevPositions[i]).Length() / (currIteration - prevIteration)/dt;
		prevPositions[i] = currPos;
	}
	prevIteration = currIteration;
	if (autoScale) makeScale();
	for (int i=0,e=subscrBodies.size(); i<e; ++i)
	{
		shared_ptr<Body> b =(*bodies)[subscrBodies[i]]; 
		b->geometricalModel->diffuseColor = getColor4Value(values[i]);
	}
}

Vector3r ColorizedVelocityFilter::getColor4Value(Real v)
{
	Vector3r color;
	if (v<midValue) 
	{
		color[0] = 0;
		color[1] = 1/(midValue-minValue)*(v-minValue);
		color[2] = -1/(midValue-minValue)*(v-minValue)+1;
	}
	else 
	{
		color[0] = 1/(maxValue-midValue)*(v-midValue);
		color[1] = -1/(maxValue-midValue)*(v-midValue)+1;
		color[2] = 0;
	}
	return color;
}


void ColorizedVelocityFilter::makeScale()
{
	if (minValue==maxValue) minValue=maxValue=values[0];
	for(int i=0,e=values.size(); i<e; ++i)
	{
		if (values[i]<minValue) minValue = values[i];
		if (values[i]>maxValue) maxValue = values[i];
	}
	midValue = (minValue+maxValue)/2;
	
	LOG_INFO("minValue:" << minValue << '\t' << "maxValue:" << maxValue << '\t' << "midValue:" << midValue);
}

void ColorizedVelocityFilter::initialize(MetaBody* ncb)
{
	first=false;


	if (onlyDynamic)
	{
		FOREACH(shared_ptr<Body> b, *ncb->bodies) { if(b->isDynamic) subscrBodies.push_back(b->getId()); }
	}
	else
		subscrBodies.assign(subscribedBodies.begin(),subscribedBodies.end());

	prevPositions.resize(subscrBodies.size());
	values.resize(subscrBodies.size());
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	for(int i=0,e=subscrBodies.size(); i<e; ++i)
		prevPositions[i] = (*bodies)[subscrBodies[i]]->physicalParameters->se3.position;
	
	midValue = (minValue+maxValue)/2;
	
	dt = Omega::instance().getTimeStep();
	prevIteration = Omega::instance().getCurrentIteration();

	LOG_INFO("bodies:" << subscrBodies.size() << '\t' << "minValue:" << minValue << '\t' << "maxValue:" << maxValue << '\t' << "midValue:" << midValue);
}


YADE_PLUGIN();
