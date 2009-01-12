# script for testing InterpolatingSpiralEngine: sphere going in a sphere-like motion around bar
O.bodies.append([utils.box([0,0,0],[.005,.005,1],dynamic=False),utils.sphere([0,.1,-1],.04,dynamic=False)])
O.engines=[
	DeusExMachina('InterpolatingSpiralEngine',{'subscribedBodies':[1],'times':[10,20,30,40,50,60,70,80,90,100],'angularVelocities':[1,2,3,4,5,3,1,-1,-3,0],'axis':[0,0,1],'axisPt':[0,0,0],'period':-1,'slope':.003,'label':'spiral'}),
]
O.dt=5e-4
O.saveTmp('initial')
from yade import qt
qt.Controller()
