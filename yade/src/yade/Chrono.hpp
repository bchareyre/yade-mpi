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

#ifndef __CHRONO_H__
#define __CHRONO_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Visual C++ : D�init _Windows
#if defined(_MSC_VER)
	#define _Windows
#endif

#include <stdio.h>

#ifdef linux
	#include <unistd.h>
	#include <string.h>
	#include <stdlib.h>
	#define NOMFICH_CPUINFO "/proc/cpuinfo"
#elif defined(_Windows)
	#include <windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Chrono
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	private : double frequency;
	private : double time;
	private : double begin;
	private : double end;
	
	// Instruction RDTSC du processeur Pentium
	private : double RDTSC();


	// Lit la fr�uence du processeur
	// Renvoie la fr�uence en Hz dans 'frequence' si le code de retour est
	// diff�ent de 1. Renvoie 0 en cas d'erreur.
	private : int readCpuFrequency ();
	
	#if defined(_Windows)
		// D�init les types uint32 et uint64 sous Windows
		typedef unsigned __int32 uint32;
		typedef unsigned __int64 uint64;

		// D�init le convertion uint64 vers double
		private : inline double uint64_to_double (const uint64 x)
		{
			#if defined(_MSC_VER)
				// Petit hack pour convertir du 64 bits en double
				// Ce hack d�end de l'endian (ici � marche sur Intel x86)
				typedef uint32 uint64_32[2];
				uint64_32 *hack = (uint64_32 *)&x;
				double dbl;
				dbl  = (*hack)[1];
				dbl *= 4294967296;
				dbl += (*hack)[0];
				return dbl;
			#else
				return (double)x;
			#endif
		}
	#endif

	
	public : Chrono();
	public : ~Chrono();
	
	public : void start();
	public : double stop();
	public : double getTime();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __CHRONO_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
