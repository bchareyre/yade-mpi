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
	StandAloneEngine('GlobalStiffnessTimeStepper',{'defaultDt':1e-4,'active':True,'timeStepUpdateInterval':50}),
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

s=Body()
s.shape=GeometricalModel('Sphere',{'radius':1,'diffuseColor':[0,1,0]})
s.mold=InteractingGeometry('InteractingSphere',{'radius':1,'diffuseColor':[1,0,0]})
s.phys=PhysicalParameters('BodyMacroParameters',{'se3':[0,0,2,1,0,0,0],'mass':1000,'inertia':[7e4,7e4,7e4],'young':3e9,'poisson':0.3})
s.bound=BoundingVolume('AABB',{'diffuseColor':[0,0,1]})
s['isDynamic']=True

b=Body()
b.shape=GeometricalModel('Box',{'extents':[.5,.5,.5],'diffuseColor':[1,0,0]})
b.mold=InteractingGeometry('InteractingBox',{'extents':[.5,.5,.5],'diffuseColor':[0,1,0]})
b.phys=PhysicalParameters('BodyMacroParameters',{'se3':[0,0,0,1,0,0,0],'mass':2000,'inertia':[1e5,1e5,1e5],'young':3e9,'poisson':0.3})
b.bound=BoundingVolume('AABB',{'diffuseColor':[0,0,1]})
b['isDynamic']=False

o.bodies.append(b)
o.bodies.append(s)

o.save('/tmp/a.xml')

# load that with the QtGUI
import os
os.system(yadeExecutable+' -N QtGUI -S /tmp/a.xml')

