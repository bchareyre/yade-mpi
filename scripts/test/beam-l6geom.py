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
O.bodies[0].state.blockedDOFs=['x','y','z','rx','ry','rz']
# small dt to see in realtime how it swings; real critical is higher, but much less than p-wave
O.dt=.001*utils.PWaveTimeStep() 

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop([Ig2_Sphere_Sphere_L6Geom_Inc()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_L6Geom_FrictPhys_Linear(charLen=1)]),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=0.03),
]
O.saveTmp()

try:
	from yade import qt
	v=qt.View(); v.axes=True
except: pass
O.run()
