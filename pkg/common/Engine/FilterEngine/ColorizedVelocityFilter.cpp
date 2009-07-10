/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ColorizedVelocityFilter.hpp"
#include <yade/pkg-common/ColorScale.hpp>

CREATE_LOGGER(ColorizedVelocityFilter);

ColorizedVelocityFilter::ColorizedVelocityFilter() : FilterEngine() 
{
	first=true;
	autoScale=true;
	minValue=0;
	maxValue=0;
	onlyDynamic=true;
	prevPositions.clear();
	prevIteration=0;
	dt=0;
	subscrBodies.clear();

	posX=0;
	posY=0.2;
	width=0.05;
	height=0.5;
	title="Velocity, m/s";
}


ColorizedVelocityFilter::~ColorizedVelocityFilter()
{
}

bool ColorizedVelocityFilter::isActivated(MetaBody* rootBody)
{
	return FilterEngine::isActivated(rootBody);
}

void ColorizedVelocityFilter::registerAttributes()
{
	FilterEngine::registerAttributes();
	REGISTER_ATTRIBUTE(autoScale);
	REGISTER_ATTRIBUTE(onlyDynamic);
	REGISTER_ATTRIBUTE(minValue);
	REGISTER_ATTRIBUTE(maxValue);

	REGISTER_ATTRIBUTE(posX);
	REGISTER_ATTRIBUTE(posY);
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(title);
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
	makeScale();
	updateColorScale();
	for (int i=0,e=subscrBodies.size(); i<e; ++i)
	{
		shared_ptr<Body> b =(*bodies)[subscrBodies[i]]; 
		b->geometricalModel->diffuseColor = getColor4Value(values[i]);
	}
}

void ColorizedVelocityFilter::updateColorScale()
{
	legend->posX = posX; 
	legend->posY = posY; 
	legend->width = width; 
	legend->height = height; 

	if (cur_minValue==minValue && cur_maxValue==maxValue) return;

	cur_minValue=minValue;
	cur_maxValue=maxValue;
	legend->colors.resize(3);
	Real h = (maxValue-minValue)/(legend->colors.size()-1);
	for(unsigned int i=0; i<legend->colors.size(); ++i)
		legend->colors[i] = getColor4Value(minValue+h*i);

	legend->labels.resize(5);
	h = (maxValue-minValue)/(legend->labels.size()-1);
	for(unsigned int i=0; i<legend->labels.size(); ++i)
	{
		char tmp[64];
		sprintf(tmp, "%6.3f", minValue+h*i);
		legend->labels[i]=string(tmp);
	}
}

void ColorizedVelocityFilter::makeScale()
{
	if (autoScale)
	{
		minValue=maxValue=values[0];
		for(int i=0,e=values.size(); i<e; ++i)
		{
			if (values[i]<minValue) minValue = values[i];
			if (values[i]>maxValue) maxValue = values[i];
		}
		LOG_INFO("minValue:" << minValue << '\t' << "maxValue:" << maxValue);
	}
}

Vector3r ColorizedVelocityFilter::getColor4Value(Real v)
{
	Vector3r color;
	Real midValue = (minValue+maxValue)/2;
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

void ColorizedVelocityFilter::initialize(MetaBody* ncb)
{
	first=false;

	widget = shared_ptr<Body>(new Body);
	legend = shared_ptr<ColorScale>(new ColorScale);
	widget->geometricalModel = legend;
	legend->title = title;
	widget->physicalParameters = shared_ptr<PhysicalParameters>(new PhysicalParameters);
	widget->isDynamic=false;
	ncb->bodies->insert(widget);

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
	
	dt = Omega::instance().getTimeStep();
	prevIteration = Omega::instance().getCurrentIteration();

	LOG_INFO("bodies:" << subscrBodies.size() << '\t' << "minValue:" << minValue << '\t' << "maxValue:" << maxValue);
}


YADE_PLUGIN();
