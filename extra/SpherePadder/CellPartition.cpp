/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "CellPartition.hpp"

CellPartition::CellPartition()
{

}

void CellPartition::init(TetraMesh & mesh, double security_factor)
{
  if (!mesh.isOrganized)
  {
    cerr << "CellPartition::CellPartition, mesh is not valid!" << endl;
    return;
  }
  
  xmin = xmax = mesh.node[0].x;
  ymin = ymax = mesh.node[0].y;
  zmin = zmax = mesh.node[0].z;
  for (unsigned int i = 1 ; i < mesh.node.size() ; ++i)
  {
    xmin = (xmin > mesh.node[i].x) ? mesh.node[i].x : xmin;
    xmax = (xmax < mesh.node[i].x) ? mesh.node[i].x : xmax; 
    ymin = (ymin > mesh.node[i].y) ? mesh.node[i].y : ymin;
    ymax = (ymax < mesh.node[i].y) ? mesh.node[i].y : ymax;
    zmin = (zmin > mesh.node[i].z) ? mesh.node[i].z : zmin;
    zmax = (zmax < mesh.node[i].z) ? mesh.node[i].z : zmax;
  }
  
  isize = (unsigned int)((xmax - xmin) / (security_factor * mesh.mean_segment_length));
  if (isize < 1) isize = 1;
  
  jsize = (unsigned int)((ymax - ymin) / (security_factor * mesh.mean_segment_length));
  if (jsize < 1) jsize = 1;
  
  ksize = (unsigned int)((zmax - zmin) / (security_factor * mesh.mean_segment_length));
  if (ksize < 1) ksize = 1;
  
  cout << "nb cells: " << isize << ", " << jsize << ", " << ksize << endl;
  
  vector<unsigned int> kvec;
  for (unsigned int k = 0 ; k < ksize ; ++k) kvec.push_back(0);
  vector<vector<unsigned int> > jvec;
  for (unsigned int j = 0 ; j < jsize ; ++j) jvec.push_back(kvec);
  for (unsigned int i = 0 ; i < isize ; ++i) cellId.push_back(jvec);

  Cell C;
  unsigned int n = 0;
  for (unsigned int i = 0 ; i < isize ; ++i)
  {
    for (unsigned int j = 0 ; j < jsize ; ++j)
    {
      for (unsigned int k = 0 ; k < ksize ; ++k)
      {
        cell.push_back(C);
        cellId[i][j][k] = n++;
      }
    }
  }
  
  x_adjuster = (double)isize / (xmax - xmin);
  y_adjuster = (double)jsize / (ymax - ymin);
  z_adjuster = (double)ksize / (zmax - zmin);
}

void CellPartition::add(unsigned int n, double x, double y, double z)
{ 
  int i,j,k;
  i = (int)(floor((x - xmin) * x_adjuster));
  j = (int)(floor((y - ymin) * y_adjuster));
  k = (int)(floor((z - zmin) * z_adjuster));
  
  if (i >= (int)isize) current_i = isize - 1;
  else if (i < 0)      current_i = 0;
  else                 current_i = (unsigned int)i;
    
  if (j >= (int)jsize) current_j = jsize - 1;
  else if (j < 0)      current_j = 0;
  else                 current_j = (unsigned int)j;
  
  if (k >= (int)ksize) current_k = ksize - 1;
  else if (k < 0)      current_k = 0;
  else                 current_k = (unsigned int)k;
 
  cell[ cellId[current_i][current_j][current_k] ].sphereId.push_back(n);
}

void CellPartition::add_in_cell(unsigned int n, unsigned int i, unsigned int j, unsigned int k)
{
  cell[ cellId[i][j][k] ].sphereId.push_back(n);
}

void CellPartition::locateCellOf(double x, double y, double z)
{
  int i,j,k;
  
  i = (int)(floor((x - xmin) * x_adjuster));
  j = (int)(floor((y - ymin) * y_adjuster));
  k = (int)(floor((z - zmin) * z_adjuster));
  
  if (i >= (int)isize) current_i = isize - 1;
  else if (i < 0)      current_i = 0;
  else                 current_i = (unsigned int)i;
    
  if (j >= (int)jsize) current_j = jsize - 1;
  else if (j < 0)      current_j = 0;
  else                 current_j = (unsigned int)j;
  
  if (k >= (int)ksize) current_k = ksize - 1;
  else if (k < 0)      current_k = 0;
  else                 current_k = (unsigned int)k;
}


