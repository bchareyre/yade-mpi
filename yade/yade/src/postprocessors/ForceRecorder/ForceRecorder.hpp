#ifndef FORCE_RECORDER_HPP_
#define FORCE_RECORDER_HPP_

#include "Actor.hpp"

#include <string>
#include <fstream>

class ForceRecorder : public Actor
{
	public : std::string outputFile;
	public : unsigned int interval;
	
	//private : std::ofstream ofile; // FIXME - why this is crashing ?!?

	// construction
	public : ForceRecorder ();
	public : ~ForceRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : virtual void action(Body* b);
	REGISTER_CLASS_NAME(ForceRecorder);
};

REGISTER_SERIALIZABLE(ForceRecorder,false);

#endif // __BALLISTICDYNAMICENGINE_H__
