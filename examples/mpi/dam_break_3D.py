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

NSTEPS=5000 #turn it >0 to see time iterations, else only initilization TODO!HACK
#NSTEPS=50 #turn it >0 to see time iterations, else only initilization
#N=100; M=100; #(columns, rows) per thread

#################
# Check MPI world
# This is to know if it was run with or without mpiexec (see preamble of this script)
import os
rank = os.getenv('OMPI_COMM_WORLD_RANK')
if rank is not None: #mpiexec was used
	rank=int(rank)
	numThreads=int(os.getenv('OMPI_COMM_WORLD_SIZE'))
else: #non-mpi execution, numThreads will still be used as multiplier for the problem size (2 => multiplier is 1)
	numThreads=2 if len(sys.argv)<4 else (int(sys.argv[3]))
	print "numThreads",numThreads
	
if len(sys.argv)>1: #we then assume N,M are provided as 1st and 2nd cmd line arguments
	N=int(sys.argv[1]); M=int(sys.argv[2])

############  Build a scene (we use Yade's pre-filled scene)  ############

# sequential grain colors
import colorsys
from yade import pack
colorScale = (Vector3(colorsys.hsv_to_rgb(value*1.0/numThreads, 1, 1)) for value in range(0, numThreads))

#generate packing 
mn,mx=Vector3(0,2,0),Vector3(50,180,90)
pred = pack.inAlignedBox(mn,mx)
O.bodies.append(pack.regularHexa(pred,radius=1.20,gap=0)) #change radius for more spheres, r=0.45 --> ~1.5M particles 
nspheres = len(O.bodies)
for b in O.bodies: 
    b.material.frictionAngle = 0.05 
#print "len  = ", nspheres
wallIds=aabbWalls([Vector3(0,-1,-0),Vector3(80,360,200)],thickness=0)
O.bodies.append(wallIds)
WALL_ID=wallIds[2]

#print "len of bodies = ", len(O.bodies)

collider.verletDist = 0.4
collider.targetInterv=200
newton.gravity=(0,-17,0) #else nothing would move
tsIdx=O.engines.index(timeStepper) #remove the automatic timestepper. Very important: we don't want subdomains to use many different timesteps...
O.engines=O.engines[0:tsIdx]+O.engines[tsIdx+1:]
#O.dt=0.00002 #this very small timestep will make it possible to run 2000 iter without merging
O.dt=0.1*PWaveTimeStep() #very important, we don't want subdomains to use many different timesteps...


#########  RUN  ##########
def collectTiming():
	created = os.path.isfile("collect_bisection.dat")
	f=open('collect.dat','a')
	if not created: f.write("numThreads mpi omp Nspheres runtime \n")
	from yade import timing
	f.write(str(numThreads)+" "+str(os.getenv('OMPI_COMM_WORLD_SIZE'))+" "+os.getenv('OMP_NUM_THREADS')+" "+str(nspheres)+" "+str(timing.runtime())+"\n")
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
	#from yade import mpy as mp TODO!HACK
	from yade import mpf_bisection as mp
	# customize
	mp.ACCUMULATE_FORCES=True #trigger force summation on master's body (here WALL_ID)
	mp.VERBOSE_OUTPUT=False
	mp.ERASE_REMOTE=True #erase bodies not interacting wit a given subdomain?
	mp.OPTIMIZE_COM=True #L1-optimization: pass a list of double instead of a list of states
	mp.USE_CPP_MPI=True and mp.OPTIMIZE_COM #L2-optimization: workaround python by passing a vector<double> at the c++ level
	mp.NDIM=3 
	mp.BISECTION_DECOMPOSITION=True 
	mp.REASSIGN_SUBDOMAINS=True
        #mp.mpirun(1,True)
        #if rank==0: O.save('initScene.yade') 	
	mp.mpirun(NSTEPS,True)
	print "num. bodies:",len([b for b in O.bodies]),len(O.bodies)
	if rank==0:
		#mp.mprint( "Total force on floor="+str(O.forces.f(WALL_ID)[1]))
		collectTiming()
	#else: mp.mprint( "Partial force on floor="+str(O.forces.f(WALL_ID)[1]))
	mp.mergeScene()
	#if rank==0: O.save('mergedScene.yade')
	mp.MPI.Finalize()
exit()

