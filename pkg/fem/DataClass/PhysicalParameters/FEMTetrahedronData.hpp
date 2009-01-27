/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <vector>

using namespace boost::numeric;
	
// FIXME - remove Se3 - FEMTetrahedron DOES NOT need Se3 !
	
class FEMTetrahedronData : public PhysicalParameters
{
	private :
		Real	 damping
			,mass;

		void localCalcKeMatrix( ublas::matrix<Real>& nodesCoordinates );

	public :
		std::vector<unsigned int> 	ids; // FIXME - stupid serialization is not recognizing array: unsigned int ids[4]
		ublas::matrix<Real>	Ke_; // FIXME - this is actually interaction property
	
		void calcKeMatrix(MetaBody* femBody);
	
		FEMTetrahedronData();
		virtual ~FEMTetrahedronData();

/// Serialization
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(FEMTetrahedronData);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);
	
/// Indexable
	REGISTER_CLASS_INDEX(FEMTetrahedronData,RigidBodyParameters);

};

REGISTER_SERIALIZABLE(FEMTetrahedronData);


