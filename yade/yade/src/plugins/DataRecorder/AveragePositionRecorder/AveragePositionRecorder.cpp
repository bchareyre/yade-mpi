#include "AveragePositionRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include "Omega.hpp"
#include "ComplexBody.hpp"

#include <boost/lexical_cast.hpp>

AveragePositionRecorder::AveragePositionRecorder () : Actor()
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
	Actor::registerAttributes();
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
	ComplexBody * ncb = dynamic_cast<ComplexBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	Real x=0, y=0, z=0, size=0;
	
	for( bodies->gotoFirst() ; bodies->notAtEnd() ; bodies->gotoNext() )
	{
		shared_ptr<Body>& body = bodies->getCurrent();
		if( body->isDynamic && body->getId() != bigBallId )
		{ 
			size+=1.0;
			x+=ncb->bodies->getCurrent()->physicalParameters->se3.translation[0];
			y+=ncb->bodies->getCurrent()->physicalParameters->se3.translation[1];
			z+=ncb->bodies->getCurrent()->physicalParameters->se3.translation[2];
		}
	}

	x /= size;
	y /= size;
	z /= size;
	
	ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
		<< lexical_cast<string>(x) << " " 
		<< lexical_cast<string>(y) << " " 
		<< lexical_cast<string>(z) << " "

		<< lexical_cast<string>((*bodies)[bigBallId]->translation[0]) << " " // big ball
		<< lexical_cast<string>((*bodies)[bigBallId]->translation[1]) << " " 
		<< lexical_cast<string>((*bodies)[bigBallId]->translation[2]) << endl;
}

