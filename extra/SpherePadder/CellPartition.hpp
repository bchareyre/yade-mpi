/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CELL_PARTITION_HPP
#define CELL_PARTITION_HPP

#include "TetraMesh.hpp"

struct Cell
{
  vector<unsigned int> sphereId; 
};

class CellPartition
{
  protected:
    
    vector<vector<vector<unsigned int> > > cellId;
    vector<Cell> cell;
    Cell out_of_cells;
    double xmin,xmax;
    double ymin,ymax;
    double zmin,zmax;              
    
    double x_adjuster,y_adjuster,z_adjuster;
    
  public:
    
    unsigned int isize,jsize,ksize;
    unsigned int current_i,current_j,current_k;
    
    CellPartition();
    void init(TetraMesh & mesh, double security_factor = 1.0);
    void add(unsigned int n, double x, double y, double z);
	void add_in_cell(unsigned int n, unsigned int i, unsigned int j, unsigned int k);
    void locateCellOf(double x, double y, double z);
    
    Cell& get_cell   (unsigned int i,unsigned int j,unsigned int k) { return cell[ cellId[i][j][k] ]; }
    unsigned int get_cellId (unsigned int i,unsigned int j,unsigned int k) { return cellId[i][j][k]; }
    
    unsigned int i_down() { return ( (current_i > 0) ? (current_i - 1) : 0 ); }
    unsigned int i_up  () { return ( (current_i < isize - 1) ? (current_i + 1) : isize - 1 ); }
    
    unsigned int j_down() { return ( (current_j > 0) ? (current_j - 1) : 0 ); }
    unsigned int j_up  () { return ( (current_j < jsize - 1) ? (current_j + 1) : jsize - 1); }
    
    unsigned int k_down() { return ( (current_k > 0) ? (current_k - 1) : 0 ); }
    unsigned int k_up  () { return ( (current_k < ksize - 1) ? (current_k + 1) : ksize - 1); }        
};

#endif // CELL_PARTITION_HPP

