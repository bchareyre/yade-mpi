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
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
# facets material
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,**params)) 
# default spheres material
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle))

O.dt=.2*tc # time step

Rs=0.02 # mean particle radius
Rf=0.01 # dispersion (Rs±Rf*Rs)
nSpheres=1000# number of particles

# Create geometry
pln=Plane( (-.5, -.5, 0), (.5, -.5, -.05), (.5, .5, 0), (-.5, .5, -.05) ); 
plnIds=O.bodies.append(pack.gtsSurface2Facets(pln.faces(),material=facetMat,color=(0,1,0)))

fct=Plane( (-.25, -.25, .5), (.25, -.25, .5), (.25, .25, .5), (-.25, .25, .5) ); 
fctIds=O.bodies.append(pack.gtsSurface2Facets(fct.faces(),material=facetMat,color=(1,0,0),noBound=True))

# Create spheres
sp=pack.SpherePack(); 
sp.makeCloud(Vector3(-.5, -.5, 0),Vector3(.5, .5, .2), Rs, Rf, int(nSpheres), False)
spheres=O.bodies.append([utils.sphere(s[0],s[1],color=(0.929,0.412,0.412),material=dfltSpheresMat) for s in sp])
for id in spheres:
	s=O.bodies[id]
	p=utils.getViscoelasticFromSpheresInteraction(s.state['mass'],tc,en,es)
	s.mat['kn'],s.mat['cn'],s.mat['ks'],s.mat['cs']=p['kn'],p['cn'],p['ks'],p['cs']

# Create engines
O.initializers=[ BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]) ]
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_Spheres_Viscoelastic_SimpleViscoelastic()],
	),
	GravityEngine(gravity=[0,0,-9.81]),
	NewtonIntegrator(damping=0),
	ResetRandomPosition(factoryFacets=fctIds,velocity=(0,0,-2),virtPeriod=0.01,subscribedBodies=spheres,point=(0,0,-.5),normal=(0,0,1),maxAttempts=100),
]

#renderer = qt.Renderer()
#qt.View()
#O.saveTmp()
#O.run()


