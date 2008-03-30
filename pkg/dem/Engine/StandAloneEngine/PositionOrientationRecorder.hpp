/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef POSITIONORIENTATIONRECORDER_HPP
#define POSITIONORIENTATIONRECORDER_HPP

#include<yade/core/DataRecorder.hpp>
#include <string>
#include <fstream>
/*!
 * Engine recording positions and orientation to plain ASCII files, primarily intended for use with the Simulation Player.
 *
 * See http://yade.wikia.com/wiki/New:Making_videos for details .
 *
 */
class PositionOrientationRecorder : public DataRecorder
{
	public :
		//! basename of the output files; for example, /tmp/xyz will produce /tmp/xyz_000010, /tmp/xyz_000020 and so on (with interval==10)
		std::string outputFile;
		//! modulo of iteration number when the se3's will be be written out
		unsigned int interval;
		bool saveRgb;
	
		PositionOrientationRecorder ();
		~PositionOrientationRecorder ();
		virtual void action(Body* b);

	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(PositionOrientationRecorder);
	REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(PositionOrientationRecorder,false);

#endif // POSITIONORIENTATIONRECORDER_HPP

