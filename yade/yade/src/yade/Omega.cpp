#include "Omega.hpp"
#include "yadeExceptions.hpp"

#include "Vector3.hpp"

#include "IOManager.hpp"
#include "MetaBody.hpp"

#include "FileGenerator.hpp"
#include "InteractionSolver.hpp"
#include "DeusExMachina.hpp"
#include "Body.hpp"

#include "IOManager.hpp"
#include "BroadInteractor.hpp"

#include "GeometricalModel.hpp"
#include "InteractingGeometry.hpp"
#include "BoundingVolume.hpp"
#include "InteractionGeometry.hpp"
#include "InteractionPhysics.hpp"
#include "ThreadSynchronizer.hpp"
#include "SimulationLoop.hpp"
#include "FrontEnd.hpp"

#include "Preferences.hpp"
//#include "InteractionGeometryMetaEngine.hpp"
//#include "InteractionGeometryEngineUnit.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Omega::Omega()
{
	ThreadSafe::cerr("Constructing Omega  (if multiple times - check '-rdynamic' flag!)");

	simulationFileName="";
	currentIteration = 0;
	dt = 0.01;
	logFile = shared_ptr<ofstream>(new ofstream("../data/log.xml", ofstream::out | ofstream::app));
	// build simulation loop thread
	synchronizer = shared_ptr<ThreadSynchronizer>(new ThreadSynchronizer()); // FIXME - this should be optional

	preferences = shared_ptr<Preferences>(new Preferences);

	filesystem::path yadeConfigPath = filesystem::path(string(getenv("HOME")) + string("/.yade"), filesystem::native);

	if ( filesystem::exists( yadeConfigPath ) )
	{
		IOManager::loadFromFile("XMLManager",yadeConfigPath.string()+"/preferences.xml","preferences",preferences);
	}
	else
	{
		filesystem::create_directories(yadeConfigPath);
		char * buffer ;
		buffer = getenv ("YADEBINPATH"); // FIXME : to modify after splitting
		string yadeBinPath = buffer;
		preferences->dynlibsDirectories.push_back(yadeBinPath+"/dynlib/linux");
		IOManager::saveToFile("XMLManager",yadeConfigPath.string()+"/preferences.xml","preferences",preferences);
	}

	vector<string>::iterator dldi    = preferences->dynlibsDirectories.begin();
	vector<string>::iterator dldiEnd = preferences->dynlibsDirectories.end();
	for( ; dldi != dldiEnd ; ++dldi)
		ClassFactory::instance().addBaseDirectory((*dldi));
			
	// build dynlib information list
	buildDynlibList();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Omega::~Omega()
{
	//LOCK(omegaMutex);
	
	*logFile << "\t" << "<Summary Duration=\"" << sStartingSimulationTime-second_clock::local_time() << "\">" <<endl;
	*logFile << "</Simulation>" << endl << endl;
	logFile->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::logError(const string& str)
{
	//LOCK(omegaMutex);
	
	*logFile << "\t" << "<Error Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::logMessage(const string& str)
{
	//LOCK(omegaMutex);
		
	*logFile << "\t" << "<Message Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::createSimulationLoop()
{
	//LOCK(omegaMutex);
	simulationLoop   = shared_ptr<SimulationLoop>(new SimulationLoop());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::finishSimulationLoop()
{
	//LOCK(omegaMutex);
	
	if (simulationLoop)
		simulationLoop->finish();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::joinSimulationLoop()
{
	//LOCK(omegaMutex);
	
	if (simulationLoop)
	{
		simulationLoop->join();
		simulationLoop   = shared_ptr<SimulationLoop>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::doOneSimulationLoop()
{
	if (simulationLoop)
	{
		//if ((simulationLoop->isStopped()))
		//	simulationPauseDuration += microsec_clock::local_time()-msStartingPauseTime;
		//else
		msStartingPauseTime = microsec_clock::local_time();	
		simulationLoop->doOneLoop();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::startSimulationLoop()
{
//	LOCK(omegaMutex);

	if (simulationLoop && simulationLoop->isStopped())
	{
		simulationPauseDuration += microsec_clock::local_time()-msStartingPauseTime;
		simulationLoop->start();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::stopSimulationLoop()
{
//	LOCK(omegaMutex);
	if (simulationLoop && !(simulationLoop->isStopped()))
	{
		msStartingPauseTime = microsec_clock::local_time();
		simulationLoop->stop();	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::registerDynlibType(const string& name)
{	
	// called by BuildDynLibList
	shared_ptr<Factorable> f;
	try
	{
		f = ClassFactory::instance().createShared(name);
	}
	catch (FactoryError&)
	{
		return;
	}

	if (dynamic_pointer_cast<FileGenerator>(f))
		dynlibsType[name]="FileGenerator";
	else if (dynamic_pointer_cast<InteractionSolver>(f))
		dynlibsType[name]="InteractionSolver";
	else if (dynamic_pointer_cast<DeusExMachina>(f))
		dynlibsType[name]="DeusExMachina";
	else if (dynamic_pointer_cast<Body>(f))
		dynlibsType[name]="Body";
//	else if (dynamic_pointer_cast<InteractionGeometryEngineUnit>(f))
//		dynlibsType[name]="InteractionGeometryEngineUnit";
	else if (dynamic_pointer_cast<IOManager>(f))
		dynlibsType[name]="IOManager";
	else if (dynamic_pointer_cast<BroadInteractor>(f))
		dynlibsType[name]="BroadInteractor";
	else if (dynamic_pointer_cast<GeometricalModel>(f))
		dynlibsType[name]="GeometricalModel";
	else if (dynamic_pointer_cast<InteractingGeometry>(f))
		dynlibsType[name]="InteractingGeometry";
	else if (dynamic_pointer_cast<BoundingVolume>(f))
		dynlibsType[name]="BoundingVolume";
	else if (dynamic_pointer_cast<InteractionGeometry>(f))
		dynlibsType[name]="InteractionGeometry";
	else if (dynamic_pointer_cast<InteractionPhysics>(f))
		dynlibsType[name]="InteractionPhysics";

	else
		dynlibsType[name]="Unknown";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::buildDynlibList()
{
	//LOCK(omegaMutex);
	vector<string>::iterator si = preferences->dynlibsDirectories.begin();
	vector<string>::iterator siEnd = preferences->dynlibsDirectories.end();
	for( ; si != siEnd ; ++si)
	{
//		char * buffer ;
//		buffer = getenv ("YADEBINPATH"); // FIXME - yade should use config file, to check /usr/lib/yade and /home/joe/yade/lib, etc..
//		string yadeBinPath = buffer;
//		filesystem::path directory(yadeBinPath+"/dynlib/linux");

		filesystem::path directory((*si));
		
		if ( filesystem::exists( directory ) )
		{
			filesystem::directory_iterator di( directory );
			filesystem::directory_iterator diEnd;
			for ( ; di != diEnd; ++di )
			{
				if (!filesystem::is_directory(*di) && !filesystem::symbolic_link_exists(*di) && filesystem::extension(*di)!=".a")
				{
					filesystem::path name(filesystem::basename((*di)));
					int prevLength = (*di).leaf().size();
					int length = name.leaf().size();
					while (length!=prevLength)
					{
						prevLength=length;
						name = filesystem::path(filesystem::basename(name));
						length = name.leaf().size();
					}
					registerDynlibType(name.leaf().substr(3,name.leaf().size()-3));
					//cout << name.leaf() << endl;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Omega::getDynlibType(const string& libName, string& type)
{
	//LOCK(omegaMutex);
		
	map<string,string>::iterator it = dynlibsType.find(libName);
	if (it!=dynlibsType.end())
	{
		type = (*it).second;
		return true;
	}
	else
		return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const map<string,string>& Omega::getDynlibsType()
{
	//LOCK(omegaMutex);
		
	return dynlibsType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::incrementCurrentIteration()
{
	//LOCK(omegaMutex);
	
	++currentIteration;
}

long int Omega::getCurrentIteration()
{
	//LOCK(omegaMutex);
	
	return currentIteration;
}

void Omega::setSimulationFileName(const string f)
{
	//LOCK(omegaMutex);
	
	simulationFileName = f;
};

string Omega::getSimulationFileName()
{
	//LOCK(omegaMutex);
	
	return simulationFileName;
}

void Omega::loadSimulation()
{
	LOCK(Omega::instance().getRootBodyMutex());
	
	if( Omega::instance().getSimulationFileName().size() != 0  &&  filesystem::exists(simulationFileName) && filesystem::extension(simulationFileName)==".xml" )
	{
		freeRootBody();
		logMessage("Loading file " + simulationFileName);
		
		//{
		//	LOCK(rootBodyMutex);
		//	IOManager::loadFromFile("yade-lib-serialization-xml",simulationFileName,"rootBody",rootBody);
			IOManager::loadFromFile("XMLManager",simulationFileName,"rootBody",rootBody);
			sStartingSimulationTime = second_clock::local_time();
			msStartingSimulationTime = microsec_clock::local_time();
			simulationPauseDuration = msStartingSimulationTime-msStartingSimulationTime;
			msStartingPauseTime = msStartingSimulationTime;
			*logFile << "<Simulation" << " Date =\"" << sStartingSimulationTime << "\">" << endl;
			currentIteration = 0;
			simulationTime = 0;	
		//}
	}
	else
	{
		std::string error = yadeExceptions::BadFile + simulationFileName;
		cerr << error.c_str() << endl;
		throw yadeBadFile(error.c_str());
	}
}

void Omega::freeRootBody()
{
//	LOCK(rootBodyMutex);
	rootBody = shared_ptr<MetaBody>();
}

// FIXME - remove that
void Omega::setTimeStep(const Real t)
{
//	LOCK(omegaMutex);
	dt = t;
}

Real Omega::getTimeStep()
{
//	LOCK(omegaMutex);
	return dt;
}

shared_ptr<ThreadSynchronizer> Omega::getSynchronizer()
{
//	LOCK(omegaMutex); // we can safatly return it because it is constructed in Omega constructor
	return synchronizer;
}

const shared_ptr<MetaBody>& Omega::getRootBody()
{
//	LOCK(omegaMutex);
	return rootBody;
}

ptime Omega::getMsStartingSimulationTime()
{
//	LOCK(omegaMutex);
	return msStartingSimulationTime;
}

time_duration Omega::getSimulationPauseDuration()
{
	return simulationPauseDuration;
}


boost::mutex& Omega::getRootBodyMutex()
{
	return rootBodyMutex;
}

