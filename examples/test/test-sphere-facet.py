# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction preserving the original contact orientation.
# Z-gravity is being increased every 4000 iterations, the sphere dives more into the facet and stabilizes,
# etc. This process continues even if the sphere center passes on the other side of the facet and,
# (if distant transient interactions are allowed in the collider) if the sphere passes in its entirety to
# the other side of the facet. The interaction, however, still pushes in the same sense.
#
# After the gravity reaches some value, it is reset and the sphere should be pushed from the facet towards
# its original position. When the contact on the original side is lost, the interaction should be deleted.
#
#
# The only tunable sign places the sphere either on the top ot at the bottom of the facet
# and sets gravity accordingly. It can be +1 or -1
#
sign=-1
#

## PhysicalParameters 
Young = 7e6
Poisson = 0.2
Density=2700

# Append a material
mat=O.materials.append(FrictMat(young=Young,poisson=Poisson,density=Density,frictionAngle=26))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.3,gravity=(0,0,-10),label='integrator'),
	PyRunner(iterPeriod=4000,command='setGravity()'),
	]
O.bodies.append([
	facet([[-1,-1,0],[1,-1,0],[0,1,0]],fixed=True,color=[1,0,0],material=mat),
	sphere([0,0,sign*.49999],radius=.5,wire=True,material=mat),
])
O.timingEnabled=True
O.saveTmp()
O.dt=1e-4


print '** virgin dispatch matrix:'
O.engines[2].lawDispatcher.dispMatrix()
print '** class indices'
O.run(1000,True)
print '** used dispatch matrix'
O.engines[2].lawDispatcher.dispMatrix()


def setGravity():
	gz=integrator.gravity[2]
	integrator.gravity=[0,0,1.05*gz]
	if abs(gz)>=2500:
		print "Gravity reset & slow down"
		O.dt=1e-6;
		integrator.gravity=[0,0,0]
	if abs(gz)>0: print gz

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer.intrGeom=True
	qt.Controller()
except ImportError: pass


from yade import timing
O.run(100000,True)
timing.stats()
timing.reset()
O.loadTmp()
O.run(100000,True)
timing.stats()
