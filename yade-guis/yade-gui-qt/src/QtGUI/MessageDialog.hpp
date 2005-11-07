/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef MESSAGEDIALOG_HPP
#define MESSAGEDIALOG_HPP

#include <string>
#include "QtGeneratedMessageDialog.h"

using namespace std;

class MessageDialog : public QtGeneratedMessageDialog
{
	public :
		MessageDialog (const string& message,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
		virtual ~MessageDialog ();
	
	public slots :
		virtual void pbOkClicked();
		virtual void closeEvent(QCloseEvent * evt);
		void timerEvent(QTimerEvent * evt);
};

#endif // MESSAGEDIALOG_HPP

