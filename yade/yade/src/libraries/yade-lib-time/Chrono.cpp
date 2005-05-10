/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   galizzi@stalactite                                                    *
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

#include "Chrono.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Chrono::Chrono()
{
	readCpuFrequency();
	time	= 0;
	begin	= 0;
	end 	= 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

Chrono::~Chrono()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

double Chrono::RDTSC()
{
	#ifdef linux
		unsigned long long x;
		__asm__ volatile  (".byte 0x0f, 0x31" : "=A"(x));
		return (double)x;
	#else
		unsigned long a, b;
		double x;

		#ifdef _MSC_VER
			// Code pour Visual C++
			__asm
			{
				RDTSC
		#else
			// Code pour Borland et autres
			asm
			{
				db 0x0F,0x31 // instruction RDTSC
		#endif
    	
				mov [a],eax
			    mov [b],edx
			}
			x  = b;
			x *= 4294967296;
			x += a;
			return x;
	#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int Chrono::readCpuFrequency ()
{
	#ifdef linux
		const char* prefixe_cpu_mhz = "cpu MHz";
		FILE* F;
		char ligne[300+1];
		char *pos;
		int ok=0;

		// Ouvre le fichier
		F = fopen(NOMFICH_CPUINFO, "r");
		if (!F) 
			return 0;

		// Lit une ligne apres l'autre
		while (!feof(F))
		{
			// Lit une ligne de texte
			fgets (ligne, sizeof(ligne), F);

			// C'est la ligne contenant la frequence?
			if (!strncmp(ligne, prefixe_cpu_mhz, strlen(prefixe_cpu_mhz)))
			{
				// Oui, alors lit la frequence
				pos = strrchr (ligne, ':') +2;
				if (!pos) 
					break;
				if (pos[strlen(pos)-1] == '\n') 
					pos[strlen(pos)-1] = '\0';
				strcpy (ligne, pos);
				strcat (ligne,"e6");
				frequency = atof (ligne);
				ok = 1;
				break;
			}
		}
		fclose (F);
		return ok;
	#else
		uint64 Fwin;
		uint64 Twin_avant, Twin_apres;
		double Tcpu_avant, Tcpu_apres;
		double Fcpu;

		// Lit la frequence du Chronom�re Windows
		if (!QueryPerformanceFrequency((LARGE_INTEGER*)&Fwin)) 
			return 0;	

		// Avant
		Tcpu_avant = RDTSC();
		QueryPerformanceCounter((LARGE_INTEGER*)&Twin_avant);

		// Attend quelques it�ations (10 000) du Chronom�re Windows
		do
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&Twin_apres);
		} while (Twin_apres-Twin_avant < 10000);

		// Apres
		Tcpu_apres = RDTSC();
		QueryPerformanceCounter((LARGE_INTEGER*)&Twin_apres);

		// Calcule la fr�uence en MHz
		Fcpu  = (Tcpu_apres - Tcpu_avant);
		Fcpu *= uint64_to_double(Fwin);
		Fcpu /= uint64_to_double(Twin_apres - Twin_avant);
		frequency = Fcpu;
		return 1;
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Chrono::start()
{

	begin = RDTSC();

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

double Chrono::stop()
{

	end = RDTSC();
	time = (end-begin)/frequency;
	return time;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

double Chrono::getTime()
{
	end = RDTSC();
	time = (end-begin)/frequency;
	return time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
