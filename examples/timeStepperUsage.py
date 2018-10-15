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


#_____ TYPICAL RESULTS (N=1000, single core)____
#--------------------------------
#Fixed dt = 0.8 * PWave timestep:
#--------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     60126             466162us                0.94%      
#InsertionSortCollider                             23643           10263455us               20.65%      
#InteractionLoop                                   60126           22821530us               45.92%      
#"ts"                                                  0                  0us                0.00%      
#"triax"                                           60126            4447870us                8.95%      
#"newton"                                          60126           11704656us               23.55%      
#TOTAL                                                             49703674us              100.00%      

#--------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper:
#--------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     29396             234443us                0.88%      
#InsertionSortCollider                              7271            3634797us               13.66%      
#InteractionLoop                                   29396           13682597us               51.43%      
#"ts"                                               2977             342164us                1.29%      
#"triax"                                           29396            2754907us               10.36%      
#"newton"                                          29396            5955196us               22.38%      
#TOTAL                                                             26604106us              100.00%      

#WARN  /home/3S-LAB/bchareyre/yade/yade-fresh/trunk/pkg/dem/NewtonIntegrator.cpp:283 set_densityScaling: GlobalStiffnessTimeStepper found in O.engines and adjusted to match this setting. Revert in the timestepper if you don't want the scaling adjusted automatically.
#--------------------------------------------------------------------
#dt dynamicaly set with GlobalStiffness timesteper + density scaling:
#--------------------------------------------------------------------
#Name                                                    Count                 Time            Rel. time
#-------------------------------------------------------------------------------------------------------
#ForceResetter                                     27071             217134us                1.83%      
#InsertionSortCollider                               341             205483us                1.73%      
#InteractionLoop                                   27071            5238977us               44.24%      
#"ts"                                               2709             858071us                7.25%      
#"triax"                                           27071             619294us                5.23%      
#"newton"                                          27071            4704522us               39.72%      
#TOTAL                                                             11843484us              100.00%      
