################################################################################
#
# Triaxial test. Axial strain rate is prescribed and transverse prestress.
# Test is possible on prism or cylinder
# An independent c++ engine may be created from this script in the future.
#
################################################################################
from yade import pack, plot
import os

# default parameters or from table
readParamsFromTable(noTableOk=True,
	# type of test ['cyl','cube']
	testType = 'cyl',

	# material parameters
	young = 20e9,
	poisson = .2,
	frictionAngle = 1.2,
	sigmaT = 1.5e6,
	epsCrackOnset = 1e-4,
	relDuctility = 30,

	# prestress
	preStress = -3e6,
	# axial strain rate
	strainRate = -20,

	# assamlby parameters
	rParticle = .075e-3, #
	width = 2e-3,
	height = 5e-3,
	bcCoeff = 5,

	# facets division
	nw = 24,
	nh = 15,

	# output specifications
	fileName = 'test',
	exportDir = '/tmp',
	runGnuplot = False,
	runInGui = True,
)
from yade.params.table import *
assert testType in ['cyl','cube']

# materials
concMat = O.materials.append(CpmMat(
	young=young,frictionAngle=frictionAngle,poisson=poisson,sigmaT=sigmaT,
	epsCrackOnset=epsCrackOnset,relDuctility=relDuctility
))
frictMat = O.materials.append(FrictMat(
	young=young,poisson=poisson,frictionAngle=frictionAngle
))

# spheres
pred = pack.inCylinder((0,0,0),(0,0,height),.5*width) if testType=='cyl' else pack.inAlignedBox((-.5*width,-.5*width,0),(.5*width,.5*width,height)) if testType=='cube' else None
sp=SpherePack()
sp = pack.randomDensePack(pred,spheresInCell=2000,radius=rParticle,memoizeDb='/tmp/triaxTestOnCylinder.sqlite',material=concMat,returnSpherePack=True)
spheres=sp.toSimulation(color=(0,1,1))
# bottom and top of specimen. Will have prescribed velocity
bot = [O.bodies[s] for s in spheres if O.bodies[s].state.pos[2]<rParticle*bcCoeff]
top = [O.bodies[s] for s in spheres if O.bodies[s].state.pos[2]>height-rParticle*bcCoeff]
vel = strainRate*(height-rParticle*2*bcCoeff)
for s in bot:
	s.shape.color = (1,0,0)
	s.state.blockedDOFs = 'xyzXYZ'
	s.state.vel = (0,0,-vel)
for s in top:
	s.shape.color = Vector3(0,1,0)
	s.state.blockedDOFs = 'xyzXYZ'
	s.state.vel = (0,0,vel)

# facets
facets = []
if testType == 'cyl':
	rCyl2 = .5*width / cos(pi/float(nw))
	for r in xrange(nw):
		for h in xrange(nh):
			v1 = Vector3( rCyl2*cos(2*pi*(r+0)/float(nw)), rCyl2*sin(2*pi*(r+0)/float(nw)), height*(h+0)/float(nh) )
			v2 = Vector3( rCyl2*cos(2*pi*(r+1)/float(nw)), rCyl2*sin(2*pi*(r+1)/float(nw)), height*(h+0)/float(nh) )
			v3 = Vector3( rCyl2*cos(2*pi*(r+1)/float(nw)), rCyl2*sin(2*pi*(r+1)/float(nw)), height*(h+1)/float(nh) )
			v4 = Vector3( rCyl2*cos(2*pi*(r+0)/float(nw)), rCyl2*sin(2*pi*(r+0)/float(nw)), height*(h+1)/float(nh) )
			f1 = facet((v1,v2,v3),color=(0,0,1),material=frictMat)
			f2 = facet((v1,v3,v4),color=(0,0,1),material=frictMat)
			facets.extend((f1,f2))
