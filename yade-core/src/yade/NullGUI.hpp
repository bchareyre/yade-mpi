/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef NULLGUI_HPP
#define NULLGUI_HPP

// FIXME : there is a problem because I need to link with serialization when I include Factorable because Factorable use some Type described in serialization for the findClassInfo method
#include "FrontEnd.hpp"
#include <yade/yade-lib-time/Chrono.hpp>

/*! \brief
	This GUI is commmand line interface for yade - just starts calculations and never ends
	(unless, maxiter is given) and it requires a filename, also assumes -a flag.
*/
class NullGUI : public FrontEnd
{
	private :
		Chrono chron;

		int 		 interval
				,snapshotInterval;

		bool 		 progress;
		string		 snapshotName;
		long int	 maxIteration;
		
		void help();
		int loop();

	public :
		NullGUI ();
		virtual ~NullGUI ();
		int run(int argc, char *argv[]);
	
	REGISTER_CLASS_NAME(NullGUI);
	REGISTER_BASE_CLASS_NAME(FrontEnd);
};

REGISTER_FACTORABLE(NullGUI);

#endif // NULLGUI_HPP

