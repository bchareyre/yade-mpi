#!/usr/bin/python
# -*- coding: utf-8 -*-

"""Simple script which shows how to create an inlet. 
"""

shotsId,steelId=O.materials.append([
	FrictMat(young=50e9,density=6000,poisson=.2,label='shots'),
	FrictMat(young=210e9,density=7800, poisson=.3,label='steel'),
])
## same as
#
# shotsId,steelId=O.materials.index('shots'),O.materials.index('steel')
#

O.bodies.append(geom.facetBox(center=(0,0,0),extents=(30e-3,30e-3,0),wallMask=32,wire=False,material='steel',color=(0,1,.3)))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],verletDist=.05*.29e-3),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_MindlinPhys(
			# define restitution coefficients between different pairs of material ids, see the functor's documentation for details
			#en=MatchMaker(fallback='zero',matches=((steelId,shotsId,.4),(shotsId,shotsId,1)))
			en=MatchMaker(matches=((steelId,shotsId,.4),(shotsId,shotsId,1)))
		)],
		[Law2_ScGeom_MindlinPhys_Mindlin(label='contactLaw')]
	),
	NewtonIntegrator(damping=0),
	## CircularFactory: disk if length=0 or cylinder if length>0
	#CircularFactory(maxParticles=10000,radius=8e-3,length=16e-3,center=(0,-15e-3,15e-3),rMin=0.28e-3,rMax=0.29e-3,vMin=100,vMax=100,vAngle=0,massFlowRate=100./60,normal=(0,1.5,-1),label='factory',materialId=shotsId),
	## BoxFactory: a line, plane or cuboid  
	BoxFactory(maxParticles=10000,extents=(8e-3,8e-3,8e-3),center=(0,-15e-3,15e-3),rMin=0.28e-3,rMax=0.29e-3,vMin=100,vMax=100,vAngle=0,massFlowRate=100./60,normal=(0,1.5,-1),label='factory',materialId=shotsId),
	DomainLimiter(lo=(-30e-3,-30e-3,0),hi=(30e-3,30e-3,60e-3),iterPeriod=200),
	#VTKRecorder(recorders=['spheres','facets','velocity'],fileName='/tmp/nozzle-',iterPeriod=500),

	# run this every once in a while, to finalize the simulation at some point
	PyRunner(iterPeriod=10000,command='if factory.numParticles>=factory.maxParticles: O.stopAtIter=O.iter+8000; timing.stats()')
]
# the timestep must be smaller because of high linear velocities of particles
# we cannot use PWaveTimeStep directly, since there are no spheres generated yet
O.dt=SpherePWaveTimeStep(factory.rMin,O.materials[factory.materialId].density,O.materials[factory.materialId].young)
O.saveTmp()
#O.timingEnabled=True
from yade import timing
try:
	from yade import qt
	# setup 3d view
	v=qt.View()
	v.upVector=(0,0,1); v.viewDir=(-1,0,-.3); v.center(median=False)
except ImportError: pass
O.run()
