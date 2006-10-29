/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "LatticeBeamParameters.hpp"
#include <yade/yade-core/Omega.hpp>

LatticeBeamParameters::LatticeBeamParameters() : PhysicalParameters()
{
	createIndex();
	count = 0.0;
//	rotation = Vector3r(0,0,0);
//	bendingRotation = Quaternionr(1.0,0.0,0.0,0.0);
	bendingRotation = 0.0;
	lastIter_ = -1;
	nonLocalStrain = 0.0;
	nonLocalDivisor = 0.0;
}


LatticeBeamParameters::~LatticeBeamParameters()
{

}

void LatticeBeamParameters::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
/*		if(criticalTensileStrain > 0.00015) // E.l
		{ // CEMENT MATRIX
			bendingStiffness          = 0.7;	// k.b
			longitudalStiffness       = 1.0;	// k.l
			criticalCompressiveStrain = 1.0;	// E.c
			criticalTensileStrain     = 0.0002;	// E.l
		}
		else if(criticalTensileStrain > 0.00006) // E.l
		{ // AGGREGATE
			bendingStiffness          = 2.1;	// k.b
			longitudalStiffness       = 3.0;	// k.l
			criticalCompressiveStrain = 1.0;	// E.c
			criticalTensileStrain     = 0.0001333;	// E.l
		}
		else
		{ // BOND / INTERFACE
			bendingStiffness          = 0.7;	// k.b
			longitudalStiffness       = 1.0;	// k.l
			criticalCompressiveStrain = 1.0;	// E.c
			criticalTensileStrain     = 0.0002;	// E.l
		}
*/		lastIter_ = -10;


// checking

#define chk(x,y) std::cout << #y << ",      " << x << " = " << y << "\n"
		if(criticalTensileStrain > 0.00015) // E.l
		{ // CEMENT MATRIX
			static bool d1=true;
			if(d1)
			{
				d1=false;
				std::cout << "\nCEMENT:\n";
				chk("\t\tk.b",bendingStiffness);                // k.b
				chk("\tk.l",longitudalStiffness);               // k.l
				chk("E.c",criticalCompressiveStrain);           // E.c
				chk("\tE.l",criticalTensileStrain);             // E.l
			}
		}
		else if(criticalTensileStrain > 0.00006) // E.l
		{ // AGGREGATE
			static bool d2=true;
			if(d2)
			{
				d2=false;
				std::cout << "\nAGGREGATE:\n";
				chk("\t\tk.b",bendingStiffness);                // k.b
				chk("\tk.l",longitudalStiffness);               // k.l
				chk("E.c",criticalCompressiveStrain);           // E.c
				chk("\tE.l",criticalTensileStrain);             // E.l
			}
		}
		else
		{ // BOND / INTERFACE
			static bool d3=true;
			if(d3)
			{
				d3=false;
				std::cout << "\nBOND:\n";
				chk("\t\tk.b",bendingStiffness);                // k.b
				chk("\tk.l",longitudalStiffness);               // k.l
				chk("E.c",criticalCompressiveStrain);           // E.c
				chk("\tE.l",criticalTensileStrain);             // E.l
			}
		}
#undef chk




	//	bendingStiffness    *= 25000;
	//	longitudalStiffness *= 25000;
        }
}


Real LatticeBeamParameters::strain()
{
        long int it = Omega::instance().getCurrentIteration();
        if( lastIter_ == it )
                return strain_;
        else
        {
                lastIter_ = it;
                strain_ = (length - initialLength) / initialLength;
                return strain_;
        }
}

void LatticeBeamParameters::registerAttributes()
{
	PhysicalParameters::registerAttributes();
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	
	REGISTER_ATTRIBUTE(initialLength);
	REGISTER_ATTRIBUTE(length); // FIXME - can be calculated after deserialization
	
	REGISTER_ATTRIBUTE(initialDirection);
	REGISTER_ATTRIBUTE(direction); // FIXME - can be calculated after deserialization
	
	REGISTER_ATTRIBUTE(criticalTensileStrain);
	REGISTER_ATTRIBUTE(criticalCompressiveStrain);
	
	REGISTER_ATTRIBUTE(longitudalStiffness);
	REGISTER_ATTRIBUTE(bendingStiffness);

	REGISTER_ATTRIBUTE(se3Displacement);
}

