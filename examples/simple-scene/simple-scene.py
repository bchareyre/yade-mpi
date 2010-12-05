#!/usr/bin/python
# -*- coding: utf-8 -*-

## Omega is the super-class that orchestrates the whole program.
## It holds the entire simulation (MetaBody), takes care of loading/saving,
## starting/stopping the simulation, loads plugins and more.

o=Omega() # for advaned folks: this creates default MetaBody as well


## Engines are called consecutively at each iteration. Their order matters.
##
## Some of them work by themselves (GlobalEngine, PartialEngine - the difference of these two is unimportant).
##
## MetaEngines act as dispatchers and based on the type of objects they operate on, different EngineUnits are called.
o.engines=[
	## Resets forces and momenta the act on bodies
	ForceResetter(),
	## associates bounding volume - in this case, AxisAlignedBoundingBox (Aabb) - to each body.
	## Using bounding boxes created, find possible body collisions.
	## These possible collisions are inserted in O.interactions container (Scene::interactions in c++).
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_Box_Aabb(),
	]),
	InteractionLoop(
		## Decide whether the potential collisions are real; if so, create geometry information about each potential collision.
		## Here, the decision about which EngineUnit to use depends on types of _both_ bodies.
		## Note that there is no EngineUnit for box-box collision. They are not implemented.
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		## Create physical information about the interaction.
		## This may consist in deriving contact rigidity from elastic moduli of each body, for example.
		## The purpose is that the contact may be "solved" without reference to related bodies,
		## only with the information contained in contact geometry and physics.
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		## "Solver" of the contact, also called (consitutive) law.
		## Based on the information in interaction physics and geometry, it applies corresponding forces on bodies in interaction.
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	## Apply gravity: all bodies will have gravity applied on them.
	## Note the engine parameter 'gravity', a vector that gives the acceleration.
	GravityEngine(gravity=[0,0,-9.81]),
	## Forces acting on bodies are damped to artificially increase energy dissipation in simulation.
	## (In this model, the restitution coefficient of interaction is 1, which is not realistic.)
	## This MetaEngine acts on all PhysicalActions and selects the right EngineUnit base on type of the PhysicalAction.
	#
	# note that following 4 engines (till the end) can be replaced by an optimized monolithic version:
	NewtonIntegrator(damping=0.1)
]


## The yade.utils module contains some handy functions, like yade.utils.box and yade.utils.sphere.
## After this import, they will be accessible as utils.box and utils.sphere.
from yade import utils

## create bodies in the simulation: one box in the origin and one floating above it.
##
## The box:
## * extents: half-size of the box. [.5,.5,.5] is unit cube
## * center: position of the center of the box
## * dynamic: it is not dynamic, i.e. will not move during simulation, even if forces are applied to it

o.bodies.append(utils.box(center=[0,0,0],extents=[.5,.5,.5],color=[0,0,1],dynamic=False))

## The sphere
##
## * First two arguments are radius and center, respectively. They are used as "positional arguments" here:
## python will deduce based on where they are what they mean.
##
## It could also be written without using utils.sphere - see gui/py/utils.py for the code of the utils.sphere function
o.bodies.append(utils.sphere([0,0,2],1,color=[0,1,0]))

## Estimate timestep from p-wave speed and multiply it by safety factor of .2
o.dt=.01*utils.PWaveTimeStep()

## Save the scene to file, so that it can be loaded later. Supported extension are: .xml, .xml.gz, .xml.bz2.
o.save('/tmp/a.xml.bz2');
#o.run(100000); o.wait(); print o.iter/o.realtime,'iterations/sec'

def onBodySelect(id):
	print "Selected:",id
	utils.highlightNone()
	for i in O.interactions.withBody(id):
		O.bodies[i.id2 if i.id1==id else i.id1].shape.highlight=True
		print i.id1,i.id2,i.phys,i.geom

try:
	from yade import qt
	qt.View()
	qt.Controller()
except ImportError: pass
O.run(20000)
