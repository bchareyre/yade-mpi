"""Script that either grows spheres inside the cell or shrinks
the cell progressively. It prints the total volume force once in a while.
This script also shows that the collider misses some interactions as spheres
are getting one over another. That is not acceptable, of course. """
from yade import log,timing

O.engines=[
	BexResetter(),
	BoundingVolumeMetaEngine([InteractingSphere2AABB(),MetaInteractingGeometry2AABB()]),
	PeriodicInsertionSortCollider(label='collider'),
	InteractionDispatchers(
		[ef2_Sphere_Sphere_Dem3DofGeom()],
		[SimpleElasticRelationships()],
		[Law2_Dem3Dof_Elastic_Elastic()],
	),
	NewtonsDampedLaw(damping=.6)
]
import random
O.bodies.append(utils.sphere((0,0,0),.5,dynamic=False,density=1000)) # stable point
for i in xrange(150):
	O.bodies.append(utils.sphere(Vector3(10*random.random(),10*random.random(),10*random.random()),.2+.2*random.random(),density=1000))
O.periodicCell=((-5,-5,0),(5,5,10))
O.dt=.8*utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller(); qt.View()
step=.01
O.run(200,True)
if 0:
	for i in range(0,500):
		O.run(500,True)
		for b in O.bodies:
			b.shape['radius']=b.shape['radius']+step
			b.mold['radius']=b.mold['radius']+step
		for i in O.interactions:
			if not i.isReal: continue
			i.geom['effR1']=i.geom['effR1']+step
			i.geom['effR2']=i.geom['effR2']+step
		print O.iter,utils.totalForceInVolume()
else:
	for i in range(0,500):
		O.run(100,True)
		mn,mx=O.periodicCell
		step=(mx-mn); step=Vector3(.002*step[0],.002*step[1],.002*step[2])
		O.periodicCell=mn+step,mx-step
		if (i%10==0): print O.iter,utils.totalForceInVolume()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
