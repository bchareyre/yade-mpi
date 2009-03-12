# -*- encoding=utf-8 -*-

from yade import utils

## Omega
o=Omega() 

## PhysicalParameters 
Density=2400
frictionAngle=radians(35)
sphereRadius=0.05
tc = 0.001
en = 0.3
es = 0.3

## Import wall's geometry
p=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
walls = utils.import_stl_geometry('ring.stl',frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters",physParamsAttr={'kn':p['kn'],'cn':p['cn'],'ks':p['ks'],'cs':p['cs']})

def fill_cylinder_with_spheres(sphereRadius,cylinderRadius,cylinderHeight,cylinderOrigin,cylinderSlope):
	spheresCount=0
	for h in xrange(0,cylinderHeight/sphereRadius/2):
			for r in xrange(1,cylinderRadius/sphereRadius/2):
				dfi = asin(0.5/r)*2
				for a in xrange(0,6.28/dfi):
					x = cylinderOrigin[0]+2*r*sphereRadius*cos(dfi*a)
					y = cylinderOrigin[1]+2*r*sphereRadius*sin(dfi*a)
					z = cylinderOrigin[2]+h*2*sphereRadius
					s=utils.sphere([x,y*cos(cylinderSlope)+z*sin(cylinderSlope),z*cos(cylinderSlope)-y*sin(cylinderSlope)],sphereRadius,density=Density,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters")
					p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
					s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']
					o.bodies.append(s)
					spheresCount+=1
	return spheresCount

# Spheres
spheresCount=0
spheresCount+=fill_cylinder_with_spheres(sphereRadius,0.5,0.10,[0,0,0],radians(0))
print "Number of spheres: %d" % spheresCount

## Initializers 
o.initializers=[
	StandAloneEngine('PhysicalActionContainerInitializer'),
	MetaEngine('BoundingVolumeMetaEngine',
		[EngineUnit('InteractingSphere2AABB'),
			EngineUnit('InteractingFacet2AABB'),
			EngineUnit('MetaInteractingGeometry2AABB')])
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

	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleViscoelasticRelationships')]),

    ## Constitutive law
	MetaEngine('ConstitutiveLawDispatcher',[EngineUnit('ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw')]),

	DeusExMachina('GravityEngine',{'gravity':[0,-9.81,0]}),

	## Cundall damping must been disabled!
	DeusExMachina('NewtonsDampedLaw',{'damping':0}),

    ## angularVelocity = 0.73 rad/sec = 7 rpm
    DeusExMachina('RotationEngine',{'subscribedBodies':walls,'rotationAxis':[0,0,1],'rotateAroundZero':True,'angularVelocity':0.73}),
]

o.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]

for b in o.bodies:
    if b.shape.name=='Sphere':
        b.phys['blockedDOFs']=4 # blocked movement along Z

o.dt=0.2*tc

o.saveTmp('init');

from yade import qt
renderer=qt.Renderer()
renderer['Body_wire']=True
qt.Controller()

