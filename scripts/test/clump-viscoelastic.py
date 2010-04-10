# -*- coding: utf-8

from yade import utils,pack,export,qt
import gts,os,random,itertools
from numpy import *
import yade.log 

#yade.log.setLevel('NewtonIntegrator',yade.log.TRACE)

# Parameters
tc=0.001# collision time 
en=.3  # normal restitution coefficient
es=.3  # tangential restitution coefficient
frictionAngle=radians(35)# 
density=2700
# facets material
params=utils.getViscoelasticFromSpheresInteraction(10e3,tc,en,es)
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs
# default spheres material
dfltSpheresMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle)) 

O.dt=.01*tc # time step

Rs=0.1 # particle radius

# Create geometry
plnSurf = pack.sweptPolylines2gtsSurface([[Vector3(-.5,0,0),Vector3(.5,0,0),Vector3(.5, 0, -.5),Vector3(-.5, 0, -.5)]],capStart=True,capEnd=True)
plnIds=O.bodies.append(pack.gtsSurface2Facets(plnSurf.faces(),material=facetMat,color=(0,1,0)))

plnSurf1 = pack.sweptPolylines2gtsSurface([[Vector3(-.5,-.5,-.5),Vector3(.5,-.5,-.5),Vector3(.5, 1.5, -.5),Vector3(-.5, 1.5, -.5)]],capStart=True,capEnd=True)
plnIds1=O.bodies.append(pack.gtsSurface2Facets(plnSurf1.faces(),material=facetMat,color=(0,1,0)))

# Create clumps
clpId,sphId=O.bodies.appendClumped([utils.sphere(Vector3(0,Rs*2*i,Rs*2),Rs,material=dfltSpheresMat) for i in xrange(4)])
for id in sphId:
	s=O.bodies[id]
	p=utils.getViscoelasticFromSpheresInteraction(s.state['mass'],tc,en,es)
	s.mat['kn'],s.mat['cn'],s.mat['ks'],s.mat['cs']=p['kn'],p['cn'],p['ks'],p['cs']

# Create engines
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
	NewtonIntegrator(damping=0,accRigidBodyRot=True),
]


renderer = qt.Renderer()
qt.View()
O.saveTmp()

clump = O.bodies[clpId]
spheres = [ O.bodies[id] for id in sphId ]

print '\nClump:\n======'
#print '\nMaterial:'
if clump.mat:
	for k in clump.mat.keys():
		print k,'=',clump.mat[k]
else:
	print 'no material'

print '\nMold:'
if clump.mold:
	for k in clump.mold.keys():
		print k,'=',clump.mold[k]
else:
	print 'no mold'

print '\nState:'
if clump.state:
	for k in clump.state.keys():
		print k,'=',clump.state[k]
else:
	print 'no state'

print '\nControl:'
mass = sum( [ s.state['mass'] for s in spheres ] )
xm_ = [ s.state['pos'][0]*s.state['mass'] for s in spheres ]
ym_ = [ s.state['pos'][1]*s.state['mass'] for s in spheres ]
zm_ = [ s.state['pos'][2]*s.state['mass'] for s in spheres ]
centroid  = Vector3( sum(xm_)/mass, sum(ym_)/mass, sum(zm_)/mass )

def sphereInertiaTenzor(p, m, r, c):
	''' Inertia tenzor sphere with position p, mass m and radus r relative point c '''
	I = zeros((3,3))
	rp = array(p)-array(c)
	I[0,0] = 2./5.*m*r*r + m*(rp[1]**2+rp[2]**2)
	I[1,1] = 2./5.*m*r*r + m*(rp[0]**2+rp[2]**2)
	I[2,2] = 2./5.*m*r*r + m*(rp[0]**2+rp[1]**2)
	for k,l in itertools.product(arange(0,3),arange(0,3)):
		if not k==l: I[k,l] = -m*rp[k]*rp[l]
	return I

I = zeros((3,3))
for s in spheres:
	I += sphereInertiaTenzor(s.state['pos'],s.state['mass'],s.mold['radius'],centroid)
I_eigenvalues, I_eigenvectors = linalg.eig(I)

print 'mass = ', mass
print 'centroid = ', centroid
print 'I = \n', I
print 'I_eigenvalues = ', I_eigenvalues
print 'I_eigenvectors = \n', I_eigenvectors

