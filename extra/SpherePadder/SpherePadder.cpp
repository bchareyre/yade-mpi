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
                combination.push_back(lst);
        }
        
   max_overlap_rate = 1e-3;   
   n1 = n2 = n3 = n4 = n5 = n_densify = 0;  
   trace_functions = true;
   meshIsPlugged = false;             
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
  meshIsPlugged = true;
	
  // TODO mettre ce qui suite dans une fonction 'init()'
  // Si l'utilisateur n'a choisi qu'une valeur de ratio, 
  // on cree les valeur de rmin et rmax:
  if (rmoy == 0 && ratio > 0)
  {
	rmoy = 0.125 * mesh->mean_segment_length; // 1/8
	rmin = (2.0 * rmoy) / (ratio + 1.0);
	//rmax = (2.0 * ratio * rmoy) / (ratio + 1.0); 
	rmax = 2.0 * rmoy - rmin;
	//dr	 = 0.5 * (rmax - rmin); 
	dr = rmax - rmoy;
  }
}

void SpherePadder::pad_5 ()
{
  if (mesh == 0) 
  {
    cerr << "SpherePadder::pad_5, no mesh defined!" << endl;
    return;
  }
    
  if (!(mesh->isOrganized)) 
  {
    cerr << "SpherePadder::pad_5, mesh is not valid!" << endl;
    return;
  }
  
  place_at_nodes();
  place_at_segment_middle();
  cancel_overlaps();
  place_at_faces();
  place_at_tetra_centers();
  place_at_tetra_vertexes ();

  cerr << "Total number of spheres = " << sphere.size() << endl;
        
}

void SpherePadder::save_mgpost (const char* name)
{
  BEGIN_FUNCTION ("Save mgp");
  
  ofstream fmgpost(name);
  
  double xtrans = mesh->xtrans;
  double ytrans = mesh->ytrans;
  double ztrans = mesh->ztrans;
  
  fmgpost << "<?xml version=\"1.0\"?>" << endl
      << " <mgpost mode=\"3D\">" << endl
      << "  <newcolor name=\"at nodes\"/>" << endl
      << "  <newcolor name=\"at segments\"/>" << endl   
      << "  <newcolor name=\"at faces\"/>" << endl
      << "  <newcolor name=\"at tetra centers\"/>" << endl
      << "  <newcolor name=\"at tetra vertexes\"/>" << endl
      << "  <state id=\"" << 1 
      << "\" time=\"" << 0.0 << "\">" << endl;

  for (unsigned int i = 0 ; i < sphere.size() ; ++i)
  {
    fmgpost << "   <body>" << endl;
    fmgpost << "    <SPHER id=\"" << i+1 << "\" col=\"" << sphere[i].type << "\" r=\"" << sphere[i].R << "\">" << endl
        << "     <position x=\"" << sphere[i].x + xtrans << "\" y=\"" 
        << sphere[i].y + ytrans << "\" z=\"" << sphere[i].z + ztrans << "\"/>" << endl   
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
                
  END_FUNCTION;
}
	
void SpherePadder::save_Rxyz (const char* name)
{
  BEGIN_FUNCTION("Save Rxyz");
  
  ofstream file(name);
  
  double xtrans = mesh->xtrans;
  double ytrans = mesh->ytrans;
  double ztrans = mesh->ztrans;

  for (unsigned int i = 0 ; i < sphere.size() ; ++i)
  {
    file << sphere[i].R << " " << sphere[i].x + xtrans << " " << sphere[i].y + ytrans << " " << sphere[i].z + ztrans << endl;
  }
                
  END_FUNCTION;
}
        
void SpherePadder::place_at_nodes ()
{
  BEGIN_FUNCTION("Place at nodes");
        
  unsigned int segId;
  Sphere S;
  S.type = AT_NODE;
        
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
                
    S.tetraOwner = mesh->node[n].tetraOwner[0];
    mesh->tetraedre[S.tetraOwner].sphereId.push_back(n);
                
    sphere.push_back(S); ++(n1);    
  }
        
  END_FUNCTION;
}

void SpherePadder::place_at_segment_middle ()
{       
  BEGIN_FUNCTION("Place at segment middle");
  Sphere S;
  S.type = AT_SEGMENT;
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
    else if (S.R > rmax) S.R = rmoy + dr * (double)rand()/(double)RAND_MAX;
                
    S.tetraOwner = mesh->node[id1].tetraOwner[0];
    mesh->tetraedre[S.tetraOwner].sphereId.push_back(n0 + s);
    sphere.push_back(S); ++(n2);
                
    mesh->segment[s].sphereId = n0 + s;
                
  }       
  END_FUNCTION;   
}


