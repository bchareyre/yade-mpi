/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <yade/pkg-common/Recorder.hpp>
#include <string>
#include <fstream>

class CapillaryRecorder: public Recorder {
	private :
		std::ofstream ofile;
		
	public:
		virtual void action();
		virtual bool isActivated();
		virtual void postProcessAttributes(bool deserializing);
		//CapillaryRecorder ();
	
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(CapillaryRecorder,Recorder,"Store informations about a capillary interaction to a text file.",
		((unsigned int,interval,1," recording interval."))
		((int,sphereId,1,"To select the desired particles."))
		((string,outputFile,"","Name of the output file.")),
		initRun=true;);
	DECLARE_LOGGER;
	
};
REGISTER_SERIALIZABLE(CapillaryRecorder);

//// OLD CODE!!!
// class CapillaryRecorder : public Recorder
// {
// 	private :
// 		std::ofstream ofile; 
// 
// 		bool changed;
// 	
// 	public :
// 		std::string	 outputFile;
// 		unsigned int	 interval;
// 
// 		int		sphereId;
// 
// 		CapillaryRecorder ();
// 
// 		virtual void action();
// 		virtual bool isActivated();
// 	REGISTER_ATTRIBUTES(Recorder,(outputFile)(interval)(bigBallId));
// 	protected :
// 		virtual void postProcessAttributes(bool deserializing);
// 	REGISTER_CLASS_NAME(CapillaryRecorder);
// 	REGISTER_BASE_CLASS_NAME(Recorder);
// };
// 
// REGISTER_SERIALIZABLE(CapillaryRecorder);




