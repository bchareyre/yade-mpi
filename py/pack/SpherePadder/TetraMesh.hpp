/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
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
	bool normal_swap;
};

struct cmp_face{
  inline bool operator()(const Face & f1,const Face & f2){
    return
	(
	   (f1.nodeId[0] < f2.nodeId[0])
	|| (f1.nodeId[0] == f2.nodeId[0] && f1.nodeId[1] < f2.nodeId[1])
	|| (f1.nodeId[1] == f2.nodeId[1] && f1.nodeId[2] < f2.nodeId[2])
	);
  }
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

  void organize ();   
  	
public:
  
  vector <Node>       node;
  vector <Segment>    segment;
  vector <Face>       face;
  vector <Tetraedre>  tetraedre;
  
  double mean_segment_length;
  double min_segment_length;
  double max_segment_length;
  
  TetraMesh();
  
  double xtrans,ytrans,ztrans;
  bool   isOrganized;
  
  void read      (const char* name);
  void read_gmsh (const char* name);
  void read_inp  (const char* name);
  void write_surface_MGP (const char* name);
};

