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
#include <vector>

class LatticeBeamParameters : public PhysicalParameters 
{
        private:
                long int                 lastIter_;
                Real                     strain_;
        public :
                unsigned int             id1
                                        ,id2;
				
		Real  			 initialLength
                                        ,length;
                                
                Vector3r                 initialDirection
                                        ,direction; // is a unit vector
                                
                Real                     criticalTensileStrain
                                        ,criticalCompressiveStrain
                                
                                        ,longitudalStiffness
                                        ,bendingStiffness;
                                
                                
                Se3r                     se3Displacement;
                
		Real 			 count;
	//	Vector3r 		 rotation;
        //      Quaternionr              bendingRotation;
                Real                     bendingRotation;
                
                Real strain();
                Real                     nonLocalStrain
                                        ,nonLocalDivisor;
        
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

