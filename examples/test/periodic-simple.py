"""Simple test of periodic collider.
A few spheres falling down in gravity field and one moving accross.
Includes a clump.
"""

from yade import timing

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],label='collider'),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_Dem3DofGeom_FrictPhys_CundallStrack()],
	),
	TranslationEngine(translationAxis=(1,0,0),velocity=10,ids=[0]),
	NewtonIntegrator(damping=.4,gravity=[0,0,-10])
]
O.bodies.append(utils.sphere([-4,0,11],2,fixed=True))
O.bodies.append(utils.sphere([0,-2,5.5],2))
O.bodies.append(utils.sphere([0,2,5.5],2))
O.bodies.appendClumped([utils.sphere([0,4,8],.8),utils.sphere([0,5,7],.6)])
# sets up the periodic cell
O.periodic=True
O.cell.setBox(10,10,10)
# normally handled in by the simulation... but we want to have the rendering right before start
#O.cell.postProcessAttributes()
O.dt=.1*utils.PWaveTimeStep()
O.saveTmp()
from yade import qt
qt.Controller()
qt.View()
#O.timingEnabled=True; timing.reset(); O.run(200000,True); timing.stats()
