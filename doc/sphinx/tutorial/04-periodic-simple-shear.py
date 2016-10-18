# encoding: utf-8

# script for periodic simple shear test, with periodic boundary
# first compresses to attain some isotropic stress (checkStress),
# then loads in shear (checkDistorsion)
# 
# the initial packing is either regular (hexagonal), with empty bands along the boundary,
# or periodic random cloud of spheres
#
# material friction angle is initially set to zero, so that the resulting packing is dense
# (sphere rearrangement is easier if there is no friction)
#


# setup the periodic boundary
O.periodic=True
O.cell.refSize=(2,2,2)

from yade import pack,plot

# the "if 0:" block will be never executed, therefore the "else:" block will be
# to use cloud instead of regular packing, change to "if 1:" or something similar
if 0:
	# create cloud of spheres and insert them into the simulation
	# we give corners, mean radius, radius variation
	sp=pack.SpherePack()
	sp.makeCloud((0,0,0),(2,2,2),rMean=.1,rRelFuzz=.6,periodic=True)
	# insert the packing into the simulation
	sp.toSimulation(color=(0,0,1)) # pure blue
else:
	# in this case, add dense packing
	O.bodies.append(
		pack.regularHexa(pack.inAlignedBox((0,0,0),(2,2,2)),radius=.1,gap=0,color=(0,0,1))
	)

# create "dense" packing by setting friction to zero initially
O.materials[0].frictionAngle=0

# simulation loop (will be run at every step)
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(damping=.4),
	# run checkStress function (defined below) every second
	# the label is arbitrary, and is used later to refer to this engine
	PyRunner(command='checkStress()',realPeriod=1,label='checker'),
	# record data for plotting every 100 steps; addData function is defined below
	PyRunner(command='addData()',iterPeriod=100)
]

# set the integration timestep to be 1/2 of the "critical" timestep
O.dt=.5*PWaveTimeStep()

# prescribe isotropic normal deformation (constant strain rate)
# of the periodic cell
O.cell.velGrad=Matrix3(-.1,0,0, 0,-.1,0, 0,0,-.1)

# when to stop the isotropic compression (used inside checkStress)
limitMeanStress=-5e5

# called every second by the PyRunner engine
def checkStress():
	# stress tensor as the sum of normal and shear contributions
	# Matrix3.Zero is the intial value for sum(...)
	stress=sum(normalShearStressTensors(),Matrix3.Zero)
	print 'mean stress',stress.trace()/3.
	# if mean stress is below (bigger in absolute value) limitMeanStress, start shearing
	if stress.trace()/3.<limitMeanStress:
		# apply constant-rate distorsion on the periodic cell
		O.cell.velGrad=Matrix3(0,0,.1, 0,0,0, 0,0,0)
		# change the function called by the checker engine
		# (checkStress will not be called anymore)
		checker.command='checkDistorsion()'
		# block rotations of particles to increase tanPhi, if desired
		# disabled by default
		if 0:
			for b in O.bodies:
				# block X,Y,Z rotations, translations are free
				b.state.blockedDOFs='XYZ'
				# stop rotations if any, as blockedDOFs block accelerations really
				b.state.angVel=(0,0,0)
		# set friction angle back to non-zero value
		# tangensOfFrictionAngle is computed by the Ip2_* functor from material
		# for future contacts change material (there is only one material for all particles)
		O.materials[0].frictionAngle=.5 # radians
		# for existing contacts, set contact friction directly
		for i in O.interactions: i.phys.tangensOfFrictionAngle=tan(.5)

# called from the 'checker' engine periodically, during the shear phase
def checkDistorsion():
	# if the distorsion value is >.3, exit; otherwise do nothing
	if abs(O.cell.trsf[0,2])>.5:
		# save data from addData(...) before exiting into file
		# use O.tags['id'] to distinguish individual runs of the same simulation
		plot.saveDataTxt(O.tags['id']+'.txt')
		# exit the program
		#import sys
		#sys.exit(0) # no error (0)
		O.pause()

# called periodically to store data history
def addData():
	# get the stress tensor (as 3x3 matrix)
	stress=sum(normalShearStressTensors(),Matrix3.Zero)
	# give names to values we are interested in and save them
	plot.addData(exz=O.cell.trsf[0,2],szz=stress[2,2],sxz=stress[0,2],tanPhi=stress[0,2]/stress[2,2],i=O.iter)
	# color particles based on rotation amount
	for b in O.bodies:
		# rot() gives rotation vector between reference and current position
		b.shape.color=scalarOnColorScale(b.state.rot().norm(),0,pi/2.)

# define what to plot (3 plots in total)
## exz(i), [left y axis, separate by None:] szz(i), sxz(i)
## szz(exz), sxz(exz)
## tanPhi(i)
# note the space in 'i ' so that it does not overwrite the 'i' entry
plot.plots={'i':('exz',None,'szz','sxz'),'exz':('szz','sxz'),'i ':('tanPhi',)}

# better show rotation of particles
Gl1_Sphere.stripes=True

# open the plot on the screen
plot.plot()

O.saveTmp()
