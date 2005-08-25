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

#include "PositionOrientationRecorder.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-package-common/RigidBodyParameters.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/lexical_cast.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PositionOrientationRecorder::PositionOrientationRecorder () : Engine()//, ofile("")
{
	outputFile = "positionorientation";
	interval = 50;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PositionOrientationRecorder::~PositionOrientationRecorder ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PositionOrientationRecorder::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
	//	ofile.open(outputFile.c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PositionOrientationRecorder::registerAttributes()
{
	Engine::registerAttributes();
	REGISTER_ATTRIBUTE(outputFile);
	REGISTER_ATTRIBUTE(interval);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PositionOrientationRecorder::action(Body * body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	
	if( Omega::instance().getCurrentIteration() % interval == 0 /*&& ofile*/ )
	{
		ofile.open( string(outputFile+"_"+lexical_cast<string>( Omega::instance().getCurrentIteration() )).c_str() );
	
		Real tx=0, ty=0, tz=0, rw=0, rx=0, ry=0, rz=0;
			
		BodyContainer::iterator bi    = ncb->bodies->begin();
		BodyContainer::iterator biEnd = ncb->bodies->end();
		for(  ; bi!=biEnd ; ++bi )
		{
			shared_ptr<Body> b = *bi;
			tx = b->physicalParameters->se3.position[0];
			ty = b->physicalParameters->se3.position[1];
			tz = b->physicalParameters->se3.position[2];
		
			rw = b->physicalParameters->se3.orientation[0];
			rx = b->physicalParameters->se3.orientation[1];
			ry = b->physicalParameters->se3.orientation[2];
			rz = b->physicalParameters->se3.orientation[3];
			
			ofile <<	lexical_cast<string>(tx) << " " 
					<< lexical_cast<string>(ty) << " " 
					<< lexical_cast<string>(tz) << " "
					<< lexical_cast<string>(rw) << " "
					<< lexical_cast<string>(rx) << " "
					<< lexical_cast<string>(ry) << " "
					<< lexical_cast<string>(rz) << endl;
		}
		ofile.close();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

