/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "NodeRecorder.hpp"
#include<yade/core/MetaBody.hpp>
#include "LatticeBeamParameters.hpp"
#include "LatticeNodeParameters.hpp"
// to calculate strain of whole speciemen - first two bodies in subscribedBodies are Nodes. FIXME - make it clean!
#include <boost/lexical_cast.hpp>


NodeRecorder::NodeRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	subscribedBodies.clear();
	first = true;
	// FIXME ...
        nodeGroupMask           = 1;
        beamGroupMask           = 2;
}


void NodeRecorder::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	}
	else
	{
		regions_min.clear();
		regions_max.clear();
		std::list<std::pair<Vector3r,Vector3r> >::iterator i=regions.begin();
		std::list<std::pair<Vector3r,Vector3r> >::iterator e=regions.end();
		for( ; i!=e ; ++i)
		{
			regions_min.push_back(i->first);
			regions_max.push_back(i->second);
		}
	}
}

void NodeRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		regions.clear();
		std::vector<Vector3r>::iterator ii=regions_min.begin();
		std::vector<Vector3r>::iterator ei=regions_min.end();
		std::vector<Vector3r>::iterator ia=regions_max.begin();
		//std::vector<Vector3r>::iterator ea region_max.end();
		for( ; ii!=ei ; ++ii , ++ia )
		{
			regions.push_back(std::make_pair(*ii,*ia));
		}

	//	outputFile += "_"+boost::lexical_cast<std::string>(Omega::instance().getTimeStep());
	//	std::cerr << "using dt for NodeRecorder output file: " << outputFile << "\n";

		ofile.open(outputFile.c_str());

		first = true;
		subscribedBodies.clear();
	}
}


void NodeRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	//REGISTER_ATTRIBUTE(regions);
	REGISTER_ATTRIBUTE(regions_min);
	REGISTER_ATTRIBUTE(regions_max);

//	REGISTER_ATTRIBUTE(first);
//	REGISTER_ATTRIBUTE(subscribedBodies);
}


bool NodeRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void NodeRecorder::action(Body * body)
{
//	std::cerr << "NodeRecorder quits\n";
//	return;
	
	MetaBody * ncb = static_cast<MetaBody*>(body);

	if(first)
	{
	// scan the regions and subscribe bodies for each of them
	// FIXME - this is still problematic, because this should be done only
	// on the first run, regardless of saving/loading this file...
		first = false;
		subscribedBodies.resize( regions.size() );

		std::list<std::pair<Vector3r,Vector3r> >::iterator i  =regions.begin();
		std::list<std::pair<Vector3r,Vector3r> >::iterator end=regions.end();

		for( int region=0 ; i != end ; ++i, ++region )
		{
			Vector3r min = i->first;
			Vector3r max = i->second;

			if(!subscribedBodies[region].empty())
				std::cerr << "First, but not empty\n";

			BodyContainer::iterator bi    = ncb->bodies->begin();
			BodyContainer::iterator biEnd = ncb->bodies->end();
			for(  ; bi!=biEnd ; ++bi )
			{
				if( (*bi)->getGroupMask() & nodeGroupMask )
				{
					Vector3r pos = (*bi)->physicalParameters->se3.position;
					if(        pos[0] > min[0] 
						&& pos[1] > min[1] 
						&& pos[2] > min[2] 
						&& pos[0] < max[0] 
						&& pos[1] < max[1] 
						&& pos[2] < max[2] 
						&& ((*bi)->getGroupMask() & nodeGroupMask)
					  )
					{
						subscribedBodies[region].push_back((*bi)->getId());
					}
				}
			}
		}
	}

	for(size_t region=0 ; region<regions.size() ; ++region )
	{
		std::list<unsigned int>::iterator i   = subscribedBodies[region].begin();
		std::list<unsigned int>::iterator end = subscribedBodies[region].end();
		
		Real count=0;
		Vector3r sum=Vector3r(0,0,0);

		for( ; i != end ; ++i )
		{
			if( (*(ncb->bodies)).exists(*i) )
			{
				sum+=(*(ncb->bodies))[*i]->physicalParameters->se3.position;
				++count;
				
				(*(ncb->bodies))[*i]->geometricalModel->diffuseColor = Vector3r(0.0,1.0,((float)region)/1.5); // FIXME [1]
			}
		}

		sum/=count;

		ofile	<< lexical_cast<string>(sum[0]) << " " 
			<< lexical_cast<string>(sum[1]) << " " 
			<< lexical_cast<string>(sum[2]) << " \t "; 
	}
	ofile << endl; 
		
	// [1]
	// mark colors what is monitored - FIXME - provide a way to do that for ALL DataRecorders in that parent class.
	// GLDrawSomething can just put a getClassName()
}

YADE_PLUGIN();
