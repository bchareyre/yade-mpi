# -*- coding: utf-8 -*-

# Check test version of normalInelasticityTest


#Def of the material which will be used
O.materials.append(NormalInelasticMat(density=2600,young=4.0e9,poisson=.04,frictionAngle=.6,coeff_dech=3.0,label='Materiau1'))

#Def of the bodies of the simulations : 2 spheres, with names which will be useful after
O.bodies.append(utils.sphere([0,0,0], 1, fixed=True, wire=False, color=None, highlight=False)) #'Materiau1', as the latest material defined, will be used
O.bodies.append(utils.sphere([0,2,0], 1, fixed=True, wire=False, color=None, highlight=False))

lowerSphere=O.bodies[0]
upperSphere=O.bodies[1]


#Def of the engines taking part to the simulation loop
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=0),# use of verletDist>0 without NewtonIntegrator let crash ??
	InteractionLoop(
			      [Ig2_Sphere_Sphere_ScGeom6D()],
			      [Ip2_2xNormalInelasticMat_NormalInelasticityPhys(betaR=0.24)],
			      [Law2_ScGeom6D_NormalInelasticityPhys_NormalInelasticity()]
			      ),
	PyRunner(iterPeriod=1,command='letMove()')
	]


#Def of the python commands which will impose required displacements to the moving sphere
def letMove():#Load for the first 10 iterations, unload for the 7 following iterations, then reload
   if mode=='normal':
	vImposed=[0,-1,0]
	if O.iter < 25 and O.iter>14:
		vImposed=[0,1,0]
   if mode=='tangential':
	vImposed=[1,0,0]
   upperSphere.state.vel=vImposed
   upperSphere.state.pos=upperSphere.state.pos+upperSphere.state.vel*O.dt




# ------ Test of the law in the normal direction, using python commands to let move ------ #
mode='normal'
O.dt=1e-5
O.run(40,True)



# ------ Test of the law in the tangential direction, still with python function ------ #

mode='tangential'
O.run(1000)


## ------ Test of the law for the moment, using blockedDOF_s and NewtonIntegrator ------ #

##To use blockedDOF_s, the body has to be dynamic....
upperSphere.dynamic=True
upperSphere.state.blockedDOFs='xyzXYZ'
upperSphere.state.angVel=Vector3(0,0,1)
upperSphere.state.vel=Vector3(0,0,0)

O.engines=O.engines[:3]+[NewtonIntegrator()]


  
O.run(8000,True)




# Reference value of force acting on upperSphere, (r2849)
fRef=Vector3(-519943.97434309247,760000.00000497897,0)
# Reference value of torque acting on upperSphere, (r2849)
tRef=Vector3(0,0,-1279894.5796705084)

#Actual values:
f=O.forces.f(1)
t=O.forces.t(1)

dF=fRef-f
dT=tRef-t

tolerance=0.01
if (dF.norm()>tolerance or dT.norm()>tolerance):
  print "Regression concerning normalInelasticity-test. We compare loads acting on one sphere, moving with respect to another. At the end of the script (after some relative movements) we have a difference, compared to reference values, of"
  print "- force (the norm) equal to ", dF.norm()
  print "- torque (the norm) equal to ", dT.norm()
  print "Whereas tolerance is ", tolerance
  resultStatus +=1

