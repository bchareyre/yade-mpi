# -*- coding: utf-8 -*-

# Script to test the constitutive law contained in NormalInelasticityLaw : consider two spheres whose penetration of the contact evolves => Monitor of the normal force

from yade import plot

#Def of the material which will be used
O.materials.append(CohFrictMat(density=2600,young=4.0e9,poisson=.04,frictionAngle=.6,label='Materiau1'))

#Def of the bodies of the simulations : 2 spheres, with names which will be useful after
O.bodies.append(utils.sphere([0,0,0], 1, dynamic=False, wire=False, color=None, highlight=False)) #'Materiau1', as the latest material defined will be used
O.bodies.append(utils.sphere([0,2,0], 1, dynamic=False, wire=False, color=None, highlight=False))

LowerSphere=O.bodies[0]
UpperSphere=O.bodies[1]


#Def of the engines taking part to the simulation loop
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
			      [Ig2_Sphere_Sphere_ScGeom()],
			      [Ip2_2xCohFrictMat_NormalInelasticityPhys()],
			      [Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity(coeff_dech=3)]
			      ),
	PeriodicPythonRunner(iterPeriod=1,command='letMove()')
	]


#Def of the python command letMove() :
# Will move "by hand" the upperSphere towards or away from the lower one. Modifying by hand only the speed of bodies is indeed not sufficient, see NewtonsIntegrator, and https://bugs.launchpad.net/yade/+bug/398089. For such purposes you could also use TranslationEngine, or you can also use StepDisplacer, which applies finite change in position/orientation in each step
def letMove():#Load for the first 10 iterations, unload for the 7 following iterations, then reload
	vImposed=[0,-1,0]
	if O.iter < 25 and O.iter>14:
		vImposed=[0,1,0]
	UpperSphere.state.vel=vImposed
	UpperSphere.state.pos=UpperSphere.state.pos+UpperSphere.state.vel*O.dt


#Def of the python command defData() : which will be used once the interaction will really exist
def defData():
	i=O.interactions[1,0]
	VecFn=i.phys.normalForce
	VecDist=UpperSphere.state.pos-LowerSphere.state.pos
	plot.addData(Normfn=VecFn.norm(),NormfnBis=VecFn.norm(),FnY=VecFn[1],step=O.iter,unPerso=LowerSphere.shape.radius+UpperSphere.shape.radius-VecDist.norm(),unVrai=i.geom.penetrationDepth)




O.dt=1e-5

yade.qt.View()
O.run(2,True) #cycles "for free", so that the interaction between spheres will be defined (with his physics and so on)
O.engines=O.engines+[PeriodicPythonRunner(iterPeriod=1,command='defData()')]

#O.saveTmp('INL')
O.run(40,True)

# define of the plots to be made : un(step), and Fn(un)
plot.plots={'step':('unVrai',),'unPerso':('Normfn',),'unVrai':('NormfnBis',)}
plot.plot()

#NB : the shape of the curve Fn(un) seems to not be perfect. It is indeed not because of NormalInelasticityLaw. But of differences between the un computed here in this python script and the one which is computed in Ig2_Sphere_Sphere_ScGeom (see for example this "shift2"). Fn being linked to this last un, these slight differences explain the shape of the curves. If phys.penetrationDepth would exist in python, and thus could be directly considered, I think the curve would be perfect !
