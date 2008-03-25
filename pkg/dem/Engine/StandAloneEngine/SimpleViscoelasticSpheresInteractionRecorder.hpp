/*************************************************************************
*  Copyright (C) 2006 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef SIMPLEVISCOELASTICSPHERESINTERACTIONRECORDER_HPP
#define SIMPLEVISCOELASTICSPHERESINTERACTIONRECORDER_HPP

#include <yade/core/DataRecorder.hpp>
#include <yade/pkg-dem/SpheresContactGeometry.hpp>
#include <yade/pkg-dem/SimpleViscoelasticInteraction.hpp>
#include <set>

/// Save SpheresContactGeometry and SimpleViscoelasticInteraction of the
/// interaction between the body Id1 and the body Id2 into the file
/// outputBaseId1-Id2.
class SimpleViscoelasticSpheresInteractionRecorder : public DataRecorder {
private: 
    shared_ptr<SpheresContactGeometry> interactionSphere;
    shared_ptr<SimpleViscoelasticInteraction> viscoelasticInteraction;
    set<pair<int,int> > interaction_ids;

public :
    /// Base name of output file.
    string outputBase;
    /// Saves interval
    int interval;

    SimpleViscoelasticSpheresInteractionRecorder();
    ~SimpleViscoelasticSpheresInteractionRecorder();

    virtual void action(Body* body);
    bool isActivated();

protected :
    virtual void postProcessAttributes(bool deserializing);
    void registerAttributes();
    REGISTER_CLASS_NAME(SimpleViscoelasticSpheresInteractionRecorder);
    REGISTER_BASE_CLASS_NAME(DataRecorder);
};
REGISTER_SERIALIZABLE(SimpleViscoelasticSpheresInteractionRecorder,false);

#endif // SIMPLEVISCOELASTICSPHERESINTERACTIONRECORDER_HPP
