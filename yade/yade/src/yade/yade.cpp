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
#include "GUI.hpp"
#include "Omega.hpp"

int main(int argc, char *argv[])
{
	Omega::instance().init();

	if (argc!=2)
	{
		cout << "Missing parameter : name of the GUI library, try   ./yade YadeQtGUI" << endl;
		return 0;
	}

	shared_ptr<GUI> gui = dynamic_pointer_cast<GUI>(ClassFactory::instance().createShared(argv[1]));

	Omega::instance().init();

	return gui->run(argc,argv);
}
