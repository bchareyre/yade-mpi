/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FORCE_RECORDER_HPP
#define FORCE_RECORDER_HPP

#include <yade/yade-core/DataRecorder.hpp>

#include <string>
#include <fstream>

class PhysicalAction;

class ForceRecorder : public DataRecorder
{
	private :
		shared_ptr<PhysicalAction> actionForce;
		std::ofstream ofile; 

		bool changed;
	
	public :
		std::string	 outputFile;
		unsigned int	 interval
				,startId
				,endId;

		int		bigBallId; // FIXME !!!!!!!!!!
		Real		bigBallReleaseTime; // FIXME !!!!!!!!!!

		ForceRecorder ();

		virtual void registerAttributes();
		virtual void action(Body* b);
		virtual bool isActivated();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(ForceRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(ForceRecorder,false);

#endif // FORCE_RECORDER_HPP

