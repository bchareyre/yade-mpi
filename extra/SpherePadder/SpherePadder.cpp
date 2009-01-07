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

int compare_neighbor_with_distance (const void * a, const void * b)
{
  double d1 = (*(neighbor_with_distance *)a).distance;
  double d2 = (*(neighbor_with_distance *)b).distance;      
  return ( d1 > d2 ) ? 1 :-1;
}


SpherePadder::SpherePadder()
{
  vector <unsigned int> lst;
  
  for (unsigned int i = 0 ; i <= 5 ; ++i)
    for (unsigned int j = i+1 ; j <= 6 ; ++j)
      for (unsigned int k = j+1 ; k <= 7 ; ++k)
        for (unsigned int l = k+1 ; l <= 8 ; ++l)
        {
                lst.clear();
                lst.push_back(i);
                lst.push_back(j);
                lst.push_back(k);
                lst.push_back(l);
                //cout << i << ' ' << j << ' ' << k << ' ' << l << endl;
                combination.push_back(lst);
        }
        
   max_overlap_rate = 1e-2;   
   n1 = n2 = n3 = n4 = n_densify = 0;  
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
	place_at_barycentre_3();
	cancel_overlap();
        
        cerr << "nb spheres = " << sphere.size() << endl;
	
}

void SpherePadder::save_mgpost (const char* name)
{
  ofstream fmgpost(name);

  fmgpost << "<?xml version=\"1.0\"?>" << endl
	      << " <mgpost mode=\"3D\">" << endl
	      << "  <newcolor name=\"at nodes\"/>" << endl
	      << "  <newcolor name=\"at segments\"/>" << endl	
	      << "  <newcolor name=\"at faces\"/>" << endl
              << "  <newcolor name=\"debug\"/>" << endl
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
		
		sphere.push_back(S); ++(n1);	
	}
	cerr << "done" << endl;
}

void SpherePadder::place_at_segment_middle () // barycentre_2 ??
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
		sphere.push_back(S); ++(n2);	
		
		S.tetraOwner = mesh->node[id1].tetraOwner[0];
		mesh->tetraedre[S.tetraOwner].sphereId.push_back(n0 + s);
		
		mesh->segment[s].sphereId = n0 + s;
		
	}	
	cerr << "done" << endl;	
}


