/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "StrainRecorder.hpp"
#include<yade/core/MetaBody.hpp>
#include "LatticeBeamParameters.hpp"
#include "LatticeNodeParameters.hpp"
// to calculate strain of whole speciemen - first two bodies in subscribedBodies are Nodes. FIXME - make it clean!
#include <boost/lexical_cast.hpp>


StrainRecorder::StrainRecorder () : DataRecorder()
{
	outputFile = "";
	interval = 50;
	subscribedBodies.clear();
}


void StrainRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	//	outputFile += "_"+boost::lexical_cast<std::string>(Omega::instance().getTimeStep());
	//	std::cerr << "using dt for StrainRecorder output file: " << outputFile << "\n";

		//ofile.open(outputFile.c_str());
		ofile.open(outputFile.c_str(), std::ios::app);

	}
}


void StrainRecorder::registerAttributes()
{
	DataRecorder::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(subscribedBodies);
	REGISTER_ATTRIBUTE(initialLength);
}


bool StrainRecorder::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) && (ofile));
}


void StrainRecorder::action(MetaBody * ncb)
{
//	std::cerr << "StrainRecorder quits\n";
//	return;
	
	Real strain_y=0,stress_y=0,stress_s=0;//,stress_nonlocal_y=0;
	
	std::vector<unsigned int>::iterator i   = subscribedBodies.begin();
	std::vector<unsigned int>::iterator end = subscribedBodies.end();
	
	if(!ncb->bodies->exists(*i))
	{
		std::cerr << "StrainRecorder missing node\n";
		return;
	}
	LatticeNodeParameters* node1 = dynamic_cast<LatticeNodeParameters*>( (*(ncb->bodies))[*i]->physicalParameters . get() );
	(*(ncb->bodies))[*i]->geometricalModel->diffuseColor = Vector3r(1.0,0.0,0.0); // FIXME [1]
	++i;
	if(!ncb->bodies->exists(*i)) 
	{
		std::cerr << "StrainRecorder missing node\n";
		return;
	}
	LatticeNodeParameters* node2 = dynamic_cast<LatticeNodeParameters*>( (*(ncb->bodies))[*i]->physicalParameters . get() );
	(*(ncb->bodies))[*i]->geometricalModel->diffuseColor = Vector3r(1.0,0.0,0.0); // FIXME [1]
	++i;
	// FIXME - zamiast ?ledzi? tylko dwa punkty (jeden na dole i jeden u góry), to lepiej zaznaczy? dwa obszary punktów i liczy? ?redni? ich po?o?enia,
	// bo teraz, je?li który? punkt zostanie wykasowany, to nie jest mo?liwe kontynuowanie pomiarów.
	
	Real 		currentLength = node1->se3.position[1] - node2->se3.position[1];
	
	strain_y = (currentLength - initialLength) / initialLength; // odkszta?cenie ca?ej próbki
	
	//bool nonlocal = false;
	//if (ncb->transientInteractions->size() != 0) // it's non-local
	//	nonlocal = true;
	
	for( ; i != end ; ++i )
	{
		if( (*(ncb->bodies)).exists(*i) )
		{
			LatticeBeamParameters* beam 	= static_cast<LatticeBeamParameters*>( (*(ncb->bodies))[*i]->physicalParameters . get() );
			Real s_y 			= beam->strain()        * std::abs(beam->direction[1]) * beam->longitudalStiffness; // direction is a unit vector, so direction[1] is cos(angle_to_y)
			Real s_s			= beam->shearing_strain[1];// * beam->bendingStiffness;
			//Real s_nonl_y =0;
			//if(nonlocal)
			//	s_nonl_y 		= (beam->nonLocalStrain / beam->nonLocalDivisor) * std::abs(beam->direction[1]);
			(*(ncb->bodies))[*i]->geometricalModel->diffuseColor = Vector3r(0.0,1.0,1.0); // FIXME [1]
			stress_y += s_y;
			stress_s += s_s;
			//stress_nonlocal_y += s_nonl_y;
		}
	}
	
	//ofile << lexical_cast<string>(Omega::instance().getSimulationTime()) << " " 
	ofile	<< lexical_cast<string>(strain_y) << " " 
		<< lexical_cast<string>(stress_y) << " " 
		<< lexical_cast<string>(stress_s)// << " " 
		//<< lexical_cast<string>(stress_nonlocal_y)
		<< endl; 
		
	// [1]
	// mark colors what is monitored - FIXME - provide a way to do that for ALL DataRecorders in that parent class.
	// GLDrawSomething can just put a getClassName()
	 
}

YADE_PLUGIN();