elif testType == 'cube':
	nw2 = nw/4
	for r in xrange(nw2):
		for h in xrange(nh):
			v11 = Vector3( -.5*width + (r+0)*width/nw2, -.5*width, height*(h+0)/float(nh) )
			v12 = Vector3( -.5*width + (r+1)*width/nw2, -.5*width, height*(h+0)/float(nh) )
			v13 = Vector3( -.5*width + (r+1)*width/nw2, -.5*width, height*(h+1)/float(nh) )
			v14 = Vector3( -.5*width + (r+0)*width/nw2, -.5*width, height*(h+1)/float(nh) )
			f11 = facet((v11,v12,v13),color=(0,0,1),material=frictMat)
			f12 = facet((v11,v13,v14),color=(0,0,1),material=frictMat)
			v21 = Vector3( +.5*width, -.5*width + (r+0)*width/nw2, height*(h+0)/float(nh) )
			v22 = Vector3( +.5*width, -.5*width + (r+1)*width/nw2, height*(h+0)/float(nh) )
			v23 = Vector3( +.5*width, -.5*width + (r+1)*width/nw2, height*(h+1)/float(nh) )
			v24 = Vector3( +.5*width, -.5*width + (r+0)*width/nw2, height*(h+1)/float(nh) )
			f21 = facet((v21,v22,v23),color=(0,0,1),material=frictMat)
			f22 = facet((v21,v23,v24),color=(0,0,1),material=frictMat)
			v31 = Vector3( +.5*width - (r+0)*width/nw2, +.5*width, height*(h+0)/float(nh) )
			v32 = Vector3( +.5*width - (r+1)*width/nw2, +.5*width, height*(h+0)/float(nh) )
			v33 = Vector3( +.5*width - (r+1)*width/nw2, +.5*width, height*(h+1)/float(nh) )
			v34 = Vector3( +.5*width - (r+0)*width/nw2, +.5*width, height*(h+1)/float(nh) )
			f31 = facet((v31,v32,v33),color=(0,0,1),material=frictMat)
			f32 = facet((v31,v33,v34),color=(0,0,1),material=frictMat)
			v41 = Vector3( -.5*width, +.5*width - (r+0)*width/nw2, height*(h+0)/float(nh) )
			v42 = Vector3( -.5*width, +.5*width - (r+1)*width/nw2, height*(h+0)/float(nh) )
			v43 = Vector3( -.5*width, +.5*width - (r+1)*width/nw2, height*(h+1)/float(nh) )
			v44 = Vector3( -.5*width, +.5*width - (r+0)*width/nw2, height*(h+1)/float(nh) )
			f41 = facet((v41,v42,v43),color=(0,0,1),material=frictMat)
			f42 = facet((v41,v43,v44),color=(0,0,1),material=frictMat)
			facets.extend((f11,f12,f21,f22,f31,f32,f41,f42))
O.bodies.append(facets)
mass = O.bodies[0].state.mass
for f in facets:
	f.state.mass = mass
	f.state.blockedDOFs = 'XYZz'

# plots 
plot.plots = { 'e':('s',), }
def plotAddData():
	f1 = sum(O.forces.f(b.id)[2] for b in top)
	f2 = sum(O.forces.f(b.id)[2] for b in bot)
	f = .5*(f2-f1)
	s = f/(pi*.25*width*width) if testType=='cyl' else f/(width*width) if testType=='cube' else None
	e = (top[0].state.displ()[2] - bot[0].state.displ()[2]) / (height-rParticle*2*bcCoeff)
	plot.addData(
		i = O.iter,
		s = s,
		e = e,
	)

# apply prestress to facets
def addForces():
	for f in facets:
		n = f.shape.normal
		a = f.shape.area
		O.forces.addF(f.id,preStress*a*n)

# stop condition and exit of the simulation
def stopIfDamaged(maxEps=5e-3):
	extremum = max(abs(s) for s in plot.data['s'])
	s = abs(plot.data['s'][-1])
	e = abs(plot.data['e'][-1])
	if O.iter < 1000 or s > .5*extremum and e < maxEps:
		return
	f = os.path.join(exportDir,fileName)
	print 'gnuplot',plot.saveGnuplot(f,term='png')
	if runGnuplot:
		import subprocess
		os.chdir(exportDir)
		subprocess.Popen(['gnuplot',f+'.gnuplot']).wait()
	print 'Simulation finished'
	O.pause()
	#sys.exit(0) # results in some threading exception

O.dt=.5*utils.PWaveTimeStep()
enlargeFactor=1.5
O.engines=[
	ForceResetter(),
	InsertionSortCollider([
		Bo1_Sphere_Aabb(aabbEnlargeFactor=enlargeFactor,label='bo1s'),
		Bo1_Facet_Aabb()
	]),
	InteractionLoop(
		[
			Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=enlargeFactor,label='ss2d3dg'),
			Ig2_Facet_Sphere_ScGeom(),
		],
		[
			Ip2_CpmMat_CpmMat_CpmPhys(cohesiveThresholdIter=O.iter+5),
			Ip2_FrictMat_CpmMat_FrictPhys(),
			Ip2_FrictMat_FrictMat_FrictPhys(),
		],
		[
			Law2_ScGeom_CpmPhys_Cpm(),
			Law2_ScGeom_FrictPhys_CundallStrack(),
		],
	),
	PyRunner(iterPeriod=1,command="addForces()"),
	NewtonIntegrator(damping=.3),
	CpmStateUpdater(iterPeriod=50,label='cpmStateUpdater'),
	PyRunner(command='plotAddData()',iterPeriod=10),
	PyRunner(iterPeriod=50,command='stopIfDamaged()'),
]

# run one step
O.step()

# reset interaction detection enlargement
bo1s.aabbEnlargeFactor=ss2d3dg.interactionDetectionFactor=1.0

# initialize auto-updated plot
if runInGui:
	plot.plot()
	try:
		from yade import qt
		renderer=qt.Renderer()
		# uncomment following line to exagerate displacement
		#renderer.dispScale=(100,100,100)
	except:
		pass

# run
O.run()
