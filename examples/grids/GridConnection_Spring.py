#--- bruno.chareyre@hmg.inpg.fr ---
#!/usr/bin/python
# -*- coding: utf-8 -*-

# Experiment beam-like behaviour with chained cylinders + CohFrict connexions

from yade.gridpfacet import *

young=1.0e10
poisson=4
density=2.60e3 
frictionAngle=radians(30)
O.materials.append(CohFrictMat(young=young,poisson=poisson,density=density,frictionAngle=frictionAngle,normalCohesion=1e13,shearCohesion=1e13,momentRotationLaw=True,label='mat'))


O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Sphere_Aabb(),
		Bo1_GridConnection_Aabb(),
	]),
	InteractionLoop(
		# Geometric interactions
		[
          Ig2_GridNode_GridNode_GridNodeGeom6D(),
          Ig2_Sphere_GridConnection_ScGridCoGeom(),	# used for the cohesive sphere-cylinder interaction
        ],
		[
		# Interaction phusics
          Ip2_CohFrictMat_CohFrictMat_CohFrictPhys(setCohesionNow=True,setCohesionOnNewContacts=True),
        ],
		# Interaction law
		[
          Law2_ScGeom6D_CohFrictPhys_CohesionMoment(),
          Law2_ScGridCoGeom_CohFrictPhys_CundallStrack(),	# used for the cohesive sphere-cylinder interaction
        ]
	),
    GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8,label='ts'),
    NewtonIntegrator(gravity=(0,-9.81,0),damping=0.15,label='newton'),
    PyRunner(iterPeriod=500,command='history()'),

]

### Generate a spiral

### Create all nodes first
nodesIds=[]
rCyl=0.005
Ne=200
for i in range(0, Ne+1):
	omega=60.0/float(Ne); hy=0.10; hz=0.15;
	px=float(i)*(omega/60.0); py=sin(float(i)*omega)*hy; pz=cos(float(i)*omega)*hz;
	nodesIds.append( O.bodies.append( gridNode([px,py,pz],rCyl,wire=False,fixed=False,color=Vector3(0.6,0.5,0.5),material='mat') ) )

### Now create connection between the nodes
for i,j in zip( nodesIds[:-1], nodesIds[1:]):
  O.bodies.append( gridConnection(i,j,rCyl,color=Vector3(0.6,0.5,0.5),material='mat') )


def outp(id=1):
	for i in O.interactions:
		if i.id1 == 1:
			print i.phys.shearForce
			print i.phys.normalForce
			return  i

O.bodies[Ne-1].state.blockedDOFs='xyzXYZ'
yade.qt.View();


#plot some results
from yade import plot
plot.plots={'t':('pos1',None,'vel1')}
def history():
  	plot.addData(pos1=O.bodies[0].state.pos[1], # potential elastic energy
		     vel1=O.bodies[0].state.vel[1],
		     t=O.time)

#yade.qt.Renderer().bound=True
plot.plot(subPlots=False)
O.saveTmp()
#O.bodies[0].state.angVel=Vector3(0.05,0,0)

