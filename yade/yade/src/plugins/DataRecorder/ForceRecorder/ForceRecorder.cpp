#include "ForceRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"

#include <boost/lexical_cast.hpp>

ForceRecorder::ForceRecorder () : Actor()//, ofile("")
{
	outputFile = "";
	interval = 50;
}

void ForceRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	//	ofile.open(outputFile.c_str());
	}
}

void ForceRecorder::registerAttributes()
{
	Actor::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
}


void ForceRecorder::action(Body * body)
{
	RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>(body);
	
	if( Omega::instance().getCurrentIteration() % interval == 0 /*&& ofile*/ )
	
		cout << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
			<< lexical_cast<string>(rb->acceleration[0]) << " " 
			<< lexical_cast<string>(rb->acceleration[1]) << " " 
			<< lexical_cast<string>(rb->acceleration[2]) << endl;

}

