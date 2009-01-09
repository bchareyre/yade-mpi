/*************************************************************************
*  Copyright (C) 2009 by Jean Francois Jerier                            *
*  jerier@geo.hmg.inpg.fr                                                *
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TetraMesh.hpp"

TetraMesh::TetraMesh ()   
{
  isOrganized = false;
}

void TetraMesh::read_gmsh (const char* name)
{
  ifstream meshFile(name);
  if(!meshFile)
  {
    cerr << "TetraMesh::read_gmsh, cannot open file " << name << endl;
    return;
  }  

  string token;
  char not_read[150];
  meshFile >> token;

  while(meshFile)
  {
    if (token == "$Nodes") 
    {
      unsigned int nbnodes;
      unsigned int num_node;
      Node N;

      meshFile >> nbnodes; 
      for (unsigned int n = 0 ; n < nbnodes ; ++n)
      {
        meshFile >> num_node >> N.x >> N.y >> N.z;
        node.push_back(N);      
      }
    }

    if (token == "$Elements") 
    {
      unsigned int nbElements;
      unsigned int num_element, element_type, nbTags ;
      Tetraedre T;
      unsigned int t = 0;
                        
      meshFile >> nbElements;
      for (unsigned int e = 0 ; e < nbElements ; ++e)
      {
        meshFile >> num_element >> element_type;
        if (element_type != 4)  // 4-node tetrahedron
        {
          meshFile.getline(not_read,150);
          continue;
        }
          
        
        meshFile >> nbTags;
        // the third tag is the number of a mesh partition to which the element belongs
        unsigned int tag;
        for (unsigned int tg = 0 ; tg < nbTags ; ++(tg))
        { meshFile >> tag; }
        
        meshFile >> T.nodeId[0] >> T.nodeId[1] >> T.nodeId[2] >> T.nodeId[3];
                      
        // numbers begin at 0 instead of 1
        // (0 in C/C++ corresponds to 1 in the file)
        T.nodeId[0] -= 1;
        T.nodeId[1] -= 1;
        T.nodeId[2] -= 1;
        T.nodeId[3] -= 1;
                                
        node[T.nodeId[0]].tetraOwner.push_back(t);
        node[T.nodeId[1]].tetraOwner.push_back(t);
        node[T.nodeId[2]].tetraOwner.push_back(t);
        node[T.nodeId[3]].tetraOwner.push_back(t);
                                
        tetraedre.push_back(T); 
        ++t;
      }                       
    }
                
    if (token == "$EndElements") break;
                
    meshFile >> token;
  }
   
  organize ();
}

void TetraMesh::read (const char* name)
{
	ifstream meshFile(name);
	if(!meshFile)
	{
		cerr << "TetraMesh::read_data, cannot open file " << name << endl;
		return;
	}  

	string token;
	meshFile >> token;

	while(meshFile)
	{
		if (token == "NODES") 
		{
			unsigned int nbnodes;
			Node N;

			meshFile >> nbnodes; 
			for (unsigned int n = 0 ; n < nbnodes ; ++n)
			{
				meshFile >> N.x >> N.y >> N.z;
				node.push_back(N);	
			}
		}

		if (token == "TETRA") 
		{
			unsigned int nbTetra;
			Tetraedre T;
			
			meshFile >> nbTetra;
			for (unsigned int t = 0 ; t < nbTetra ; ++t)
			{
				meshFile >> T.nodeId[0] >> T.nodeId[1] >> T.nodeId[2] >> T.nodeId[3];
				
				// numbers begin at 0 instead of 1
				// (0 in C/C++ corresponds to 1 in the file)
				T.nodeId[0] -= 1;
				T.nodeId[1] -= 1;
				T.nodeId[2] -= 1;
				T.nodeId[3] -= 1;
				
				node[T.nodeId[0]].tetraOwner.push_back(t);
				node[T.nodeId[1]].tetraOwner.push_back(t);
				node[T.nodeId[2]].tetraOwner.push_back(t);
				node[T.nodeId[3]].tetraOwner.push_back(t);
				
				tetraedre.push_back(T);	
			}			
		}
		
		if (token == "EOF") break;
		
		meshFile >> token;
	}
	
	organize ();
}

int compareInt (const void * a, const void * b)
{
	return ( *(int*)a > *(int*)b ) ? 1 :-1;
}

void TetraMesh::organize ()
{
	//cout << "Organize data... " << flush;
	
	// Translate all nodes in such a manner that all coordinates are > 0
	xtrans = node[0].x;
	ytrans = node[0].y;
	ztrans = node[0].z;
	for (unsigned int i = 1 ; i < node.size() ; ++i)
	{
		xtrans = (xtrans < node[i].x) ? xtrans : node[i].x;
		ytrans = (ytrans < node[i].y) ? ytrans : node[i].y;
		ztrans = (ztrans < node[i].z) ? ztrans : node[i].z;
	}
	xtrans = (xtrans < 0.0) ? -xtrans : 0.0;
	ytrans = (ytrans < 0.0) ? -ytrans : 0.0;
	ztrans = (ztrans < 0.0) ? -ztrans : 0.0;
	for (unsigned int i = 0 ; i < node.size() ; ++i)
	{
		node[i].x += xtrans;
		node[i].y += ytrans;
		node[i].z += ztrans;
	}	
	
	// Organize tetraedre nodes in ascending order
	for (unsigned int i = 0 ; i < tetraedre.size() ; ++i)
	{
		qsort (&(tetraedre[i].nodeId[0]), 4, sizeof(int), compareInt);
	}
	
	// Face creation
	vector <Face> tmpFace; // This will contain all faces more than one time (with duplications)
	Face F;
	F.tetraOwner.push_back(0);
	F.belongBoundary = true;
	for (unsigned int i = 0 ; i < tetraedre.size() ; ++i)
	{
		F.tetraOwner[0] = i;
		
		// Note that nodes are still organized in ascending order
		F.nodeId[0] = tetraedre[i].nodeId[0];
		F.nodeId[1] = tetraedre[i].nodeId[1];
		F.nodeId[2] = tetraedre[i].nodeId[2];
		tmpFace.push_back(F);
		
		F.nodeId[0] = tetraedre[i].nodeId[1];
		F.nodeId[1] = tetraedre[i].nodeId[2];
		F.nodeId[2] = tetraedre[i].nodeId[3];
		tmpFace.push_back(F);
		
		F.nodeId[0] = tetraedre[i].nodeId[0];
		F.nodeId[1] = tetraedre[i].nodeId[2];
		F.nodeId[2] = tetraedre[i].nodeId[3];
		tmpFace.push_back(F);
		
		F.nodeId[0] = tetraedre[i].nodeId[0];
		F.nodeId[1] = tetraedre[i].nodeId[1];
		F.nodeId[2] = tetraedre[i].nodeId[3];
		tmpFace.push_back(F);		
	}
	
	face.push_back(tmpFace[0]);
	bool duplicated;
	for (unsigned int i = 1 ; i < tmpFace.size() ; ++i)
	{
		duplicated = false;
		for (unsigned int n = 0 ; n < face.size() ; ++n)
		{
			if (   tmpFace[i].nodeId[0] == face[n].nodeId[0]
			    && tmpFace[i].nodeId[1] == face[n].nodeId[1]
			    && tmpFace[i].nodeId[2] == face[n].nodeId[2])
			{
				duplicated = true;
				face[n].tetraOwner.push_back(tmpFace[i].tetraOwner[0]);
				face[n].belongBoundary = false;
				break;
			}				
		}
		
		if (!duplicated) 
		{
			face.push_back(tmpFace[i]);
		}
	}
	tmpFace.clear(); // It should be usefull to free memory before the end of the function
	
	for (unsigned int f = 0 ; f < face.size() ; ++f)
	{
		node[face[f].nodeId[0]].faceOwner.push_back(f);
		node[face[f].nodeId[1]].faceOwner.push_back(f);
		node[face[f].nodeId[2]].faceOwner.push_back(f);
	}
	
	// Segment creation
	vector <Segment> tmpSegment;
	Segment S;
	S.faceOwner.push_back(0);

	for (unsigned int i = 0 ; i < face.size() ; ++i)
	{
		S.faceOwner[0] = i;
		S.nodeId[0] = face[i].nodeId[0];
		S.nodeId[1] = face[i].nodeId[1];
		tmpSegment.push_back(S);
		
		S.nodeId[0] = face[i].nodeId[1];
		S.nodeId[1] = face[i].nodeId[2];
		tmpSegment.push_back(S);
		
		S.nodeId[0] = face[i].nodeId[0];
		S.nodeId[1] = face[i].nodeId[2];
		tmpSegment.push_back(S);
				
	}
	
	segment.push_back(tmpSegment[0]);
	for (unsigned int i = 1 ; i < tmpSegment.size() ; ++i)
	{
		duplicated = false;
		for (unsigned int n = 0 ; n < segment.size() ; ++n)
		{
			if (   tmpSegment[i].nodeId[0] == segment[n].nodeId[0]
			    && tmpSegment[i].nodeId[1] == segment[n].nodeId[1])
			{
				duplicated = true;
				segment[n].faceOwner.push_back(tmpSegment[i].faceOwner[0]);
				break;
			}
				
		}
		
		if (!duplicated) 
		{
			segment.push_back(tmpSegment[i]);
		}
	}
	
	for (unsigned int s = 0 ; s < segment.size() ; ++s)
	{
		node[segment[s].nodeId[0]].segmentOwner.push_back(s);
		node[segment[s].nodeId[1]].segmentOwner.push_back(s);
	}
		
	// Compute length of segments
	double lx,ly,lz;
	unsigned int id1,id2;
	mean_segment_length = 0.0;
	min_segment_length = 0.0;
	max_segment_length = 0.0;
	for (unsigned int s = 0 ; s < segment.size() ; ++s)
	{
		id1 = segment[s].nodeId[0];
		id2 = segment[s].nodeId[1];
		
		lx  = node[id2].x - node[id1].x;
	    ly  = node[id2].y - node[id1].y;
	    lz  = node[id2].z - node[id1].z;
		
		mean_segment_length += (segment[s].length = sqrt(lx*lx + ly*ly + lz*lz));
		min_segment_length = (segment[s].length < min_segment_length) ? segment[s].length : min_segment_length;
		max_segment_length = (segment[s].length > max_segment_length) ? segment[s].length : max_segment_length;
	}
	mean_segment_length /= (double)(segment.size());
	
	
	// Define tetraedre neighbors
	for (unsigned int t1 = 0 ; t1 < tetraedre.size() ; ++t1)
	{
		for (unsigned int t2 = t1 ; t2 < tetraedre.size() ; ++t2)
		{
			if (   (tetraedre[t1].nodeId[0] > tetraedre[t2].nodeId[3]) 
				|| (tetraedre[t1].nodeId[3] < tetraedre[t2].nodeId[0]) ) continue;
                        
                        // FIXME mettre du while... (?)
			for (unsigned int i = 0 ; i < 4 ; i++)
                        {
			        for (unsigned int j = 0 ; j < 4 ; j++)
				{
				    if (tetraedre[t1].nodeId[i] == tetraedre[t2].nodeId[j])
				    {
				      tetraedre[t1].tetraNeighbor.push_back(t2);
				      tetraedre[t2].tetraNeighbor.push_back(t1);
				      break;
				    }
				}
                        }	  
		}
	}
	
        isOrganized = true;
}



