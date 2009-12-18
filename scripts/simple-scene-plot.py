#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

O.initializers=[
		BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()])
]
O.engines=[
	BexResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[SimpleElasticRelationships()],
		[ef2_Spheres_Elastic_ElasticLaw()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=.2),
	###
	### NOTE this extra engine:
	###
	### You want snapshot to be taken every 1 sec (realTimeLim) or every 50 iterations (iterLim),
	### whichever comes soones. virtTimeLim attribute is unset, hence virtual time period is not taken into account.
	### If there is too much data, they will be reduced automatically (every second number will be ditched) but
	### the upper limit will always be guarded. 
	### 
	### The engine _must_ be labeled 'plotDataCollector', so that the reducer may find it and adjust its periods if necessary.
	###
	PeriodicPythonRunner(iterPeriod=20,command='myAddPlotData()',label='plotDataCollector')
]
from yade import utils
O.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0]))
O.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0]))
O.dt=.2*utils.PWaveTimeStep()


############################################
##### now the part pertaining to plots #####
############################################

from math import *
from yade import plot
## we will have 2 plots:
## 1. t as function of i (joke test function)
## 2. i as function of t on left y-axis ('|||' makes the separation) and z_sph, v_sph (as green circles connected with line) and z_sph_half again as function of t
plot.plots={'i':('t'),'t':('z_sph','|||',('v_sph','go-'),'z_sph_half')}

## this function is called by plotDataCollector
## it should add data with the labels that we will plot
## if a datum is not specified (but exists), it will be NaN and will not be plotted
def myAddPlotData():
	## store some numbers under some labels
	sph=O.bodies[1]
	plot.addData(t=O.time,i=O.iter,z_sph=sph.state.pos[2],z_sph_half=.5*sph.state.pos[2],v_sph=sqrt(sum([v**2 for v in sph.state['vel']])))

O.run(int(2./O.dt),True);
print "Now calling plot.plot() to show the figures (close them to continue)."
plot.plot()
plot.saveGnuplot('/tmp/a')
