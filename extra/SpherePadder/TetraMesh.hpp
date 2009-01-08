/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TETRA_MESH_HPP
#define TETRA_MESH_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h> // for qsort
#include <math.h>

using namespace std;

struct Node
{
	double x,y,z;
	vector<unsigned int> tetraOwner;
	vector<unsigned int> faceOwner;
	vector<unsigned int> segmentOwner;
};

struct Segment
{
	unsigned int nodeId[2];
	double length;
	vector<unsigned int> faceOwner;
	unsigned int sphereId;
};

struct Face
{
	unsigned int nodeId[3];
	vector<unsigned int> tetraOwner; // FIXME utile ???
	vector<unsigned int> sphereId;
	bool belongBoundary;
};

struct Tetraedre
{
	unsigned int nodeId[4];
	vector<unsigned int> sphereId;
	vector<unsigned int> tetraNeighbor;
};
	
class TetraMesh
{
protected:
    
  double xtrans,ytrans,ztrans;
  void organize (); 
  	
public:
  
  vector<Node>       node;
  vector<Segment>    segment;
  vector<Face>       face;
  vector<Tetraedre>  tetraedre;
  
  double mean_segment_length;
  double min_segment_length;
  double max_segment_length;
  
  void read_data (const char* name);

};

#endif // TETRA_MESH_HPP
