#include "PositionOrientationRecorder.hpp"
#include "RigidBodyParameters.hpp"
#include "Omega.hpp"
#include "MetaBody.hpp"

#include <boost/lexical_cast.hpp>

PositionOrientationRecorder::PositionOrientationRecorder () : Engine()//, ofile("")
{
	outputFile = "";
	interval = 50;
}

PositionOrientationRecorder::~PositionOrientationRecorder ()
{

}

void PositionOrientationRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	//	ofile.open(outputFile.c_str());
	}
}

void PositionOrientationRecorder::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
}


void PositionOrientationRecorder::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	
	if( Omega::instance().getCurrentIteration() % interval == 0 /*&& ofile*/ )
	{
		ofile.open( string(outputFile+"_"+lexical_cast<string>( Omega::instance().getCurrentIteration() )).c_str() );
	
		Real tx=0, ty=0, tz=0, rw=0, rx=0, ry=0, rz=0;
		for( ncb->bodies->gotoFirst() ; ncb->bodies->notAtEnd() ; ncb->bodies->gotoNext() )
		{
			tx = ncb->bodies->getCurrent()->physicalParameters->se3.position[0];
			ty = ncb->bodies->getCurrent()->physicalParameters->se3.position[1];
			tz = ncb->bodies->getCurrent()->physicalParameters->se3.position[2];
		
			rw = ncb->bodies->getCurrent()->physicalParameters->se3.orientation[0];
			rx = ncb->bodies->getCurrent()->physicalParameters->se3.orientation[1];
			ry = ncb->bodies->getCurrent()->physicalParameters->se3.orientation[2];
			rz = ncb->bodies->getCurrent()->physicalParameters->se3.orientation[2];
			
			ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " "
				<< lexical_cast<string>(tx) << " " 
				<< lexical_cast<string>(ty) << " " 
				<< lexical_cast<string>(tz) << " "
				<< lexical_cast<string>(rw) << " "
				<< lexical_cast<string>(rx) << " "
				<< lexical_cast<string>(ry) << " "
				<< lexical_cast<string>(rz) << endl;
		}
		ofile.close();
	}
}

