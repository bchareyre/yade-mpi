/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/lib-factory/Factorable.hpp>
#include <QtGeneratedSimulationPlayer.h>

class GLSimulationPlayerViewer;

class QtSimulationPlayer : public QtGeneratedSimulationPlayer, public Factorable
{
	public:
		QtSimulationPlayer();
		~QtSimulationPlayer();
		void pushMessage(std::string);
		list<string> messages;
		GLSimulationPlayerViewer* glSimulationPlayerViewer;
		void enableControls(bool);

	private:
		void setParameters();

	public slots:
		virtual void pbInputDirectoryClicked();
		virtual void pbLoadClicked();
		virtual void pbInputConfigFileClicked();

		virtual void pbOutputDirectoryClicked();
		virtual void cbSaveSnapShotsToggled(bool b);

		virtual void pbPlayClicked();
		virtual void pbPauseClicked();
		virtual void pbStepClicked();
		virtual void pbResetClicked();
		
		virtual void cbAllowFiltrationToggled(bool b);
        virtual void pbRefreshFiltersClicked();
		
		virtual void cbBodyWireToggled(bool b);
 	
	protected:
		void closeEvent(QCloseEvent *);
		virtual void keyPressEvent(QKeyEvent *);

	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(QtSimulationPlayer);
	REGISTER_BASE_CLASS_NAME(Factorable);

};


REGISTER_FACTORABLE(QtSimulationPlayer);


