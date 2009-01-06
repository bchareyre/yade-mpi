/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SpherePadder.hpp"

int main()
{
	SpherePadder * padder = new SpherePadder();
	padder->read_data("padding.dat");
	TetraMesh * mesh = new TetraMesh();
	mesh->read_data("test.msh");
	padder->plugTetraMesh(mesh);
	
	padder->pad_5();
	
	padder->save_mgpost("mgp.out.001");
	
	return 0;
}



