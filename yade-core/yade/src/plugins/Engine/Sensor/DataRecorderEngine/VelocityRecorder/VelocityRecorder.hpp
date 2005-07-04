#ifndef VELOCITY_RECORDER_HPP
#define VELOCITY_RECORDER_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/Engine.hpp>


#include <string>
#include <fstream>

class VelocityRecorder : public Engine
{
	public : std::string outputFile;
	public : unsigned int interval;
	public : unsigned int bigBallId;
	
	private : std::ofstream ofile;

	// construction
	public : VelocityRecorder ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : virtual void registerAttributes();
	

	public : virtual void action(Body* b);
	public : virtual bool isActivated();
	REGISTER_CLASS_NAME(VelocityRecorder);
};

REGISTER_SERIALIZABLE(VelocityRecorder,false);

#endif // VELOCITY_RECORDER_HPP 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

