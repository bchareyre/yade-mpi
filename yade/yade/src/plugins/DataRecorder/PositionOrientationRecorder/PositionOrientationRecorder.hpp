#ifndef FORCE_RECORDER_HPP_
#define FORCE_RECORDER_HPP_

#include "Actor.hpp"

#include <string>
#include <fstream>

class PositionOrientationRecorder : public Actor
{
	public : std::string outputFile;
	public : unsigned int interval;
	
	private : std::ofstream ofile; // FIXME - why this is crashing ?!?

	// construction
	public : PositionOrientationRecorder ();
	public : ~PositionOrientationRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : virtual void action(Body* b);
	REGISTER_CLASS_NAME(PositionOrientationRecorder);
};

REGISTER_SERIALIZABLE(PositionOrientationRecorder,false);

#endif // __BALLISTICDYNAMICENGINE_H__
