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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __QTENGINEEDITOR_HPP__
#define __QTENGINEEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtGeneratedSimulationPlayer.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-factory/Factorable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class QtSimulationPlayer : public QtGeneratedSimulationPlayer, public Factorable
{
	public : QtSimulationPlayer();
	public : ~QtSimulationPlayer();

	private : void setParameters();

	public slots : virtual void pbInputDirectoryClicked();
	public slots : virtual void pbLoadClicked();
	public slots : virtual void pbInputConfigFileClicked();

	public slots : virtual void pbOutputDirectoryClicked();
	public slots : virtual void cbSaveSnapShotsToggled(bool b);

	public slots : virtual void pbPlayClicked();
	public slots : virtual void pbPauseClicked();
	public slots : virtual void pbStepClicked();
	public slots : virtual void pbResetClicked();
	
 	protected    : void closeEvent(QCloseEvent *);

	REGISTER_CLASS_NAME(QtSimulationPlayer);
	REGISTER_BASE_CLASS_NAME(Factorable);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(QtSimulationPlayer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTENGINEEDITOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
