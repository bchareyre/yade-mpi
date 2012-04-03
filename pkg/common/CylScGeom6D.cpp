#include "CylScGeom6D.hpp"
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>

CylScGeom::~CylScGeom() {}
CylScGeom6D::~CylScGeom6D() {}



void CylScGeom6D::precomputeRotations(const State& rbp1, const State& rbp2, bool isNew, bool creep) {
    initRotations(rbp1,rbp2);
}

void CylScGeom6D::initRotations(const State& state1, const State& state2)
{
    initialOrientation1	= state1.ori;
    initialOrientation2	= state2.ori;
    twist=0;
    bending=Vector3r::Zero();
    twistCreep=Quaternionr(1.0,0.0,0.0,0.0);
}

YADE_PLUGIN((CylScGeom6D)(CylScGeom));

