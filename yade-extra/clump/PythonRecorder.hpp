// (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

#ifndef PYTHON_RECORDER_HPP
#define PYTHON_RECORDER_HPP

#ifndef EMBED_PYTHON
	#error EMBED_PYTHON must be defined for this module to work!
#endif

#include<string>
#include<yade/yade-core/DataRecorder.hpp>
#include<yade/yade-lib-base/Logging.hpp>

/*! Recorder that executes arbitrary python expression when called.
 *
 * The expressions to access yade's internals are described in documentation for pyade.py and pyade.cpp.
 *
 * */
class PythonRecorder : public DataRecorder
{
	private:
	public :
		//! Constructor that imports pyade module (warns if there is an error).
		PythonRecorder();
		virtual void action(Body* b);
		virtual bool isActivated(){return true;}
		virtual void registerAttributes(){DataRecorder::registerAttributes(); REGISTER_ATTRIBUTE(expression); REGISTER_ATTRIBUTE(initExpression); REGISTER_ATTRIBUTE(outputFile);}
		//! This expression will be interpreted when the engine is called.
		std::string expression;
		//! Piece of python code run on intialization
		std::string initExpression;
		//! If not empty, python sys.stdout will be redirected to this file (overwritten every time!)
		std::string outputFile;
		//std::string initExpression;
	protected :
		virtual void postProcessAttributes(bool deserializing);
	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(PythonRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);

};

REGISTER_SERIALIZABLE(PythonRecorder,false);

#endif // PYTHON_RECORDER_HPP

