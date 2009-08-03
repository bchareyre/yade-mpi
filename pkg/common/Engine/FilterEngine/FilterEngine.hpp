/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/DeusExMachina.hpp>
class FilterEngine : public DeusExMachina {
	public:
		/// Activation of filtration from GUI
		static bool isFiltrationActivated;

		/// Activation of the filter
		bool isFilterActivated;

		bool isActivated(MetaBody*);

		FilterEngine();
	REGISTER_ATTRIBUTES(DeusExMachina,(isFilterActivated));
	REGISTER_CLASS_AND_BASE(FilterEngine,DeusExMachina);
};
REGISTER_SERIALIZABLE(FilterEngine);


