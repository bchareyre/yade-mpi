
""" NOTE
Needs yade compiled with CGAL feature
"""

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop([Ig2_Facet_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()],),
	NewtonIntegrator(damping=0.01,gravity=[0,0,-10]),
]
mat=O.materials.append(FrictMat(young=1e3,poisson=.2,density=1000,frictionAngle=20))
O.bodies.append([
	facet([[-1,-1,0],[1,-1,0],[0,1,0]],fixed=True,color=[1,0,0],material=mat),
	facet([[1,-1,0],[0,1,0,],[1,.5,.5]],fixed=True,material=mat)
])
import random
if 1:
	for i in range(0,100):
		O.bodies.append(sphere([random.gauss(0,1),random.gauss(0,1),random.uniform(1,2)],random.uniform(.02,.05),material=mat))
		O.bodies[len(O.bodies)-1].state.vel=Vector3(random.gauss(0,.1),random.gauss(0,.1),random.gauss(0,.1))
else:
	O.bodies.append(sphere([0,0,.6],.5),material=mat)
O.dt=1e-4
O.saveTmp('init')

# compare 2 colliders:
if 1:
	O.timingEnabled=True
	from yade import timing
	for collider in InsertionSortCollider(),PersistentTriangulationCollider(haveDistantTransient=True):
		for i in range(2):
			O.loadTmp('init')
			replaceCollider(collider)
			O.run(100,True)
			timing.reset()
			O.run(50000,True)
			timing.stats()
else:
	#O.run(100,True)
	O.step()
	print len(O.interactions)
	#O.bodies[2].phys['se3']=[-.6,0,.6,1,0,0,0]
	#O.step()
