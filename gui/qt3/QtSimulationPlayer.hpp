/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTSIMULATIONPLAYER_HPP
#define QTSIMULATIONPLAYER_HPP

#include<yade/core/Omega.hpp>
#include<yade/lib-factory/Factorable.hpp>
#include <QtGeneratedSimulationPlayer.h>

class QtSimulationPlayer : public QtGeneratedSimulationPlayer, public Factorable
{
	public : QtSimulationPlayer();
	public : ~QtSimulationPlayer();

	private : void setParameters();
		list<string> messages;
	public: void pushMessage(std::string);


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
//						void keyPressEvent(QKeyEvent *){};
	public:
	void enableControls(bool);

	REGISTER_CLASS_NAME(QtSimulationPlayer);
	REGISTER_BASE_CLASS_NAME(Factorable);

};


REGISTER_FACTORABLE(QtSimulationPlayer);

#endif // QTSIMULATIONPLAYER_HPP

