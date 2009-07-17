/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "QtGUIGenerator.hpp"
#include <sstream>
#include <boost/any.hpp>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>

using namespace boost;

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


QtGUIGenerator::~QtGUIGenerator()
{

}


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

std::string sanitize(const std::string num)
{
	std::string ret="";
	int len=num.size();
	if(len < 7 )
		return num;
	// is that a number?
	try
	{
		boost::lexical_cast<Real>(num);
	}
	catch(std::bad_cast&)
	{
		return num;
	}
	// case .....000001
	if(num[len-2] == '0' && num[len-3] == '0')
	{
		for(int pos=len-4 ; pos>=0 ; --pos)
		{
			if(ret.size() != 0 || num[pos] != '0' || num[pos-1] == '.')
			{
				ret = num[pos] + ret;
			}
		}
	}
	else 
	{// case ....9999998
		if(num[len-2] == '9' && num[len-3] == '9')
		{
			for(int pos=len-4 ; pos>=0 ; --pos)
			{
				if(ret.size() != 0 || num[pos] != '9')
				{
					if(ret.size() == 0)
					{
						ret = (unsigned char)((int)(num[pos])+1);
					}
					else
					{
						ret = num[pos] + ret;
					}
				}
			}
		}
		else
		{// exponential part e-..
			if(num[len-3] == '-' && num[len-4] == 'e')
			{
				std::string exp =std::string("e-")+num[len-2]+num[len-1];
				std::string mant="";
				for(int pos=len-5 ; pos>=0 ; --pos)
				{
					mant = num[pos] + mant;
				}
				ret=sanitize(mant)+exp;
			}
			else
			{// exponential part e..
				if(num[len-3] == 'e')
				{
					std::string exp =std::string("e")+num[len-2]+num[len-1];
					std::string mant="";
					for(int pos=len-4 ; pos>=0 ; --pos)
					{
						mant = num[pos] + mant;
					}
					ret=sanitize(mant)+exp;
				}
				else
				{
					return num;
				}
			}
		}
	}
	try
	{
		Real a = boost::lexical_cast<Real>(ret);
		Real b = boost::lexical_cast<Real>(num);
		if(a-b == 0) // the sanitized number and original number must be exactly the same
		{
			return ret;
		}
		else
		{
			std::cerr << "INFO: sanitize failed: " << a << " != " << b << "\n";
			return num; // return original numer, since they are different
		}
	}
	catch(std::bad_cast&)
	{
		std::cerr << "INFO: sanitize failed: " << ret << " != " << num << "\n";
		return num; // oops, the number got corrupted somehow
	}
};


void QtGUIGenerator::buildGUI(shared_ptr<Serializable> s,  QWidget * widget)
{

	currentWidget = widget;

	XMLFormatManager xmlManager; // FIXME - beacuse all IOFormatManagers are using pointers to static functions it is all a mess...

	descriptors.clear();
	lookUp.clear();

	serializable = s;

	if (serializable->getArchives().empty()) // attributes are not registered
		serializable->registerSerializableAttributes(false);

	Serializable::Archives archives = serializable->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for( ; ai!=aiEnd ; ++ai)
	{
		if (       (*ai)->isFundamental()
		        // FIXME ...
			// oh well. It is already decided that we will use boost::serialization. And this hack only proves that
			// current system is broken. I am so tired of NOT loading the filename from generator save files, 
			// that I had to add this....
		        && (*ai)->getName() != "outputFileName"
			&& (*ai)->getName() != "serializationDynlib" )
		{
			shared_ptr<AttributeDescriptor> descriptor(new AttributeDescriptor);
			
			descriptor->name = (*ai)->getName();
			
			stringstream stream;
			(*ai)->serialize(stream,*(*ai),0);
			IOFormatManager::parseFundamental(stream.str(), descriptor->strings);
				
			descriptor->label = new QLabel( widget);
			descriptor->label->setText(descriptor->name); // was: +" : "
			
			unsigned int nbStrings = descriptor->strings.size();
			for(unsigned int i=0;i<nbStrings;i++)
			{
				any instance = (*ai)->getAddress();
				if(bool ** b = any_cast<bool*>(&instance))
				{
					QCheckBox * cb = new QCheckBox(widget);
					cb->setChecked(**b);
					descriptor->widgets.push_back(cb);
					descriptor->types.push_back(AttributeDescriptor::BOOLEAN);
				}
				else
				{
					QLineEdit* le = new QLineEdit(widget);
					le->setText(sanitize(descriptor->strings[i]));
					descriptor->widgets.push_back(le);
					descriptor->types.push_back(AttributeDescriptor::FLOATING);
				}
			}
		
			// not possible to store descriptor into a map or set ?????!!! I don't know why but it is crashing, so I use a vector and lookup table
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
	
	//s->unregisterSerializableAttributes(false);

}


void QtGUIGenerator::deserialize(shared_ptr<Serializable> s)
{
	this->deserialize(s.get());
};

void QtGUIGenerator::deserialize(Serializable* s)
{
	XMLFormatManager xmlManager; // FIXME - beacuse all IOFormatManagers are using pointers to static functions it is all a mess...
	
	s->registerSerializableAttributes(true);
	Serializable::Archives archives = s->getArchives();

	Serializable::Archives::iterator ai    = archives.begin();
	Serializable::Archives::iterator aiEnd = archives.end();
	for(; ai!=aiEnd ; ++ai)
	{
		if (       (*ai)->isFundamental()
		        // FIXME ...
			// oh well. It is already decided that we will use boost::serialization. And this hack only proves that
			// current system is broken. I am so tired of NOT loading the filename from generator save files, 
			// that I had to add this....
		        && (*ai)->getName() != "outputFileName"
			&& (*ai)->getName() != "serializationDynlib" )
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
	//s->unregisterSerializableAttributes(true);

}


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


void QtGUIGenerator::pushButtonOkClicked()
{
	deserialize(serializable);
	currentWidget->close();
}


void QtGUIGenerator::pushButtonApplyClicked()
{
	deserialize(serializable);
}


void QtGUIGenerator::pushButtonCancelClicked()
{
	currentWidget->close();
}

