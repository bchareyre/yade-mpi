/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization-xml/XMLFormatManager.hpp>
#include <map>
#include <vector>
#include <qobject.h>
#include <qlabel.h>
#include <qobject.h>
#include <qframe.h>

using namespace std;
using namespace boost;

class QtGUIGenerator : public QObject
{
	Q_OBJECT
	
	private :

		struct AttributeDescriptor
		{
				typedef enum {INTEGER,FLOATING,BOOLEAN} AttributeType;
			
				AttributeDescriptor() { strings.clear();widgets.clear();types.clear();};
				virtual ~AttributeDescriptor()	{ }

				string			name;
				vector<string>		strings;
				QLabel*			label;
				vector<AttributeType>	types;
				vector<QWidget*>	widgets;
			//	vector<QLineEdit*>	lineEdits;
		};
		bool		 resizeHeight
				,resizeWidth
				,showButtons;

		int		 translationX
				,translationY
				,shiftX
				,shiftY
				,buttonWidth
				,buttonHeight
				,widgetWidth
				,widgetHeight;

		QWidget *	currentWidget;

		string getString(shared_ptr<AttributeDescriptor> d, int widgetNum);
		void reArrange(QWidget * widget);
		void addButtons(QWidget * widget);
	
	public :
		vector<shared_ptr<AttributeDescriptor> > descriptors;
		map<string,int> lookUp;
		shared_ptr<Serializable> serializable;	

		void setShowButtons(bool b) { showButtons=b;};
		void setResizeHeight(bool b) { resizeHeight=b;};
		void setResizeWidth(bool b) { resizeWidth=b;};
		void setTranslation(int x, int y) { translationX=x; translationY=y; };
		void setShift(int x, int y) {shiftX=x;shiftY=y;};
	
		QtGUIGenerator ();
		virtual ~QtGUIGenerator ();

		void buildGUI(shared_ptr<Serializable> s, QWidget * widget);
		void deserialize(shared_ptr<Serializable> s);
		void deserialize(Serializable* s);
	
	public slots :
		virtual void pushButtonOkClicked() ;
		virtual void pushButtonApplyClicked() ;
		virtual void pushButtonCancelClicked() ;
};


