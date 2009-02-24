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
  return (d1 > d2) ? 1 :-1;
}

int compare_tetra_porosity (const void * a, const void * b)
{
  double d1 = (*(tetra_porosity *)a).void_volume;
  double d2 = (*(tetra_porosity *)b).void_volume;      
  return (d1 < d2) ? 1 :-1;
}

int compareDouble (const void * a, const void * b)
{
  return ( *(double*)a > *(double*)b ) ? 1 :-1;
}

SpherePadder::SpherePadder()
{
  vector <unsigned int> lst;
  unsigned int nb = 5;
  
  for (unsigned int i = 0 ; i <= nb ; ++i)
    for (unsigned int j = i+1 ; j <= nb+1 ; ++j)
      for (unsigned int k = j+1 ; k <= nb+2 ; ++k)
        for (unsigned int l = k+1 ; l <= nb+3 ; ++l)
  {
    lst.clear();
    lst.push_back(i);
    lst.push_back(j);
    lst.push_back(k);
    lst.push_back(l);
    combination.push_back(lst);
  }
        
   max_overlap_rate = 1e-4;   
   n1 = n2 = n3 = n4 = n5 = n_densify = 0;  
   trace_functions = true;
   meshIsPlugged = false;   
   probeIsDefined = false;          
   ratio = 5.0; rmoy = 0.0;
   virtual_radius_factor = 5.0;
   
/* FIXME
   pour le moment, l'utilisateur ne peut entre qu'un ratio.
   Le rayon des sphere depend du maillage (des longueurs des segments)
*/

}

void SpherePadder::check_inProbe(unsigned int i)
{
  if (!probeIsDefined)    return;
  if (sphere[i].R <= 0.0) return;
  
  double dx = sphere[i].x - xProbe;
  double dy = sphere[i].y - yProbe;
  double dz = sphere[i].z - zProbe;
  double squared_distance = dx*dx + dy*dy + dz*dz;
  if (squared_distance + sphere[i].R*sphere[i].R < RProbe * RProbe) sphereInProbe.push_back(i);
}

void SpherePadder::add_spherical_probe(double Rfact)
{
  double xmin,xmax;
  double ymin,ymax;
  double zmin,zmax;
  double R;
  
  xmin=xmax=mesh->node[0].x;
  ymin=ymax=mesh->node[0].y;
  zmin=zmax=mesh->node[0].z;
  for (unsigned int i = 1 ; i < mesh->node.size() ; ++i)
  {
    xmin = (xmin > mesh->node[i].x) ? mesh->node[i].x : xmin;
    xmax = (xmax < mesh->node[i].x) ? mesh->node[i].x : xmax; 
    ymin = (ymin > mesh->node[i].y) ? mesh->node[i].y : ymin;
    ymax = (ymax < mesh->node[i].y) ? mesh->node[i].y : ymax;
    zmin = (zmin > mesh->node[i].z) ? mesh->node[i].z : zmin;
    zmax = (zmax < mesh->node[i].z) ? mesh->node[i].z : zmax;
  }
  xProbe = 0.5 * (xmin + xmax);
  yProbe = 0.5 * (ymin + ymax);
  zProbe = 0.5 * (zmin + zmax);
  RProbe = (xmax - xmin);
  RProbe = (RProbe < (R = ymax - ymin)) ? RProbe : R;
  RProbe = (RProbe < (R = zmax - zmin)) ? RProbe : R;
  RProbe *= 0.5 * Rfact;
  probeIsDefined = true;
  compacity_file.open("compacity.dat");
}
            
double SpherePadder::compacity_in_probe(unsigned int ninsered)
{
  if (!probeIsDefined) return -1.0;
  
  double dr = 0.0;
  double Vs = 0.0;
  double fact = 1.333333333 * M_PI;
  for (unsigned int i = 1 ; i < sphereInProbe.size() ; ++i)
  {
    Vs += fact * sphere[i].R * sphere[i].R * sphere[i].R;
  }
  
  // TODO ameliorer cette fonction en prennant en compte les spheres coupees...
  
  dr = Vs / (fact*RProbe*RProbe*RProbe);
  compacity_file << ninsered << ' ' << dr << endl;
  return (dr);
}

