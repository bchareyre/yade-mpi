/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
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

#ifndef __MATH_H__
#define __MATH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Math
{

	// Return -1 if the input is negative, 0 if the input is zero, and +1 if the input is positive.
	public : static int sign (int iValue);
	public : static float sign (float fValue);

	// Generate a random number in [0,1).
	// The random number generator may be seeded by a first call to UnitRandom with a positive seed.
	public : static float unitRandom (float fSeed = 0.0f);

	// Generate a random number in [-1,1).
	// The random number generator may be seeded by a first call to SymmetricRandom with a positive seed.
	public : static float symmetricRandom (float fSeed = 0.0f);

	// Generate a random number in [min,max).
	// The random number generator may be seeded by a first call to IntervalRandom with a positive seed.
	public : static float intervalRandom (float fMin, float fMax, float fSeed = 0.0f);

	// common constants
	public : static const float MAX_FLOAT;
	public : static const float PI;
	public : static const float TWO_PI;
	public : static const float HALF_PI;
	public : static const float INV_TWO_PI;
	public : static const float DEG_TO_RAD;
	public : static const float RAD_TO_DEG;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Math.ipp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __MATH_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

