/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Omega.hpp>
#include<yade/lib-opengl/GLWindowsManager.hpp>
#include<yade/lib-serialization-qt/QtGUIGenerator.hpp>
#include<yade/core/RenderingEngine.hpp>
#include<qlayout.h>
#include<qframe.h>
#include<qscrollview.h>
#include<qspinbox.h>
#include"GLViewer.hpp"
#include"QtGeneratedSimulationController.h"

class SimulationController : public QtGeneratedSimulationController
{
	private :
		QtGUIGenerator guiGen;	
		QWidget * parentWorkspace;	
		//map<int,GLViewer* > glViews;
		int maxNbViews;
		int refreshTime;
		int updateTimerId;
		bool sync;
		bool syncRunning;
		bool hasSimulation;
		long lastRenderedIteration;

		const int iterPerSec_TTL_ms;
		long  iterPerSec_LastIter;
		double iterPerSec;
        boost::posix_time::time_duration duration;
        boost::posix_time::time_duration estimation;
		boost::posix_time::ptime iterPerSec_LastLocalTime;

	
		void doUpdate();
		void restartTimer();

		QScrollView * scrollView;
		QFrame * scrollViewFrame;
		QVBoxLayout* scrollViewLayout;
		void addNewView();
	
	public : 
		void loadSimulationFromFileName(const std::string& fileName,bool center=true, bool useTimeStepperIfPresent=true);
		bool changeSkipTimeStepper,skipTimeStepper,changeTimeStep,wasUsingTimeStepper;
		SimulationController (QWidget * parent=NULL);
		virtual ~SimulationController () {}; 
        
	public slots :
		virtual void pbApplyClicked();
		virtual void pbLoadClicked();
		virtual void pbSaveClicked();
		virtual void pbNewViewClicked();
		virtual void pbStopClicked();
		virtual void pbStartClicked();
		virtual void pbResetClicked();
		virtual void pbReferenceClicked();
		virtual void pbCenterSceneClicked();
		virtual void pbOneSimulationStepClicked();
		virtual void bgTimeStepClicked(int i);
		virtual void sb10PowerSecondValueChanged(int);
		virtual void sbSecondValueChanged(int);
		virtual void sbRefreshValueChanged(int);
		virtual void cbSyncToggled(bool);
		virtual void pbStart2Clicked();
		virtual void pbXYZ_clicked();
		virtual void pbYZX_clicked();
		virtual void pbZXY_clicked();



        
        protected :
                void closeEvent(QCloseEvent *evt);
                virtual void timerEvent(QTimerEvent* );

		friend class YadeQtMainWindow;
	DECLARE_LOGGER;
};


