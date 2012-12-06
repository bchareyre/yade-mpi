#!/usr/bin/python
# -*- coding: utf-8 -*-

## Demonstrate scene construction using the default list of engines
## Note that it is no longer necessary to define O.dt
## See simple-scene.py for the complete version with explicit engines definition 

typedEngine("NewtonIntegrator").gravity=(0,0,-9.81)
typedEngine("NewtonIntegrator").damping=0.1
O.bodies.append(box(center=[0,0,0],extents=[.5,.5,.5],color=[0,0,1],fixed=True))
O.bodies.append(sphere([0,0,2],1,color=[0,1,0]))
O.save('/tmp/a.xml.bz2');