void SpherePadder::place_at_faces ()
{
  BEGIN_FUNCTION("Place at faces");
 
  // FIXME move the following loops in TetraMesh or at the end of place_at_segment_middle ??
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
  S.type = AT_FACE;

  unsigned int ns = sphere.size();
  const double div3 = 0.3333333333333;
  Sphere S1,S2,S3;

  for (unsigned int f = 0 ; f < mesh->face.size() ; ++f)
  {

    S1 = sphere[ mesh->face[f].sphereId[0] ];
    S2 = sphere[ mesh->face[f].sphereId[1] ];
    S3 = sphere[ mesh->face[f].sphereId[2] ];

    S.x = div3 * (S1.x + S2.x + S3.x); 
    S.y = div3 * (S1.y + S2.y + S3.y); 
    S.z = div3 * (S1.z + S2.z + S3.z);                 
    S.R = rmin;
                
    S.tetraOwner = mesh->node[ mesh->face[f].nodeId[0] ].tetraOwner[0];
    mesh->tetraedre[S.tetraOwner].sphereId.push_back(ns++);
    sphere.push_back(S); ++(n3);
  }
        
  for (unsigned int n = (n1+n2) ; n < sphere.size() ; ++n)
  {
    place_sphere_4contacts(n);
  }
        
  END_FUNCTION;  
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


void SpherePadder::cancel_overlaps()
{
        
  BEGIN_FUNCTION("Cancel_overlaps");
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
          while ( (distance = distance_spheres(i,j)) < distance_max )
          {                                               
            k = 1.0 + distance / (sphere[i].R + sphere[j].R);
            sphere[i].R *= k;
            sphere[j].R *= k;
          }
        }
      }
    }
  }
  END_FUNCTION;
}


unsigned int SpherePadder::place_sphere_4contacts (unsigned int sphereId)
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

        if (sphere[j].R <= 0.0) continue;
        
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
  S.R += neighbor[0].distance;
  if (S.R >= 0.0) sphere[sphereId].R = S.R;
  else            sphere[sphereId].R = 0.0;

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
    s1 = neighbor[ possible_combination[c][0] ].sphereId;
    s2 = neighbor[ possible_combination[c][1] ].sphereId;
    s3 = neighbor[ possible_combination[c][2] ].sphereId;
    s4 = neighbor[ possible_combination[c][3] ].sphereId;

    failure = place_fifth_sphere(s1,s2,s3,s4,S);
    
    if (!failure)
    {
//       for (unsigned n = 0 ; n < sphere.size() ; ++n)
//       {
//         if (sphereId == n) continue;
//         if ((distance_centre_spheres(S,sphere[n]) - (S.R + sphere[n].R)) < -max_overlap_rate * rmin) { failure = 128; break; }
//       }
      
      double distance_max = -max_overlap_rate * rmin;
      for (unsigned n = 0 ; n < neighbor.size() ; ++n)
      {
        if ((distance_centre_spheres(S,sphere[neighbor[n].sphereId]) - (S.R + sphere[neighbor[n].sphereId].R)) < distance_max) 
        { failure = 128; break; }
      }
      
    }  

    if (!failure)
    {
      sphere[sphereId].x = S.x;
      sphere[sphereId].y = S.y;
      sphere[sphereId].z = S.z;
      sphere[sphereId].R = S.R;
      return 1;
    }

  }

  return 0;
}

double SpherePadder::distance_vector3 (double V1[],double V2[])
{
  double D[3];
  D[0] = V2[0] - V1[0];
  D[1] = V2[1] - V1[1];
  D[2] = V2[2] - V1[2];
  return ( sqrt(D[0]*D[0] + D[1]*D[1] + D[2]*D[2]) );
}

