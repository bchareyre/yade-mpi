#ifndef AVERAGE_POSISTION_RECORDER_HPP_
#define AVERAGE_POSISTION_RECORDER_HPP_

#include "Engine.hpp"

#include <string>
#include <fstream>

class AveragePositionRecorder : public Engine
{
	public : std::string outputFile;
	public : unsigned int interval;
	public : unsigned int bigBallId;
	
	private : std::ofstream ofile;
	

	// construction
	public : AveragePositionRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : virtual void registerAttributes();
	

	public : virtual void action(Body* b);
	public : virtual bool isActivated();
	REGISTER_CLASS_NAME(AveragePositionRecorder);
};

REGISTER_SERIALIZABLE(AveragePositionRecorder,false);

#endif // AVG_POS_RECORDER_HPP_
