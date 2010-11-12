/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

// conditionally disable GroupRelationData, remove the #if 0 to re-enable
// is anyone using those? Vincent?

#if 0
#define YADE_GROUP_RELATION_DATA


#include<yade/lib/serialization/Serializable.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

//! \brief  Define the parameters between groupMasks
//! \author V. Richefeu


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

class GroupRelationData : public Serializable
{
  typedef map<string ,unsigned int > mapIdParam;

private:

  unsigned int ngrp_;                   // Number of groupMasks
  unsigned int npar_;                   // Number of parameters

  map <string ,unsigned int > idParam_; // Hash table for parameter identifiers
  vector <double **> lpar_;             // Table of parameter values

  bool isActivated_;
  //string commands_;
  vector<string> commands_;
    
public:
  
  GroupRelationData(); 
  GroupRelationData(unsigned int ngrp); 
  ~GroupRelationData();
  
  //void activate();
  bool isActivated() {return isActivated_;}

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
  
  void read(istream & is);  
  void write(ostream & os);

  REGISTER_ATTRIBUTES(Serializable,(commands_));
  REGISTER_CLASS_AND_BASE(GroupRelationData,Serializable);
 
  public:
	void postLoad(GroupRelationData&);
};
REGISTER_SERIALIZABLE(GroupRelationData);

#endif
