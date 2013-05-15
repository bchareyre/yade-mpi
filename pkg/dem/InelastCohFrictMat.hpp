#pragma once

#include "CohFrictMat.hpp"


class InelastCohFrictMat : public CohFrictMat
{
	public :
		virtual ~InelastCohFrictMat ();

/// Serialization
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InelastCohFrictMat,CohFrictMat,"",
		((Real,eTT,0.0,,"Tension elasticity modulus"))
		((Real,eTC,0.0,,"Compresion elasticity modulus"))
		((Real,eB,0.0,,"bending elasticity modulus"))
		((Real,gTw,0.0,,"twist elasticity modulus"))	
		((Real,sigmaB,0.0,,"max. Bending stress"))
		((Real,sigmaTw,0.0,,"max. Twist stress"))
		((Real,creepT,0.0,,"Tension/compression creeping"))
		((Real,creepB,0.0,,"bending creeping"))
		((Real,creepTw,0.0,,"twist creeping"))
		((Real,unloadT,0.0,,"Tension/compression plastic unload"))
		((Real,unloadB,0.0,,"bending plastic unload"))
		((Real,unloadTw,0.0,,"twist plastic unload"))
		((Real,disElT,0.0,,"max elastic displacement on Traction"))
		((Real,disElC,0.0,,"max elastic displacement on Compresion"))
		((Real,epsilonMaxT,0.0,,"Maximal plastic strain Tension"))
		((Real,epsilonMaxC,0.0,,"Maximal plastic strain compression"))
		((Real,phiBMax,0.0,,"Maximal plastic bending strain"))
		((Real,phiTwMax,0.0,,"Maximal plastic bending strain")),
		createIndex();			  
					);
/// Indexable
	REGISTER_CLASS_INDEX(InelastCohFrictMat,CohFrictMat);
};

REGISTER_SERIALIZABLE(InelastCohFrictMat);
