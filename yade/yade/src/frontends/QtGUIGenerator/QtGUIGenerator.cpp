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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUIGenerator::~QtGUIGenerator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void QtGUIGenerator::buildGUI(shared_ptr<Serializable> s, QWidget * /* parent ( unused )*/, QFrame* frame)
{

	serializable = s;
	
	serializable->registerAttributes();

	frame->setCaption(serializable->getClassName().c_str());

	Serializable::Archives archives = serializable->getArchives();


	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	int nbLines=0;
	for( ; ai!=aiEnd ; ++ai)
	{
		if ((*ai)->isFundamental())
		{
			QLabel *label = new QLabel( frame, (*ai)->getName().c_str() );
			label->setText((*ai)->getName().c_str());
			label->setGeometry( QRect( 10, (20+5)*nbLines+10, 150, 20 ) );

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
				QLineEdit* le = new QLineEdit(frame, (*ai)->getName().c_str());
				le->setText(tokens[i]);
				descriptor->lineEdits.push_back(le);
			}
			
			unsigned int size = (100-(nbElements-1)*5)/nbElements;
			for(unsigned int i=0;i<nbElements;i++)
			{
				descriptor->lineEdits[i]->setGeometry( QRect( 150+(size+5)*i, (20+5)*nbLines+10, size, 20 ) );
			}
			
			// not possible to store descriptor into a map or set ?????!!!
			descriptors.push_back(descriptor);
			lookUp[descriptor->name] = descriptors.size()-1;
			
			nbLines++;
		}
	}

	QPushButton *ok    = new QPushButton( frame, "OK");
	QPushButton *apply = new QPushButton( frame, "APPLY");
	QPushButton *cancel = new QPushButton( frame, "CANCEL");
	ok->setText("OK");
	apply->setText("Apply");
	cancel->setText("Cancel");
	int width = 260+20;

	ok->setGeometry( QRect( (width-3*70-2*5)/2, (20+5)*nbLines+30, 70, 30 ) );
	cancel->setGeometry( QRect( (width-3*70-2*5)/2+75, (20+5)*nbLines+30, 70, 30 ) );
	apply->setGeometry( QRect( (width-3*70-2*5)/2+75+75, (20+5)*nbLines+30, 70, 30 ) );

	QSize size(width,nbLines*(20+5)+20+50);
	frame->resize(size);
	frame->setMinimumSize(size);
	frame->setMaximumSize(size);
	frame->setEnabled(true);
	frame->move(10,10);
	
	//catcher = new QtSignalCatcher();
	connect( ok, SIGNAL( clicked() ), this, SLOT( pushButtonOkClicked() ) );
	connect( apply, SIGNAL( clicked() ), this, SLOT( pushButtonApplyClicked() ) );
	connect( cancel, SIGNAL( clicked() ), this, SLOT( pushButtonCancelClicked() ) );

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
