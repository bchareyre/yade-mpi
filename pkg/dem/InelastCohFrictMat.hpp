/*************************************************************************
*  Copyright (C) 2012 by Ignacio Olmedo nolmedo.manich@gmail.com         *
*  Copyright (C) 2012 by François Kneib   francois.kneib@gmail.com       *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#pragma once

#include "../common/ElastMat.hpp"

class InelastCohFrictMat : public FrictMat
{
	public :
		virtual ~InelastCohFrictMat ();

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InelastCohFrictMat,FrictMat,"",
		((Real,tensionModulus,0.0,,"Tension elasticity modulus"))
		((Real,compressionModulus,0.0,,"Compresion elasticity modulus"))
		((Real,shearModulus,0.0,,"shear elasticity modulus"))
		((Real,alphaKr,2.0,,"Dimensionless coefficient used for the rolling stiffness."))
		((Real,alphaKtw,2.0,,"Dimensionless coefficient used for the twist stiffness."))

		((Real,nuBending,0.0,,"Bending elastic stress limit"))
		((Real,nuTwist,0.0,,"Twist elastic stress limit"))
		((Real,sigmaTension,0.0,,"Tension elastic stress limit"))
		((Real,sigmaCompression,0.0,,"Compression elastic stress limit"))
		((Real,shearCohesion,0.0,,"Shear elastic stress limit"))
		
		((Real,creepTension,0.0,,"Tension/compression creeping coefficient. Usual values between 0 and 1."))
		((Real,creepBending,0.0,,"Bending creeping coefficient. Usual values between 0 and 1."))
		((Real,creepTwist,0.0,,"Twist creeping coefficient. Usual values between 0 and 1."))
		
		((Real,unloadTension,0.0,,"Tension/compression plastic unload coefficient. Usual values between 0 and +infinity."))
		((Real,unloadBending,0.0,,"Bending plastic unload coefficient. Usual values between 0 and +infinity."))
		((Real,unloadTwist,0.0,,"Twist plastic unload coefficient. Usual values between 0 and +infinity."))
		
		((Real,epsilonMaxTension,0.0,,"Maximal plastic strain tension"))
		((Real,epsilonMaxCompression,0.0,,"Maximal plastic strain compression"))
		((Real,etaMaxBending,0.0,,"Maximal plastic bending strain"))
		((Real,etaMaxTwist,0.0,,"Maximal plastic twist strain")),
		createIndex();			  
					);
/// Indexable
	REGISTER_CLASS_INDEX(InelastCohFrictMat,FrictMat);
};

REGISTER_SERIALIZABLE(InelastCohFrictMat);
