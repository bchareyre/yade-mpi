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
#include <getopt.h>

using namespace std;

void help()
{
	cout <<
	"\nYet Another Dynamic Engine, pre-alpha.\n\n\
	-h	- print this help\n\
	-H	- print help for selected fronted\n\
	-i	- specify name of frontend interface library:\n\
			QtGUI, NullGUI\n\
		(other GUIs, like ncurses, command line and\n\
		network-based interfaces can be added later)\n\
	\n";
}

int main(int argc, char *argv[])
{
	string frontEndLibName="";

	int ch;
	opterr = 0;
	if( ( ch = getopt(argc,argv,"hi:") ) != -1)
		switch(ch)
		{
			case 'h'	: help();		return 1;
			case 'i'	: frontEndLibName = optarg;	break;
			default		: help();		return 1;
		}

	if( frontEndLibName.size() == 0 )
	{
		help();
		return 0;
	}

	shared_ptr<FrontEnd> frontEnd = dynamic_pointer_cast<FrontEnd>(ClassFactory::instance().createShared(frontEndLibName));
	int ok = frontEnd->run(argc,argv);
	
	cout << "YADE ENDED CORRECTLY ..." << endl;
	
	return ok;
}
