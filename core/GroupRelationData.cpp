/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GroupRelationData.hpp"

GroupRelationData::GroupRelationData() : ngrp_(2), npar_(0)
{ 
  isActivated_ = false;
}

GroupRelationData::GroupRelationData(unsigned int ngrp) : ngrp_(ngrp), npar_(0)
{ 
  isActivated_ = false;
}

GroupRelationData::~GroupRelationData() 
{
  // Free memory for the action table
/*
  if (act_ != 0)
    {
    for(unsigned int i=0 ; i<ngrp_ ; ++i)
      {
      delete [] act_[i];
      }
    delete [] act_;
    act_ = 0;
    }
  */

  // Free memory for each parameter table
  for(unsigned int p=0 ; p<npar_ ; ++p)
    {
    if (lpar_[p] != 0)
      {
      for(unsigned int i=0 ; i<ngrp_ ; ++i)
        {
        delete [] lpar_[p][i];
        }
      delete [] lpar_[p];
      lpar_[p] = 0;
      }
    }
}

// void GroupRelationData::activate()
// {
//   istringstream istr;
//   istr.str(commands_);
//   read(istr);
// }

/*
bool GroupRelationData::act(unsigned int g1, unsigned int g2) const
{
  if (g1 < ngrp_ && g2 < ngrp_)
    return act_[g1][g2];
  else
    cerr << "@GroupRelationData::act, bad groupMask number" << endl;
  
  return false;
}

void GroupRelationData::activate(unsigned int g1, unsigned int g2)
{
  if (g1 < ngrp_ && g2 < ngrp_)
    {
    act_[g1][g2] = true;
    act_[g2][g1] = true;
    }
  else
    cerr << "@GroupRelationData::activate, bad groupMask number" << endl;
}

void GroupRelationData::deactivate(unsigned int g1, unsigned int g2)
{
  if (g1 < ngrp_ && g2 < ngrp_)
    {
    act_[g1][g2] = false;
    act_[g2][g1] = false;
    }
  else
    cerr << "@GroupRelationData::deactivate, bad groupMask number" << endl;
}
*/

bool GroupRelationData::exists(string name)
{
  map<string, unsigned int >::const_iterator ip = idParam_.find(name);
  if (ip == idParam_.end()) return false;
  else return true;
}

unsigned int GroupRelationData::getId(string name)
{
  map<string, unsigned int >::const_iterator ip = idParam_.find(name);
  if (ip != idParam_.end())
    {
    return (unsigned int) ip->second;
    }
  else
    {
    return npar_;
    }    
}

double GroupRelationData::getParameter(string name, unsigned int g1, unsigned int g2) const
{
  // Retrieve the parameter identifier
  unsigned int idPar;
  map<string, unsigned int >::const_iterator ip = idParam_.find(name);
  if (ip != idParam_.end())
    {
    idPar = ip->second;
    }
  else
    {
    cerr << "@GroupRelationData::getParameter, parameter " << name << " not found" << endl;
    return 0.0;
    }
  
  // Retrieve the value
  if (g1 < ngrp_ && g2 < ngrp_)
    {
    return lpar_[idPar][g1][g2];
    }
  else
    {
    cerr << "@GroupRelationData::getParameter, bad groupMask number" << endl;
    }
  
  return 0.0;    
}

double GroupRelationData::getParameterQuickly(unsigned int idPar, unsigned int g1, unsigned int g2) const
{
  // Here we have some confidence in the user in order to access more quickly the parameters !
  return lpar_[idPar][g1][g2];    
}

void GroupRelationData::setParameter(string name, unsigned int g1, unsigned int g2, double value)
{
  // Retrieve the parameter identifier
  unsigned int idPar;
  map<string, unsigned int >::const_iterator ip = idParam_.find(name);
  if (ip != idParam_.end())
    {
    idPar = ip->second;
    }
  else
    {
    cerr << "@GroupRelationData::setParameter, unknown parameter" << name << endl;
    return;
    }
  
  // Affect the value
  if (g1 < ngrp_ && g2 < ngrp_)
    {
    lpar_[idPar][g1][g2] = value;
    lpar_[idPar][g2][g1] = value;
    }
  else
    {
    cerr << "@GroupRelationData::setParameter, bad groupMask number" << endl;
    }
  
  return;    
}

