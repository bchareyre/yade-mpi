/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "NullGUI.hpp"
#include "MetaBody.hpp"
#include "FileGenerator.hpp"
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

NullGUI::NullGUI ()
{
	interval = 100;
	progress = false;
	snapshotInterval = -1;
	snapshotName = "";
	maxIteration = 0;
	binary = 0;
	filegen = "";
	file = "";
}


NullGUI::~NullGUI()
{

}


void NullGUI::help()
{
	cout <<
"\n" << Omega::instance().yadeVersionName << " NullGUI frontend.\n\
\n\
	-H		- print this help.\n\
\n\
    user feedback:\n\
	-v number	- specify iteration INTERVAL in which other tasks are\n\
			  performed, default is set to 100.\n\
	-p		- print progress information every INTERVAL iteration\n\
			  so you see that calculations are going on.\n\
\n\
    input:\n\
	-f name		- specify filename to load, or filegenerator configuration file\n\
\n\
    output:\n\
	-s number	- if specified, a snapshot is saved every 'number'\n\
			  INTERVALs. Eg. if INTERVAL is 100, and 'number' is 5,\n\
			  you will have a snpashot every 500th iteration.\n\
	-S name		- specify base filename of snapshot. Defaults to input\n\
			  filename. Filename has appended iteration number.\n\
			  Or specify FileGenerator output file if generating a file.\n\
			  (without extension, use -b to get binary output)\n\
        -Q name		- do not calculate, just save what is loaded and exit\n\
			  (for doing data conversions)\n\
	-F name		- FileGenerator name, must be specified with -f that specifies\n\
			  the accompanying configuration file (used for file generation)\n\
			  and with -S that specifies the output file to save\n\
\n\
    options:\n\
	-m number	- specify maximum number of iterations\n\
			  ( 0 = unlimited, tested every INTERVAL iteration).\n\
	-t number	- set time step in seconds, default is 0.01 (FIXME - inside .xml)\n\
	-b number	- save in binary .yade format instead of .xml\n\
\n";
//      -g number       - set gravity, default is 9.81 (FIXME - inside .xml)\n
}

std::string quickSave("");

int NullGUI::run(int argc, char* argv[])
{

	int ch;
	while( ( ch = getopt(argc,argv,"Hf:s:S:v:pm:t:g:bQ:F:") ) != -1)
		switch(ch)
		{
			case 'H'        : help();                                               return 1;
			case 'v'	: interval = lexical_cast<int>(optarg);			break;
			case 'p'	: progress = true;					break;
			case 'F'	: filegen = optarg;					break;
			case 'f'	: file = optarg;					break;
			case 's'        : snapshotInterval = lexical_cast<int>(optarg);		break;
			case 'S'        : snapshotName = optarg;				break;
			case 'Q'        : quickSave = optarg;					break;
			case 'm'        : maxIteration = lexical_cast<long int>(optarg);	break;
			case 'b'        : binary = true;					break;
			case 't'        : Omega::instance().setTimeStep
						(lexical_cast<Real>(optarg));			break;
//			case 'g'	: Omega::instance().setGravity
//						(Vector3r(0,-lexical_cast<Real>(optarg),0));	break;
			default		:help(); 						return 1;
		}
	if(filegen.empty()) // calculations
	{
		Omega::instance().setSimulationFileName(file);
		if(snapshotName.empty() ) snapshotName = file;
		if(Omega::instance().getSimulationFileName() == "") 
			help(), exit(0);
		return loop();
	} 
	else // FileGenerator
		return gen();

	assert(false); // never reach this place
}


