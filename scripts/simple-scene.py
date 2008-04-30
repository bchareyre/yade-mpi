#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-

o=Omega() # this creates default rootBody as well

# is used in both initializers and engines, assign to a temporary
aabbDisp=MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])

o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer',{'physicalActionNames':['Force','Momentum','GlobalStiffness']}),
	aabbDisp]

o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	aabbDisp,
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	StandAloneEngine('GlobalStiffnessCounter',{'interval':50}),
	StandAloneEngine('GlobalStiffnessTimeStepper',{'defaultDt':1e-5,'active':True,'timeStepUpdateInterval':50}),
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
]


from yade import utils
o.bodies.append(utils.box(extents=[.5,.5,.5],center=[0,0,0],dynamic=False,color=[1,0,0]))
o.bodies.append(utils.sphere(1,[0,0,2],color=[0,1,0]))
# o.dt=.2*utils.PWaveTimeStep()

o.save('/tmp/a.xml')
print "===================== SAVING FINISHED ====================="
o.load('/tmp/a.xml')
import sys
sys.exit(0)

if True: # shorter, but doesn't test the (de)serializer, since binary format is used
	# will run in background
	utils.runInQtGui()
else:
	## we could save it to a file just as well
	o.save('/tmp/a.xml')
	# load that with the QtGUI
	import os,yade.runtime
	os.system(yade.runtime.executable+' -N QtGUI -S /tmp/a.xml &')

