# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction.

#O.bodyContainer="BodyVector"

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],nBins=5,sweepLength=5e-3),
	#SpatialQuickSortCollider(),
	InteractionLoop(
		[Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=0.01,gravity=[0,0,-10]),
	]
O.bodies.append([
	facet([[-1,-1,0],[1,-1,0],[0,1,0]],fixed=True,color=[1,0,0]),
	facet([[1,-1,0],[0,1,0,],[1,.5,.5]],fixed=True)
])

#Gl1_Facet(normals=True)

import random,sys

def addRandomSphere():
	return O.bodies.append(sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05)))
	O.bodies[len(O.bodies)-1].state.vel=[random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1)]

for i in range(0,100): addRandomSphere()

O.dt=1e-4
#O.run()
O.saveTmp('init')
from yade import qt
qt.Controller()
qt.View()
if 1:
	for i in range(0,1000):
		O.run(50,True);
		if random.choice([True,False]):
			idOld=random.randint(2,len(O.bodies)-1)
			O.bodies.erase(idOld)
			print "-%d"%idOld,
		else:
			idNew=addRandomSphere()
			print "+%d"%idNew,
		sys.stdout.flush()
