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

#include "QtGUIGenerator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <qlabel.h>
#include <qpushbutton.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUIGenerator::QtGUIGenerator () : XMLManager(), QtGUISignalCatcher()
{

	resizeHeight = true;
	resizeWidth  = true;
	translationX = 0;
	translationY = 0;
	shiftX       = 0;
	shiftY       = 0;
	showButtons  = true;
	
	buttonWidth  = 70;
	buttonHeight = 30;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUIGenerator::~QtGUIGenerator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::buildGUI(shared_ptr<Serializable> s,  QWidget * widget)
{
cerr << 1 << endl;
	serializable = s;
cerr << 2 << endl;
	serializable->registerAttributes();
cerr << 3 << endl;
	widget->setCaption(serializable->getClassName().c_str());
cerr << 4 << endl;
	Serializable::Archives archives = serializable->getArchives();
cerr << 5 << endl;
	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	int nbLines=0;
	for( ; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			QLabel *label = new QLabel( widget, (*ai)->getName().c_str() );
			label->setText((*ai)->getName().c_str());
			label->setGeometry( QRect( shiftX, shiftY+(20+5)*nbLines, 150, 20 ) );

			stringstream stream;
			(*ai)->serialize(stream,*(*ai),0);
			vector<string> tokens;
			IOManager::parseFundamental(stream.str(), tokens);
			if (tokens.size()==0) // FIXME : parseFundamental is not working if the string contain only 1 value
				tokens.push_back(stream.str());
				
			shared_ptr<AttributeDescriptor> descriptor(new AttributeDescriptor);
			
			descriptor->name = (*ai)->getName();
			descriptor->lineEdits.clear();
			
			unsigned int nbElements = tokens.size();
			
			for(unsigned int i=0;i<nbElements;i++)
			{
				QLineEdit* le = new QLineEdit(widget, (*ai)->getName().c_str());
				le->setText(tokens[i]);
				descriptor->lineEdits.push_back(le);
			}
			
			unsigned int size = (100-(nbElements-1)*5)/nbElements;
			for(unsigned int i=0;i<nbElements;i++)
			{
				descriptor->lineEdits[i]->setGeometry( QRect( shiftX+150+(size+5)*i, shiftY+(20+5)*nbLines, size, 20 ) );
			}
			
			// not possible to store descriptor into a map or set ?????!!!
			descriptors.push_back(descriptor);
			lookUp[descriptor->name] = descriptors.size()-1;
			
			nbLines++;
		}
	}

	
	int width;
	if (resizeWidth)
		width = 260+20;
	else
		width = widget->size().width();

	if (showButtons)
	{		
		QPushButton *ok    = new QPushButton( widget, "OK");
		QPushButton *apply = new QPushButton( widget, "APPLY");
		QPushButton *cancel = new QPushButton( widget, "CANCEL");
		ok->setGeometry( QRect( (width-3*buttonWidth-2*5)/2, (20+5)*nbLines+40, buttonWidth, buttonHeight ) );
		cancel->setGeometry( QRect( (width-3*buttonWidth-2*5)/2+buttonWidth+5, (20+5)*nbLines+40, buttonWidth, buttonHeight ) );
		apply->setGeometry( QRect( (width-3*buttonWidth-2*5)/2+(buttonWidth+5)*2, (20+5)*nbLines+40, buttonWidth, buttonHeight ) );
		ok->setText("OK");
		apply->setText("Apply");
		cancel->setText("Cancel");
		connect( ok, SIGNAL( clicked() ), this, SLOT( pushButtonOkClicked() ) );
		connect( apply, SIGNAL( clicked() ), this, SLOT( pushButtonApplyClicked() ) );
		connect( cancel, SIGNAL( clicked() ), this, SLOT( pushButtonCancelClicked() ) );
	}
	
	QSize newSize = widget->size();
	
	if (resizeWidth)
		newSize.setWidth(width);
	if (resizeHeight)
		if (showButtons)
			newSize.setHeight(nbLines*(20+5)+50+buttonHeight);
		else
			newSize.setHeight(nbLines*(20+5)+40);
	
	widget->resize(newSize);
	widget->setMinimumSize(newSize);
	widget->setMaximumSize(newSize);
	widget->setEnabled(true);
	
	QPoint p = widget->pos();
	widget->move(p.x()+translationX,p.y()+translationY);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::pushButtonOkClicked()
{
	serializable->registerAttributes();
	Serializable::Archives archives = serializable->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for(; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			string str;
			int i = lookUp[(*ai)->getName()];
			int nbLineEdit = descriptors[i]->lineEdits.size();
			
			if (nbLineEdit==1)
				str = descriptors[i]->lineEdits[0]->text().data();
			else
			{
				str="{";
				for(int j=0;j<nbLineEdit;j++)
				{
					str+= descriptors[i]->lineEdits[j]->text().data();
					str+=" ";
				}
				str[str.size()-1]='}';
				
			}
			cout << str << endl;
			stringstream voidStream;
		
			(*ai)->deserialize(voidStream,*(*ai),str);
		}
	}

	//ac.markProcessed();
	serializable->unregisterSerializableAttributes(true);
	cout << "pushButtonOkClicked" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::pushButtonApplyClicked()
{
	cout << "pushButtonApplyClicked" << endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::pushButtonCancelClicked()
{
	cout << "pushButtonCancelClicked" << endl;
}
