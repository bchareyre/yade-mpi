# -*- coding: utf-8 -*-

# Script to test the constitutive law contained in NormalInelasticityLaw : consider two spheres moving one to each other (this script illustrates different ways of moving spheres)
#- first penetration of the contact evolves => Monitor of the normal force
#- then, test in tangential direction
#- finally for what concerns moment transfer, with relative orientation changes (use frictionAngle=0.0 in this case to study more easily what's going on)

#Different graphs illustrate the effects of the different loadings. The run is paused at each plot window (so that there is time to observe them). Type "Return" in the Yade terminal to go ahead.

from yade import plot

#Def of the material which will be used
O.materials.append(NormalInelasticMat(density=2600,young=4.0e9,poisson=.04,frictionAngle=.6,coeff_dech=3.0,label='Materiau1'))

#Def of the bodies of the simulations : 2 spheres, with names which will be useful after
O.bodies.append(utils.sphere([0,0,0], 1, dynamic=False, wire=False, color=None, highlight=False)) #'Materiau1', as the latest material defined, will be used
O.bodies.append(utils.sphere([0,2,0], 1, dynamic=False, wire=False, color=None, highlight=False))

lowerSphere=O.bodies[0]
upperSphere=O.bodies[1]


#Def of the engines taking part to the simulation loop
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
			      [Ig2_Sphere_Sphere_ScGeom()],
			      [Ip2_2xNormalInelasticMat_NormalInelasticityPhys(betaR=0.24)],
			      [Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity()]
			      ),
	PeriodicPythonRunner(iterPeriod=1,command='letMove()')
	]


#Def of the python command letMove() :
# Will move "by hand" the upperSphere towards or away from the lower one. Modifying by hand only the speed of bodies is indeed not sufficient, see NewtonsIntegrator, and https://bugs.launchpad.net/yade/+bug/398089. Alternative way is presented below
def letMove():#Load for the first 10 iterations, unload for the 7 following iterations, then reload
	vImposed=[0,-1,0]
	if O.iter < 25 and O.iter>14:
		vImposed=[0,1,0]
	upperSphere.state.vel=vImposed
	upperSphere.state.pos=upperSphere.state.pos+upperSphere.state.vel*O.dt


#Def of the python command defData() : which will be used once the interaction will really exist
def defData():
	i=O.interactions[1,0]
	vecFn=i.phys.normalForce
	vecDist=upperSphere.state.pos-lowerSphere.state.pos
	plot.addData(normFn=vecFn.norm(),normFnBis=vecFn.norm(),fnY=vecFn[1],step=O.iter,
	  unPerso=lowerSphere.shape.radius+upperSphere.shape.radius-vecDist.norm(),unVrai=i.geom.penetrationDepth,
	  gamma=upperSphere.state.pos[0]-lowerSphere.state.pos[0],fx=O.forces.f(0)[0],torque=O.forces.t(1)[2])
	#print i.geom.penetrationDepth


# ------ Test of the law in the normal direction, using python commands to let move ------ #

O.dt=1e-5

yade.qt.View()
O.run(2,True) #cycles "for free", so that the interaction between spheres will be defined (with his physics and so on)
O.engines=O.engines+[PeriodicPythonRunner(iterPeriod=1,command='defData()')]



O.run(40,True)

# define of the plots to be made : un(step), and Fn(un)
plot.plots={'step':('unVrai',),'unPerso':('normFn',),'unVrai':('normFnBis',)}
plot.plot()
raw_input()

#NB : these different unVrai and unPerso illustrate the definition of penetrationDepth really used in the code (computed in Ig2_Sphere_Sphere_ScGeom) which is slightly different from R1 + R2 - Distance (see for example this "shift2"). According to the really used penetrationDepth, Fn evolves as it should

#O.saveTmp('EndComp')
#O.save('FinN_I_Test.xml')


# ------ Test of the law in the tangential direction, using StepDisplacer ------ #

dpos=Vector3.Zero
Vector3.__init__(dpos,1*O.dt,0,0)

O.engines=O.engines[:4]+[StepDisplacer(subscribedBodies=[1],deltaSe3=(dpos,Quaternion.Identity),setVelocities=True)]+O.engines[5:]
O.run(1000)
plot.plots={'step':('gamma',),'gamma':('fx',)}
plot.plot()
raw_input()
plot.plots={'normFn':('fx',)}
plot.plot()
raw_input()
#pylab.show() #to pause on the plot window. Effective only first time

#-- Comments --#
#	- evolution of Fx with gamma normal (flat at the beginning because of the order of engines)
#	- un decreases indeed during this shear, but maybe a zoom on the curves is needed to see it.
#	- We can observe that the force state of the sample decreases a line with a slope equal to tan(~34.5°)=tan(~0.602 rad). Why not strict equality ? Because of the measure of the slope or because something else ? To see...
#	- during this phase O.forces.t(0).norm() / O.forces.f(0)[0] seems to increase between 0.502 and 0.507 (according to r=[T[i]/F[i] for i in range(50,T.__len__()-20) ])
#		Note to explain this that Fx = O.forces.f(0)[0] is more and more different from Ft, from which we can expect Ft = 	
#		Torque /Radius



## ------ Test of the law for the moment, using blockedDOF_s ------ #
#O.loadTmp('EndComp')

##To use blockedDOF_s, the body has to be dynamic....
upperSphere.dynamic=True
upperSphere.state.blockedDOFs='x','rx','y','ry','z','rz'
upperSphere.state.angVel=Vector3(0,0,1)
upperSphere.state.vel=Vector3(0,0,0)
i=O.interactions[1,0]
O.engines=O.engines[:4]+[NewtonIntegrator()]+O.engines[5:]#+[PeriodicPythonRunner(iterPeriod=1,command='printInfo()')]


def printInfo():
  print O.iter
  print upperSphere.state.ori
  print i.geom.penetrationDepth
  

O.run(8000,True)
plot.plots={'step':('torque',)}
plot.plot()
