/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef POSITIONORIENTATIONRECORDER_HPP
#define POSITIONORIENTATIONRECORDER_HPP

#include <yade/yade-core/Engine.hpp>
#include <string>
#include <fstream>

class PositionOrientationRecorder : public Engine
{
	public :
		std::string outputFile;
		unsigned int interval;
	
		std::ofstream ofile;

		PositionOrientationRecorder ();
		~PositionOrientationRecorder ();
		virtual void action(Body* b);

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(PositionOrientationRecorder);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(PositionOrientationRecorder,false);

#endif // POSITIONORIENTATIONRECORDER_HPP

