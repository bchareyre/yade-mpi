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

#define DEBUG

unsigned int           mesh_format;
vector <unsigned int>  output_format;
char                   mesh_file_name[100];
char                   output_file_name[100];
void resume();

int main()
{ 
#ifdef DEBUG
  
 
  TetraMesh * mesh = new TetraMesh();
  //mesh->read_gmsh("meshes/test2.msh");      
  mesh->read("meshes/test.tetra");
  
  SpherePadder * padder = new SpherePadder();
  padder->plugTetraMesh(mesh);
  //padder->add_spherical_probe(0.7);
        
  padder->pad_5();
        
 // padder->save_mgpost("mgp.out.001");
  // padder->save_Rxyz("spheres.Rxyz");
  return 0;  

#else
  
  char name_with_ext[100];
  
  bool mesh_format_is_defined      = false;
  bool mesh_file_name_is_defined   = false;  
  bool output_file_name_is_defined = false;
      
  // default parameters
  mesh_format = 1;
  strcpy(mesh_file_name,"unknown");
  strcpy(output_file_name,"unknown");
  
  TetraMesh    * mesh   = new TetraMesh();
  SpherePadder * padder = new SpherePadder();
  
  unsigned int answer = 99;
  
  while(1)
  {
    answer = 99;
    cout << endl;
    cout << "1. Define mesh format" << endl;
    cout << "2. Define mesh file name" << endl;
    cout << "3. Define output format" << endl;
    cout << "4. Define output file name" << endl;
    cout << "5. Resume" << endl;
    cout << "6. GO" << endl;
    cout << endl;
    cout << "0. Quit" << endl;
    cout << endl << "> ";
    cin >> answer;
    
    switch (answer)
    {
      case 0: return 0;
      
      case 1:
        answer = 99;
        cout << endl;
        cout << "1. SpherePadder" << endl;
        cout << "2. gmsh" << endl;
        cout << endl << "> ";
        cin >> answer;
        if (answer >= 1 && answer <=2)
        {
          mesh_format = answer;
          mesh_format_is_defined = true;
        }
        break;
      
      case 2:
        cout << "Enter mesh file name: ";
        cin >> mesh_file_name;
        mesh_file_name_is_defined = true;
        resume();
        break;
        
      case 3:
        answer = 99;
        cout << endl;
        cout << "1. mgpost" << endl;
        cout << "2. Rxyz" << endl;
        cout << endl;
        cout << "0. Done" << endl;
        cout << endl << "> ";
        cin >> answer;
        
        if (answer == 0) break;
        else if (answer >= 1 && answer <= 2)
        {
          bool added = false;
          for (unsigned int i = 0 ; i < output_format.size() ; i++)
          {
            if (answer == output_format[i]) { added = true; break;} 
          }
          if (!added) output_format.push_back(answer);
        }
        break;
        
      case 4:
        cout << "Enter output file name (without extension): ";
        cin >> output_file_name;
        output_file_name_is_defined = true;
        resume();
        break;
        
        case 5: resume(); break;        
        
      case 6:
        
        if (!mesh_file_name_is_defined)
        {
          cout << "mesh file name is not defined!" << endl;
          break; 
        }
        
        if (!output_file_name_is_defined)
        {
          cout << "output file name is not defined!" << endl;
          break; 
        }
          
        if (mesh_format_is_defined)
        {
          switch (mesh_format)
          {
            case 1:
              mesh->read(mesh_file_name);
              break;
              
            case 2:
              mesh->read_gmsh(mesh_file_name);
              break;
          }
        }
        else
        {
          cout << "mesh format is not defined!" << endl;
          break; 
        }
        
        if (!(padder->meshIsPlugged)) padder->plugTetraMesh(mesh);
        
        if (output_format.empty())
        {
          cout << "output format not defined!" << endl;
          break; 
        }
        
        padder->pad_5(); // TODO controlled by user...
        
        for (unsigned int i = 0 ; i < output_format.size() ; i++)
        {
          switch (output_format[i])
          {
            case 1:
              strcpy(name_with_ext,output_file_name);
              strcat(name_with_ext,".mgp");
              padder->save_mgpost(name_with_ext);
              cout << "file " << name_with_ext << " has been created" << endl;
              break;
              
            case 2:
              strcpy(name_with_ext,output_file_name);
              strcat(name_with_ext,".Rxyz");
              padder->save_Rxyz(name_with_ext);
              cout << "file " << name_with_ext << " has been created" << endl;
              break;
          }
        }
        
        return 0;
      
      default: 
        break;
      
    }
  }
        
  return 0;
  
#endif // DEBUG 
}


void resume()
{
  cout << "--------------------------------------------------------" << endl;
  cout << "mesh format: ";
  switch (mesh_format)
  {
    case 1: cout << "SpherePadder" << endl; break;
    case 2: cout << "gmsh" << endl;break;
    default: cout << "unknown" << endl;
  }
  
  cout << "mesh file name: " << mesh_file_name << endl;

  cout << "output format: ";
  if ((!output_format.empty()))
  {
    cout << "[";
    for (unsigned int i = 0 ; i < output_format.size() ; i++)
    {
      switch (output_format[i])
      {
        case 1: cout << " mgpost ";break;
        case 2: cout << " Rxyz ";break;
        default: break;
      }
    }
    cout << "]" << endl;
  }
  else cout << "unknown" << endl;
    
  cout << "output file name (without extension): " << output_file_name << endl;
  
  cout << "--------------------------------------------------------" << endl;
}



