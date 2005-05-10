#include "ForceRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include <yade/Omega.hpp>
#include <yade/MetaBody.hpp>
#include "Force.hpp"

#include <boost/lexical_cast.hpp>

ForceRecorder::ForceRecorder () : Engine(), actionForce(new Force)
{
	outputFile = "";
	interval = 50;
	startId = 0;
	endId = 1;
	changed = false;
}

void ForceRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}

void ForceRecorder::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(startId);
	REGISTER_ATTRIBUTE(endId);
	REGISTER_ATTRIBUTE(bigBallId);
	REGISTER_ATTRIBUTE(bigBallReleaseTime);
}

bool ForceRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}

void ForceRecorder::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	Real x=0, y=0, z=0;
	
	for( unsigned int i = startId ; i <= endId ; ++i )
	{
		Vector3r force = dynamic_cast<Force*>(ncb->actionParameters->find( i , actionForce->getClassIndex() ) . get() )->force;
		
		x+=force[0];
		y+=force[1];
		z+=force[2];
	}
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << endl;

		
	// FIXME all that lines do not belong to ForceRecorder
	if( bigBallReleaseTime < Omega::instance().getSimulationTime() && (!changed) )
	{
		changed = true;
		(*(ncb->bodies))[bigBallId]->isDynamic = true;
	}
}

