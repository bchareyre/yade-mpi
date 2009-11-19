# -*- coding: utf-8

from yade import utils,pack,export,qt
import gts,os

def Plane(v1,v2,v3,v4):
	pts = [ [Vector3(v1),Vector3(v2),Vector3(v3),Vector3(v4)] ]
	return pack.sweptPolylines2gtsSurface(pts,capStart=True,capEnd=True)

# Parameters
tc=0.001# collision time 
en=0.3  # normal restitution coefficient
es=0.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
kw=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)

O.dt=.2*tc # time step

Rs=0.02 # mean particle radius
Rf=0.01 # dispersion (Rs±Rf*Rs)
nSpheres=1000# number of particles

# Create geometry
pln=Plane( (-.5, -.5, 0), (.5, -.5, -.05), (.5, .5, 0), (-.5, .5, -.05) ); 
plnIds=O.bodies.append(pack.gtsSurface2Facets(pln.faces(),frictionAngle=frictionAngle,physParamsClass='SimpleViscoelasticBodyParameters',color=(0,1,0),**kw))

fct=Plane( (-.25, -.25, .5), (.25, -.25, .5), (.25, .25, .5), (-.25, .25, .5) ); 
fctIds=O.bodies.append(pack.gtsSurface2Facets(fct.faces(),frictionAngle=frictionAngle,physParamsClass='SimpleViscoelasticBodyParameters',color=(1,0,0),noBoundingVolume=True))

# Create spheres
sp=pack.SpherePack(); 
sp.makeCloud(Vector3(-.5, -.5, 0),Vector3(.5, .5, .2), Rs, Rf, int(nSpheres), False)
spheres=O.bodies.append([utils.sphere(s[0],s[1],color=(0.929,0.412,0.412),density=density,frictionAngle=frictionAngle,physParamsClass="SimpleViscoelasticBodyParameters") for s in sp])
for id in spheres:
	s=O.bodies[id]
	p=utils.getViscoelasticFromSpheresInteraction(s.phys['mass'],tc,en,es)
	s.phys['kn'],s.phys['cn'],s.phys['ks'],s.phys['cs']=p['kn'],p['cn'],p['ks'],p['cs']

# Create engines
O.initializers=[ BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]) ]
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB(), MetaInteractingGeometry2AABB()
	]),
	InsertionSortCollider(nBins=5,sweepLength=.1*Rs),
	InteractionDispatchers(
		[InteractingSphere2InteractingSphere4SpheresContactGeometry(), InteractingFacet2InteractingSphere4SpheresContactGeometry()],
		[SimpleViscoelasticRelationships()],
		[ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw()],
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonsDampedLaw(damping=0),
	ResetRandomPosition(factoryFacets=fctIds,velocity=(0,0,-2),virtPeriod=0.01,subscribedBodies=spheres,point=(0,0,-.5),normal=(0,0,1),maxAttempts=100),
]

renderer = qt.Renderer()
qt.View()
O.saveTmp()
O.run()


