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
#include "IOManager.hpp"
#include "XMLManager.hpp"
#include <sstream>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

QtGUIGenerator::QtGUIGenerator ()
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

	// FIXME : following line means that Serialization MUST be cleaned-up, finished, etc. Because with this, Serialization is simply a junk of rubbish and mess!!
	XMLManager xmlManager;


	s->registerAttributes();

	frame->setCaption(s->getClassName().c_str());

	Serializable::Archives archives = s->getArchives();


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

			vector<QLineEdit *> lineEdits;
			stringstream stream;
			(*ai)->serialize(stream,*(*ai),0);
			string str;

			while (!stream.eof())
			{
				lineEdits.push_back(new QLineEdit(frame, (*ai)->getName().c_str() ));
				stream >> str;
				lineEdits.back()->setText(str);
			}
			for(unsigned int i=0;i<lineEdits.size();i++)
			{
				int size = (100-(lineEdits.size()-1)*5)/lineEdits.size();
				lineEdits[i]->setGeometry( QRect( 150+(size+5)*i, (20+5)*nbLines+10, size, 20 ) );
			}
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

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
