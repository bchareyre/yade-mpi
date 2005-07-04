#ifndef FORCE_RECORDER_HPP_
#define FORCE_RECORDER_HPP_

#include <yade/Engine.hpp>

#include <string>
#include <fstream>

class PhysicalAction;

class ForceRecorder : public Engine
{
	public : std::string outputFile;
	public : unsigned int interval;
	public : unsigned int startId,endId;
	public : int bigBallId; // FIXME !!!!!!!!!!
	public : Real bigBallReleaseTime; // FIXME !!!!!!!!!!
	private : shared_ptr<PhysicalAction> actionForce;
	
	private : std::ofstream ofile; 

	private : bool changed;
	// construction
	public : ForceRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : virtual void registerAttributes();

	public : virtual void action(Body* b);
	public : virtual bool isActivated();
	REGISTER_CLASS_NAME(ForceRecorder);
};

REGISTER_SERIALIZABLE(ForceRecorder,false);

#endif // __BALLISTICDYNAMICENGINE_H__
