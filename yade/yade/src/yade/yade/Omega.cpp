#include "Omega.hpp"
#include "Vector3.hpp"

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
	filename="";
	maxiter = 0; // unlimited

	gravity_x = 0.0;
	gravity_y = -9.81;
	//gravity_y = 0.0;
	gravity_z = 0.0;
	//	setGravity = Vector3(0,-9.81,0);
	//dt = 0.04;
	dt = 0.01;
	//dt = 0.015; // max for cloth, rotating box is little slower, but both work.

	logFile = shared_ptr<ofstream>(new ofstream("../data/log.xml", ofstream::out | ofstream::app));

	startingSimulationTime = second_clock::local_time();

	*logFile << "<Simulation" << " Date =\"" << startingSimulationTime << "\">" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Vector3 Omega::getGravity()
{
	return Vector3(gravity_x,gravity_y,gravity_z);
}

void Omega::setGravity(Vector3 g)
{
	gravity_x = g.x;
	gravity_y = g.y;
	gravity_z = g.z;
}

void Omega::setTimestep(const string t)
{
	dt = lexical_cast<float>(t);
}

float Omega::getTimestep()
{
	return dt;
}

void Omega::setFilename(const string f)
{
	filename = f;
};

string Omega::getFilename()
{
	return filename;
}

void Omega::setMaxiter(const string m)
{
	maxiter = lexical_cast<long int>(m);
}

long int Omega::getMaxiter()
{
	return maxiter;
}

