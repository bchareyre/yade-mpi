# coding: utf-8
# 2012 © Bruno Chareyre <bruno.chareyre_A_hmg.inpg.fr>
"Test and demonstrate the use of timestepper and density scaling."
from yade import *
from yade import pack,qt,timing
O.periodic=True

O.cell.hSize=Matrix3(0.1, 0, 0,
		     0 ,0.1, 0,
		    0, 0, 0.1)
		    
sp=pack.SpherePack()
num=sp.makeCloud(Vector3().Zero,O.cell.refSize,rRelFuzz=.9,num=2000,periodic=True,seed=1)
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()],verletDist=.05*radius),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(label='ts',timeStepUpdateInterval=1),
	PeriTriaxController(dynCell=True,mass=0.2,maxUnbalanced=0.001, relStressTol=0.01,goal=(-1e4,-1e4,0),stressMask=3,globUpdate=5, maxStrainRate=(10.,10.,10.),doneHook='triaxDone()',label='triax'),
	NewtonIntegrator(damping=.2),
]

phase=0
def triaxDone():
	global phase
	if phase==0:
		O.pause()

O.timingEnabled=1
qt.View()
O.saveTmp()

#======   First, we won't use a timestepper at all, we fix O.dt using PWave timestep   =========#
timing.reset()
O.dt=0.8*utils.PWaveTimeStep() #for fair comparison, we use the same safety factor as in GS timestepper
ts.active=False
O.run(100000,True);#it will actually stop before 100k iterations as soon as the packing is stable
print "--------------------------------"
print "Fixed dt = 0.8 * PWave timestep:"
print "--------------------------------"
timing.stats()

#======   Now we use the timestepper to adjust dt dynamicaly =========#
O.reload()
timing.reset()
O.dt=100000000 #or whatever
ts.active=True
O.run(100000,True);
print "--------------------------------------------------"
print "dt dynamicaly set with GlobalStiffness timesteper:"
print "--------------------------------------------------"
timing.stats()

#======  And finaly, the timestepper with density scaling =========#
O.reload()
timing.reset()
O.dt=100000000 #or whatever
#We force dt approx. 10e5 larger than the previous one. The inertia of bodies will adjusted automaticaly...
ts.targetDt=1
#... but not that of cell, hence we have to adjust it
triax.mass=triax.mass*(10e4)**2
O.run(1000000,True);
print "--------------------------------------------------------------------"
print "dt dynamicaly set with GlobalStiffness timesteper + density scaling:"
print "--------------------------------------------------------------------"
timing.stats()


#_____ TYPICAL RESULTS ____
#--------------------------------
#Fixed dt = 0.8 * PWave timestep:
#--------------------------------
#Name                                                    Count                 Time            Rel. time
#"ts"                                                  0                  0us                0.00%
#NewtonIntegrator                                  45561           32739725us               23.42%
#TOTAL                                                            139784778us              100.00%

#--------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper:
#--------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#"ts"                                              13626            3508618us                7.44%
#NewtonIntegrator                                  13626           10191836us               21.61%
#TOTAL                                                             47155258us              100.00%

#--------------------------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper + density scaling:
#--------------------------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#"ts"                                               8241            2413068us                8.67%
#NewtonIntegrator                                   8241            6434085us               23.13%
#TOTAL                                                             27818244us              100.00%    

