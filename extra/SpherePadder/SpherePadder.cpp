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

double rand01()
{
	return (double)rand()/(double)RAND_MAX;
}

void SpherePadder::read_data (const char* filename)
{
	//FIXME : 
	// si on utilise la class dans un autre programme c'est peu etre pas util
		
	//rmin = 1e-2;
	//rmax = 2e-2;
	//rmoy = 0.5 * (rmin + rmax);
	ratio = 3.0; rmoy = 0.0;

/* FIXME
pour le moment, l'utilisateur ne peut entre qu'un ratio.
Le rayon des sphere depend du maillage (des longueurs des segments)
*/


}
	
void SpherePadder::plugTetraMesh (TetraMesh * pluggedMesh)
{
	mesh = pluggedMesh;
	
	// Si l'utilisateur n'a choisi qu'une valeur de ratio, 
    // on cree les valeur de rmin et rmax:
	if (rmoy == 0 && ratio > 0)
	{
		rmoy = 0.125 * mesh->mean_segment_length; // 1/8
	    rmin = (2.0 * rmoy) / (ratio + 1.0);
	    //rmax = (2.0 * ratio * rmoy) / (ratio + 1.0); // FIXME ask jf
		rmax = 2.0 * rmoy - rmin;
	    //dr	 = 0.5 * (rmax - rmin); // FIXME : voir avec jf ce qu'il fait vraiment (sa version (rmax+rmin)/2)
		dr = rmax - rmoy;
	}
}

void SpherePadder::pad_5 ()
{
	// TODO check if all si ok (mesh exist...)
	place_at_nodes();
	place_at_segment_middle();
	place_at_faces_barycentre(); // TODO tester avec barycentres
	cancel_overlap();
	// ...
}

void SpherePadder::save_mgpost (const char* name)
{
  ofstream fmgpost(name);

  fmgpost << "<?xml version=\"1.0\"?>" << endl
	      << " <mgpost mode=\"3D\">" << endl
	      << "  <newcolor name=\"nodes\"/>" << endl
	      << "  <newcolor name=\"middle nodes\"/>" << endl	
	      << "  <newcolor name=\"fixme\"/>" << endl
	      << "  <state id=\"" << 1 
	      << "\" time=\"" << 0.0 << "\">" << endl;

  for (unsigned int i = 0 ; i < sphere.size() ; ++i)
    {
    fmgpost << "   <body>" << endl;
    fmgpost << "    <SPHER id=\"" << i+1 << "\" col=\"" << sphere[i].type << "\" r=\"" << sphere[i].R << "\">" << endl
		    << "     <position x=\"" << sphere[i].x << "\" y=\"" << sphere[i].y << "\" z=\"" << sphere[i].z << "\"/>" << endl	 
            << "    </SPHER>" << endl << flush;

// tmp (bricolage)
if (i < mesh->node.size())
		for (unsigned int s = 0 ; s < mesh->segment.size() ; ++s)
		{
			if (mesh->segment[s].nodeId[0] == i)
			{
				fmgpost << "    <SPSPx antac=\"" << mesh->segment[s].nodeId[1] + 1 << "\"/>" << endl;
			}
		}


    fmgpost << "   </body>" << endl;
    }

  fmgpost << "  </state>" << endl
	      << " </mgpost>" << endl;
		
}
	
void SpherePadder::place_at_nodes ()
{
	unsigned int segId;
	Sphere S;
	S.type = 0;
	S.owner.push_back(0);
	
	cerr << "place at nodes... ";
	
	for (unsigned int n = 0 ; n < mesh->node.size() ; ++n)
	{
		S.x = mesh->node[n].x;
		S.y = mesh->node[n].y;
		S.z = mesh->node[n].z;
		S.R = mesh->segment[ mesh->node[n].segmentOwner[0] ].length;
		for (unsigned int i = 1 ; i < mesh->node[n].segmentOwner.size() ; ++i)
		{
			segId = mesh->node[n].segmentOwner[i];
			S.R = (S.R < mesh->segment[segId].length) ? S.R : mesh->segment[segId].length;
		}	
		S.R /= 4.0;
		S.owner[0] = n;// FIXME utile ???
		
		S.tetraOwner = mesh->node[n].tetraOwner[0];
		mesh->tetraedre[S.tetraOwner].sphereId.push_back(n);
		
		sphere.push_back(S);	
	}
	cerr << "done" << endl;
}

