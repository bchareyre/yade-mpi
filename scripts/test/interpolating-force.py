# encoding: utf-8
# 2009 Václav Šmilauer <eudoxos@arcig.cz>

# script showing how to use InterpolatingDirectedForceEngine,
# simply pushing one free sphere agains a fixed one with varying force
#
# The force evolution is sine wave, but it could really be any data

from numpy import arange

nPulses=4 # run for total of 4 pulses
freq=10. # 5 pulses per second
times=arange(0,1/freq,.01/freq) # generate 100 points equally distributed over the period (can be much more)
maxMag=1e5 # maximum magnitude of applied force
magnitudes=[.5*maxMag*(sin(t*(freq*2*pi))+1) for t in times] # generate points on sine wave over 1 period, but shifted up to be ∈(0,2)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_Basic()]
	),
	# subscribedBodies: what bodies is force applied to
	# direction: direction of the force (normalized automatically), constant
	# magnitudes: series of force magnitude
	# times: time points at which magnitudes are defined
	# wrap: continue from t0 once t_last is reached
	# label: automatically defines python variable of that name pointing to this engine
	InterpolatingDirectedForceEngine(subscribedBodies=[1],direction=[0,0,-1],magnitudes=magnitudes,times=times,wrap=True,label='forcer'),
	# without damping, the interaction never stabilizes and oscillates wildly… try it
	NewtonIntegrator(damping=0.01),
	# collect some data to plot periodically (every 50 steps)
	PyRunner(iterPeriod=1,command='myAddPlotData()')
]

O.bodies.append([
	utils.sphere([0,0,0],1,dynamic=False,color=[1,0,0]),
	utils.sphere([0,0,2],1,color=[0,1,0])
])

# elastic timestep
O.dt=.5*utils.PWaveTimeStep()

# callback for plotDataCollector
import yade.plot as yp
def myAddPlotData():
	yp.addData(t=O.time,F_applied=forcer.force[2],supportReaction=O.forces.f(0)[2])

O.saveTmp()

# try open 3d view, if not running in pure console mode
try:
	import yade.qt
	yade.qt.View()
except ImportError: pass

# run so many steps such that prescribed number of pulses is done
O.run(int((nPulses/freq)/O.dt),True)

# plot the time-series of force magnitude
import pylab
pylab.plot(times,magnitudes,label='Force magnitude over 1 pulse'); pylab.legend(('Force magnitude',)); pylab.xlabel('t'); pylab.grid(True)
# plot some recorded data
yp.plots={'t':('F_applied','supportReaction')}
yp.plot()

