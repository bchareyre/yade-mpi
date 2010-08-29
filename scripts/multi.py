#
# https://yade-dem.org/wiki/ScriptParametricStudy
#
## read parameters from table here
from yade import utils, plot
utils.readParamsFromTable(gravity=-9.81,density=2400,initialSpeed=10,noTableOk=True)
from yade.params.table import *
print gravity,density,initialSpeed

O.materials.append(FrictMat(young=30e9,density=density,poisson=.3)) ## use the 'density' parameter here

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb(),]),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys(),],
		[Law2_ScGeom_FrictPhys_Basic(),]
	),
	GravityEngine(gravity=(0,0,gravity)), ## here we use the 'gravity' parameter
	PeriodicPythonRunner(iterPeriod=100,command='myAddPlotData()',label='plotDataCollector'),
	NewtonIntegrator(damping=0.4)
]
O.bodies.append([
	utils.box([0,50,0],extents=[1,50,1],dynamic=False,color=[1,0,0]),
	utils.sphere([0,0,10],1,color=[0,1,0])
])

O.bodies[1].state.vel=(0,initialSpeed,0) ## assign initial velocity

O.dt=.8*utils.PWaveTimeStep()
## o.saveTmp('initial')
def myAddPlotData():
	s=O.bodies[1]
	plot.addData({'t':O.time,'y_sph':s.state.pos[1],'z_sph':s.state.pos[2]})
plot.plots={'y_sph':('z_sph',)}

# run 30000 iterations
O.run(20000,True)

# write some results to a common file
# (we rely on the fact that 2 processes will not write results at exactly the same time)
# 'a' means to open for appending
file('multi.out','a').write('%s %g %g %g %g\n'%(O.tags['description'],gravity,density,initialSpeed,O.bodies[1].state.pos[1]))
print 'gnuplot',plot.saveGnuplot(O.tags['id'])
