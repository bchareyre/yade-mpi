# script for testing InterpolatingHelixEngine: sphere going in a sphere-like motion around bar
O.bodies.append([box([0,0,0],[.005,.005,1],fixed=True),sphere([0,.1,-1],.04,fixed=False)])
O.engines=[
	InterpolatingHelixEngine(ids=[1],times=[10,20,30,40,50,60,70,80,90,100],angularVelocities=[1,2,3,4,5,3,1,-1,-3,0],rotationAxis=[0,0,1],zeroPoint=[0,0,0], wrap=True,slope=.003,label='spiral'),
]
O.dt=4e-6
O.saveTmp('initial')
from yade import qt
qt.Controller()
