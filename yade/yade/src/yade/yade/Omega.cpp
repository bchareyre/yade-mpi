#include "Omega.hpp"
#include "Vector3.hpp"

#include "IOManager.hpp" // is this allowed? perhaps loadTheFile should be in pimpl of Omega ? (pointer to implementation)
#include "NonConnexBody.hpp"

#include <cstdlib>

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/convenience.hpp>
             
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

#include "SimulationLoop.hpp"
#include "FrontEnd.hpp"

Omega::Omega ()
{
	cerr << "Constructing Omega  (if multiple times - check '-rdynamic' flag!)" << endl;
	init();
}

Omega::~Omega ()
{
	*logFile << "\t" << "<Summary Duration=\"" << startingSimulationTime-second_clock::local_time() << "\">" <<endl;
	*logFile << "</Simulation>" << endl << endl;
	logFile->close();
}

void Omega::logError(const string& str)
{
	*logFile << "\t" << "<Error Date=\"" << startingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
};

void Omega::logMessage(const string& str)
{
	*logFile << "\t" << "<Message Date=\"" << startingSimulationTime-second_clock::local_time() << "\" " << "Message =\""<< str << "\"" << endl;
};

void Omega::init()
{
	fileName="";
	maxiter = 0; // unlimited
	iter = 0;
	automatic=false;
	progress=false;

	// FIXME - this must be a parameter in .xml !!!
	//gravity_x = 0.0;
	//gravity_y = -10.0;
	//gravity_y = 0.0;
	//gravity_z = 0.0;
	gravity = Vector3r(0,-9.81,0);
	//dt = 0.04;
	dt = 0.01;
	//dt = 0.015; // max for cloth, rotating box is little slower, but both work.

	logFile = shared_ptr<ofstream>(new ofstream("../data/log.xml", ofstream::out | ofstream::app));

	startingSimulationTime = second_clock::local_time();

	*logFile << "<Simulation" << " Date =\"" << startingSimulationTime << "\">" << endl;
	

	// build dynlib information list
	buildDynlibList();
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
	buffer = getenv ("YADEBINPATH");
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

long int& Omega::getIterReference()
{
	return iter;
}

long int Omega::getIter()
{
	return iter;
}

/// FIXME - everything below SHOULD NOT be inside Omega.

void Omega::setGravity(Vector3r g)
{
	//gravity_x = g.x();
	//gravity_y = g.y();
	//gravity_z = g.z();
	gravity = g;
}

Vector3r Omega::getGravity()
{
	return gravity; //Vector3r(gravity_x,gravity_y,gravity_z);
}

/// FIXME - maybe some settings class, or something....

void Omega::setTimestep(const string t)
{
	dt = lexical_cast<float>(t);
}

float Omega::getTimestep()
{
	return dt;
}

void Omega::setFileName(const string f)
{
	fileName = f;
};

string Omega::getFileName()
{
	return fileName;
}

void Omega::loadTheFile()
{

	if( Omega::instance().getFileName().size() != 0  &&  filesystem::exists(fileName) )
	{

		IOManager::loadFromFile("XMLManager",fileName,"rootBody",Omega::instance().rootBody);
		Omega::instance().logMessage("Loading file " + fileName);
	}
	else
	{
		cout << "\nWrong filename, please specify filename with -f, or through your GUI.\n";
		exit(1);
	}
}

void Omega::setMaxiter(const string m)
{
	maxiter = lexical_cast<long int>(m);
}

long int Omega::getMaxiter()
{
	return maxiter;
}

void Omega::setAutomatic(bool b)
{
	automatic = b;
}

bool Omega::getAutomatic()
{
	return automatic;
}

void Omega::setProgress(bool b)
{
	progress = b;
}

bool Omega::getProgress()
{
	return progress;
}


void Omega::endOfSimulationLoop()
{
	gui->endOfSimulationLoop();
}
