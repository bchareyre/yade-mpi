# gravity deposition in box, showing how to plot and save history of data,
# and how to control the simulation while it is running by calling
# python functions from within the simulation loop

# import yade modules that we will use below
from yade import pack, plot

# create rectangular box from facets
O.bodies.append(utils.facetBox((.5,.5,.5),(.5,.5,.5),wallMask=31))

# create empty sphere packing
# sphere packing is not equivalent to particles in simulation, it contains only the pure geometry
sp=pack.SpherePack()
# generate randomly spheres with uniform radius distribution
sp.makeCloud((0,0,0),(1,1,1),rMean=.05,rRelFuzz=.5)
# add the sphere pack to the simulation
sp.toSimulation()

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		# handle sphere+sphere and facet+sphere collisions
		[Ig2_Sphere_Sphere_L3Geom(),Ig2_Facet_Sphere_L3Geom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_L3Geom_FrictPhys_ElPerfPl()]
	),
	GravityEngine(gravity=(0,0,-9.81)),
	NewtonIntegrator(damping=0.4),
	# call the checkUnbalanced function (defined below) every 2 seconds
	PyRunner(command='checkUnbalanced()',realPeriod=2),
	# call the addPlotData function every 200 steps
	PyRunner(command='addPlotData()',iterPeriod=100)
]
O.dt=.5*utils.PWaveTimeStep()

# enable energy tracking; any simulation parts supporting it
# can create and update arbitrary energy types, which can be
# accessed as O.energy['energyName'] subsequently
O.trackEnergy=True

# if the unbalanced forces goes below .05, the packing
# is considered stabilized, therefore we stop collected
# data history and stop
def checkUnbalanced():
	if utils.unbalancedForce()<.05:
		O.pause()
		plot.saveDataTxt('bbb.txt.bz2')
		# plot.saveGnuplot('bbb') is also possible

# collect history of data which will be plotted
def addPlotData():
	# each item is given a names, by which it can be the unsed in plot.plots
	# the **O.energy converts dictionary-like O.energy to plot.addData arguments
	plot.addData(i=O.iter,unbalanced=utils.unbalancedForce(),**O.energy)

# define how to plot data: 'i' (step number) on the x-axis, unbalanced force
# on the left y-axis, all energies on the right y-axis
# (O.energy.keys is function which will be called to get all defined energies)
# None separates left and right y-axis
plot.plots={'i':('unbalanced',None,O.energy.keys)}

# show the plot on the screen, and update while the simulation runs
plot.plot()

O.saveTmp()