int NullGUI::loop()
{
	try
	{
		Omega::instance().loadSimulation();
	}
	catch(SerializableError& e)
	{
		std::cerr << file << " cannot be loaded: " << e.what() << "\n";
		exit(0);
	}
		
	if(quickSave.size()!=0)
		std::cerr << "saving "<< quickSave << "\n", Omega::instance().saveSimulation(quickSave), exit(1);

	cerr << "Starting computation of file: " << Omega::instance().getSimulationFileName() << endl;

	if( maxIteration == 0)
		cerr << "No maxiter specified, computations will run forever, to set it, use flag -m\n";
	else
		cerr << "Computing " << maxIteration << " iterations\n";

	cerr << "Using timestep: " << Omega::instance().getTimeStep() << endl;

	filesystem::path p(snapshotName);
	snapshotName = filesystem::basename(p);
	if( snapshotInterval != -1 )
		cerr 	<< "Saving snapshot every " << snapshotInterval*interval << " iterations, \n"
			<< "to filename: " << snapshotName << "_[0-9]" << endl;

	long int intervals = 0;
	boost::posix_time::ptime start = boost::posix_time::second_clock::local_time();
	while(1)
	{
		Omega::instance().getRootBody()->moveToNextTimeStep();
		Omega::instance().incrementCurrentIteration();
		Omega::instance().incrementSimulationTime();

		if(Omega::instance().getCurrentIteration() % interval == 0 )
		{
			++intervals;

			// print progress...
			if(progress)
				cerr << "iteration: " << Omega::instance().getCurrentIteration() << endl;

			// save snapshot
			if( ( snapshotInterval != -1 ) && (intervals % snapshotInterval == 0) )
			{
				string fileName = "../data/" 
					+ snapshotName 
			//		+ "__dt_" + lexical_cast<string>(Omega::instance().getTimeStep())
			//		+ "__it_" + lexical_cast<string>(Omega::instance().getCurrentIteration()) 
					+ "_" + lexical_cast<string>(Omega::instance().getCurrentIteration()) 
					+ (binary?".yade":".xml");
				cerr << "saving snapshot: " << fileName << "   ...";
				Omega::instance().saveSimulation(fileName);
				cerr << " done.\n";
			}
		}

		// finish computation
		if( ( maxIteration !=0 ) &&  (Omega::instance().getCurrentIteration() >= maxIteration) )
		{
			cerr << "Calc finished at: " << Omega::instance().getCurrentIteration() << endl;
			cerr << "Computation time: " << boost::posix_time::to_simple_string(boost::posix_time::time_duration(boost::posix_time::second_clock::local_time() - start)) << endl;
			exit(0);
		}
	}
}

int NullGUI::gen()
{
	if(snapshotName.empty())
		std::cerr << "please specufy output file name with -S\n", exit(0);
	if(file.empty())
		std::cerr << "please specufy config file name with -f\n", exit(0);
	boost::shared_ptr<Factorable> tmpf;
	try
	{
		tmpf = ClassFactory::instance().createShared(filegen);
	}
	catch(FactoryError& e)
	{
		std::cerr << filegen << " is not available: " << e.what() << "\n", exit(0);
	}
	boost::shared_ptr<FileGenerator> f = dynamic_pointer_cast<FileGenerator>(tmpf);
	if( f == 0 )
		std::cerr << filegen << " is not a FileGenerator.\n", exit(0);
	try
	{
		std::cerr << "loading FileGenerator configuration file: " << file << "\n";
		IOFormatManager::loadFromFile("XMLFormatManager",file,"fileGenerator",f);
	}
	catch(SerializableError& e)
	{
		std::cerr << file << " cannot be loaded: " << e.what() << "\n", exit(0);
	}
	std::cerr	<< "calling FileGenerator: " << filegen 
			<< ",\nwith config file: " << file 
			<< ",\nto generate file: " << snapshotName << (binary?".yade":".xml") << "\n\n";
	f->setFileName(snapshotName + (binary?".yade":".xml"));
	f->setSerializationLibrary((binary?"BINFormatManager":"XMLFormatManager"));
	std::cerr << "\n" << f->generateAndSave() << "\n";
	return 0;
};

