/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef FEM_SET_TEXT_LOADER_HPP
#define FEM_SET_TEXT_LOADER_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/PhysicalParametersEngineUnit.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/MetaBody.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class FEMSetTextLoader : public PhysicalParametersEngineUnit
{
	private 	: 	int nodeGroupMask, tetrahedronGroupMask;
	public  	:	string fileName; 

	public 		: 	virtual void go(	  const shared_ptr<PhysicalParameters>&
							, Body*);
	
	public 		: 	void createNode( 	  shared_ptr<Body>& body
							, Vector3r position
							, unsigned int id);
					
	public 		: 	void createTetrahedron(   const MetaBody* rootBody
							, shared_ptr<Body>& body
							, unsigned int id
							, unsigned int id1
							, unsigned int id2
							, unsigned int id3
							, unsigned int id4);

	protected: virtual void registerAttributes();	
	REGISTER_CLASS_NAME(FEMSetTextLoader);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(FEMSetTextLoader,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FEM_SET_TEXT_LOADER_HPP 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

