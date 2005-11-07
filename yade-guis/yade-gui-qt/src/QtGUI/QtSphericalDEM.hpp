/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef QTSPHERICALDEM_HPP
#define QTSPHERICALDEM_HPP

#include <qlayout.h>
#include <qframe.h>
#include <qscrollview.h>
#include <QtGeneratedSphericalDEMSimulator.h>
#include <boost/shared_ptr.hpp>
#include <yade/yade-lib-time/Chrono.hpp>
#include <yade/yade-lib-factory/Factorable.hpp>
#include <yade/yade-core/StandAloneSimulator.hpp>

using namespace boost;

class QtSphericalDEM : public QtGeneratedSphericalDEMSimulator, public Factorable
{
	private :
		shared_ptr<StandAloneSimulator> simulator;
		int	currentIteration;
		int	maxIteration;
		bool	stop;
		Chrono	chron;
		bool	record;

	public :
		QtSphericalDEM (QWidget * parent = 0, const char * name = 0 );
		virtual ~QtSphericalDEM ();

	public slots :
		virtual void pbPathClicked();
		virtual void pbLoadClicked();
		virtual void pbStartClicked();
		virtual void pbStopClicked();
		virtual void bgTimeStepClicked(int i);
		virtual void pbOutputDirectoryClicked();
		virtual void cbRecordToggled(bool b) { record = b;};

	protected :
		void closeEvent(QCloseEvent *evt);

	REGISTER_CLASS_NAME(QtSphericalDEM);
	REGISTER_BASE_CLASS_NAME(Factorable);
};

REGISTER_FACTORABLE(QtSphericalDEM);

#endif // QTSPHERICALDEM_HPP

