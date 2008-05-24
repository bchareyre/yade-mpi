#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

o=Omega()
o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer'),
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingBox2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-9.81]}),
	MetaEngine('PhysicalActionDamper',[
		EngineUnit('CundallNonViscousForceDamping',{'damping':0.2}),
		EngineUnit('CundallNonViscousMomentumDamping',{'damping':0.2})
	]),
	MetaEngine('PhysicalActionApplier',[
		EngineUnit('NewtonsForceLaw'),
		EngineUnit('NewtonsMomentumLaw'),
	]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogPositionIntegrator')]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogOrientationIntegrator')]),
	###
	### NOTE this extra engine
	###
	StandAloneEngine('PlotDataGetter',{'timeInterval':.01,'addPlotDataCall':'myAddPlotData()'})
]
from yade import utils
o.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=2400))
o.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0],young=30e9,poisson=.3,density=2400))
o.dt=.2*utils.PWaveTimeStep()


############################################
##### now the part pertaining to plots #####
############################################

from math import *
import yade.plot
## we will have 2 plots:
## 1. t as function of i (joke test function)
## 2. z_sph and v_sph (rendered as green triangles, 'g^') as function of t
yade.plot.plots={'i':('t'),'t':('z_sph',('v_sph','g^'))}

## static var to specify max number of samples we want to have (1000 is default, no change)
# yade.plot.maxDataLen=1000
## this function will be called by PlotDataGetter
## it should add data with the labels that we will plot
## if a datum is not specified (but exists), it will be NaN and will not be plotted
def myAddPlotData():
	## store some numbers under some labels
	sph=o.bodies[1]
	yade.plot.addData({'t':o.time,'i':o.iter,'z_sph':sph.phys['se3'][2],'v_sph':sqrt(sum([v**2 for v in sph.phys['velocity']]))})

o.run();
print """Now, you can say

 yade.plot.show()

to see figures. Calculation will be suspended until all plot windows will have been closed.
"""
