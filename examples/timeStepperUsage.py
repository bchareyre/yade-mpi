# coding: utf-8
# 2012 © Bruno Chareyre <bruno.chareyre_A_hmg.inpg.fr>
"Test and demonstrate the use of timestepper and density scaling."

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
	GlobalStiffnessTimeStepper(timeStepUpdateInterval=10,label='ts'),
	PeriTriaxController(dynCell=True,mass=0.2,maxUnbalanced=0.01, relStressTol=0.01,goal=(-1e4,-1e4,0),stressMask=3,globUpdate=5, maxStrainRate=(10.,10.,10.),doneHook='triaxDone()',label='triax'),
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
triax.mass /= (0.8*PWaveTimeStep())**2
triax.maxStrainRate *= 0.8*PWaveTimeStep()
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
#ForceResetter                                     58166             455809us                0.82%
#InsertionSortCollider                             23641           17777093us               31.92%
#InteractionLoop                                   58166           21812997us               39.17%
#"ts"                                                  0                  0us                0.00%
#"triax"                                           58166            4329739us                7.77%
#"newton"                                          58166           11314162us               20.32%
#TOTAL                                                             55689802us              100.00%

#--------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper:
#--------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     37471             296255us                0.82%
#InsertionSortCollider                              6589            5608966us               15.57%
#InteractionLoop                                   37471           18167532us               50.44%
#"ts"                                               3785             477777us                1.33%
#"triax"                                           37471            3801429us               10.55%
#"newton"                                          37471            7664305us               21.28%
#TOTAL                                                             36016267us              100.00%

#WARN  /home/3S-LAB/bchareyre/yade/yade-git/trunk/pkg/dem/NewtonIntegrator.cpp:282 set_densityScaling: GlobalStiffnessTimeStepper found in O.engines and adjusted to match this setting. Revert in the timestepper if you don't want the scaling adjusted automatically.
#--------------------------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper + density scaling:
#--------------------------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     31666             251568us                1.55%
#InsertionSortCollider                               429             429116us                2.64%
#InteractionLoop                                   31666            8135458us               50.06%
#"ts"                                               3168             244340us                1.50%
#"triax"                                           31666            1282567us                7.89%
#"newton"                                          31666            5909985us               36.36%
#TOTAL                                                             16253037us              100.00%
