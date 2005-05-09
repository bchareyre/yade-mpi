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

#ifndef __QTGUIGENERATOR_HPP__
#define __QTGUIGENERATOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <qframe.h>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <qobject.h>
#include <qlabel.h>
#include <qobject.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief 

	
*/
class QtGUIGenerator : public QObject
{

	Q_OBJECT

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	class AttributeDescriptor
	{
		public : typedef enum {INTEGER,FLOATING,BOOLEAN} AttributeType;
		
		public : AttributeDescriptor() { strings.clear();widgets.clear();types.clear();};
		public : ~AttributeDescriptor()
		{

			//if (label)
			//	label->~QLabel();
			//for(unsigned int i=0;i<widgets.size();i++)
			//	if (widgets[i])
			//		delete widgets[i];
			//strings.clear();
			//types.clear();
			//widgets.clear();
		}
		public : string name;
		public : vector<string> strings;
		public : QLabel* label;
		public : vector<AttributeType> types;
		//public : vector<QLineEdit*> lineEdits;
		public : vector<QWidget*> widgets;
	};
	
	public : vector<shared_ptr<AttributeDescriptor> > descriptors;
	public : map<string,int> lookUp;
	public : shared_ptr<Serializable> serializable;	

	private : bool resizeHeight;
	private : bool resizeWidth;
	private : int translationX;
	private : int translationY;
	private : int shiftX;
	private : int shiftY;
	private : bool showButtons;
	public : void setShowButtons(bool b) { showButtons=b;};
	public : void setResizeHeight(bool b) { resizeHeight=b;};
	public : void setResizeWidth(bool b) { resizeWidth=b;};
	public : void setTranslation(int x, int y) { translationX=x; translationY=y; };
	public : void setShift(int x, int y) {shiftX=x;shiftY=y;};
	
	private : string getString(shared_ptr<AttributeDescriptor> d, int widgetNum);
	private : int buttonWidth;
	private : int buttonHeight;
	private : int widgetWidth;
	private : int widgetHeight;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*! Constructor */
	public : QtGUIGenerator ();

	/*! Destructor */
	public : virtual ~QtGUIGenerator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : void buildGUI(shared_ptr<Serializable> s, QWidget * widget);
	public : void deserialize(shared_ptr<Serializable> s);
	
	private : void reArrange(QWidget * widget);
	private : void addButtons(QWidget * widget);
	
	public slots : virtual void pushButtonOkClicked() ;
	public slots : virtual void pushButtonApplyClicked() ;
	public slots : virtual void pushButtonCancelClicked() ;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __QTGUIGENERATOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

