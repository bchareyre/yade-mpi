///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Omega.hpp"

#include "Vector3.hpp"

#include "IOManager.hpp" // is this allowed? perhaps loadTheFile should be in pimpl of Omega ? (pointer to implementation)
#include "NonConnexBody.hpp"

#include "FileGenerator.hpp"
#include "DynamicEngine.hpp"
#include "KinematicEngine.hpp"
#include "Body.hpp"
#include "CollisionFunctor.hpp"
#include "IOManager.hpp"
#include "BroadCollider.hpp"
#include "NarrowCollider.hpp"
#include "GeometricalModel.hpp"
#include "CollisionGeometry.hpp"
#include "BoundingVolume.hpp"
#include "InteractionGeometry.hpp"

#include "ThreadSynchronizer.hpp"
#include "SimulationLoop.hpp"
#include "FrontEnd.hpp"

#include "MultiMethodFunctor2D.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Omega::Omega ()
{
	cerr << "Constructing Omega  (if multiple times - check '-rdynamic' flag!)" << endl;
	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Omega::~Omega ()
{
	*logFile << "\t" << "<Summary Duration=\"" << sStartingSimulationTime-second_clock::local_time() << "\">" <<endl;
	*logFile << "</Simulation>" << endl << endl;
	logFile->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::logError(const string& str)
{
	*logFile << "\t" << "<Error Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::logMessage(const string& str)
{
	*logFile << "\t" << "<Message Date=\"" << sStartingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::init()
{
	simulationFileName="";
	currentIteration = 0;

	gravity = Vector3r(0,-9.81,0); // FIXME
	//dt = 0.04;
	dt = 0.01; // FIXME
	//dt = 0.015; // max for cloth, rotating box is little slower, but both work.

	logFile = shared_ptr<ofstream>(new ofstream("../data/log.xml", ofstream::out | ofstream::app));

	// build dynlib information list
	buildDynlibList(); // FIXME - this shouldn't be called in constructor, because frontend launches too long, when just asking for help -H

	// build simulation loop thread
	synchronizer     = shared_ptr<ThreadSynchronizer>(new ThreadSynchronizer()); // FIXME - this should be optional

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::createSimulationLoop()
{
	finishSimulationLoop();
	joinSimulationLoop();
	simulationLoop   = shared_ptr<SimulationLoop>(new SimulationLoop());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::finishSimulationLoop()
{
	if (simulationLoop)
		simulationLoop->finish();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::joinSimulationLoop()
{
	if (simulationLoop)
	{
		simulationLoop->join();
		freeSimulation();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::registerDynlibType(const string& name)
{
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
	else if (dynamic_pointer_cast<DynamicEngine>(f))
		dynlibsType[name]="DynamicEngine";
	else if (dynamic_pointer_cast<KinematicEngine>(f))
		dynlibsType[name]="KinematicEngine";
	else if (dynamic_pointer_cast<Body>(f))
		dynlibsType[name]="Body";
	else if (dynamic_pointer_cast<CollisionFunctor>(f))
		dynlibsType[name]="CollisionFunctor";
	else if (dynamic_pointer_cast<IOManager>(f))
		dynlibsType[name]="IOManager";
	else if (dynamic_pointer_cast<BroadCollider>(f))
		dynlibsType[name]="BroadCollider";
	else if (dynamic_pointer_cast<NarrowCollider>(f))
		dynlibsType[name]="NarrowCollider";
	else if (dynamic_pointer_cast<GeometricalModel>(f))
		dynlibsType[name]="GeometricalModel";
	else if (dynamic_pointer_cast<CollisionGeometry>(f))
		dynlibsType[name]="CollisionGeometry";
	else if (dynamic_pointer_cast<BoundingVolume>(f))
		dynlibsType[name]="BoundingVolume";
	else if (dynamic_pointer_cast<InteractionGeometry>(f))
		dynlibsType[name]="InteractionGeometry"; // FIXME : change name of the subproject
	else
		dynlibsType[name]="Unknown";


}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Omega::buildDynlibList()
{
	char * buffer ;
	buffer = getenv ("YADEBINPATH"); // FIXME - yade should use config file, to check /usr/lib/yade and /home/joe/yade/lib, etc..
	string yadeBinPath = buffer;

	filesystem::path directory(yadeBinPath+"/dynlib/linux");
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Omega::getDynlibType(const string& libName, string& type)
{
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

void Omega::incrementCurrentIteration()
{
	++currentIteration;
}

long int Omega::getCurrentIteration()
{
	return currentIteration;
}


void Omega::setSimulationFileName(const string f)
{
	simulationFileName = f;
};

string Omega::getSimulationFileName()
{
	return simulationFileName;
}

void Omega::loadSimulation()
{

	if( Omega::instance().getSimulationFileName().size() != 0  &&  filesystem::exists(simulationFileName) )
	{
		freeSimulation();
		IOManager::loadFromFile("XMLManager",simulationFileName,"rootBody",Omega::instance().rootBody);
		Omega::instance().logMessage("Loading file " + simulationFileName);

		sStartingSimulationTime = second_clock::local_time();
		msStartingSimulationTime = microsec_clock::local_time();

		*logFile << "<Simulation" << " Date =\"" << sStartingSimulationTime << "\">" << endl;
		currentIteration = 0;
		simulationTime = 0;
	}
	else
	{
		cout << "\nWrong filename, please specify filename using your frontend.\n";
		exit(1);
	}
}

void Omega::freeSimulation()
{
	rootBody = shared_ptr<NonConnexBody>();
}

void Omega::startSimulationLoop()
{
	simulationLoop->start();
}

void Omega::stopSimulationLoop()
{
	simulationLoop->stop();
}


// FIXME - remove that
void Omega::setGravity(Vector3r g)
{
	gravity = g;
}

Vector3r Omega::getGravity()
{
	return gravity;
}

// FIXME - remove that
void Omega::setTimeStep(const double t)
{
	dt = t;
}

double Omega::getTimeStep()
{
	return dt;
}
