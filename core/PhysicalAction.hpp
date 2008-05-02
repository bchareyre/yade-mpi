/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTION_HPP
#define PHYSICALACTION_HPP


#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

/* Helper macro for Engines and Engine units to declare what PhysicalParameters (Bex, BodyExternalVariables) they want to use */
#define NEEDS_BEX(...) public: virtual std::list<std::string> getNeededBex(){ const char* bex[]={ __VA_ARGS__ "", NULL}; std::list<std::string> ret; for(int i=0; bex[i]!=NULL;i++){ret.push_back(std::string(bex[i]));} return ret; }

class PhysicalAction : public Serializable, public Indexable
{
	public :
// FIXME - correct usage of this class, so that functions add(), etc.. are actually used!
//		virtual void add(const shared_ptr<PhysicalAction>& )	{throw;};
//		virtual void sub(const shared_ptr<PhysicalAction>& )	{throw;};

		virtual void reset() 				{throw;};
		virtual shared_ptr<PhysicalAction> clone()	{throw;};
	
/// Serialization										///
	REGISTER_CLASS_NAME(PhysicalAction);

/// Indexable											///
	REGISTER_INDEX_COUNTER(PhysicalAction);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
};

REGISTER_SERIALIZABLE(PhysicalAction,false);

#endif // PHYSICALACTION_HPP

