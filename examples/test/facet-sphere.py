# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction.
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	#SpatialQuickSortCollider(),
	InteractionLoop(
		[Ig2_Facet_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=0.01,gravity=[0,0,-10]),
	]

O.bodies.append([
	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],fixed=True,color=[1,0,0]),
	utils.facet([[1,-1,0],[0,1,0,],[1,.5,.5]],fixed=True)
])

import random
for i in range(0,100):
	s=utils.sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05))
	s.state.vel=Vector3(random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1))
	O.bodies.append(s)

O.dt=utils.PWaveTimeStep()
O.run()
O.saveTmp('init')

if 0:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Interaction_geometry']=True
	qt.Controller()
#except ImportError: pass
O.saveTmp()
O.loadTmp()

if 0:
	O.timingEnabled=True
	from yade import timing
	for i in range(4):
		timing.reset()
		O.loadTmp('init')
		O.run(100000,True)
		timing.stats()
	quit()
