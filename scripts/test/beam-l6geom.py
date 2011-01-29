#
# Demonstrate L6Geom class with suspended hook-shaped beam fixed at one side, subject to gravity
#
import numpy
# radius, number and distance of spheres
rad,num=1,6; dist=1.9999*rad
# one arm
O.bodies.append([utils.sphere((0,y,0),rad,wire=True) for y in numpy.arange(0,2*num-1,dist)])
# the lateral arm
O.bodies.append([utils.sphere((x,(num-1)*2*rad,0),rad,wire=True) for x in numpy.arange(dist,1+num/2,dist)])
# support sphere
O.bodies[0].state.blockedDOFs='xyzXYZ'
# small dt to see in realtime how it swings; real critical is higher, but much less than p-wave
O.dt=.01*utils.PWaveTimeStep() 

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop([Ig2_Sphere_Sphere_L6Geom(distFactor=-1)],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L6Geom_FrictPhys_Linear(charLen=1)]),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=0.1),
]
O.saveTmp()

try:
	from yade import qt
	v=qt.View(); v.axes=True
	v.viewDir=Vector3(-0.888,-0.2,-0.4144)
	v.eyePosition=Vector3(18.16,8.235,5.12)
	v.sceneRadius=7.5
	v.upVector=Vector3(-0.46,0.3885,0.798)
	v.screenSize=(900,900)
	rr=qt.Renderer()
	rr.intrGeom=True
	Gl1_L6Geom.phiScale=30; Gl1_L3Geom.uScale=20
	O.engines=O.engines+[
		qt.SnapshotEngine(fileBase=O.tmpFilename(),label='snapper',iterPeriod=300,deadTimeout=20),
		PyRunner(iterPeriod=330000,command='utils.makeVideo(snapper.snapshots,out="beam-l6geom.avi"); snapper.dead=True; O.pause()')
	]
except ImportError: pass
O.run()
