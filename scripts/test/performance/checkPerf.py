# -*- coding: utf-8

from yade import utils,pack,export,geom,timing,bodiesHandling
import time,numpy
	
radRAD=[23.658,				#5000 elements
	40.455,				#25000 elements
	50.97,				#50000 elements
	64.218,				#100000 elements
	80.91]				#200000 elements
	#109.811]			#500000 elements

iterN=[12000,	#5000 elements
	2500,				#25000 elements
	1400,				#50000 elements
	800,				#100000 elements
	200]				#200000 elements
	#10]			#500000 elements

coefCor=[110,
	28,
	18,
	9,
	2]
	#0.1]

iterVel=[]
testTime=[]
particlesNumber=[]

numberTests = 3

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
		
		params=utils.getViscoelasticFromSpheresInteraction(tc,en,es)
		defMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs
		
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
		sphMass = utils.getSpheresVolume()*density
		
		# Create engines
		O.engines=[
			ForceResetter(),
			InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
			InteractionLoop(
				[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
				[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
				[Law2_ScGeom_ViscElPhys_Basic()],
			),
			GravityEngine(gravity=[0,0,-9.81]),
			NewtonIntegrator(damping=0),
		]
		
		print "number of bodies %d"%len(O.bodies)
		O.timingEnabled=True
		tStart=time.time()
		
		O.run(nbIter)
		O.wait()
		
		tEnd=time.time()
		print
		print 'Elapsed ', tEnd-tStart, ' sec'
		print 'Performance ', nbIter/(tEnd-tStart), ' iter/sec'
		print 'Extrapolation on 1e5 iters ', (tEnd-tStart)/nbIter*1e5/3600., ' hours'
		print
		timing.stats()
		iterVel += [nbIter/(tEnd-tStart)]
		testTime += [tEnd-tStart]
		particlesNumber += [len(O.bodies)]


tEndAll=time.time()
commonTime = tEndAll-tStartAll

print "Common time ", commonTime, "s"
print
print

scoreIterVel=0.0
for i in range(len(radRAD)):
	iterAv=0.0
	iterVelNumpy=numpy.empty(3)
	for z in range(numberTests):
		iterVelNumpy[z]=iterVel[z*len(radRAD)+i]
	avgVel = numpy.average(iterVelNumpy)
	dispVel = numpy.std(iterVelNumpy)/numpy.average(iterVelNumpy)*100.0
	if (dispVel>10):
		print "Calculation velocity is unstable, try to close all programs and start performance tests again"
	
	print particlesNumber[i]," spheres, velocity=",avgVel, "+-",dispVel,"%"
	scoreIterVel+=avgVel/coefCor[i]*1000.0
print
print
scoreIterVel = int(scoreIterVel)
print scoreIterVel
print "Number of threads ", os.environ['OMP_NUM_THREADS']
print"___________________________________________________"
print "CPU info", os.system('cat /proc/cpuinfo')
sys.exit(0)
