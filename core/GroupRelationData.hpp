/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

//! \brief  Define the visibility and the parameters between groups of elements
//! \author V. Richefeu

// Note that the parameters of this class cannot be serialized yet


// bool **act_
// |
// +------------------------------------+
// |     |   1     2     4        ...   |  <---- groupMasks
// | 1   |   YES   YES   YES      ...   |
// | 2   |         NO    YES      ...   |
// | 4   |  (SYM.)       YES      ...   |
// | ... |                        ...   |
// +------------------------------------+
//   ^groupMasks 


//  vector <double **> lpar_
//  |
//  |   +------------------------------------+
//  |   | Kn  |   1     2     4        ...   |  <---- groupMasks
//  |   | 1   |   1e8   1e8   1e10     ...   |
//  +---| 2   |         xx    1e8      ...   |
//  |   | 4   |  (SYM.)       1e8      ...   |
//  |   | ... |                        ...   |
//  |   +------------------------------------+
//  |     ^groupMasks 
//  |
//  |   +------------------------------------+
//  |   | nu  |   1     2     4        ...   |  <---- groupMasks
//  |   | 1   |   0.4   0.3   0.3      ...   |
//  +---| 2   |         xx    0.0      ...   |
//  |   | 4   |  (SYM.)       0.5      ...   |
//  |   | ... |                        ...   |
//  |   +------------------------------------+
//  |     ^groupMasks 
//  ...

class GroupRelationData
{
  typedef map<string ,unsigned int > mapIdParam;

private:

  unsigned int ngrp_;                   // Number of groups
  unsigned int npar_;                   // Number of parameters

  map <string ,unsigned int > idParam_; // Hash table for parameter identifiers
  bool ** act_;                         // Table of visibility 
  vector <double **> lpar_;             // Table of parameter values
// FIXME - the following should be better in particular for serialization (?)
// vector<vector<bool> > act_;
// vector<vector<vector<double> > > lpar_;
    
public:
  
  GroupRelationData(); 
  GroupRelationData(unsigned int ngrp); 
  ~GroupRelationData();
  
  //! Return true if the body with groupMask g1 'see' the body with groupMask g2
  //! \param g1  First groupMask
  //! \param g2  Second groupMask  
  bool act(unsigned int g1, unsigned int g2) const;

  //! Activate the visibility of the bodies with groupMasks g1 and g2
  //! \param g1  First groupMask
  //! \param g2  Second groupMask  
  void activate(unsigned int g1, unsigned int g2);
  
  //! Deactivate the visibility of the groups with groupMasks g1 and g2
  //! \param g1  First groupMask
  //! \param g2  Second groupMask
  void deactivate(unsigned int g1, unsigned int g2);
  
  //! Return true if the parameter exist
  //! \param name  Parameter name 
  bool exists(string name);
  
  //! Get a parameter identifier from his name (returns the number of parameters if name is not found)
  //! \param name  Parameter name
  unsigned int getId(string name);
  
  //! Get a parameter value from his name (returns 0 in case of failure)
  //! \param name  Parameter name 
  //! \param g1    First groupMask
  //! \param g2    Second groupMask
  double getParameter(string name, unsigned int g1, unsigned int g2) const;
  
  //! Get quickly a parameter value from an identifier (crashes in case of failure)
  //! \param idPar Parameter identifier (see function getId) 
  //! \param g1    First groupMask
  //! \param g2    Second groupMask
  double getParameterQuickly(unsigned int idPar, unsigned int g1, unsigned int g2) const;
  
  //! Set a parameter value
  //! \param name  Name of the parameter 
  //! \param g1    First groupMask
  //! \param g2    Second groupMask
  //! \param value Parameter value
  void setParameter(string name, unsigned int g1, unsigned int g2, double value);

  //! Add a new parameter
  void addParameter(string name);
  
  //! 
  void initActivator();
  
// Was used in the code gdm-tk, but not usefull in YADE
/*
  void read(istream & is);  
  void write(ostream & os);
*/
  

};

