#include "ForceRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"
#include "ActionForce.hpp"

#include <boost/lexical_cast.hpp>

ForceRecorder::ForceRecorder () : Actor(), actionForce(new ActionForce)
{
	outputFile = "";
	interval = 50;
	id = 0;
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
	Actor::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(id);
	REGISTER_ATTRIBUTE(bigBallId);
	REGISTER_ATTRIBUTE(bigBallReleaseTime);
}

bool ForceRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}

void ForceRecorder::action(Body * body)
{
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	Vector3r f = dynamic_cast<ActionForce*>(ncb->actions->find(id,actionForce->getClassIndex()).get())->force;

	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(f[0]) << " " 
		<< lexical_cast<string>(f[1]) << " " 
		<< lexical_cast<string>(f[2]) << endl;
		
	if( bigBallReleaseTime < Omega::instance().getSimulationTime() )
		(*(ncb->bodies))[bigBallId]->isDynamic = true;
}

