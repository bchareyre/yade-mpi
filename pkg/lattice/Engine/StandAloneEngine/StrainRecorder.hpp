/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef STRAIN_RECORDER_HPP
#define STRAIN_RECORDER_HPP 

#include<yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

//class PhysicalAction;

class StrainRecorder : public DataRecorder
{
	private :
//		shared_ptr<PhysicalAction> actionForce;
		std::ofstream ofile; 

	public :
		std::string	 outputFile;
		unsigned int	 interval;
		std::vector<unsigned int> subscribedBodies;
		Real 		 initialLength;

		StrainRecorder ();

		virtual void registerAttributes();
		virtual void action(MetaBody*);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(StrainRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(StrainRecorder,false);

#endif // STRAIN_RECORDER_HPP 

