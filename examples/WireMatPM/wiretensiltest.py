# -*- coding: utf-8 -*-
from yade import utils, pack, plot

#### short description of script
print 'This script shows a tensile test of a net by using the UniaxialStrainer'

#### define parameters for the net
# wire diameter
d = 2.7/1000.
# particle radius
radius = d*4.
# define piecewise lineare stress-strain curve
strainStressValues=[(0.0019230769,2.5e8),(0.0192,3.2195e8),(0.05,3.8292e8),(0.15,5.1219e8),(0.25,5.5854e8),(0.3,5.6585e8),(0.35,5.6585e8)]
# elastic material properties
particleVolume = 4./3.*pow(radius,3)*pi
particleMass = 3.9/1000.
density = particleMass/particleVolume
young = strainStressValues[0][1] / strainStressValues[0][0]
poisson = 0.3


#### material definition
netMat = O.materials.append( WireMat( young=young,poisson=poisson,density=density,isDoubleTwist=True,diameter=d,strainStressValues=strainStressValues,lambdaEps=0.4,lambdak=0.66) )

wireMat = O.materials.append( WireMat( young=young,poisson=poisson,density=density,isDoubleTwist=False,diameter=3.4/1000,strainStressValues=strainStressValues ) )


#### get net packing
kw = {'color':[1,1,0],'wire':True,'highlight':False,'fixed':False,'material':netMat}
[netpack,lx,ly] = pack.hexaNet( radius=radius, cornerCoord=[0,0,0], xLength=1.0, yLength=0.55, mos=0.08, a=0.04, b=0.04, startAtCorner=False, isSymmetric=True, **kw )
O.bodies.append(netpack)
print 'Real net length in x-direction [m]: ', lx
print 'Real net length in y-direction [m]: ', ly


#### get bodies for single wire at the boundary in y-direction and change properties
bb = utils.uniaxialTestFeatures(axis=0)
negIds,posIds=bb['negIds'],bb['posIds']

for id in negIds:
	O.bodies[id].material = O.materials[wireMat]
	O.bodies[id].shape.color = [0,0,1]
for id in posIds:
	O.bodies[id].material = O.materials[wireMat]
	O.bodies[id].shape.color = [0,0,1]


#### define engines to create link
interactionRadius=2.8
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=interactionRadius,label='aabb')]), 
	InteractionLoop(
	[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=interactionRadius,label='Ig2ssGeom')],
	[Ip2_WireMat_WireMat_WirePhys(linkThresholdIteration=1,label='interactionPhys')],
	[Law2_ScGeom_WirePhys_WirePM(linkThresholdIteration=1,label='interactionLaw')]
	),
	NewtonIntegrator(damping=0.),
]

#### define additional vertical interactions at the boundary
utils.createInteraction(negIds[0],negIds[2])
utils.createInteraction(negIds[3],negIds[4])
utils.createInteraction(negIds[5],negIds[6])
utils.createInteraction(negIds[7],negIds[1])
utils.createInteraction(posIds[0],posIds[2])
utils.createInteraction(posIds[3],posIds[4])
utils.createInteraction(posIds[5],posIds[6])
utils.createInteraction(posIds[7],posIds[1])

#### time step definition for first time step to create links
O.step()


#### initialize values for UniaxialStrainer
bb = utils.uniaxialTestFeatures(axis=1)
negIds,posIds,axis,crossSectionArea=bb['negIds'],bb['posIds'],bb['axis'],bb['area']
strainRateTension = 0.1
setSpeeds = True


##### delete horizontal interactions for corner particles
bb = utils.uniaxialTestFeatures(axis=1)
negIds,posIds,axis,crossSectionArea=bb['negIds'],bb['posIds'],bb['axis'],bb['area']


##### delete some interactions
O.interactions.erase(0,4)
O.interactions.erase(0,5)
O.interactions.erase(1,154)
O.interactions.erase(1,155)
O.interactions.erase(2,26)
O.interactions.erase(2,27)
O.interactions.erase(3,176)
O.interactions.erase(3,177)

#### time step definition for deleting some links which have been created by the Ig2 functor
O.step()


#### initializes now the interaction detection factor
aabb.aabbEnlargeFactor=-1.
Ig2ssGeom.interactionDetectionFactor=-1.


#### define engines for simulation with UniaxialStrainer
O.engines = O.engines[:3] + [ UniaxialStrainer(strainRate=strainRateTension,axis=axis,asymmetry=1,posIds=posIds,negIds=negIds,crossSectionArea=crossSectionArea,blockDisplacements=True,blockRotations=False,setSpeeds=setSpeeds,label='strainer'),
NewtonIntegrator(damping=0.5),
PyRunner(initRun=True,iterPeriod=1000,command='addPlotData()'),
]


#### plot some results
plot.plots={'un':('Fn',)}
plot.plot(noShow=False, subPlots=False)

def addPlotData():
	Fn = 0.
	for i in posIds:
		try:
			inter=O.interactions.withBody(i)[0]
			F = abs(inter.phys.normalForce[1])
		except:
			F = 0
		Fn += F
	un = O.bodies[O.bodies[posIds[0]].id].state.pos[1] - O.bodies[O.bodies[posIds[0]].id].state.refPos[1]
	if un > 0.10:
		O.pause()
	plot.addData( un=un*1000, Fn=Fn/1000 )


#### time step definition for simulation
## critical time step proposed by Bertrand
kn = 16115042 # stiffness of single wire from code
O.dt = 0.2*sqrt(particleMass/(2.*kn))


#### to see it
from yade import qt
v = qt.Controller()
v = qt.View()
rr = qt.Renderer()
rr.intrAllWire = True
