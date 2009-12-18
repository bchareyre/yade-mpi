#
# see http://yade.wikia.com/wiki/ScriptParametricStudy#scripts.2Fmulti.py for explanations
#
## read parameters from table here
from yade import utils, plot
utils.readParamsFromTable(gravity=-9.81,density=2400,initialSpeed=10,noTableOk=True)
print gravity,density,initialSpeed

o=Omega()
o.initializers=[
		BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()])
	]
o.engines=[
	PhysicalActionContainerReseter(),
	BoundDispatcher([
		Bo1_Sphere_Aabb(),
		Bo1_Box_Aabb(),
	]),
	PersistentSAPCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[SimpleElasticRelationships(),],
		[ef2_Spheres_Elastic_ElasticLaw(),]
	),
	GravityEngine(gravity=(0,0,gravity)), ## here we use the 'gravity' parameter
	PeriodicPythonRunner(iterPeriod=100,command='myAddPlotData()',label='plotDataCollector'),
	NewtonIntegrator(damping=0.4)
]
o.bodies.append(utils.box([0,50,0],extents=[1,50,1],dynamic=False,color=[1,0,0],young=30e9,poisson=.3,density=density)) ## here we use the density parameter
o.bodies.append(utils.sphere([0,0,10],1,color=[0,1,0],young=30e9,poisson=.3,density=density)) ## here again

o.bodies[1].phys['velocity']=[0,initialSpeed,0] ## assign initial velocity

o.dt=.8*utils.PWaveTimeStep()
## o.saveTmp('initial')
def myAddPlotData():
	s=O.bodies[1]
	plot.addData({'t':O.time,'y_sph':s.phys.pos[1],'z_sph':s.phys.pos[2]})
plot.plots={'y_sph':('z_sph',)}

# run 30000 iterations
o.run(20000,True)

# write some results to a common file
# (we rely on the fact that 2 processes will not write results at exactly the same time)
# 'a' means to open for appending
file('multi.out','a').write('%s %g %g %g %g\n'%(o.tags['description'],gravity,density,initialSpeed,o.bodies[1].phys.pos[1]))
print 'gnuplot',plot.saveGnuplot(O.tags['id'])
