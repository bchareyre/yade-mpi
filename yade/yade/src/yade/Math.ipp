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
 
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdlib.h>

#ifdef WIN32
	#include <limits.h>
	#include <float.h>
#else
	#include <values.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline int Math::sign (int i)
{
    if ( i > 0 )
        return 1;

    if ( i < 0 )
        return -1;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float Math::sign (float f)
{
    if ( f > 0.0f )
        return 1.0f;

    if ( f < 0.0f )
        return -1.0f;

    return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float Math::unitRandom (float seed)
{
    if ( seed > 0.0f )
        srand((unsigned int)seed);

     return float(rand())/float(RAND_MAX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float Math::symmetricRandom (float seed)
{
    if ( seed > 0.0f )
        srand((unsigned int)seed);

    return 2.0f*float(rand())/float(RAND_MAX) - 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline float Math::intervalRandom (float min, float max, float seed)
{
    if ( seed > 0.0f )
        srand((unsigned int)seed);

    return min + (max-min)*float(rand())/float(RAND_MAX);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
