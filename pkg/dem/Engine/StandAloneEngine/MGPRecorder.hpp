/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/core/DataRecorder.hpp>
#include <yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/GeometricalModel.hpp>
//#include <set>


class MGPRecorder : public DataRecorder 
{
private:
    //shared_ptr<RigidBodyParameters> rigidBodyParameters;
    //set<int> body_ids;


public :
    string outputBase; 
    int interval;
    int stateId;

    MGPRecorder ();
    ~MGPRecorder ();

    virtual void action(MetaBody*);
    bool isActivated();

protected :
    virtual void postProcessAttributes(bool deserializing);
    void registerAttributes();
    REGISTER_CLASS_NAME(MGPRecorder);
    REGISTER_BASE_CLASS_NAME(DataRecorder);
};

REGISTER_SERIALIZABLE(MGPRecorder);


