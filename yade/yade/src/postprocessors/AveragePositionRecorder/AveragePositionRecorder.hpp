#ifndef AVERAGE_POSISTION_RECORDER_HPP_
#define AVERAGE_POSISTION_RECORDER_HPP_

#include "Actor.hpp"

#include <string>
#include <fstream>

class AveragePositionRecorder : public Actor
{
	public : std::string outputFile;
	public : unsigned int interval;
	
	//private : std::ofstream ofile; // FIXME - why this is crashing ?!?

	// construction
	public : AveragePositionRecorder ();
	public : ~AveragePositionRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : virtual void action(Body* b);
	REGISTER_CLASS_NAME(AveragePositionRecorder);
};

REGISTER_SERIALIZABLE(AveragePositionRecorder,false);

#endif // AVG_POS_RECORDER_HPP_
