/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef STLIMPORTER_HPP
#define STLIMPORTER_HPP
#include <yade/pkg-common/Facet.hpp>
#include <yade/pkg-common/InteractingFacet.hpp>
#include "STLReader.hpp"
#include<yade/core/Body.hpp>
#include<yade/core/BodyContainer.hpp>

class STLImporter {
    public:
	STLImporter();	

	/// open file
	bool open(const char* filename);

	/// number of facets 
	unsigned int number_of_facets;

	/// set wire attribute for facets (by default false)
	bool wire;

	/// import geometry 
	void import(shared_ptr<BodyContainer> bodies, unsigned int begin=0, bool noInteractingGeometry=false);

	DECLARE_LOGGER;
protected:
		vector<Vector3r> tr; 
};

#endif // STLIMPORTER_HPP