void SpherePadder::place_at_segment_middle ()
{	
	cerr << "place at segment middle... ";
	Sphere S;
	S.type = 1;
	S.owner.push_back(0);
	double x1,y1,z1;
	double x2,y2,z2;
	unsigned int id1,id2;
	unsigned int n0 = sphere.size();
	
	for (unsigned int s = 0 ; s < mesh->segment.size() ; ++s)
	{
		id1 = mesh->segment[s].nodeId[0];
		id2 = mesh->segment[s].nodeId[1];
		
		x1  = mesh->node[id1].x;
		y1  = mesh->node[id1].y;
		z1  = mesh->node[id1].z;
		
		x2  = mesh->node[id2].x;
		y2  = mesh->node[id2].y;
		z2  = mesh->node[id2].z;
		
		S.x = 0.5 * (x1 + x2);
		S.y = 0.5 * (y1 + y2);
		S.z = 0.5 * (z1 + z2);
		S.R = 0.125 * mesh->segment[s].length;
		if (S.R < rmin) S.R = rmin;
		else if (S.R > rmax) S.R = rmoy + dr * rand01();
		S.owner[0] = s;
		sphere.push_back(S);	
		
		S.tetraOwner = mesh->node[id1].tetraOwner[0];
		mesh->tetraedre[S.tetraOwner].sphereId.push_back(n0 + s);
		
		mesh->segment[s].sphereId = n0 + s;
		
	}	
	cerr << "done" << endl;	
}




void SpherePadder::place_at_faces_barycentre ()
{
	cerr << "place at faces_barycentre... ";

	unsigned int sphereId,faceId;
	for (unsigned int s = 0 ; s < mesh->segment.size() ; ++s)
	{
		sphereId = mesh->segment[s].sphereId;
		for (unsigned int f = 0 ; f < mesh->segment[s].faceOwner.size() ; ++f)
		{
			faceId = mesh->segment[s].faceOwner[f];
			mesh->face[ faceId ].sphereId.push_back( sphereId );
		}
	}
	
	Sphere S;
	S.type = 2;
	// todo S.owner ...
	unsigned int ns = sphere.size();
        const double untiers = 0.3333333333333;
	double R1,R2,R3;

	for (unsigned int f = 0 ; f < mesh->face.size() ; ++f)
	{

		Sphere S1 = sphere[ mesh->face[f].sphereId[0] ];
		Sphere S2 = sphere[ mesh->face[f].sphereId[1] ];
		Sphere S3 = sphere[ mesh->face[f].sphereId[2] ];

		S.x = untiers * (S1.x + S2.x + S3.x); 
		S.y = untiers * (S1.y + S2.y + S3.y); 
		S.z = untiers * (S1.z + S2.z + S3.z); 
		
		R1 = distance_centre_spheres(S1,S) - S1.R;
		R2 = distance_centre_spheres(S2,S) - S2.R;
		R3 = distance_centre_spheres(S3,S) - S3.R;
		S.R = (R1>R2)    ? R2 : R1;
		S.R = (S.R > R3) ? R3 : S.R;
		
		sphere.push_back(S);

		S.tetraOwner = mesh->node[ mesh->face[f].nodeId[0] ].tetraOwner[0];
		mesh->tetraedre[S.tetraOwner].sphereId.push_back(ns++);
	}
	cerr << "done" << endl;
}








double SpherePadder::distance_spheres(unsigned int i, unsigned int j)
{
	double lx,ly,lz;
	lx  = sphere[j].x - sphere[i].x;
	ly  = sphere[j].y - sphere[i].y;
	lz  = sphere[j].z - sphere[i].z;
	return (sqrt(lx*lx + ly*ly + lz*lz) - sphere[i].R - sphere[j].R);
}

double SpherePadder::distance_centre_spheres(Sphere& S1, Sphere& S2)
{
	double lx,ly,lz;
	lx  = S2.x - S1.x;
	ly  = S2.y - S1.y;
	lz  = S2.z - S1.z;
	return (sqrt(lx*lx + ly*ly + lz*lz));
}


void SpherePadder::cancel_overlap()
{
	
	cerr << "cancel_overlap... ";
	unsigned int current_tetra_id,tetra_neighbor_id,j;
	Tetraedre current_tetra, tetra_neighbor;
	double distance,k;
	
	for(unsigned int i = 0 ; i <  sphere.size(); ++i)
	{
		if (sphere[i].R < 0.0) continue;
		current_tetra_id = sphere[i].tetraOwner;
		current_tetra = mesh->tetraedre[current_tetra_id];
		
		for (unsigned int t = 0 ; t < current_tetra.tetraNeighbor.size() ; ++t)
		{
			tetra_neighbor_id = current_tetra.tetraNeighbor[t];
			tetra_neighbor = mesh->tetraedre[tetra_neighbor_id];
			for (unsigned int n = 0 ; n < tetra_neighbor.sphereId.size() ; ++n)
			{
				j = tetra_neighbor.sphereId[n];

				if (sphere[j].R < 0.0) continue;
				if (i < j)
				{
					while ( (distance = distance_spheres(i,j)) < -0.001)
					{						
						k = 1.0 + distance / (sphere[i].R+ sphere[j].R);
						sphere[i].R *= k;
						sphere[j].R *= k;
					}
				}
			}
		}
	}
	cerr << "done" << endl;
}


void SpherePadder::place_at_tetra_centers ()
{
// TODO	
}
