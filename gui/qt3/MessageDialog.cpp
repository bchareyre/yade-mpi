/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "MessageDialog.hpp"


#include <qtextedit.h>
#include <iostream>
#include <qpushbutton.h>


using namespace std;


MessageDialog::MessageDialog (const string& message,QWidget* parent, const char* name, bool modal, WFlags fl) : QtGeneratedMessageDialog(parent,name,modal,fl)
{
	
	teMessage->setText(message.c_str());

// I have fixed Message dialog, so it is using layouts now, and it is better, because you can resize it, and size of the content chages correctly
//	teMessage->adjustSize();
//	teMessage->move(10,10);
	
//	resize(teMessage->size().width()+20,teMessage->size().height()+30+pbOk->size().height());
//	pbOk->move((size().width()-pbOk->size().width())/2,teMessage->size().height()+20);

	QSize s1 = parent->size();
	QSize s2 = size();
	QSize s = s1-s2;
	reparent(parent,QPoint(s.width()/2,s.height()/2));	
	
	show();
	startTimer(10);
}


MessageDialog::~MessageDialog ()
{

}


void MessageDialog::pbOkClicked()
{
	emit close();
}
	

void MessageDialog::closeEvent(QCloseEvent * evt)
{
	QtGeneratedMessageDialog::closeEvent(evt);
}


void MessageDialog::timerEvent(QTimerEvent * /*evt*/)
{
	//cout << "timer" << endl;
}

