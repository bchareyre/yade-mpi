/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef VERTEX_HPP
#define VERTEX_HPP

#include<yade/core/GeometricalModel.hpp>

class Vertex : public GeometricalModel {
	public :
		Vertex ();
		virtual ~Vertex ();

	protected :
	    void registerAttributes();
	    REGISTER_CLASS_NAME(Vertex);
	    REGISTER_BASE_CLASS_NAME(GeometricalModel);
	    REGISTER_CLASS_INDEX(Vertex,GeometricalModel);
};

REGISTER_SERIALIZABLE(Vertex,false);

#endif // VERTEX_HPP

