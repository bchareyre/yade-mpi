// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#ifndef PYTHON_RECORDER_HPP
#define PYTHON_RECORDER_HPP

#ifndef EMBED_PYTHON
	#error EMBED_PYTHON must be defined for this module to work!
#endif

#include<yade/yade-core/DataRecorder.hpp>
#include<yade/yade-lib-base/Logging.hpp>

/*! Recorder that executes arbitrary python expression when called.
 *
 * The expressions to access yade's internals are described in documentation for pyade.py and pyade.cpp.
 * */
class PythonRecorder : public DataRecorder
{
	public :
		//! Constructor that imports pyade module (warns if there is an error).
		PythonRecorder();
		virtual void action(Body* b);
		virtual bool isActivated(){return true;}
		virtual void registerAttributes(){DataRecorder::registerAttributes(); REGISTER_ATTRIBUTE(expression);}
		//! This expression will be interpreted when the engine is called.
		std::string expression;
	protected :
		virtual void postProcessAttributes(bool deserializing){}
	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(PythonRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);

};

REGISTER_SERIALIZABLE(PythonRecorder,false);

#endif // PYTHON_RECORDER_HPP