void GroupRelationData::addParameter(string name)
{
  double ** p = 0;
  
  //idParam_[name] = npar_++;
  idParam_.insert(mapIdParam::value_type(name,npar_++));
  
  p = new double * [ngrp_];
  if (p != 0)
    {
    for(unsigned int i = 0 ; i < ngrp_ ; ++i)
      {
      p[i] = new double [ngrp_];
      }
    }
  else
    cerr << "@GroupRelationData::addParameter, allocation problem" << endl;
  
  for (unsigned int i = 0 ; i < ngrp_ ; ++i)
    for (unsigned int j = 0 ; j < ngrp_ ; ++j)
      p[i][j] = 0.0;
  
  lpar_.push_back(p);
}

/*
void GroupRelationData::initActivator()
{
  act_ = new bool * [ngrp_];
  for(unsigned int i = 0 ; i < ngrp_ ; ++i)
    {
    act_[i] = new bool [ngrp_];
    }
  
  // By default, all bodies can act on all other bodies 
  for (unsigned int i = 0 ; i < ngrp_ ; ++i)
    for (unsigned int j = 0 ; j < ngrp_ ; ++j)
      act_[i][j] = true;
}
*/


void GroupRelationData::read(istream & is)
{
  string token;
  
  is >> token;	
  while(is)
    {	
    if      (token == "ngrp")      
      {
      is >> ngrp_;
      if (ngrp_ == 0) cerr << "GroupRelationData::read, ngrp can not be 0" << endl;
      }
    else if (token == "parameter")
      { 
      if (ngrp_ == 0) cerr << "GroupRelationData::read, ngrp can not be 0" << endl;
      string name;
      is >> name;
      addParameter(name);
      }
    else if (token == "setall")
      { 
      string parName;
      double value;
      
      is >> parName >> value;
      
      for (unsigned int g1=0;g1<ngrp_;++g1)
        for (unsigned int g2=0;g2<ngrp_;++g2)
          setParameter(parName,g1,g2,value);
      }
    else if (token == "set")
      { 
      string parName;
      unsigned int g1,g2;
      double value;
      
      is >> parName >> g1 >> g2 >> value;
      
      setParameter(parName,g1,g2,value);
      }
    else if (token == "}") break;
    else cerr << "@GroupRelationData::read, Unknown token: " << token << endl;
    
    is >> token;
    }

    if (ngrp_ > 0) isActivated_ = true;
}

/*
void GroupRelationData::write(ostream & os)
{
  os << "ngrp " << ngrp_ << endl;

  mapIdParam::iterator imap = idParam_.begin();
  string parName;
  while (imap != idParam_.end())
    {
    parName = imap->first;
    os << "parameter " << parName << endl;
    
    for (unsigned int g1=0;g1<ngrp_;++g1)
      {
      for (unsigned int g2=0;g2<ngrp_;++g2)  
        {
        if (lpar_[imap->second][g1][g2]) 
          {
          os << "set " << parName << " " 
          << g1 << " " << g2 << " " 
          << lpar_[imap->second][g1][g2] << endl;
          }
        }
      }
    
    ++imap;
    }
  
}
*/

void GroupRelationData::preProcessAttributes(bool deserializing)
{
  if(deserializing)
  {
    //
  }
}


void GroupRelationData::postProcessAttributes(bool deserializing)
{
  if(deserializing)
  {
    string cmdstring = "";
    for (unsigned int i = 0 ; i< commands_.size() ; ++i)
      cmdstring = cmdstring + commands_[i] + " ";

    istringstream istr;
    istr.str(cmdstring);
    
    cout << istr.str() << endl;

    read(istr);
  }
}




