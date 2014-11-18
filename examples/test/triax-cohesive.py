# encoding: utf-8
# 2012 ©Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
# This variant of triax-basic.py shows the usage of cohesive contact laws and moments at contacts

from yade import pack

sp=pack.SpherePack()
## corners of the initial packing
mn,mx=Vector3(0,0,0),Vector3(10,10,10)

## box between mn and mx, avg radius ± ½(20%), 2k spheres
sp.makeCloud(minCorner=mn,maxCorner=mx,rRelFuzz=.2,num=600)

## create material #0, which will be used as default
O.materials.append(CohFrictMat(young=15e6,poisson=0.4,density=2600,frictionAngle=radians(30),normalCohesion=1e6,shearCohesion=1e6,momentRotationLaw=True,etaRoll=0.1,label='spheres'))
O.materials.append(FrictMat(young=15e6,poisson=.4,frictionAngle=0,density=0,label='frictionlessWalls'))


## copy spheres from the packing into the scene
O.bodies.append([sphere(center,rad,material='spheres') for center,rad in sp])
## create walls around the packing
walls=aabbWalls(material='frictionlessWalls')
wallIds=O.bodies.append(walls)

triax=TriaxialCompressionEngine(
	wall_bottom_id=wallIds[2],
	wall_top_id=wallIds[3],
	wall_left_id=wallIds[0],
	wall_right_id=wallIds[1],
	wall_back_id=wallIds[4],
	wall_front_id=wallIds[5],
	internalCompaction=False,
	sigmaIsoCompaction=-50e3,
	sigmaLateralConfinement=-50e3,
	max_vel=10,
	strainRate=0.03,
	label="triax"
)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		#box-sphere interactions will be the simple normal-shear law, we use ScGeom for them
		[Ig2_Sphere_Sphere_ScGeom6D(),Ig2_Box_Sphere_ScGeom()],
		#Boxes will be frictional (FrictMat), so the sphere-box physics is FrictMat vs. CohFrictMat, the Ip type will be found via the inheritance tree (CohFrictMat is a FrictMat) and will result in FrictPhys interaction physics
		#and will result in a FrictPhys
		[Ip2_FrictMat_FrictMat_FrictPhys(),Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(label="cohesiveIp")],
		#Finally, two different contact laws for sphere-box and sphere-sphere
		[Law2_ScGeom_FrictPhys_CundallStrack(),Law2_ScGeom6D_CohFrictPhys_CohesionMoment(
			useIncrementalForm=True, #useIncrementalForm is turned on as we want plasticity on the contact moments
			always_use_moment_law=False,  #if we want "rolling" friction even if the contact is not cohesive (or cohesion is broken), we will have to turn this true somewhere
			label='cohesiveLaw')]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.5),
	triax,
	NewtonIntegrator(damping=.4)
]

from yade import plot
O.engines=O.engines[0:5]+[PyRunner(iterPeriod=20,command='history()',label='recorder')]+O.engines[5:7]
def history():
  	plot.addData(e11=-O.engines[4].strain[0], e22=-O.engines[4].strain[1], e33=-O.engines[4].strain[2],
		    s11=-O.engines[4].stress(0)[0],
		    s22=-O.engines[4].stress(2)[1],
		    s33=-O.engines[4].stress(4)[2],
		    i=O.iter)

plot.plots={'i':(('e11',"bo"),('e22',"ro"),('e33',"go"),None,('s11',"bx"),('s22',"rx"),('s33',"gx"))}
plot.plot()

print "computing, be patient..."
#First run without moment and without cohesion
O.run(7000,True)

#This will reload the autosaved compacted sample
O.reload()

#second run with rolling friction
O.engines[2].lawDispatcher.functors[1].always_use_moment_law = True
O.run(6000,True)
O.reload()

#third run with rolling friction + cohesion
O.engines[2].lawDispatcher.functors[1].always_use_moment_law = True
#We assign cohesion to all contacts at the next iteration
O.engines[2].physDispatcher.functors[1].setCohesionNow = True
O.run(6000,True)

