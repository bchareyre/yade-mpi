/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_BEAM_PARAMETERS_HPP
#define LATTICE_BEAM_PARAMETERS_HPP 

#include <yade/yade-core/PhysicalParameters.hpp>

class LatticeBeamParameters : public PhysicalParameters 
{
	public :
		unsigned int 	 id1
				,id2;
				
		Real  		 initialLength
				,length;
				
		Vector3r 	 initialDirection
				,direction;
				
		Real 		 criticalTensileStrain
				,criticalCompressiveStrain
				
				,longitudalStiffness
				,bendingStiffness
				
				,strain;
				
		Se3r 		 previousSe3;
		
		void calcStrain();
	
		LatticeBeamParameters();
		virtual ~LatticeBeamParameters();

/// Serialization
	protected :
		virtual void postProcessAttributes(bool);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(LatticeBeamParameters);
	REGISTER_BASE_CLASS_NAME(PhysicalParameters);
	
/// Indexable
	REGISTER_CLASS_INDEX(LatticeBeamParameters,PhysicalParameters);

};

REGISTER_SERIALIZABLE(LatticeBeamParameters,false);

#endif // LATTICE_BEAM_PARAMETERS_HPP 

