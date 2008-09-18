/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef IFACET_HPP
#define IFACET_HPP

#include<yade/core/GeometricalModel.hpp>

class Facet : public GeometricalModel {
    public :
	vector<Vector3r> vertices;

	Facet ();
	virtual ~Facet ();

    protected :
	    void registerAttributes();
    REGISTER_CLASS_NAME(Facet);
    REGISTER_BASE_CLASS_NAME(GeometricalModel);
    REGISTER_CLASS_INDEX(Facet,GeometricalModel);
};

REGISTER_SERIALIZABLE(Facet,false);

#endif // IFACET_HPP

