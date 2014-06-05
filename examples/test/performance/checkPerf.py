# -*- coding: utf-8

from yade import pack,export,geom,timing,bodiesHandling
import time,numpy
	
radRAD=[
	23.658,				#5000 elements
	40.455,				#25000 elements
	50.97,				#50000 elements
	64.218,				#100000 elements
	80.91,				#200000 elements
	#109.811,			#500000 elements
]

iterN=[
	12000,			#5000 elements
	2500,				#25000 elements
	1400,				#50000 elements
	800,				#100000 elements
	200,				#200000 elements
	#10,				#500000 elements
]

coefCor=[
	110,
	28,
	18,
	9,
	2,
	#0.1,
]

iterVel=[]
testTime=[]
particlesNumber=[]
data=[]

numberTests = 3
initIter = 1	# number of initial iterations excluded from performance check (e.g. Collider is initialised in first step therefore exclude first step from performance check)

tStartAll=time.time()

for z in range(numberTests):
	for i in range(len(radRAD)):
		rR = radRAD[i]
		nbIter=iterN[i]
		O.reset()
		
		tc=0.001
		en=.003
		es=.003
		frictionAngle=radians(35)
		density=2300
		defMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,tc=tc,en=en,et=es))
		
		O.dt=.1*tc # time step
		rad=0.5 # particle radius
		tolerance = 0.0001
		
		SpheresID=[]
		SpheresID+=O.bodies.append(pack.regularHexa(pack.inSphere((Vector3(0.0,0.0,0.0)),rad),radius=rad/rR,gap=rad/rR*0.5,material=defMat))
		
		geometryParameters = bodiesHandling.spheresPackDimensions(SpheresID)
		print len(SpheresID)
		
		floorId=[]
		floorId+=O.bodies.append(geom.facetBox(geometryParameters['center'],geometryParameters['extends']/2.0*1.05,material=defMat)) #Floor
		
		#Calculate the mass of spheres
		sphMass = getSpheresVolume()*density
		
		# Create engines
		O.engines=[
			ForceResetter(),
			InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
			InteractionLoop(
				[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
				[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
				[Law2_ScGeom_ViscElPhys_Basic()],
			),
			NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
		]
		
		print "number of bodies %d"%len(O.bodies)
		# Initialize the collider else it is not possible to compare the results with different nbIter
		O.run(initIter,1)
		O.timingEnabled=True
		timing.reset()
		tStart=time.time()
		# add initIter to nbIter to so that the iterations considered in the perfromance analysis are actually nbIter
		O.run(nbIter+initIter)
		O.wait()
		
		tEnd=time.time()
		print
		print 'Elapsed ', tEnd-tStart, ' sec'
		print 'Performance ', nbIter/(tEnd-tStart), ' iter/sec'
		print 'Extrapolation on 1e5 iters ', (tEnd-tStart)/nbIter*1e5/3600., ' hours'
		print "=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*"
		timing.stats()
		iterVel += [nbIter/(tEnd-tStart)]
		testTime += [tEnd-tStart]
		particlesNumber += [len(O.bodies)]

tEndAll=time.time()
commonTime = tEndAll-tStartAll

print "Common time ", commonTime, "s"
print
print

print "___________________________________________________"
print
print "SUMMARY"
print
scoreIterVel=0.0
for i in range(len(radRAD)):
	iterAv=0.0
	iterVelNumpy=numpy.empty(numberTests)
	for z in range(numberTests):
		iterVelNumpy[z]=iterVel[z*len(radRAD)+i]
	avgVel = numpy.average(iterVelNumpy)
	dispVel = numpy.std(iterVelNumpy)/numpy.average(iterVelNumpy)*100.0
	if (dispVel>10.):
		print "Calculation velocity is unstable, try to close all programs and start performance tests again"
	
	print particlesNumber[i]," spheres, calculation velocity=",avgVel, "iter/sec +/-",dispVel,"%"
	data+=[[particlesNumber[i],avgVel,avgVel*dispVel/100.]]
	scoreIterVel+=avgVel/coefCor[i]*1000.0
print
scoreIterVel = int(scoreIterVel)
## write output file for graph
import subprocess
cmd = "cat /proc/cpuinfo | grep \'model name\' | uniq"
#processor = subprocess.check_output(cmd, shell=True).lstrip('model name\t:').strip() # needs python >=2.7.0
process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
processor = process.communicate()[0].lstrip('model name\t:').strip()
cmd = "cat /proc/cpuinfo | grep processor | wc -l"
#cores = subprocess.check_output("cat /proc/cpuinfo | grep processor | wc -l", shell=True).strip() # needs python >=2.7.0 
process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
cores = process.communicate()[0].strip()
header='# '+ processor + ' ('+cores+' cores)'
numThreads=os.environ['OMP_NUM_THREADS'] if (len(os.environ['OMP_NUM_THREADS'])==2) else ('0'+os.environ['OMP_NUM_THREADS'])
filename=version+"_j"+numThreads+".dat"
#numpy.savetxt(filename,numpy.array(data),fmt=['%i','%g','%g'], header=header) # needs numpy >=1.7.0
fid = open( filename, 'w' )
fid.write(header+"\n")
for l in data:
	fid.write("%d %f %f\n"%(l[0],l[1],l[2]))
fid.close()
print "Summary saved to "+filename
print 
print "SCORE: " + str(scoreIterVel)
print "Number of threads: ", os.environ['OMP_NUM_THREADS']
print "___________________________________________________"
print

print "CPU info:"
cmd = "lscpu"
#cpuinfo=subprocess.check_output(cmd, shell=True) # needs python >=2.7.0
process = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
cpuinfo = process.communicate()[0].lstrip('model name\t:').strip()
print cpuinfo
sys.exit(0)
