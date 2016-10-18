# demonstrate 3d postprocessing with yade
#
# 1. qt.SnapshotEngine saves images of the 3d view as it appears on the screen periodically
#    makeVideo is then used to make real movie from those images
# 2. VTKRecorder saves data in files which can be opened with Paraview
#    see the User's manual for an intro to Paraview

# generate loose packing
from yade import pack, qt
sp=pack.SpherePack()
sp.makeCloud((0,0,0),(2,2,2),rMean=.1,rRelFuzz=.6,periodic=True)
# add to scene, make it periodic
sp.toSimulation()

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	NewtonIntegrator(damping=.4),
	# save data for Paraview
	VTKRecorder(fileName='3d-vtk-',recorders=['all'],iterPeriod=1000),
	# save data from Yade's own 3d view
	qt.SnapshotEngine(fileBase='3d-',iterPeriod=200,label='snapshot'),
	# this engine will be called after 20000 steps, only once
	PyRunner(command='finish()',iterPeriod=20000)
]
O.dt=.5*PWaveTimeStep()

# prescribe constant-strain deformation of the cell
O.cell.velGrad=Matrix3(-.1,0,0, 0,-.1,0, 0,0,-.1)

# we must open the view explicitly (limitation of the qt.SnapshotEngine)
qt.View()

# this function is called when the simulation is finished
def finish():
	# snapshot is label of qt.SnapshotEngine
	# the 'snapshots' attribute contains list of all saved files
	makeVideo(snapshot.snapshots,'3d.mpeg',fps=10,bps=10000)
	O.pause()

# set parameters of the renderer, to show network chains rather than particles
# these settings are accessible from the Controller window, on the second tab ("Display") as well
rr=yade.qt.Renderer()
rr.shape=False
rr.intrPhys=True