void SpherePadder::plugTetraMesh (TetraMesh * pluggedMesh)
{
  mesh = pluggedMesh;
  partition.init(*mesh);
  meshIsPlugged = true;
	
  // TODO mettre ce qui suite dans une fonction 'init()'
  // Si l'utilisateur n'a choisi qu'une valeur de ratio, 
  // on cree les valeur de rmin et rmax:
  if (rmoy == 0 && ratio > 0)
  {
	rmoy = 0.125 * mesh->mean_segment_length; // 1/8 = 0.125
	rmin = (2.0 * rmoy) / (ratio + 1.0); 
	rmax = 2.0 * rmoy - rmin; 
	dr = rmax - rmoy;
        cerr << "rmax = " << rmax << endl;
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
  
  time_t start_time = clock();
  
  place_at_nodes();
  //save_granulo("nodes_granulo.dat");
  place_at_segment_middle();
  //save_granulo("segments_granulo.dat");
  cancel_overlaps();
  //save_granulo("cancel_overlaps_granulo.dat");
  place_at_faces();
  //save_granulo("faces_granulo.dat");
  place_at_tetra_centers();
  //save_granulo("tetra_centers_granulo.dat");
  place_at_tetra_vertexes ();
  //save_granulo("tetra_vertexes_granulo.dat");
  //detect_overlap();
  //place_virtual_spheres(); // TODO deplacer dans une fonction 'densify' par exemple
  

  time_t stop_time = clock();
  
  cerr << "Total number of spheres    = " << sphere.size() << endl;
  cerr << "Number at nodes            = " << n1 << endl;
  cerr << "Number at segments         = " << n2 << endl;
  cerr << "Number near faces          = " << n3 << endl;
  cerr << "Number near tetra centers  = " << n4 << endl;
  cerr << "Number near tetra vextexes = " << n5 << endl; 
  
  float time_used = (float)(stop_time - start_time) / 1000000.0;
  cerr << "Time used = " << time_used << " s" << endl;      
}

void SpherePadder::tetra_pad() // EN TRAVAUX !!!!!!!!!!!!!!!!!!!!!!!!!
{
  place_at_nodes();
  place_at_segment_middle();
  cancel_overlaps();
  place_at_faces();
  detect_overlap();
  
  for (unsigned int i = 0 ; i < sphere.size() ; i++) 
  {
    //cout << sphere[i].x << "\t" << sphere[i].y << "\t" << sphere[i].z << "\t" << sphere[i].R << "\t" << i << endl;
    triangulation.insert_node(sphere[i].x, sphere[i].y, sphere[i].z, i, false);
  }
  
  //unsigned int id1=0,id2=0,id3=0,id4=0;
  tetra_porosity P;
  if (!tetra_porosities.empty()) tetra_porosities.clear();
  // if triangulatio empty ...
  triangulation.init_current_tetrahedron();
  do
  {
    triangulation.current_tetrahedron_get_nodes(P.id1,P.id2,P.id3,P.id4);
    P.volume = triangulation.current_tetrahedron_get_volume();
    P.void_volume = P.volume - solid_volume_of_tetrahedron(sphere[P.id1], sphere[P.id2], sphere[P.id3], sphere[P.id4]);
    tetra_porosities.push_back(P);
  } while (triangulation.next_tetrahedron());
  qsort(&(tetra_porosities[0]),tetra_porosities.size(),sizeof(tetra_porosity),compare_tetra_porosity);
  
  for (unsigned int i = 0 ; i < tetra_porosities.size() ; i++) 
  {
    cout << tetra_porosities[i].volume  << "\t" << tetra_porosities[i].void_volume << endl; 
  }
/*
  Sphere S;
  S.type = AT_TETRA_CENTER;
  triangulation.init_current_tetrahedron();
  do
  {
    unsigned int id1,id2,id3,id4;
    triangulation.current_tetrahedron_get_nodes(id1,id2,id3,id4);
   // place_fifth_sphere(id1,id2,id3,id4,S);
    
    S.x = 0.25 * (sphere[id1].x + sphere[id2].x +sphere[id3].x + sphere[id4].x);
    S.y = 0.25 * (sphere[id1].y + sphere[id2].y +sphere[id3].y + sphere[id4].y);
    S.z = 0.25 * (sphere[id1].z + sphere[id2].z +sphere[id3].z + sphere[id4].z);
    S.R = rmin; // rand...
    
    sphere.push_back(S);
    
    //cerr << "add\n";
  } while (triangulation.next_tetrahedron());
  
*/
  
  
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
    //if (sphere[i].R < rmin) continue;
    
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
	  if (mesh->segment[s].nodeId[1] < mesh->node.size())
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

void SpherePadder::save_granulo(const char* name)
{
  vector<double> D;
  for (unsigned int i = 0 ; i < sphere.size() ; ++i)
  {
    if (sphere[i].R <= 0.0) continue; 
    D.push_back(2.0 * sphere[i].R);
  }
  qsort (&(D[0]), D.size(), sizeof(double), compareDouble);
  
  ofstream file(name);
  double inv_N = 1.0 / (double)D.size();
  for (unsigned int i = 0 ; i < D.size() ; ++i)
  {
    file << D[i] << ' ' << (double)i * inv_N << endl;
  }
}
        
void SpherePadder::place_at_nodes ()
{
  BEGIN_FUNCTION("Place at nodes");
        
  unsigned int segId;
  Sphere S;
  S.type = AT_NODE;
        
  for (unsigned int n = 0 ; n < mesh->node.size() ; ++n)
  {
   if (mesh->node[n].segmentOwner.empty()) continue;
  
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
                
    sphere.push_back(S); ++(n1); 
    //check_inProbe(n);compacity_in_probe(n);
    partition.add(n,S.x,S.y,S.z);
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
  unsigned int ns = sphere.size();
        
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
    
    sphere.push_back(S); ++(n2); 
    //check_inProbe(ns);compacity_in_probe(ns);
    partition.add(ns,S.x,S.y,S.z);
                
    mesh->segment[s].sphereId = ns;
    ++ns;
                
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
    
    sphere.push_back(S); ++(n3);
    place_sphere_4contacts(ns);
    //partition.add(ns,S.x,S.y,S.z); // test
    ++ns;
  }
        
//   for (unsigned int n = (n1+n2) ; n < sphere.size() ; ++n)
//   {
//     place_sphere_4contacts(n);
//     check_inProbe(n);compacity_in_probe(n);
//   }
        
  END_FUNCTION;  
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
                
    //S.tetraOwner = t;
    //mesh->tetraedre[t].sphereId.push_back(ns++);
    sphere.push_back(S); ++(n4);
    place_sphere_4contacts(ns);
    ++ns;
  }
        
//   for (unsigned int n = (n1+n2+n3) ; n < sphere.size() ; ++n)
//   {
//     place_sphere_4contacts(n);
//     check_inProbe(n);compacity_in_probe(n);
//   }
        
  END_FUNCTION;  
}

void SpherePadder::place_at_tetra_vertexes ()
{
  BEGIN_FUNCTION("Place at tetra vertexes");
    
  Sphere S;
  S.type = AT_TETRA_VERTEX;
  Tetraedre T;
  
  //unsigned int ns = sphere.size(); 
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
                
    double pondere = 0.333333333; // FIXME parametrable
    for (unsigned int n = 0 ; n < 4 ; ++n)
    {
      S.x = pondere * mesh->node[ T.nodeId[n] ].x + (1.0-pondere) * centre[0];
      S.y = pondere * mesh->node[ T.nodeId[n] ].y + (1.0-pondere) * centre[1];
      S.z = pondere * mesh->node[ T.nodeId[n] ].z + (1.0-pondere) * centre[2];
      
      //S.tetraOwner = t;
      //mesh->tetraedre[t].sphereId.push_back(ns++);
      sphere.push_back(S); ++(n5);
    }
  }
        
  for (unsigned int n = (n1+n2+n3+n4) ; n < sphere.size() ; ++n)
  {
    place_sphere_4contacts(n);
    check_inProbe(n);compacity_in_probe(n);
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

unsigned int SpherePadder::place_sphere_4contacts (unsigned int sphereId, unsigned int nb_combi_max)
{ 
  Sphere S = sphere[sphereId];
  Sphere Sbackup;
//   unsigned int current_tetra_id = S.tetraOwner;
//   Tetraedre current_tetra = mesh->tetraedre[current_tetra_id];
//   unsigned int j;
//   unsigned int tetra_neighbor_id;
//   Tetraedre tetra_neighbor;

  vector<neighbor_with_distance> neighbor;
  //vector<unsigned int> j_ok;
  neighbor_with_distance N;
                
  /*
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
  */
  unsigned int id;
  //Cell& current_cell = partition.get_cell(0,0,0);
  Cell current_cell;
  
  partition.locateCellOf(S.x,S.y,S.z);
  if (!partition.cell_is_found) 
  { 
    cerr << "cell not found\n";
    sphere[sphereId].R = 0.0;
    return 0;
  }
  
  for (unsigned int i = partition.i_down() ; i <= partition.i_up() ; ++i)
  {
    for (unsigned int j = partition.j_down() ; j <= partition.j_up() ; ++j)
    {
      for (unsigned int k = partition.k_down() ; k <= partition.k_up() ; ++k)
      {
        
        current_cell = partition.get_cell(i,j,k);
        for (unsigned int s = 0 ; s < current_cell.sphereId.size() ; ++s)
        {
          id = current_cell.sphereId[s];
          if (id != sphereId)
          {
            N.sphereId = id;
            N.distance = distance_spheres(sphereId,id);
            neighbor.push_back(N);
          }
        }
        
      }
    }
  }
      
  qsort(&(neighbor[0]),neighbor.size(),sizeof(neighbor_with_distance),compare_neighbor_with_distance); 
  S.R += neighbor[0].distance;
  if (S.R >= rmin && S.R <= rmax) sphere[sphereId].R = S.R;
  else if (S.R > rmax)            sphere[sphereId].R = rmax;
  else                            sphere[sphereId].R = 0.0;

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
  unsigned int nb_combi;
  nb_combi = (nb_combi_max < possible_combination.size()) ? nb_combi_max : possible_combination.size();
  Sbackup = S;
  for (unsigned int c = 0 ; c < nb_combi ; ++c)
  {    
    s1 = neighbor[ possible_combination[c][0] ].sphereId;
    s2 = neighbor[ possible_combination[c][1] ].sphereId;
    s3 = neighbor[ possible_combination[c][2] ].sphereId;
    s4 = neighbor[ possible_combination[c][3] ].sphereId;
    
    S = Sbackup;
    failure = place_fifth_sphere(s1,s2,s3,s4,S);
    
    if (!failure)
    {
      
//       for (unsigned n = 0 ; n < sphere.size() ; ++n)
//       {
//         if (sphereId == n) continue;
//         if ((distance_centre_spheres(S,sphere[n]) - (S.R + sphere[n].R)) < -max_overlap_rate * rmin) { failure = 128; break; }
//       }
      
//       double distance_max = -max_overlap_rate * rmin;
//       for (unsigned n = 0 ; n < neighbor.size() ; ++n)
//       {
//         if ((distance_centre_spheres(S,sphere[neighbor[n].sphereId]) - (S.R + sphere[neighbor[n].sphereId].R)) < distance_max) 
//         { failure = 128; break; }
//       }
      
      partition.locateCellOf(S.x,S.y,S.z);
      if (!partition.cell_is_found) 
      { 
        //cerr << "Cell not found !!!!?" << endl;
//         failure = 128;
        
        for (unsigned n = 0 ; n < sphere.size() ; ++n)
        {
          if (sphereId == n) continue;
          if ((distance_centre_spheres(S,sphere[n]) - (S.R + sphere[n].R)) < -max_overlap_rate * rmin) { failure = 128; break; }
        }

//         for (unsigned int i = 0 ; i < partition.isize ; i += partition.isize - 1)
//         {
//           for (unsigned int j = 0 ; j < partition.jsize ; j += partition.jsize - 1)
//           {
//             for (unsigned int k = 0 ; k < partition.ksize ; k += partition.ksize - 1)
//             {
//         
//               current_cell = partition.get_cell(i,j,k);
//               for (unsigned int s = 0 ; s < current_cell.sphereId.size() ; ++s)
//               {
//                 id = current_cell.sphereId[s];
//                 if (id != sphereId)
//                 {
//                   if ((distance_centre_spheres(S,sphere[id]) - (S.R + sphere[id].R)) < -max_overlap_rate * rmin) { failure = 128; break; }
//                 }
//               }
//         
//             }
//           }
//         }
        
      }
      else
      {
        
//         for (unsigned n = 0 ; n < sphere.size() ; ++n)
//         {
//           if (sphereId == n) continue;
//           if ((distance_centre_spheres(S,sphere[n]) - (S.R + sphere[n].R)) < -max_overlap_rate * rmin) { failure = 128; break; }
//         }
        
//         cerr << "sphere.size() = " << sphere.size() << endl;
//         cerr << "current ijk = " << partition.current_i << ", "<< partition.current_j << ", "<< partition.current_k << endl ;
        unsigned int nb = 0;
        for (unsigned int i = partition.i_down() ; i <= partition.i_up() ; ++i)
        {
          for (unsigned int j = partition.j_down() ; j <= partition.j_up() ; ++j)
          {
            for (unsigned int k = partition.k_down() ; k <= partition.k_up() ; ++k)
            {
        
              current_cell = partition.get_cell(i,j,k);
//              cerr << "current_cell.sphereId.size() = " << current_cell.sphereId.size() << endl;
//               cerr << "ijk = " << i << ", " << j << ", " << k << endl;
              nb += current_cell.sphereId.size();
              
              for (unsigned int s = 0 ; s < current_cell.sphereId.size() ; ++s)
              {
                id = current_cell.sphereId[s];
                if (id != sphereId && sphere[id].R > 0.0)
                {
                  if ((distance_centre_spheres(S,sphere[id]) - (S.R + sphere[id].R)) < -max_overlap_rate * rmin) { failure = 128; break; }

                }
              }
        
            }
          }
        }
        
//         cerr << "Total  = " << nb << endl << endl;
        
      }  
            
      
      
    }  

    if (!failure)
    {
      sphere[sphereId].x = S.x;
      sphere[sphereId].y = S.y;
      sphere[sphereId].z = S.z;
      sphere[sphereId].R = S.R;
      partition.add(sphereId,S.x,S.y,S.z);
      return 1;
    }

  }

  if (sphere[sphereId].R > 0.0) partition.add(sphereId,S.x,S.y,S.z);
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

    // Replace x, y and z in Equation (1) and solve the second order equation A*r^2 + B*r + C = 0

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
  
  // FIXME do not use sqrt to speed up... (squared_distance_vector3)
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



 void SpherePadder::place_virtual_spheres()
{
  
  BEGIN_FUNCTION("Place virtual spheres...");
    
  Tetraedre current_tetra, tetra_neighbor;
  
  unsigned int sphereId, current_tetra_id, n1, n2, n3, n4, s1, s2, s3, s4;
  const double div3 = 0.3333333333333;
  Sphere S1,S2,S3,S4,S;
  double sphere_virtual_radius, k;
  double scalar_product;
  double vect1 [3], vect2 [3], vect3 [3], perpendicular_vector [3];

  bool added;
  vector<unsigned int> j_ok;
   
  sphere_virtual_radius = (mesh->mean_segment_length) * virtual_radius_factor;
  k = sphere_virtual_radius;
  sphereId = sphere.size();
  
  
  for (unsigned int f = 0 ; f < mesh->face.size() ; ++f)
  {
  
    if ( mesh->face[f].belongBoundary == true ) 
    {
           
      current_tetra_id = mesh->face[f].tetraOwner[0];
      current_tetra = mesh->tetraedre[current_tetra_id];
    
      n1 = current_tetra.nodeId[0];
      n2 = current_tetra.nodeId[1];
      n3 = current_tetra.nodeId[2];
      n4 = current_tetra.nodeId[3];
    
      
      s1 = mesh->face[f].nodeId[0];
      s2 = mesh->face[f].nodeId[1];
      s3 = mesh->face[f].nodeId[2];
    
    
      if ((n1 != s1) && (n1 != s2) && (n1 != s3))
      {
        s4 = n1;
      }
      else if ((n2 != s1) && (n2 != s2) && (n2 != s3))
      {
        s4 = n2;
      }    
      else if ((n3 != s1) && (n3 != s2) && (n3 != s3))
      {
        s4 = n3;
      }    
      else
      {
        s4 = n4;
      }
   
      S1 = sphere[ s1 ];
      S2 = sphere[ s2 ];
      S3 = sphere[ s3 ];
      S4 = sphere[ s4 ];  
    
      const double inv_distance_sphere_1_2 = 1 / distance_centre_spheres( S1, S2);
      const double inv_distance_sphere_1_3 = 1 / distance_centre_spheres( S1, S3);
      const double inv_distance_sphere_1_4 = 1 / distance_centre_spheres( S1, S4);

      vect1 [0] = (S1.x - S2.x) * inv_distance_sphere_1_2;
      vect1 [1] = (S1.y - S2.y) * inv_distance_sphere_1_2;
      vect1 [2] = (S1.z - S2.z) * inv_distance_sphere_1_2;

      vect2 [0] = (S1.x - S3.x) * inv_distance_sphere_1_3;
      vect2 [1] = (S1.y - S3.y) * inv_distance_sphere_1_3;
      vect2 [2] = (S1.z - S3.z) * inv_distance_sphere_1_3;

      vect3 [0] = (S1.x - S4.x) * inv_distance_sphere_1_4;
      vect3 [1] = (S1.y - S4.y) * inv_distance_sphere_1_4;
      vect3 [2] = (S1.z - S4.z) * inv_distance_sphere_1_4;
    

      perpendicular_vector [0] = (vect1 [1] * vect2 [2]) - (vect1 [2] * vect2 [1]);  
      perpendicular_vector [1] = (vect1 [2] * vect2 [0]) - (vect1 [0] * vect2 [2]);
      perpendicular_vector [2] = (vect1 [0] * vect2 [1]) - (vect1 [1] * vect2 [0]);

      const double inv_norme = 1 / sqrt((perpendicular_vector [0] * perpendicular_vector [0]) + (perpendicular_vector [1] * perpendicular_vector [1]) + (perpendicular_vector [2] * perpendicular_vector [2]));
    
      perpendicular_vector [0] *= inv_norme;  
      perpendicular_vector [1] *= inv_norme;
      perpendicular_vector [2] *= inv_norme;


      scalar_product =  ( (perpendicular_vector [0] * vect3 [0]) + (perpendicular_vector [1] * vect3 [1]) + (perpendicular_vector [2] * vect3 [2]) ) ;

      if ( k < 0) k = -k;   
      if ( scalar_product < 0) k = -k;
   
    // First virtual sphere
      /////////////////////////////////////////////   
      S.x = S1.x + k * perpendicular_vector [0];
      S.y = S1.y + k * perpendicular_vector [1];
      S.z = S1.z + k * perpendicular_vector [2];
      S.R = sphere_virtual_radius;
      S.type = VIRTUAL;
      S.tetraOwner = current_tetra_id;   
      
      added = false;
      for (unsigned int n = 0 ; n < j_ok.size() ; ++n) 
      {
        if (S.x == sphere[ j_ok[n] ].x && S.y == sphere[ j_ok[n] ].y && S.z == sphere[ j_ok[n] ].z) 
        {
          added = true;
          break;
        }
      }   
        
      if (!added) 
      {
        j_ok.push_back(sphereId);
        sphere.push_back(S); 
        partition.add(sphereId++,S.x,S.y,S.z);
      }
    
    // Second virtual sphere
      /////////////////////////////////////////////

      S.x = S2.x + k * perpendicular_vector [0];
      S.y = S2.y + k * perpendicular_vector [1];
      S.z = S2.z + k * perpendicular_vector [2];
      S.R = sphere_virtual_radius;
      S.type = VIRTUAL;
      S.tetraOwner = current_tetra_id;   
      
      added = false;
      for (unsigned int n = 0 ; n < j_ok.size() ; ++n) 
      {
        if (S.x == sphere[ j_ok[n] ].x && S.y == sphere[ j_ok[n] ].y && S.z == sphere[ j_ok[n] ].z) 
        {
          added = true;
          break;
        }
      }   
        
      if (!added) 
      {
        j_ok.push_back(sphereId);
        sphere.push_back(S); 
        partition.add(sphereId++,S.x,S.y,S.z);
      }
    
    // Third virtual sphere
      ///////////////////////////////////////////////   
      S.x = S3.x + k * perpendicular_vector [0];
      S.y = S3.y + k * perpendicular_vector [1];
      S.z = S3.z + k * perpendicular_vector [2];
      S.R = sphere_virtual_radius;
      S.type = VIRTUAL;
      S.tetraOwner = current_tetra_id;   
      
      added = false;
      for (unsigned int n = 0 ; n < j_ok.size() ; ++n) 
      {
        if (S.x == sphere[ j_ok[n] ].x && S.y == sphere[ j_ok[n] ].y && S.z == sphere[ j_ok[n] ].z) 
        {
          added = true;
          break;
        }
      }   
        
      if (!added) 
      {
        j_ok.push_back(sphereId);
        sphere.push_back(S); 
        partition.add(sphereId++,S.x,S.y,S.z);
      }
    
     // Fourth virtual sphere
      ///////////////////////////////////////////// 

      S.x = (S1.x + S2.x + S3.x) * div3 + k * perpendicular_vector [0];
      S.y = (S1.y + S2.y + S3.y) * div3 + k * perpendicular_vector [1];
      S.z = (S1.z + S2.z + S3.z) * div3 + k * perpendicular_vector [2];
      S.R = sphere_virtual_radius;

      S.type = VIRTUAL;
      S.tetraOwner = current_tetra_id;
      sphere.push_back(S);     
      partition.add(sphereId++,S.x,S.y,S.z);
    
      ///////////////////////////////////////////// 
    }

  }
  
  unsigned int id;
  Cell current_cell;
//    cout << "la porosite dans ce tetraedre est de   =   " << calculated_porosity(Sphere& S1, Sphere& S2, Sphere& S3, Sphere& S4)
//Les spheres en interpenetrations avec les spheres virtuelles vont être mises au zéro (A Optimiser)
  double distance_max = -max_overlap_rate * rmin;

  for ( unsigned int n = 0 ; n < j_ok.size() ; ++n) //spheres virtuelles
  { 
    S = sphere[j_ok[n]];             
    partition.locateCellOf(S.x,S.y,S.z);
//         cerr << "sphere.size() = " << sphere.size() << endl;
//         cerr << "current ijk = " << partition.current_i << ", "<< partition.current_j << ", "<< partition.current_k << endl ;
    for (unsigned int i = partition.i_down() ; i <= partition.i_up() ; ++i)
    {
      for (unsigned int j = partition.j_down() ; j <= partition.j_up() ; ++j)
      {
        for (unsigned int k = partition.k_down() ; k <= partition.k_up() ; ++k)
        {
          current_cell = partition.get_cell(i,j,k);
//        cerr << "current_cell.sphereId.size() = " << current_cell.sphereId.size() << endl;
//        cerr << "ijk = " << i << ", " << j << ", " << k << endl;    
          for (unsigned int s = 0 ; s < current_cell.sphereId.size() ; ++s)
          {
            id = current_cell.sphereId[s];
            if (sphere[id].type != VIRTUAL && sphere[id].R > 0.0)
            {
              if ((distance_centre_spheres(S,sphere[id]) - (S.R + sphere[id].R)) < distance_max) sphere[id].R = 0; 

            }
          }
        
        }
      }
    }

  }
            
  END_FUNCTION; 
  
}


void SpherePadder::cancel_overlaps()
{
        
  BEGIN_FUNCTION("Cancel_overlaps");
  //unsigned int current_tetra_id,tetra_neighbor_id,j;
  //Tetraedre current_tetra, tetra_neighbor;
  double distance;
  //double k;
  double distance_max = -max_overlap_rate * rmin;
 
  /*
  for(unsigned int i = 0 ; i <  sphere.size(); ++i)
  {
    if (sphere[i].R <= 0.0) continue;
    
    current_tetra_id = sphere[i].tetraOwner;
    current_tetra    = mesh->tetraedre[current_tetra_id];
                
    for (unsigned int t = 0 ; t < current_tetra.tetraNeighbor.size() ; ++t)
    {
      tetra_neighbor_id = current_tetra.tetraNeighbor[t];
      tetra_neighbor    = mesh->tetraedre[tetra_neighbor_id];
      for (unsigned int n = 0 ; n < tetra_neighbor.sphereId.size() ; ++n)
      {
        j = tetra_neighbor.sphereId[n];

        if (sphere[j].R <= 0.0) continue;
        //if (i < j)
        if (i != j)
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
*/
  unsigned int id;
  Cell current_cell;
  double kfact;
  
  for(unsigned int sphereId = 0 ; sphereId <  sphere.size() ; ++(sphereId))
  {
    
    partition.locateCellOf(sphere[sphereId].x,sphere[sphereId].y,sphere[sphereId].z);
    if (!partition.cell_is_found) continue;
  
//     cerr << "ijk = " << partition.current_i << ", " << partition.current_j << ", " << partition.current_k << endl;
//     cerr << "i down up = " << partition.i_down() << ", " << partition.i_up() << endl;
//     cerr << "j down up = " << partition.j_down() << ", " << partition.j_up() << endl;
//     cerr << "k down up = " << partition.k_down() << ", " << partition.k_up() << endl << endl;
    
    for (unsigned int i = partition.i_down() ; i <= partition.i_up() ; ++i)
    {
      for (unsigned int j = partition.j_down() ; j <= partition.j_up() ; ++j)
      {
        for (unsigned int k = partition.k_down() ; k <= partition.k_up() ; ++k)
        {
          current_cell = partition.get_cell(i,j,k);
          for (unsigned int s = 0 ; s < current_cell.sphereId.size() ; ++s)
          {
            id = current_cell.sphereId[s];
            if (id != sphereId && sphere[id].R > 0.0)
            {
              while ( (distance = distance_spheres(sphereId,id)) < distance_max )
              {                                               
                kfact = 1.0 + distance / (sphere[sphereId].R + sphere[id].R);
                sphere[sphereId].R *= kfact;
                sphere[id].R       *= kfact;
              }
            }
          }
        }
      }
    }
     
  }
  
  
/*
  for(unsigned int i = 0 ; i <  sphere.size() -1 ; ++i)
  {

    for (unsigned int j = i+1 ; j < sphere.size() ; ++j)
      {

        if (sphere[j].R <= 0.0) continue;

        while ( (distance = distance_spheres(i,j)) < distance_max )
          {                                               
            k = 1.0 + distance / (sphere[i].R + sphere[j].R);
            sphere[i].R *= k;
            sphere[j].R *= k;
          }
        
      }
    }
*/
  
  END_FUNCTION;
}



// FIXME attention aux interpenetrations avec rayons nuls
 void SpherePadder::detect_overlap ()
{
  BEGIN_FUNCTION("Detect overlap");
  cerr << endl;
  Sphere S1,S2;
  unsigned int nb_overlap = 0;
  double d;

  //ofstream file("overlap.dat");
  
  for (unsigned int n = 0 ; n < sphere.size()-1 ; ++n)
  {
    S1 = sphere[ n ];
    if ( S1.type != VIRTUAL && S1.R > rmin)
    {
      for (unsigned int t = n+1 ; t < sphere.size() ; ++t)
      {
        S2 = sphere[ t ];
        if ( S2.type != VIRTUAL && S2.R > rmin)
        {
          d = distance_spheres(n, t);
          //if (d <= 0.0) file << d/sphere[n].R << ' ' << d/sphere[t].R << endl;
          
          if (d < -max_overlap_rate * rmin)
          {
            cerr << "Overlap!" << endl;
            partition.locateCellOf(S1.x,S1.y,S1.z);
            cerr << "ijk (S1) = " << partition.current_i << ", " << partition.current_j << ", " << partition.current_k << endl;
            partition.locateCellOf(S2.x,S2.y,S2.z);
            cerr << "ijk (S2) = " << partition.current_i << ", " << partition.current_j << ", " << partition.current_k << endl;
            cerr << "  Types           = " << S1.type << ", " << S2.type << endl;
            cerr << "  Radii           = " << S1.R << ", " << S2.R << endl;
            cerr << "  pos S1          = " << S1.x << ", " << S1.y << ", " << S1.z << endl;
            cerr << "  pos S2          = " << S2.x << ", " << S2.y << ", " << S2.z << endl;
            cerr << "  Distance / rmin = " << d/rmin << endl;
            cerr << "  rmin            = " << rmin << endl;
            sphere[n].type = sphere[t].type = AT_TETRA_VERTEX;
            sphere[n].R = sphere[t].R = 0.0;
            //place_sphere_4contacts( (n > t) ? n : t );
            ++(nb_overlap);
          }
        }
      }
    }
  }
  cerr << "NB Overlap = " << nb_overlap << endl;
  
  END_FUNCTION; 
}        


double SpherePadder::solid_volume_of_tetrahedron(Sphere& S1, Sphere& S2, Sphere& S3, Sphere& S4)
{
  double volume_portion1;
  double volume_portion2;
  double volume_portion3;
  double volume_portion4;
  
  double point1[4];
  point1[0] = S1.x;
  point1[1] = S1.y;
  point1[2] = S1.z;
  point1[3] = S1.R;
  
  double point2[4];
  point2[0] = S2.x;
  point2[1] = S2.y;
  point2[2] = S2.z;
  point2[3] = S2.R;

  double point3[4];
  point3[0] = S3.x;
  point3[1] = S3.y;
  point3[2] = S3.z;
  point3[3] = S3.R;
  
  double point4[4];
  point4[0] = S4.x;
  point4[1] = S4.y;
  point4[2] = S4.z;
  point4[3] = S4.R;
  
  volume_portion1 = spherical_triangle(point1, point2, point3, point4);
  volume_portion2 = spherical_triangle(point2, point1, point3, point4);
  volume_portion3 = spherical_triangle(point3, point1, point2, point4);
  volume_portion4 = spherical_triangle(point4, point1, point2, point3);
/*
  double segment_12[3];
  double segment_23[3];
  double segment_34[3];
  
  segment_12[0] = point1[0] - point2[0];
  segment_12[1] = point1[1] - point2[1];
  segment_12[2] = point1[2] - point2[2];
  
  segment_23[0] = point2[0] - point3[0];
  segment_23[1] = point2[1] - point3[1];
  segment_23[2] = point2[2] - point3[2];  
  
  segment_34[0] = point3[0] - point4[0];
  segment_34[1] = point3[1] - point4[1];
  segment_34[2] = point3[2] - point4[2];
  
  double determinant_matrix[3]; 
  
  determinant_matrix[0] = segment_12[0] * ( segment_23[1]*segment_34[2]  - segment_23[2]*segment_34[1]);
  determinant_matrix[1] = -segment_23[0] * ( segment_12[1]*segment_34[2]  - segment_12[2]*segment_34[1]);
  determinant_matrix[2] = segment_34[0] * ( segment_12[1]*segment_23[2]  - segment_12[2]*segment_23[1]);
  
  const double div6 = 0.16666666666666667;
  double volume_tetrahedron = div6 * sqrt(determinant_matrix[0]*determinant_matrix[0] 
        + determinant_matrix[1]*determinant_matrix[1] + determinant_matrix[2]*determinant_matrix[2]);
  
  return (volume_tetrahedron - volume_portion1 - volume_portion2 - volume_portion3 - volume_portion4 );
  */
  return (volume_portion1 + volume_portion2 + volume_portion3 + volume_portion4);
  
}


double SpherePadder::spherical_triangle (double point1[],double point2[],double point3[],double point4[])
{

  double rayon = point1[3];
  if (rayon == 0.0) return 0.0;
  
  double vect12[3], vect13[3], vect14[3];
  const double pi = 3.14159265;

  vect12[0] = point2[0] - point1[0];
  vect12[1] = point2[1] - point1[1];
  vect12[2] = point2[2] - point1[2];

  vect13[0] = point3[0] - point1[0];
  vect13[1] = point3[1] - point1[1];
  vect13[2] = point3[2] - point1[2];

  vect14[0] = point4[0] - point1[0];
  vect14[1] = point4[1] - point1[1];
  vect14[2] = point4[2] - point1[2];

  double norme12 = sqrt( (vect12[0]*vect12[0]) + (vect12[1]*vect12[1]) + (vect12[2]*vect12[2]) ); 
  double norme13 = sqrt( (vect13[0]*vect13[0]) + (vect13[1]*vect13[1]) + (vect13[2]*vect13[2]) ); 
  double norme14 = sqrt( (vect14[0]*vect14[0]) + (vect14[1]*vect14[1]) + (vect14[2]*vect14[2]) ); 

  double A = acos( (vect12[0]*vect13[0] + vect12[1]*vect13[1] + vect12[2]*vect13[2]) / (norme13 * norme12));
  double B = acos( (vect12[0]*vect14[0] + vect12[1]*vect14[1] + vect12[2]*vect14[2]) / (norme14 * norme12));
  double C = acos( (vect14[0]*vect13[0] + vect14[1]*vect13[1] + vect14[2]*vect13[2]) / (norme13 * norme14));

  double a = acos( (cos(A) - cos(B) * cos(C)) / (sin(B) * sin(C)) );
  double b = acos( (cos(B) - cos(C) * cos(A)) / (sin(C) * sin(A)) );
  double c = acos( (cos(C) - cos(A) * cos(B)) / (sin(A) * sin(B)) );


  double aire_triangle_spherique = rayon * rayon * (a + b + c - pi);

  double aire_sphere = 4 * pi * rayon * rayon;

  // attention division par zero !!!!
  return ( (4 * 0.3333333 * pi * rayon * rayon * rayon) * (aire_triangle_spherique / aire_sphere) ) ;

}




