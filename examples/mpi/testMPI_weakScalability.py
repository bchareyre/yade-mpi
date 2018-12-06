# In order for mpy module to work, don't forget to make a symlink to yade executable named "yadeimport.py":
# ln -s path/to/yade/yade-version path/to/yade/yadeimport.py
#
# Possible executions of this script
### Parallel:
# mpiexec -n 4 yade-mpi -n -x testMPIxNxM.py
# mpiexec -n 4 yade-mpi  -n -x testMPIxN.py N M # (n-1) subdomains with NxM spheres each
### Monolithic:
# yade-mpi -n -x testMPIxN.py 
# yade-mpi -n -x testMPIxN.py N M
# yade-mpi -n -x testMPIxN.py N M n
# in last line the optional argument 'n' has the same meaning as with mpiexec, i.e. total number of bodies will be (n-1)*N*M but on single core
### Openmp:
# yade-mpi -j4 -n -x testMPIxN.py N M n
### Nexted MPI * OpenMP
# needs testing...
'''
This script simulates spheres falling on a plate using a distributed memory approach based on mpy module
The number of spheres assigned to one particular process (aka 'worker') is N*M, they form a regular patern.
The master process (rank=0) has no spheres assigned; it is in charge of getting the total force on the plate
The number of subdomains depends on argument 'n' of mpiexec. Since rank=0 is not assigned a regular subdomain the total number of spheres is (n-1)*N*M

'''
readParamsFromTable(noTableOk=True,N=100,M=10)
from yade.params.table import *

NSTEPS=2000 #turn it >0 to see time iterations, else only initilization
#N=100; M=100; #(columns, rows) per thread

#################
# Check MPI world
# This is to know if it was run with or without mpiexec (see preamble of this script)
import os
rank = os.getenv('OMPI_COMM_WORLD_RANK')
if rank is not None: #mpiexec was used
	rank=int(rank)
	numThreads=int(os.getenv('OMPI_COMM_WORLD_SIZE'))
# else: #non-mpi execution, numThreads will still be used as multiplier for the problem size (2 => multiplier is 1)
# 	numThreads=2 if len(sys.argv)<4 else (int(sys.argv[3]))
# 	print "numThreads",numThreads
# 	
# if len(sys.argv)>1: #we then assume N,M are provided as 1st and 2nd cmd line arguments
# 	N=int(sys.argv[1]); M=int(sys.argv[2])

############  Build a scene (we use Yade's pre-filled scene)  ############

# sequential grain colors
import colorsys
colorScale = (Vector3(colorsys.hsv_to_rgb(value*1.0/numThreads, 1, 1)) for value in range(0, numThreads))

#add spheres
if rank==0:
	for sd in range(0,numThreads-1):
		col = next(colorScale)
		ids=[]
		for i in range(N):#(numThreads-1) x N x M spheres, one thread is for master and will keep only the wall, others handle spheres
			for j in range(M):
				id = O.bodies.append(sphere((sd*N+i+j/30.,j,0),0.500,color=col)) #a small shift in x-positions of the rows to break symmetry
				ids.append(id)
		if rank is not None:# assigning subdomain!=0 in single thread would freeze the particles (Newton skips them)
			for id in ids: O.bodies[id].subdomain = sd+1

WALL_ID=O.bodies.append(box(center=(numThreads*N*0.5,-0.5,0),extents=(2*numThreads*N,0,2),fixed=True))

collider.verletDist = 0.7
collider.targetInterv=200
newton.gravity=(0,-10,0) #else nothing would move
tsIdx=O.engines.index(timeStepper) #remove the automatic timestepper. Very important: we don't want subdomains to use many different timesteps...
O.engines=O.engines[0:tsIdx]+O.engines[tsIdx+1:]
#O.dt=0.00002 #this very small timestep will make it possible to run 2000 iter without merging
O.dt=0.1*PWaveTimeStep() #very important, we don't want subdomains to use many different timesteps...


def collectTimingMPI():
	created = os.path.isfile("collect_2D_weakScalability-"+timeStr+".dat")
	f=open("collect_2D_weakScalability-"+timeStr+".dat",'a')
	if not created: f.write("numThreads mpi omp Nspheres force TotalRuntime isendRecvForces sendRecvStates waitForces collisionChecker collider\n")
	from yade import timing
	#f.write(str(globalDomain.numThreads)+" "+str(os.getenv('OMPI_COMM_WORLD_SIZE'))+" "+os.getenv('OMP_NUM_THREADS')+" "+str(len(O.bodies))++" "+str(O.forces.f(WALL_ID)[1])+" "+str(timing.runtime())+" ""\n")
	f.write("%s %s %s %g %g %g %g %g %g %g\n" % (numThreads, str(os.getenv('OMPI_COMM_WORLD_SIZE')), os.getenv('OMP_NUM_THREADS'), len(O.bodies), O.forces.f(WALL_ID)[1], timing.runtime()/1e6, isendRecvForcesRunner.execTime/1e6, sendRecvStatesRunner.execTime/1e6,waitForcesRunner.execTime/1e6, collisionChecker.execTime/1e6, collider.execTime/1e6))
	f.close()

#########  RUN  ##########
def collectTiming():
	created = os.path.isfile("collect.dat")
	f=open('collect.dat','a')
	if not created: f.write("numThreads mpi omp Nspheres N M runtime \n")
	from yade import timing
	f.write(str(numThreads)+" "+str(os.getenv('OMPI_COMM_WORLD_SIZE'))+" "+os.getenv('OMP_NUM_THREADS')+" "+str(N*M*(numThreads-1))+" "+str(N)+" "+str(M)+" "+str(timing.runtime())+"\n")
	f.close()


if rank is None: #######  Single-core  ######
	O.timingEnabled=True
	O.run(NSTEPS,True)
	#print "num bodies:",len(O.bodies)
	from yade import timing
	timing.stats()
	collectTiming()
	print "num. bodies:",len([b for b in O.bodies]),len(O.bodies)
	print "Total force on floor=",O.forces.f(WALL_ID)[1]
else: #######  MPI  ######
	#import yade's mpi module
	from yade import mpy as mp
	# customize
	mp.ACCUMULATE_FORCES=True #trigger force summation on master's body (here WALL_ID)
	mp.VERBOSE_OUTPUT=False
	mp.ERASE_REMOTE=True #erase bodies not interacting wit a given subdomain?
	mp.OPTIMIZE_COM=True #L1-optimization: pass a list of double instead of a list of states
	mp.USE_CPP_MPI=True and mp.OPTIMIZE_COM #L2-optimization: workaround python by passing a vector<double> at the c++ level

	mp.mpirun(NSTEPS,True)
	print "num. bodies:",len([b for b in O.bodies]),len(O.bodies)
	if rank==0:
		mp.mprint( "Total force on floor="+str(O.forces.f(WALL_ID)[1]))
		collectTimingMPI()
	else: mp.mprint( "Partial force on floor="+str(O.forces.f(WALL_ID)[1]))
	mp.mergeScene()
	#if rank==0: O.save('mergedScene.yade')
	mp.MPI.Finalize()
exit()
