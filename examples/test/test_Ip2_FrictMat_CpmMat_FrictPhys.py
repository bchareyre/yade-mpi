from yade import *
from yade import plot,qt
import sys


young=25e9
poisson=.2
sigmaT=3e6
frictionAngle=atan(1)
density=4800 ## 4800  # twice the density, since porosity is about .5 (.62)
epsCrackOnset=1e-4
relDuctility=300
intRadius=1.5

concMat  = O.materials.append(CpmMat(young=young,poisson=poisson,density=4800,sigmaT=3e6,relDuctility=30,epsCrackOnset=1e-4,neverDamage=False))
frictMat = O.materials.append(FrictMat(young=young,poisson=poisson,density=4800))

b1 = sphere((0,0,0),1,material=concMat)
b1.state.vel = Vector3(1,0,0)
b2 = sphere((0,5,0),1,material=concMat)
b2.state.vel = Vector3(2,-2,0)
b3 = sphere((0,-4,0),1,material=frictMat)
b3.state.vel = Vector3(1,3,0)
b4 = facet(((2,-5,-5),(2,-5,10),(2,10,-5)),material=frictMat)

O.bodies.append((b1,b2,b3,b4))

O.dt = 5e-6
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[
			Ig2_Sphere_Sphere_ScGeom(),
			Ig2_Facet_Sphere_ScGeom()
		],
		[
			Ip2_CpmMat_CpmMat_CpmPhys(),
			Ip2_FrictMat_CpmMat_FrictPhys(),
			Ip2_FrictMat_FrictMat_FrictPhys(),
		],
		[
			Law2_ScGeom_CpmPhys_Cpm(),
			Law2_ScGeom_FrictPhys_CundallStrack()
		]
	),
	NewtonIntegrator(label='newton'),
]
O.step()

try:
	from yade import qt
	qt.View()
except:
	O.run()
