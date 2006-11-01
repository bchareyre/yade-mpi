/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMULATIONCONTROLLER_HPP
#define SIMULATIONCONTROLLER_HPP

#include <yade/yade-lib-opengl/GLWindowsManager.hpp>
#include <yade/yade-lib-serialization-qt/QtGUIGenerator.hpp>
#include <yade/yade-core/RenderingEngine.hpp>
#include <qlayout.h>
#include <qframe.h>
#include <qscrollview.h>
#include "GLViewer.hpp"
#include "QtGeneratedSimulationController.h"
#include "SimulationControllerUpdater.hpp"

class SimulationController : public QtGeneratedSimulationController
{
	private :
		QtGUIGenerator guiGen;	
		QWidget * parentWorkspace;	
		shared_ptr<RenderingEngine> renderer;
		map<int,GLViewer* > glViews;
		int maxNbViews;
		int refreshTime;
		bool sync;
	
		shared_ptr<SimulationControllerUpdater> updater;

		QScrollView * scrollView;
		QFrame * scrollViewFrame;
		QVBoxLayout* scrollViewLayout;
		void addNewView();
	
	public : 
		bool	 changeSkipTimeStepper
			,skipTimeStepper
			,changeTimeStep
                        ,wasUsingTimeStepper;

                SimulationController (QWidget * parent=0);
                void redrawAll();
                virtual ~SimulationController (); 
        
        public slots :
		virtual void pbApplyClicked();
		virtual void pbLoadClicked();
		virtual void pbSaveClicked();
		virtual void pbNewViewClicked();
		virtual void pbStopClicked();
		virtual void pbStartClicked();
		virtual void pbResetClicked();
		virtual void pbCenterSceneClicked();
		virtual void pbOneSimulationStepClicked();
		virtual void bgTimeStepClicked(int i);
		virtual void sb10PowerSecondValueChanged(int);
		virtual void sbSecondValueChanged(int);
		virtual void sbRefreshValueChanged(int);
		virtual void cbSyncToggled(bool);
		virtual void pbStart2Clicked();
		void closeGLViewEvent(int id);

        
        protected :
                void closeEvent(QCloseEvent *evt);
                virtual void timerEvent(QTimerEvent* );
};

#endif // SIMULATIONCONTROLLER_HPP

