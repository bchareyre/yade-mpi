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

#ifndef FEM_NODE_PARAMETERS_HPP 
#define FEM_NODE_PARAMETERS_HPP 

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-common/ParticleParameters.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class FEMNodeData : public ParticleParameters
{
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes 											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	public: Vector3r initialPosition; // mayber this should be stored in FEMTetrahedron? (but then this data will be stored multiple times, since each tetrahedron will copy the same coordinades for each of its nodes...)
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods 											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : FEMNodeData();
	public : virtual ~FEMNodeData();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(FEMNodeData);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(FEMNodeData,ParticleParameters);

};

REGISTER_SERIALIZABLE(FEMNodeData,false);

#endif // __LATTICENODEPARAMETERS_HPP__


