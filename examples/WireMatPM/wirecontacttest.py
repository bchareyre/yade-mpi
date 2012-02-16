# -*- coding: utf-8 -*-
from yade import plot, qt

#### define parameters for the net
# wire diameter
d = 2.7/1000.
# particle radius
radius = d*4.
# define piecewise linear stress-strain curve [Pa]
strainStressValues=[(0.0019230769,2.5e8),(0.0192,3.2195e8),(0.05,3.8292e8),(0.15,5.1219e8),(0.25,5.5854e8),(0.3,5.6585e8),(0.35,5.6585e8)]

# elastic material properties
particleVolume = 4./3.*pow(radius,3)*pi
particleMass = 3.9/1000.
density = particleMass/particleVolume
young = strainStressValues[0][1] / strainStressValues[0][0]
poisson = 0.3


#### material definition
netMat = O.materials.append( WireMat( young=young,poisson=poisson,frictionAngle=radians(30),density=density,isDoubleTwist=True,diameter=d,strainStressValues=strainStressValues,lambdaEps=0.4,lambdak=0.66) )
wireMat = O.materials.append( WireMat( young=young,poisson=poisson,frictionAngle=radians(30),density=density,isDoubleTwist=False,diameter=3.4/1000,strainStressValues=strainStressValues ) )

blocMat = O.materials.append(FrictMat(young=60e3,poisson=0.15,frictionAngle=radians(30),density=44.5/((4./3.*pi*0.02**3)*1576.)))


#### define parameters for the net packing
# mesh geometry
mos = 0.08
a = 0.04
b = 0.04
# wire diameter
d = 2.7/1000.
# net dimension
cornerCoord=[0,0,0]
Lx = 2.
Ly = 2.
# properties of particles
kw = {'color':[0,1,0],'wire':True,'highlight':False,'fixed':False,'material':netMat}


##### create packing for net
[netpack,lx,ly] = hexaNet( radius=radius, cornerCoord=cornerCoord, xLength=Lx, yLength=Ly, mos=mos, a=a, b=b, startAtCorner=True, isSymmetric=False, **kw )
O.bodies.append(netpack)


#### get bodies for single wire at the boundary in y-direction and change properties
bb = uniaxialTestFeatures(axis=0)
negIds,posIds=bb['negIds'],bb['posIds']

for id in negIds:
	O.bodies[id].material = O.materials[wireMat]
	O.bodies[id].shape.color = [0,0,1]
for id in posIds:
	O.bodies[id].material = O.materials[wireMat]
	O.bodies[id].shape.color = [0,0,1]


#### define engines to create link
interactionRadius=2.8	# value has to be adjusted according to the particle size of the net and the mesh opening size of the net (check always if links are created)
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=interactionRadius,label='aabb')]), 
	InteractionLoop(
	[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=interactionRadius,label='Ig2ssGeom')],
	[Ip2_WireMat_WireMat_WirePhys(linkThresholdIteration=1,label='wire_wire'),Ip2_FrictMat_FrictMat_FrictPhys(label='block_wire')],
	[Law2_ScGeom_WirePhys_WirePM(linkThresholdIteration=1,label='Law_1'),Law2_ScGeom_FrictPhys_CundallStrack(label='Law_2')]
	),
	NewtonIntegrator(damping=0.),
]


#### define additional vertical interactions at the boundary for boundary wire
for i in range(24)[1::2]: # odd - start at second item and take every second item
	createInteraction(negIds[i],negIds[i+1])
del posIds[1]
posIds.append(1)
for i in range(25)[::2]: # even  - start at the beginning at take every second item
	createInteraction(posIds[i],posIds[i+1])


#### time step definition for first time step to create links
O.step()


##### delete horizontal interactions for corner particles
bb = uniaxialTestFeatures(axis=1)
negIds,posIds,axis,crossSectionArea=bb['negIds'],bb['posIds'],bb['axis'],bb['area']


##### delete some interactions
O.interactions.erase(0,50)
O.interactions.erase(0,51)
O.interactions.erase(1,1250)
O.interactions.erase(1,1251)

#### time step definition for deleting some links which have been created by the Ig2 functor
O.step()


#### initializes now the interaction detection factor
aabb.aabbEnlargeFactor=-1.
Ig2ssGeom.interactionDetectionFactor=-1.


#### define boundary conditions
fixedIds=negIds
movingIds=posIds

for id in fixedIds:
	O.bodies[id].shape.color = [1,0,0]
	O.bodies[id].state.blockedDOFs='xyzXYZ'
for id in movingIds:
	O.bodies[id].shape.color = [1,0,0]
	O.bodies[id].state.blockedDOFs='xyzXYZ'


#### import block as a sphere after net has been created
bloc=O.bodies.append(sphere([1.0,1.0,0.65],radius=0.15,wire=False,highlight=False,color=[1,1,0],material=blocMat))
O.bodies[bloc].state.isDamped=False	# switch damping off since free fall under gravity


#### plot some results
plot.plots={'t':['vz',None,('f_unbal','g--')]}
#plot.liveInterval=2.
plot.plot(noShow=False, subPlots=False)

def addPlotData():
	plot.addData(t=O.time, vz=-O.bodies[bloc].state.vel[2], f_unbal=unbalancedForce(useMaxForce=False) )


#### define engines for simulation
v = qt.Controller()
v = qt.View()
rr = qt.Renderer()
rr.intrAllWire = True
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(label='aabb')]), 
	InteractionLoop(
	[Ig2_Sphere_Sphere_ScGeom(label='Ig2ssGeom')],
	[Ip2_WireMat_WireMat_WirePhys(label='wire_wire'),Ip2_FrictMat_FrictMat_FrictPhys(label='block_wire')],
	[Law2_ScGeom_WirePhys_WirePM(label='Law_1'),Law2_ScGeom_FrictPhys_CundallStrack(label='Law_2')]
	),
	NewtonIntegrator(damping=0.2,gravity=[0,0,-9.81],label='NewtonGravity'),
	PyRunner(initRun=True,iterPeriod=100,command='addPlotData()'),
]


#### time step definition for simulation
## critical time step proposed by Bertrand
kn = 16115042 # stiffness of single wire from code, has to be changed if you change the stress-strain curve for the wire
O.dt = 0.2*sqrt(particleMass/(2.*kn))

O.run(200000)
