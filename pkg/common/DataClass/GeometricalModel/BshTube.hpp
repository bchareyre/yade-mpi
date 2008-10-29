/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BSH_TUBE_HPP
#define BSH_TUBE_HPP


#include<yade/core/GeometricalModel.hpp>


class BshTube : public GeometricalModel
{
	public :
		Real  radius;
                Real  half_height;

		BshTube ();
		virtual ~BshTube ();
	
/// Serialization
	
	REGISTER_CLASS_NAME(BshTube);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);

	public : void registerAttributes();
	
/// Indexable
	
	REGISTER_CLASS_INDEX(BshTube,GeometricalModel);

};

REGISTER_SERIALIZABLE(BshTube,false);

#endif // BSH_TUBE_HPP

