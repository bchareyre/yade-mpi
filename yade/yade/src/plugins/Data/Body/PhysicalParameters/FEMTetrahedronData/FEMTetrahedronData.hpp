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

#ifndef __LATTICEBEAMPARAMETERS_HPP__
#define __LATTICEBEAMPARAMETERS_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/MetaBody.hpp>
#include <yade-common/RigidBodyParameters.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric;
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
	
// FIXME - remove Se3 - FEMTetrahedron DOES NOT need Se3 !
	
class FEMTetrahedronData : public PhysicalParameters
{
	public  : std::vector<unsigned int> 	ids; // FIXME - stupid serialization is not recognizing array: unsigned int ids[4]
	public  : ublas::matrix<Real>	Ke_; // FIXME - this is actually interaction property
	private : Real damping, mass;
	private : void localCalcKeMatrix( ublas::matrix<Real>& nodesCoordinates );
	
	public  : void calcKeMatrix(MetaBody* femBody);
	
	
	public  : FEMTetrahedronData();
	public  : virtual ~FEMTetrahedronData();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(FEMTetrahedronData);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(FEMTetrahedronData,RigidBodyParameters);

};

REGISTER_SERIALIZABLE(FEMTetrahedronData,false);

#endif // __LATTICEBEAMPARAMETERS_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

