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
#include<yade/lib-serialization-qt/QtGUIGenerator.hpp>
#include <QtGeneratedSimulationPlayer.h>
#include<qlayout.h>
#include<qframe.h>
#include<qscrollview.h>

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

		QScrollView * scrollView;
		QFrame * scrollViewFrame;
		QVBoxLayout* scrollViewLayout;
		QtGUIGenerator guiGen;	

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

		virtual void pbApplyClicked();
		
		virtual void cbAllowFiltrationToggled(bool b);
        virtual void pbRefreshFiltersClicked();
		
	protected:
		void closeEvent(QCloseEvent *);
		virtual void keyPressEvent(QKeyEvent *);

	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(QtSimulationPlayer);
	REGISTER_BASE_CLASS_NAME(Factorable);

};


REGISTER_FACTORABLE(QtSimulationPlayer);