unsigned int SpherePadder::place_fifth_sphere(unsigned int s1, unsigned int s2, unsigned int s3, unsigned int s4, Sphere& S)
{
  double C1[3],C2[3],C3[3],C4[3];
  double R1,R2,R3,R4;
  C1[0] = sphere[s1].x ; C1[1] = sphere[s1].y ; C1[2] = sphere[s1].z ; R1 = sphere[s1].R;  
  C2[0] = sphere[s2].x ; C2[1] = sphere[s2].y ; C2[2] = sphere[s2].z ; R2 = sphere[s2].R; 
  C3[0] = sphere[s3].x ; C3[1] = sphere[s3].y ; C3[2] = sphere[s3].z ; R3 = sphere[s3].R; 
  C4[0] = sphere[s4].x ; C4[1] = sphere[s4].y ; C4[2] = sphere[s4].z ; R4 = sphere[s4].R; 
  
  unsigned int fail_det          =  1;
  unsigned int fail_delta        =  2;
  unsigned int fail_radius       =  4;
  unsigned int fail_overlap      =  8;
  unsigned int fail_gap          = 16;
  unsigned int fail_radius_range = 32;
  unsigned int fail_NaN          = 64;

  // (x-x1)^2 + (y-y1)^2 + (z-z1)^2 = (r+r1)^2   (1)
  // (x-x2)^2 + (y-y2)^2 + (z-z2)^2 = (r+r2)^2   (2)
  // (x-x3)^2 + (y-y3)^2 + (z-z3)^2 = (r+r3)^2   (3)
  // (x-x4)^2 + (y-y4)^2 + (z-z4)^2 = (r+r4)^2   (4)

  // (2)-(1)
  double a = 2.0 * (C1[0] - C2[0]);
  double b = 2.0 * (C1[1] - C2[1]);
  double c = 2.0 * (C1[2] - C2[2]);
  double d = 2.0 * (R1 - R2);
  double e = (C1[0]*C1[0] + C1[1]*C1[1] + C1[2]*C1[2] - R1*R1) - (C2[0]*C2[0] + C2[1]*C2[1] + C2[2]*C2[2] - R2*R2);

  // (3)-(1)
  double aa = 2.0 * (C1[0] - C3[0]);
  double bb = 2.0 * (C1[1] - C3[1]);
  double cc = 2.0 * (C1[2] - C3[2]);
  double dd = 2.0 * (R1 - R3);
  double ee = (C1[0]*C1[0] + C1[1]*C1[1] + C1[2]*C1[2] - R1*R1) - (C3[0]*C3[0] + C3[1]*C3[1] + C3[2]*C3[2] - R3*R3);

  // (4)-(1)
  double aaa = 2.0 *(C1[0] - C4[0]);
  double bbb = 2.0 *(C1[1] - C4[1]);
  double ccc = 2.0 *(C1[2] - C4[2]);
  double ddd = 2.0 *(R1 - R4);
  double eee = (C1[0]*C1[0] + C1[1]*C1[1] + C1[2]*C1[2] - R1*R1) - (C4[0]*C4[0] + C4[1]*C4[1] + C4[2]*C4[2] - R4*R4);

  // compute the determinant of matrix A (system AX = B)
  //      [a  ,  b,  c];    [x]     [e  -  d*r]
  //  A = [aa ,bb ,cc ];  X=[y]   B=[ee - dd*r]
  //      [aaa,bbb,ccc];    [z]     [eee-ddd*r]

  double DET = a*(bb*ccc-bbb*cc) - aa*(b*ccc-bbb*c) + aaa*(b*cc-bb*c);
  double R = 0.0;
  double centre[3];
  if (DET != 0.0)
  {
    //        [a11,a12,a13]
    // A^-1 = [a21,a22,a23] * 1/det(A)
    //        [a31,a32,a33]  
          
    double inv_DET = 1.0/DET;
    double a11 =  (bb*ccc-bbb*cc) * inv_DET;
    double a12 = -(b*ccc-bbb*c)   * inv_DET;
    double a13 =  (b*cc-bb*c)     * inv_DET;
    double a21 = -(aa*ccc-aaa*cc) * inv_DET;
    double a22 =  (a*ccc-aaa*c)   * inv_DET;
    double a23 = -(a*cc-aa*c)     * inv_DET;
    double a31 =  (aa*bbb-aaa*bb) * inv_DET;
    double a32 = -(a*bbb-aaa*b)   * inv_DET;
    double a33 =  (a*bb-aa*b)     * inv_DET;

    double xa = -(a11*d + a12*dd + a13*ddd);
    double xb =  (a11*e + a12*ee + a13*eee);

    double ya = -(a21*d + a22*dd + a23*ddd);
    double yb =  (a21*e + a22*ee + a23*eee);

    double za = -(a31*d + a32*dd + a33*ddd);
    double zb =  (a31*e + a32*ee + a33*eee);

    // Replace x,y and z in Equation (1) and solve the second order equation A*r^2 + B*r + C = 0

    double A = xa*xa + ya*ya + za*za - 1.0;
    double B = 2.0*(xa*(xb-C1[0])) + 2.0*(ya*(yb-C1[1])) + 2.0*(za*(zb-C1[2])) - 2.0*R1;
    double C = (xb-C1[0])*(xb-C1[0]) + (yb-C1[1])*(yb-C1[1]) + (zb-C1[2])*(zb-C1[2]) - R1*R1;

    double DELTA = B*B - 4.0*A*C;
    double RR1,RR2;

    if (DELTA >= 0.0)
    {
      double inv_2A = 1.0 / (2.0*A);
      double sqrt_DELTA = sqrt(DELTA);
      RR1 = (-B + sqrt_DELTA) * inv_2A;
      RR2 = (-B - sqrt_DELTA) * inv_2A;
    }
    else return fail_delta;

    if      (RR1 > 0.0) R = RR1;
    else if (RR2 > 0.0) R = RR2;
    else if (RR1 <= 0.0 && RR2 <= 0.0) return fail_radius;
    
    if (R < rmin || R > rmax) return fail_radius_range;

    centre[0] = xa * R + xb;
    centre[1] = ya * R + yb;
    centre[2] = za * R + zb;
  }
  else return fail_det;
  
  // FIXME use not sqrt to speed up... (squared_distance_vector3)
  // for the moment it is not critical
  
  // Check interpenetration between spheres
  double distance1 = distance_vector3 (centre,C1) - (R + R1);
  double distance2 = distance_vector3 (centre,C2) - (R + R2);
  double distance3 = distance_vector3 (centre,C3) - (R + R3);
  double distance4 = distance_vector3 (centre,C4) - (R + R4);
  
  double half_distance_rate = -0.5 * max_overlap_rate;
  if (     ( distance1 < half_distance_rate * (R + R1)) 
        || ( distance2 < half_distance_rate * (R + R2))
        || ( distance3 < half_distance_rate * (R + R3)) 
        || ( distance4 < half_distance_rate * (R + R4)) )
  { return fail_overlap; }
  
  // The gap between spheres must not be too large
  double distance_max = max_overlap_rate * rmin;
  if (     ( distance1 > distance_max) 
        || ( distance2 > distance_max)
        || ( distance3 > distance_max) 
        || ( distance4 > distance_max) )
  { return fail_gap; } 
  
  
  // Check if there is NaN 
  if (     (centre[0] != centre[0])
        || (centre[1] != centre[1])
        || (centre[2] != centre[2])
        || (R != R))
  { return fail_NaN; }
  
  S.x = centre[0];
  S.y = centre[1];
  S.z = centre[2];
  S.R = R;
  
  return 0;
}

