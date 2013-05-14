# coding: utf-8
# 2012 © Bruno Chareyre <bruno.chareyre_A_hmg.inpg.fr>
"Test and demonstrate the use of timestepper and density scaling."
from yade import *
from yade import pack,qt,timing
O.periodic=True

O.cell.hSize=Matrix3(0.1, 0, 0,
		     0 ,0.1, 0,
		    0, 0, 0.1)

n=1000

sp=pack.SpherePack()
num=sp.makeCloud(Vector3().Zero,O.cell.refSize,rRelFuzz=.5,num=n,periodic=True,seed=1)
O.bodies.append([sphere(s[0],s[1]) for s in sp])

#make the problem more interesting by giving a smaller mass to one of the bodies, different stiffness present similar difficulties
O.bodies[3].state.mass = O.bodies[n-1].state.mass*0.01

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(label='ts'),
	PeriTriaxController(dynCell=True,mass=0.2,maxUnbalanced=0.001, relStressTol=0.01,goal=(-1e4,-1e4,0),stressMask=3,globUpdate=5, maxStrainRate=(10.,10.,10.),doneHook='triaxDone()',label='triax'),
	NewtonIntegrator(damping=.2,label="newton"),
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
O.dt=0.8*PWaveTimeStep() #for fair comparison, we use the same safety factor as in GS timestepper, allthough many scripts use 0.5 or even 0.1*PWaveTimeStep()
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
O.dt=1000000 #or whatever
#We force dt=1. The inertia of bodies will adjusted automatically...
newton.densityScaling=True
#... but not that of cell, hence we have to adjust it or the problem becomes unstable
triax.mass=triax.mass*(1e4)**2
O.run(1000000,True);
print "--------------------------------------------------------------------"
print "dt dynamicaly set with GlobalStiffness timesteper + density scaling:"
print "--------------------------------------------------------------------"
timing.stats()


#_____ TYPICAL RESULTS (n=1000)____

#--------------------------------
#Fixed dt = 0.8 * PWave timestep:
#--------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     77246             326813us                0.24%
#InsertionSortCollider                             23571           29447986us               21.73%
#InteractionLoop                                   77246           65954607us               48.67%
#"ts"                                                  0                  0us                0.00%
#"triax"                                           77246           11876934us                8.76%
#"newton"                                          77246           27914757us               20.60%
#TOTAL                                                            135521099us              100.00%

#--------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper:
#--------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     45666             193813us                0.23%
#InsertionSortCollider                              6727            9456709us               11.09%
#InteractionLoop                                   45666           44245384us               51.87%
#"ts"                                              45666            6267682us                7.35%
#"triax"                                           45666            8234896us                9.65%
#"newton"                                          45666           16906118us               19.82%
#TOTAL                                                             85304605us              100.00%

#--------------------------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper + density scaling:
#--------------------------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     23936             102435us                0.25%
#InsertionSortCollider                               305             534572us                1.29%
#InteractionLoop                                   23936           23748011us               57.11%
#"ts"                                              23936            3520397us                8.47%
#"triax"                                           23936            4623106us               11.12%
#"newton"                                          23936            9051032us               21.77%
#TOTAL                                                             41579556us              100.00%
