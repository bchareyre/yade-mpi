/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "ClassFactory.hpp"

#include "Omega.hpp"
#include "FrontEnd.hpp"

#include <iostream>
using namespace std;
void help()
{
	cout << "\n\
Yet Another Dynamic Engine, pre-alpha.\n\
\n\
	-h	- print this help\n\
	-i	- specify name of frontend interface library: \n\
		  	YadeQtGUI, NullGUI\n\
		  (other GUIs, like ncurses, command line and\n\
		  network-based interfaces can be added later)\n\
	-f	- specify filename to load\n\
	-m	- specify maximum number of iterations ( 0 = unlimited,\n\
		  tested every 100th iteration)\n\
	-p	- print progress every 100th iteration\n\
	-t	- set time step in seconds\n\
	-a	- automatic start of computation\n\
\n\
";
}

int main(int argc, char *argv[])
{
	string frontend="";

	int ch;
	while((ch=getopt(argc,argv,"hi:f:m:t:ap"))!=-1) // use ':', when additional parameter optarg is expected and used
		switch(ch)
		{
			case 'h' : help();					return 1;
			case 'i' : frontend = optarg;				break;
			case 'f' : Omega::instance().setFileName(optarg);	break;
			case 'm' : Omega::instance().setMaxiter(optarg);	break;
			case 't' : Omega::instance().setTimestep(optarg);	break;
			case 'a' : Omega::instance().setAutomatic(true);	break;
			case 'p' : Omega::instance().setProgress(true);		break; // FIXME change that !!!
			default  : help();					return 1;
	}

	if( frontend.size() == 0 )
	{
		help();
		return 0;
	}

	Omega::instance().gui = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(frontend));

	return Omega::instance().gui->run(argc,argv);
}
