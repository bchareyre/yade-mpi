/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <yade/core/DataRecorder.hpp>
#include <yade/pkg-common/RigidBodyParameters.hpp>
#include <set>

/// Saves RigidBodyParameters of the body Id in the file outputBaseId,
class RigidBodyRecorder : public DataRecorder {
    private:
	shared_ptr<RigidBodyParameters> rigidBodyParameters;
	set<int> body_ids;


public :
    string outputBase; 
    int interval;


    RigidBodyRecorder ();
    ~RigidBodyRecorder ();

    virtual void action(MetaBody*);
      bool isActivated(MetaBody*);

protected :
    virtual void postProcessAttributes(bool deserializing);
	 REGISTER_ATTRIBUTES(DataRecorder,(outputBase)(interval));
    REGISTER_CLASS_NAME(RigidBodyRecorder);
    REGISTER_BASE_CLASS_NAME(DataRecorder);
};
REGISTER_SERIALIZABLE(RigidBodyRecorder );


