#include "ForceRecorder.hpp"
#include "RigidBody.hpp"
#include "Omega.hpp"
#include "NonConnexBody.hpp"

#include <boost/lexical_cast.hpp>

ForceRecorder::ForceRecorder () : Actor()//, ofile("")
{
	outputFile = "";
	interval = 50;
}

ForceRecorder::~ForceRecorder ()
{

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
	RigidBody * rb = dynamic_cast<RigidBody*>(body);
	
	if( Omega::instance().getCurrentIteration() % interval == 0 /*&& ofile*/ )
	
		cout << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
			<< lexical_cast<string>(rb->acceleration[0]) << " " 
			<< lexical_cast<string>(rb->acceleration[1]) << " " 
			<< lexical_cast<string>(rb->acceleration[2]) << endl;

}

