/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef FILTRATIONAL_ENGINE_HPP
#define FILTRATIONAL_ENGINE_HPP

#include<yade/core/DeusExMachina.hpp>
class FiltrEngine : public DeusExMachina {
	public:
		/// Activation of filtration from GUI
		static bool isFiltrationActivated;

		/// Activation of the filter
		bool isFilterActivated;

		bool isActivated();

		FiltrEngine();
	
		void registerAttributes()
		{
//			REGISTER_ATTRIBUTE(isFiltrationActivated);
			REGISTER_ATTRIBUTE(isFilterActivated);
		}

	protected :
	REGISTER_CLASS_NAME(FiltrEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(FiltrEngine,false);

typedef FiltrEngine FiltrationalEngine;

#endif // FILTRATIONAL_ENGINE_HPP

