# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction.

#O.bodyContainer="BodyVector"

O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(nBins=5,sweepLength=5e-3),
	#SpatialQuickSortCollider(),
	InteractionDispatchers(
		[ef2_Facet_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	GravityEngine(gravity=[0,0,-10]),
	NewtonsDampedLaw(damping=0.01),
	]
O.bodies.append([
	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],dynamic=False,color=[1,0,0],young=1e3),
	utils.facet([[1,-1,0],[0,1,0,],[1,.5,.5]],dynamic=False,young=1e3)
])

#GLDrawInteractingFacet(normals=True)

import random,sys

def addRandomSphere():
	return O.bodies.append(utils.sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05),velocity=[random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1)]))

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
