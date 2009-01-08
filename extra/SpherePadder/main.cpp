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
	TetraMesh * mesh = new TetraMesh();
	mesh->read_data("test.msh");
        
        SpherePadder * padder = new SpherePadder();
	padder->plugTetraMesh(mesh);
	
	padder->pad_5();
	
	padder->save_mgpost("mgp.out.001");
        padder->save_Rxyz("out.Rxyz");
        
	return 0;
}