void SpherePadder::place_at_tetra_centers ()
{
  BEGIN_FUNCTION("Place at tetra centers");
    
  Sphere S;
  S.type = AT_TETRA_CENTER;
  Tetraedre T;
  
  unsigned int ns = sphere.size(); 
  Node N1,N2,N3,N4;

  for (unsigned int t = 0 ; t < mesh->tetraedre.size() ; ++t)
  {
    T = mesh->tetraedre[t];
    N1 = mesh->node[T.nodeId[0]];
    N2 = mesh->node[T.nodeId[1]];
    N3 = mesh->node[T.nodeId[2]];
    N4 = mesh->node[T.nodeId[3]];
    
    S.x = 0.25 * (N1.x + N2.x + N3.x + N4.x); 
    S.y = 0.25 * (N1.y + N2.y + N3.y + N4.y); 
    S.z = 0.25 * (N1.z + N2.z + N3.z + N4.z); 

    S.R = rmin;
                
    S.tetraOwner = t;
    mesh->tetraedre[t].sphereId.push_back(ns++);
    sphere.push_back(S); ++(n4);
  }
        
  for (unsigned int n = (n1+n2+n3) ; n < sphere.size() ; ++n)
  {
    place_sphere_4contacts(n);
  }
        
  END_FUNCTION;  
}

void SpherePadder::place_at_tetra_vertexes ()
{
  BEGIN_FUNCTION("Place at tetra vertexes");
    
  Sphere S;
  S.type = AT_TETRA_VERTEX;
  Tetraedre T;
  
  unsigned int ns = sphere.size(); 
  Node N1,N2,N3,N4;
  double centre[3];

  for (unsigned int t = 0 ; t < mesh->tetraedre.size() ; ++t)
  {
    T = mesh->tetraedre[t];
    N1 = mesh->node[T.nodeId[0]];
    N2 = mesh->node[T.nodeId[1]];
    N3 = mesh->node[T.nodeId[2]];
    N4 = mesh->node[T.nodeId[3]];
    
    centre[0] = 0.25 * (N1.x + N2.x + N3.x + N4.x); 
    centre[1] = 0.25 * (N1.y + N2.y + N3.y + N4.y); 
    centre[2] = 0.25 * (N1.z + N2.z + N3.z + N4.z); 

    S.R = rmin;
                
    double pondere = .333333333; // FIXME parametrable
    for (unsigned int n = 0 ; n < 4 ; ++n)
    {
      S.x = pondere * mesh->node[ T.nodeId[n] ].x + (1.0-pondere) * centre[0];
      S.y = pondere * mesh->node[ T.nodeId[n] ].y + (1.0-pondere) * centre[1];
      S.z = pondere * mesh->node[ T.nodeId[n] ].z + (1.0-pondere) * centre[2];
      
      S.tetraOwner = t;
      mesh->tetraedre[t].sphereId.push_back(ns++);
      sphere.push_back(S); ++(n5);
    }
  }
        
  for (unsigned int n = (n1+n2+n3+n4) ; n < sphere.size() ; ++n)
  {
    place_sphere_4contacts(n);
  }
        
  END_FUNCTION; 
}
    





