# encoding: utf-8
from math import *


# "instantiate" Omega, i.e. create proxy object to Omega and rootBody
o=Omega()

# we will use this in both initializers and engines, so we save it to a temp variable to save typing
aabbDispatcher=MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])

o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer',{'physicalActionNames':['Force','Momentum','GlobalStiffness']}),
	aabbDispatcher]

o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	aabbDispatcher,
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingBox2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	StandAloneEngine('GlobalStiffnessCounter',{'interval':500}),
	StandAloneEngine('GlobalStiffnessTimeStepper',{'defaultDt':1e-4,'active':True,'timeStepUpdateInterval':500}),
	DeusExMachina('AxialGravityEngine',{'axisPoint':[0,0,0],'axisDirection':[1,0,0],'acceleration':100}),
	MetaEngine('PhysicalActionDamper',[
		EngineUnit('CundallNonViscousForceDamping',{'damping':0.4}),
		EngineUnit('CundallNonViscousMomentumDamping',{'damping':0.4})
	]),
	MetaEngine('PhysicalActionApplier',[
		EngineUnit('NewtonsForceLaw'),
		EngineUnit('NewtonsMomentumLaw'),
	]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogPositionIntegrator')]),
	MetaEngine('PhysicalParametersMetaEngine',[EngineUnit('LeapFrogOrientationIntegrator')]),
]

#shorthand functions
def newSphere(center,radius):
	s=Body()
	s.shape=GeometricalModel('Sphere',{'radius':radius,'diffuseColor':[0,1,0]})
	s.mold=InteractingGeometry('InteractingSphere',{'radius':radius,'diffuseColor':[1,0,0]})
	s.phys=PhysicalParameters('BodyMacroParameters',{'se3':center+[1,0,0,0],'mass':1000,'inertia':[7e4,7e4,7e4],'young':3e9,'poisson':0.3})
	s.bound=BoundingVolume('AABB',{'diffuseColor':[0,0,1]})
	s['isDynamic']=True
	return s

def newBox(center,extents):
	b=Body()
	b.shape=GeometricalModel('Box',{'extents':extents,'diffuseColor':[1,0,0]})
	b.mold=InteractingGeometry('InteractingBox',{'extents':extents,'diffuseColor':[0,1,0]})
	b.phys=PhysicalParameters('BodyMacroParameters',{'se3':[center[0],center[1],center[2],1,0,0,0],'mass':2000,'inertia':[1e5,1e5,1e5],'young':3e9,'poisson':0.3})
	b.bound=BoundingVolume('AABB',{'diffuseColor':[0,0,1]})
	return b

rCenter=4
rBall=.2
wallThickness=2*rBall
wallSize=5*rCenter
wallDist=5*rBall
#central cylinder composed of several spheres
nMax=10
for n in range(nMax):
	x=wallDist*(n/nMax-.5)
	sCent=newSphere([x,0,0],rCenter);
	o.bodies.append(sCent)

#lateral walls, they have wallDist gap inbetween
wLat1=newBox([0+.5*wallDist+.5*wallThickness,0,0],[.5*wallThickness,wallSize,wallSize]); o.bodies.append(wLat1);
wLat2=newBox([0-.5*wallDist-.5*wallThickness,0,0],[.5*wallThickness,wallSize,wallSize]); o.bodies.append(wLat2);

#angle walls, they cross at the x-axis
wAng1=newBox([0,0,0],[.55*wallDist,.5*wallThickness,wallSize*sqrt(2)]); wAng1.phys['se3']=[0,0,0,1,0,0,pi/4]; o.bodies.append(wAng1); 
wAng2=newBox([0,0,0],[.55*wallDist,.5*wallThickness,wallSize*sqrt(2)]); wAng2.phys['se3']=[0,0,0,1,0,0,-pi/4]; o.bodies.append(wAng2)

#cap
wCap=newBox([0,0,wallSize],[.55*wallDist,wallSize,.5*wallThickness]); o.bodies.append(wCap)

# all bodies up to now are fixed and only wire is will be shown
for b in o.bodies:
	b.shape['wire']=True
	b['isDynamic']=False

import random
def randomColor():
	return [random.random(),random.random(),random.random()]

### now the falling balls in 
maxima=[.5*(wallDist/rBall),rCenter/rBall,rCenter/rBall] # number of spheres in x,y,z directions
for ix in range(int(maxima[0])):
	for iy in range(int(maxima[1])):
		for iz in range(int(maxima[2])):
			x,y,z=2.0*rBall*(ix-maxima[0]/2+.5),2.0*rBall*(iy-maxima[1]/2+.5),2.0*rBall*iz+2*rCenter
			#print x,y,z,rBall
			s=newSphere([x,y,z],rBall)
			color=randomColor()
			s.shape['diffuseColor']=color
			s.mold['diffuseColor']=color
			s['isDynamic']=True
			o.bodies.append(s)

o.save('/tmp/a.xml')

if 0:
	import os,time
	os.system(yadeExecutable+' -N QtGUI -S /tmp/a.xml')
else:
	o.run(30000);
	o.wait()
	o.save('/tmp/a_30000.xml')
	#os.system(yadeExecutable+' -N QtGUI -S /tmp/a_10000.xml')

