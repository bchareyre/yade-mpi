#include "AveragePositionRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include <yade/Omega.hpp>
#include <yade/MetaBody.hpp>

#include <boost/lexical_cast.hpp>

AveragePositionRecorder::AveragePositionRecorder () : Engine()
{
	outputFile = "";
	interval = 50;
	bigBallId = 0;
}

void AveragePositionRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		ofile.open(outputFile.c_str());
	}
}

void AveragePositionRecorder::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(bigBallId);
}

bool AveragePositionRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void AveragePositionRecorder::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		shared_ptr<Body>& body = bodies->getCurrent();
		if( body->isDynamic && body->getId() != bigBallId )
		{ 
			size+=1.0;
			x+=body->physicalParameters->se3.position[0];
			y+=body->physicalParameters->se3.position[1];
			z+=body->physicalParameters->se3.position[2];
		}
	}

	x /= size;
	y /= size;
	z /= size;
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << " "

		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[0]) << " " // big ball
		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[1]) << " " 
		<< lexical_cast<string>((*bodies)[bigBallId]->physicalParameters->se3.position[2]) << endl;
}

