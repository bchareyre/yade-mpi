# gravity deposition, continuing with oedometric test after stabilization
# shows also how to run parametric studies with yade-batch

# The components of the batch are:
# 1. table with parameters, one set of parameters per line (ccc.table)
# 2. readParamsFromTable which reads respective line from the parameter file
# 3. the simulation muse be run using yade-batch, not yade
#
# $ yade-batch --job-threads=1 03-oedometric-test.table 03-oedometric-test.py
#

# load parameters from file if run in batch
# default values are used if not run from batch
readParamsFromTable(rMean=.05,rRelFuzz=.3,maxLoad=1e6,minLoad=1e4)
# make rMean, rRelFuzz, maxLoad accessible directly as variables later
from yade.params.table import *

# create box with free top, and ceate loose packing inside the box
from yade import pack, plot
O.bodies.append(geom.facetBox((.5,.5,.5),(.5,.5,.5),wallMask=31))
sp=pack.SpherePack()
sp.makeCloud((0,0,0),(1,1,1),rMean=rMean,rRelFuzz=rRelFuzz)
sp.toSimulation()

O.engines=[
	ForceResetter(),
	# sphere, facet, wall
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),Bo1_Wall_Aabb()]),
	InteractionLoop(
		# the loading plate is a wall, we need to handle sphere+sphere, sphere+facet, sphere+wall
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom(),Ig2_Wall_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(gravity=(0,0,-9.81),damping=0.5),
	# the label creates an automatic variable referring to this engine
	# we use it below to change its attributes from the functions called
	PyRunner(command='checkUnbalanced()',realPeriod=2,label='checker'),
]
O.dt=.5*PWaveTimeStep()

# the following checkUnbalanced, unloadPlate and stopUnloading functions are all called by the 'checker'
# (the last engine) one after another; this sequence defines progression of different stages of the
# simulation, as each of the functions, when the condition is satisfied, updates 'checker' to call
# the next function when it is run from within the simulation next time

# check whether the gravity deposition has already finished
# if so, add wall on the top of the packing and start the oedometric test
def checkUnbalanced():
	# at the very start, unbalanced force can be low as there is only few contacts, but it does not mean the packing is stable
	if O.iter<5000: return 
	# the rest will be run only if unbalanced is < .1 (stabilized packing)
	if unbalancedForce()>.1: return 
	# add plate at the position on the top of the packing
	# the maximum finds the z-coordinate of the top of the topmost particle
	O.bodies.append(wall(max([b.state.pos[2]+b.shape.radius for b in O.bodies if isinstance(b.shape,Sphere)]),axis=2,sense=-1))
	global plate        # without this line, the plate variable would only exist inside this function
	plate=O.bodies[-1]  # the last particles is the plate
	# Wall objects are "fixed" by default, i.e. not subject to forces
	# prescribing a velocity will therefore make it move at constant velocity (downwards)
	plate.state.vel=(0,0,-.1)
	# start plotting the data now, it was not interesting before
	O.engines=O.engines+[PyRunner(command='addPlotData()',iterPeriod=200)]
	# next time, do not call this function anymore, but the next one (unloadPlate) instead
	checker.command='unloadPlate()'

def unloadPlate():
	# if the force on plate exceeds maximum load, start unloading
	if abs(O.forces.f(plate.id)[2])>maxLoad:
		plate.state.vel*=-1
		# next time, do not call this function anymore, but the next one (stopUnloading) instead
		checker.command='stopUnloading()'

def stopUnloading():
	if abs(O.forces.f(plate.id)[2])<minLoad:
		# O.tags can be used to retrieve unique identifiers of the simulation
		# if running in batch, subsequent simulation would overwrite each other's output files otherwise
		# d (or description) is simulation description (composed of parameter values)
		# while the id is composed of time and process number
		plot.saveDataTxt(O.tags['d.id']+'.txt')
		O.pause()
	
def addPlotData():
	if not isinstance(O.bodies[-1].shape,Wall):
		plot.addData(); return
	Fz=O.forces.f(plate.id)[2]
	plot.addData(Fz=Fz,w=plate.state.pos[2]-plate.state.refPos[2],unbalanced=unbalancedForce(),i=O.iter)

# besides unbalanced force evolution, also plot the displacement-force diagram
plot.plots={'i':('unbalanced',),'w':('Fz',)}
plot.plot()

O.run()
# when running with yade-batch, the script must not finish until the simulation is done fully
# this command will wait for that (has no influence in the non-batch mode)
waitIfBatch()


