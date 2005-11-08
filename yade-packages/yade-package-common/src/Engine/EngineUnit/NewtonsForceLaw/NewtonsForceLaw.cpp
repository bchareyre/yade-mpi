/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "NewtonsForceLaw.hpp"
#include "ParticleParameters.hpp"
#include "Force.hpp"


void NewtonsForceLaw::go( const shared_ptr<PhysicalAction>& a
			, const shared_ptr<PhysicalParameters>& b
			, const Body* bb)
{
	Force * af = dynamic_cast<Force*>(a.get());
	ParticleParameters * p = dynamic_cast<ParticleParameters*>(b.get());
	
	//FIXME : should be += and we should add an Engine that reset acceleration at the beginning
	// if another PhysicalAction also acts on acceleration then we are overwritting it here
	
//	std::cout << bb->getId() << std::endl;
//	std::cout << a->getClassIndex() << " " << a->getClassName() << std::endl;
//	std::string ch;
//	std::cin >> ch;
	
	p->acceleration = p->invMass*af->force;
}