void SpherePadder::place_at_barycentre_3 ()
{
	cerr << "place at barycentre 3... ";
 
        // FIXME move the following loops in TetraMesh
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
	unsigned int ns0 = sphere.size();
        unsigned int ns = ns0; 
        const double div3 = 0.3333333333333;
	double R1,R2,R3;
        Sphere S1,S2,S3;

	for (unsigned int f = 0 ; f < mesh->face.size() ; ++f)
	{

		S1 = sphere[ mesh->face[f].sphereId[0] ];
		S2 = sphere[ mesh->face[f].sphereId[1] ];
		S3 = sphere[ mesh->face[f].sphereId[2] ];

                S.x = div3 * (S1.x + S2.x + S3.x); 
                S.y = div3 * (S1.y + S2.y + S3.y); 
                S.z = div3 * (S1.z + S2.z + S3.z); 
		
		R1 = distance_centre_spheres(S1,S) - S1.R;
		R2 = distance_centre_spheres(S2,S) - S2.R;
		R3 = distance_centre_spheres(S3,S) - S3.R;
		S.R = (R1  > R2) ? R2 : R1;
		S.R = (S.R > R3) ? R3 : S.R;
		
		sphere.push_back(S); ++(n3);

		S.tetraOwner = mesh->node[ mesh->face[f].nodeId[0] ].tetraOwner[0];
		mesh->tetraedre[S.tetraOwner].sphereId.push_back(ns++);
	}
	cerr << "done" << endl;
        

        for (unsigned int n = (n1+n2) ; n < sphere.size() ; ++n)
        {
          place_sphere_4contacts(n,20);
        }
            
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


void SpherePadder::cancel_overlap() // FIXME rename cancel_overlaps
{
	
	cerr << "cancel_overlap... ";
	unsigned int current_tetra_id,tetra_neighbor_id,j;
	Tetraedre current_tetra, tetra_neighbor;
	double distance,k;
        double distance_max = -max_overlap_rate * rmax;
	
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
					while ( (distance = distance_spheres(i,j)) < distance_max)
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



// FIXME nom : find_4contacts(...)
unsigned int SpherePadder::place_sphere_4contacts (unsigned int sphereId, unsigned int nb_iter_max) // FIXME nb_iter_max utile ???
{ 
  Sphere S = sphere[sphereId];
  unsigned int current_tetra_id = S.tetraOwner;
  Tetraedre current_tetra = mesh->tetraedre[current_tetra_id];
  unsigned int j;
  unsigned int tetra_neighbor_id;
  Tetraedre tetra_neighbor;

  vector<neighbor_with_distance> neighbor;
  vector<unsigned int> j_ok;
  neighbor_with_distance N;
                
  j_ok.push_back(sphereId);
  bool added;
  for (unsigned int t = 0 ; t < current_tetra.tetraNeighbor.size() ; ++t)
  {
      tetra_neighbor_id = current_tetra.tetraNeighbor[t];
      tetra_neighbor = mesh->tetraedre[tetra_neighbor_id];
      for (unsigned int n = 0 ; n < tetra_neighbor.sphereId.size() ; ++n)
      {
        j = tetra_neighbor.sphereId[n];

        added = false;
        for (unsigned int k = 0 ; k < j_ok.size() ; ++k) 
        {
          if (j == j_ok[k]) 
          {
            added = true;
            break;
          }
        }
        
        if (!added) 
        {
          N.sphereId = j;
          N.distance = distance_spheres(sphereId,j);
          // if (N.distance < rmax)
          neighbor.push_back(N);
          j_ok.push_back(j);
        }
      }
  }
  
  qsort(&(neighbor[0]),neighbor.size(),sizeof(neighbor_with_distance),compare_neighbor_with_distance); 
  
  vector<vector<unsigned int> > possible_combination;
  for (unsigned int c = 0 ; c < combination.size() ; ++c)
  {
    if (combination[c][0] >= neighbor.size()) continue;
    if (combination[c][1] >= neighbor.size()) continue;
    if (combination[c][2] >= neighbor.size()) continue;
    if (combination[c][3] >= neighbor.size()) continue;
    possible_combination.push_back(combination[c]);
  }
  
  unsigned int s1,s2,s3,s4;
  unsigned int failure;
  for (unsigned int c = 0 ; c < possible_combination.size() ; ++c)
  {
    // check if combination is possible
    
    s1 = neighbor[ possible_combination[c][0] ].sphereId;
    s2 = neighbor[ possible_combination[c][1] ].sphereId;
    s3 = neighbor[ possible_combination[c][2] ].sphereId;
    s4 = neighbor[ possible_combination[c][3] ].sphereId;
    //cout << sphereId << ' ' << s1 << ' ' << s2 << ' ' << s3 << ' ' << s4 << endl;

    failure = place_fifth_sphere(s1,s2,s3,s4,S);
    //cerr << success << " " ;
    if (!failure)
    {
      sphere[sphereId].x = S.x;
      sphere[sphereId].y = S.y;
      sphere[sphereId].z = S.z;
      sphere[sphereId].R = S.R;
      sphere[sphereId].type = 3; // debug
      //cerr << "placed\n" << "R = " << S.R << endl;
      return 1;
    }

  }
  //cerr << "fail\n";
  return 0;
}

double norm(double U[])
{
  int i;
  double n = 0;
  for (i = 0 ; i <= 2 ; i++)
  {
    n += U[i] * U[i];
  }
  return sqrt(n);
}

void moins(double U[],double V[],double Rep[])
{
  int i;
  for (i=0;i<=2;i++)
  {
    Rep[i] = V[i] - U[i]; 
  }
}

void product(double P[3][3],double U[],double Rep[])
{
  double var = 0;
  double C[3];
  int i,j;
  for (j = 0 ; j <= 2 ; j++)
  {
    for(i = 0 ; i <= 2 ; i++)
    {
      var = var + P[j][i]*U[i];                          
    }                      
    C[j] = var;
    var = 0;
  }
     
  for (i = 0 ; i <= 2 ; i++)
  {
    Rep[i] = C[i];
  }
}



unsigned int SpherePadder::place_fifth_sphere(unsigned int s1, unsigned int s2, unsigned int s3, unsigned int s4, Sphere& S)
//void calcul_5(double C1[],double R1,double C2[],double R2,double C3[],double R3,double C4[],double R4,double Rep[],double valeur_interpenetration)
{
  double C1[3],C2[3],C3[3],C4[3];
  double R1,R2,R3,R4;
  C1[0] = sphere[s1].x ; C1[1] = sphere[s1].y ; C1[2] = sphere[s1].z ; R1 = sphere[s1].R;  
  C2[0] = sphere[s2].x ; C2[1] = sphere[s2].y ; C2[2] = sphere[s2].z ; R2 = sphere[s2].R; 
  C3[0] = sphere[s3].x ; C3[1] = sphere[s3].y ; C3[2] = sphere[s3].z ; R3 = sphere[s3].R; 
  C4[0] = sphere[s4].x ; C4[1] = sphere[s4].y ; C4[2] = sphere[s4].z ; R4 = sphere[s4].R; 
  
  // bool -> 1 2 4 8 et return (b1 | b2 | bool2 | bool4)
  unsigned int fail_det          = 1;
  unsigned int fail_delta        = 2;
  unsigned int fail_radius       = 4;
  unsigned int fail_overlap      = 8;
  unsigned int fail_radius_range = 16;
  
 /*
  *(x-x1)^2 + (y-y1)^2 + (z-z1)^2 = (r+r1)^2   (1)
  *(x-x2)^2 + (y-y2)^2 + (z-z2)^2 = (r+r2)^2   (2)
  *(x-x3)^2 + (y-y3)^2 + (z-z3)^2 = (r+r3)^2   (3)
  *(x-x4)^2 + (y-y4)^2 + (z-z4)^2 = (r+r4)^2   (4)
  */

  /* (2)-(1) */

  double a = 2.0 * (C1[0]-C2[0]);
  double b = 2.0 * (C1[1]-C2[1]);
  double c = 2.0 * (C1[2]-C2[2]);
  double d = 2.0 * (R1-R2);
  double e = (C1[0]*C1[0]+C1[1]*C1[1]+C1[2]*C1[2]-R1*R1)-(C2[0]*C2[0]+C2[1]*C2[1]+C2[2]*C2[2]-R2*R2);

  /* (3)-(1) */

  double aa = 2.0 * (C1[0]-C3[0]);
  double bb = 2.0 * (C1[1]-C3[1]);
  double cc = 2.0 * (C1[2]-C3[2]);
  double dd = 2.0 * (R1-R3);
  double ee = (C1[0]*C1[0]+C1[1]*C1[1]+C1[2]*C1[2]-R1*R1)-(C3[0]*C3[0]+C3[1]*C3[1]+C3[2]*C3[2]-R3*R3);

  /* (4)-(1) */

  double aaa = 2.0 *(C1[0]-C4[0]);
  double bbb = 2.0 *(C1[1]-C4[1]);
  double ccc = 2.0 *(C1[2]-C4[2]);
  double ddd = 2.0 *(R1-R4);
  double eee = (C1[0]*C1[0]+C1[1]*C1[1]+C1[2]*C1[2]-R1*R1)-(C4[0]*C4[0]+C4[1]*C4[1]+C4[2]*C4[2]-R4*R4);

 /*
  *calcul du determinant de la matrice A du systeme AX=B
  *   [a  ,  b,  c];    [x]     [e  -  d*r]
  * A=[aa ,bb ,cc ];  X=[y]   B=[ee - dd*r]
  *   [aaa,bbb,ccc];    [z]     [eee-ddd*r]
  * developpement par rapport � la 1�re colonne     
  */

  double DET=a*(bb*ccc-bbb*cc)-aa*(b*ccc-bbb*c)+aaa*(b*cc-bb*c);
  double R = 0.0;
  double centre[3];
  if (DET != 0.0)
  {

   /*
    * Calcul de A^-1
    *       [a11,a12,a13]
    * A^-1= [a21,a22,a23] *1/det(A)
    *       [a31,a32,a33]        
    */

    double a11=(bb*ccc-bbb*cc)/DET;
    double a12=-(b*ccc-bbb*c)/DET;
    double a13=(b*cc-bb*c)/DET;
    double a21=-(aa*ccc-aaa*cc)/DET;
    double a22=(a*ccc-aaa*c)/DET;
    double a23=-(a*cc-aa*c)/DET;
    double a31=(aa*bbb-aaa*bb)/DET;
    double a32=-(a*bbb-aaa*b)/DET;
    double a33=(a*bb-aa*b)/DET;

    double xa=-(a11*d+a12*dd+a13*ddd);
    double xb=(a11*e+a12*ee+a13*eee);

    double ya=-(a21*d+a22*dd+a23*ddd);
    double yb=(a21*e+a22*ee+a23*eee);

    double za=-(a31*d+a32*dd+a33*ddd);
    double zb=(a31*e+a32*ee+a33*eee);

/*On remplace x,y et z dans l'�quation (1) et on r�soud l'�quation
 * du second degr� en r A*r^2+B*r+C=0 */

    double A=xa*xa+ya*ya+za*za-1;
    double B=2*(xa*(xb-C1[0]))+2*(ya*(yb-C1[1]))+2*(za*(zb-C1[2]))-2*R1;
    double C=(xb-C1[0])*(xb-C1[0])+(yb-C1[1])*(yb-C1[1])+(zb-C1[2])*(zb-C1[2])-R1*R1;

    double DELTA = B*B - 4.0*A*C;
    double RR1,RR2;

    if (DELTA >= 0.0)
    {
      RR1 = (-B + sqrt(DELTA)) / (2.0*A);
      RR2 = (-B - sqrt(DELTA)) / (2.0*A);
    }
    else return fail_delta;
    

    if      (RR1 > 0.0) R = RR1;
    else if (RR2 > 0.0) R = RR2;

    if (RR1 <= 0.0 && RR2 <= 0.0) return fail_radius;
    if (R < rmin || R > rmax) return fail_radius_range;

    centre[0] = xa * R + xb;
    centre[1] = ya * R + yb;
    centre[2] = za * R + zb;
  }
  else return fail_det;

/*------------------------------------------------------------------------------
 * Interpenetration entre spheres (x, y , z, rayon )
 *----------------------------------------------------------------------------*/
  // U4 = C4 - centre
  double U4[3];
  moins(centre,C4,U4);

  // U3 = C3 - centre
  double U3[3];
  moins(centre,C3,U3);

  // U2 = C2 - centre
  double U2[3];
  moins(centre,C2,U2);

  // U1 = C1 - centre
  double U1[3];
  moins(centre,C1,U1);

  if (    (norm(U4)-(R4+R) < -0.5 * max_overlap_rate*(R+R4)) 
       || (norm(U3)-(R3+R) < -0.5 * max_overlap_rate*(R+R3))
       || (norm(U2)-(R2+R) < -0.5 * max_overlap_rate*(R+R2)) 
       || (norm(U1)-(R1+R) < -0.5 * max_overlap_rate*(R+R1)) )
  {
    return fail_overlap;
  }

  S.x = centre[0];
  S.y = centre[1];
  S.z = centre[2];
  S.R = R;
  
  return 0;
}








void SpherePadder::place_at_tetra_centers ()
{
// TODO	
}
