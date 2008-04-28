/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CAPILLARY_RECORDER_HPP
#define CAPILLARY_RECORDER_HPP

#include <yade/core/DataRecorder.hpp>

#include <string>
#include <fstream>

class PhysicalAction;

class CapillaryRecorder : public DataRecorder
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		std::ofstream ofile; 

		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval;

		int		bigBallId;

		CapillaryRecorder ();

		virtual void registerAttributes();
		virtual void action(MetaBody*);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(CapillaryRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(CapillaryRecorder,false);

#endif // CAPILLARY_RECORDER_HPP

