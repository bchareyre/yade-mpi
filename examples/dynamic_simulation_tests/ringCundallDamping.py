# -*- encoding=utf-8 -*-

from yade import utils

## Omega
o=Omega() 

## PhysicalParameters 
Young = 0.15e9
Poisson = 0.3
Density=500
frictionAngle=radians(30)
sphereRadius=0.05

## Import wall's geometry
walls = utils.import_stl_geometry('ring.stl',young=Young,poisson=Poisson,frictionAngle=frictionAngle)

def fill_cylinder_with_spheres(sphereRadius,cylinderRadius,cylinderHeight,cylinderOrigin,cylinderSlope):
	spheresCount=0
	for h in xrange(0,cylinderHeight/sphereRadius/2):
			for r in xrange(1,cylinderRadius/sphereRadius/2):
				dfi = asin(0.5/r)*2
				for a in xrange(0,6.28/dfi):
					x = cylinderOrigin[0]+2*r*sphereRadius*cos(dfi*a)
					y = cylinderOrigin[1]+2*r*sphereRadius*sin(dfi*a)
					z = cylinderOrigin[2]+h*2*sphereRadius
					o.bodies.append(utils.sphere([x,y*cos(cylinderSlope)+z*sin(cylinderSlope),z*cos(cylinderSlope)-y*sin(cylinderSlope)],sphereRadius,young=Young,poisson=Poisson,density=Density,frictionAngle=frictionAngle))
					spheresCount+=1
	return spheresCount

## Spheres
spheresCount=0
spheresCount+=fill_cylinder_with_spheres(sphereRadius,0.5,0.10,[0,0,0],radians(0))
print "Number of spheres: %d" % spheresCount

## Initializers 
o.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',
		[EngineUnit('InteractingSphere2AABB'),
			EngineUnit('InteractingFacet2AABB'),
			EngineUnit('MetaInteractingGeometry2AABB')]),
	]

## Engines 
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingFacet2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('MacroMicroElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	DeusExMachina('GravityEngine',{'gravity':[0,-9.81,0]}),

	## NOTE: Non zero Cundall damping affected a dynamic simulation!
	DeusExMachina('NewtonsDampedLaw',{'damping':0}),

    ## angularVelocity = 0.73 rad/sec = 7 rpm
    DeusExMachina('RotationEngine',{'subscribedBodies':walls,'rotationAxis':[0,0,1],'rotateAroundZero':True,'angularVelocity':0.73}),
]

o.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

for b in o.bodies:
    if b.shape.name=='Sphere':
        b.phys['blockedDOFs']=4 # blocked movement along Z

o.dt=0.2*utils.PWaveTimeStep()

o.saveTmp('init');

from yade import qt
renderer=qt.Renderer()
renderer['Body_wire']=True
qt.Controller()

