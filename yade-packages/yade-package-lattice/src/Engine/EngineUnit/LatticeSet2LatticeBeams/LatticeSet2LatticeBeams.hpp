/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_SET_2_LATTICE_BEAMS_HPP
#define LATTICE_SET_2_LATTICE_BEAMS_HPP 


#include <yade/yade-package-common/GeometricalModelEngineUnit.hpp>
#include <yade/yade-core/BodyContainer.hpp>


class LatticeSet2LatticeBeams : public GeometricalModelEngineUnit
{
	private :
		void calcBeamsPositionOrientationNewLength(shared_ptr<Body>& body, const shared_ptr<BodyContainer>& bodies);

	public : 
		void go(	  const shared_ptr<PhysicalParameters>&
				, shared_ptr<GeometricalModel>&
				, const Body*);

	REGISTER_CLASS_NAME(LatticeSet2LatticeBeams);
	REGISTER_BASE_CLASS_NAME(GeometricalModelEngineUnit);

};

REGISTER_SERIALIZABLE(LatticeSet2LatticeBeams,false);

#endif // LATTICE_SET_2_LATTICE_BEAMS_HPP 

