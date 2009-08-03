# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction.
O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),InteractingFacet2AABB()]),
	PersistentSAPCollider(),
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
import random
for i in range(0,100):
	O.bodies.append(utils.sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05),velocity=[random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1)]))

O.miscParams=[GLDrawSphere(glutUse=True)]
O.dt=1e-4
O.saveTmp('init')


from yade import log
#log.setLevel("ef2_Facet_Sphere_Dem3DofGeom",log.TRACE)
try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Interaction_geometry']=True
	qt.Controller()
except ImportError: pass


if 1:
	O.timingEnabled=True
	from yade import timing
	for i in range(4):
		timing.reset()
		O.loadTmp('init')
		O.run(100000,True)
		timing.stats()
	quit()
