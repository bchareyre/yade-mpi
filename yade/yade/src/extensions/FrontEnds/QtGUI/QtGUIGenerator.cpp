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
#include <qpushbutton.h>
#include <boost/any.hpp>
#include <qlineedit.h>
#include <qcheckbox.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUIGenerator::QtGUIGenerator () : QObject()
{

	resizeHeight = true;
	resizeWidth  = true;
	showButtons  = true;
		
	translationX = 0;
	translationY = 0;
	shiftX       = 0;
	shiftY       = 0;
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

void QtGUIGenerator::addButtons(QWidget * widget)
{		
	QPushButton *ok    = new QPushButton( widget, "OK");
	QPushButton *apply = new QPushButton( widget, "APPLY");
	QPushButton *cancel = new QPushButton( widget, "CANCEL");
	
	int nbFundamentals = descriptors.size();
		
	ok->setGeometry( QRect( (widgetWidth-3*buttonWidth-2*5)/2, (20+5)*nbFundamentals+40, buttonWidth, buttonHeight ) );
	cancel->setGeometry( QRect( (widgetWidth-3*buttonWidth-2*5)/2+buttonWidth+5, (20+5)*nbFundamentals+40, buttonWidth, buttonHeight ) );
	apply->setGeometry( QRect( (widgetWidth-3*buttonWidth-2*5)/2+(buttonWidth+5)*2, (20+5)*nbFundamentals+40, buttonWidth, buttonHeight ) );

	ok->setText("OK");
	apply->setText("Apply");
	cancel->setText("Cancel");
	
	connect( ok, SIGNAL( clicked() ), this, SLOT( pushButtonOkClicked() ) );
	connect( apply, SIGNAL( clicked() ), this, SLOT( pushButtonApplyClicked() ) );
	connect( cancel, SIGNAL( clicked() ), this, SLOT( pushButtonCancelClicked() ) );
	
	widgetHeight += 10+buttonHeight;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::reArrange(QWidget * widget)
{	
	
	if (resizeWidth)
		widgetWidth = 400;
	else
		widgetWidth = widget->size().width();

	int maxLabelLength = 0;
	for(unsigned int i=0;i<descriptors.size();i++)
	{
		descriptors[i]->label->adjustSize();
		int length = descriptors[i]->label->size().width();
		if (length>maxLabelLength)
			maxLabelLength = length;
	}
	
	for(unsigned int i=0;i<descriptors.size();i++)
	{
		descriptors[i]->label->setGeometry( QRect( shiftX, shiftY+(20+5)*i, maxLabelLength, 20 ) );
		descriptors[i]->label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
		unsigned int nbStrings = descriptors[i]->strings.size();
		unsigned int leWidth = (widgetWidth-maxLabelLength-shiftX-10-10-(nbStrings-1)*5)/nbStrings;	
		for(unsigned int j=0;j<nbStrings;j++)
			descriptors[i]->widgets[j]->setGeometry( QRect( shiftX+maxLabelLength+10+(leWidth+5)*j, shiftY+(20+5)*i, leWidth, 20 ) );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::buildGUI(shared_ptr<Serializable> s,  QWidget * widget)
{

	XMLManager xmlManager;
	descriptors.clear();
	lookUp.clear();

	serializable = s;

	serializable->registerAttributes();

	Serializable::Archives archives = serializable->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			shared_ptr<AttributeDescriptor> descriptor(new AttributeDescriptor);
			
			descriptor->name = (*ai)->getName();
			
			stringstream stream;
			(*ai)->serialize(stream,*(*ai),0);
			IOManager::parseFundamental(stream.str(), descriptor->strings);
				
			descriptor->label = new QLabel( widget);
			descriptor->label->setText(descriptor->name+" : ");
			
			unsigned int nbStrings = descriptor->strings.size();
			for(unsigned int i=0;i<nbStrings;i++)
			{
				any instance = (*ai)->getAddress();
				try
				{
					bool * b = any_cast<bool*>(instance);
					QCheckBox * cb = new QCheckBox(widget);
					cb->setChecked(*b);
					descriptor->widgets.push_back(cb);
					descriptor->types.push_back(AttributeDescriptor::BOOLEAN);
				}
				catch(...)
				{
					QLineEdit* le = new QLineEdit(widget);
					le->setText(descriptor->strings[i]);
					descriptor->widgets.push_back(le);
					descriptor->types.push_back(AttributeDescriptor::FLOATING);
				}
			}
		
			// not possible to store descriptor into a map or set ?????!!!
			descriptors.push_back(descriptor);
			lookUp[descriptor->name] = descriptors.size()-1;
		}
	}

	reArrange(widget);
	
	if (resizeHeight)
		widgetHeight = descriptors.size()*(20+5)+40;
	else
		widgetHeight = widget->size().height();
	
	
	if (showButtons)
		addButtons(widget);
	
	QSize newSize;
	
	newSize.setWidth(widgetWidth);
	newSize.setHeight(widgetHeight);
	
	widget->resize(newSize);
	widget->setMinimumSize(newSize);
	widget->setMaximumSize(newSize);
	
	widget->setEnabled(true);
	
	QPoint p = widget->pos();
	widget->move(p.x()+translationX,p.y()+translationY);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::deserialize(shared_ptr<Serializable> s)
{
	s->registerAttributes();
	Serializable::Archives archives = s->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for(; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			string str;
			int i = lookUp[(*ai)->getName()];
			
			int nbWidget = descriptors[i]->widgets.size();
					
			if (nbWidget==1)
				str = getString(descriptors[i],0);
			else
			{
				str="{";
				for(int j=0;j<nbWidget;j++)
				{
					str+= getString(descriptors[i],j);
					str+=" ";
				}
				str[str.size()-1]='}';
				
			}
			
			stringstream voidStream;
		
			(*ai)->deserialize(voidStream,*(*ai),str);
		}
	}

	//ac.markProcessed();
	s->unregisterSerializableAttributes(true);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string QtGUIGenerator::getString(shared_ptr<AttributeDescriptor> d, int widgetNum)
{
	switch (d->types[widgetNum])
	{
		case AttributeDescriptor::FLOATING : 
		case AttributeDescriptor::INTEGER : 
		{
			
			return dynamic_cast<QLineEdit*>(d->widgets[widgetNum])->text().data();
			
		}
		break;
		case AttributeDescriptor::BOOLEAN :
		{
			if (dynamic_cast<QCheckBox*>(d->widgets[widgetNum])->isChecked())
				return string("1");
			else
				return string("0");
		}
		break;
		default	: return "";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::pushButtonOkClicked()
{
	deserialize(serializable);
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
