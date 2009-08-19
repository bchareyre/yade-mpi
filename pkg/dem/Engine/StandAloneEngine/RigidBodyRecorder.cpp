/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "RigidBodyRecorder.hpp"
#include <yade/core/Omega.hpp>
#include <yade/core/MetaBody.hpp>
#include <boost/lexical_cast.hpp>

RigidBodyRecorder::RigidBodyRecorder(): DataRecorder(), rigidBodyParameters(new RigidBodyParameters)
{
	outputBase = "rigidbody";
	interval=50;
}


RigidBodyRecorder::~RigidBodyRecorder ()
{

}


void RigidBodyRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
        //
	}
}



bool RigidBodyRecorder::isActivated(MetaBody*)
{
   return ((Omega::instance().getCurrentIteration() % interval == 0));
}

void RigidBodyRecorder::action(MetaBody * ncb)
{
    shared_ptr<BodyContainer>& bodies = ncb->bodies;

    BodyContainer::iterator bi	    = bodies->begin();
    BodyContainer::iterator biEnd   = bodies->end();
    for(;bi!=biEnd;++bi)
    {
	const shared_ptr<Body>& b = *bi;
	int bId = b->getId();

	const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>(b->physicalParameters.get());
	std::string outputFile = outputBase+lexical_cast<string>(bId) + ".dat";

	std::ofstream ofs;
	
	if (body_ids.insert(bId).second)
	{
	    ofs.open(outputFile.c_str());
	    if (!ofs)
	    {
		cerr << "ERROR: Can't open file: " << outputFile << std::endl;
		return;
	    }
	    ofs << "#(t - time; c - position; v - velocity; a - acceleration; w - angular velocity; b - angular acceleration)\n" 
		<< "#t(1)\tcx(2)\tcy(3)\tcz(4)\tvx(5)\tvy(6)\tvz(7)\tax(8)\tay(9)\taz(10)\twx(11)\twy(12)\twz(13)\tbx(14)\tby(15)\tbz(16)" 
		<< std::endl;
	}
	else
	{
	    ofs.open(outputFile.c_str(),ios::app);
	    if (!ofs)
	    {
		cerr << "ERROR: Can't open file: " << outputFile << std::endl;
		return;
	    }
	}
	
	ofs << lexical_cast<string>( Omega::instance().getCurrentIteration() ) << '\t'
	    << p->se3.position[0] << '\t'
	    << p->se3.position[1] << '\t'
	    << p->se3.position[2] << '\t'
	    << p->velocity[0] << '\t'
	    << p->velocity[1] << '\t'
	    << p->velocity[2] << '\t'
	    << p->acceleration[0] << '\t'
	    << p->acceleration[1] << '\t'
	    << p->acceleration[2] << '\t'
	    << p->angularVelocity[0] << '\t'
	    << p->angularVelocity[1] << '\t'
	    << p->angularVelocity[2] << '\t'
	    << p->angularAcceleration[0] << '\t'
	    << p->angularAcceleration[1] << '\t'
	    << p->angularAcceleration[2] << '\t'
	    << endl;
	ofs.close();
    }
}

YADE_PLUGIN((RigidBodyRecorder));
