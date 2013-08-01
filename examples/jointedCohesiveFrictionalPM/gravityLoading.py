# -*- coding: utf-8 -*-

O=Omega()
from yade import plot, pack,utils,ymport

#### controling parameters
packing='parallellepiped_10_persistentPlane30Deg'
smoothContact=True
jointFrict=radians(20)
jointDil=radians(0)
output='jointDip30_jointFrict20'
maxIter=10000

#### Import of the sphere assembly
def sphereMat(): return JCFpmMat(type=1,young=1e8,frictionAngle=radians(30),density=3000) ## Rq: density needs to be adapted as porosity of real rock is different to granular assembly due to difference in porosity (utils.sumForces(baseBodies,(0,1,0))/(Z*X) should be equal to Gamma*g*h with h=Y, g=9.82 and Gamma=2700 kg/m3
O.bodies.append(ymport.text(packing+'.spheres',scale=1.,shift=Vector3(0,0,0),material=sphereMat))

## preprocessing to get dimensions of the packing
dim=utils.aabbExtrema()
dim=utils.aabbExtrema()
xinf=dim[0][0]
xsup=dim[1][0]
X=xsup-xinf
yinf=dim[0][1]
ysup=dim[1][1]
Y=ysup-yinf
zinf=dim[0][2]
zsup=dim[1][2]
Z=zsup-zinf

## preprocessing to get spheres dimensions
R=0
Rmax=0
numSpheres=0.
for o in O.bodies:
 if isinstance(o.shape,Sphere):
   numSpheres+=1
   R+=o.shape.radius
   if o.shape.radius>Rmax:
     Rmax=o.shape.radius
Rmean=R/numSpheres

#### Identification of the spheres on joint (some DIY here!) -> work to do on import function textExt to directly load material properties from the ascii file
inFile=open(packing+'_jointedPM.spheres','r')
for line in inFile:
    if '#' in line : continue
    id = int(line.split()[0])
    onJ = int(line.split()[1])
    nj = int(line.split()[2])
    j11 = float(line.split()[3])
    j12 = float(line.split()[4])
    j13 = float(line.split()[5])
    j21 = float(line.split()[6])
    j22 = float(line.split()[7])
    j23 = float(line.split()[8])
    j31 = float(line.split()[9])
    j32 = float(line.split()[10])
    j33 = float(line.split()[11])
    O.bodies[id].mat.onJoint=onJ
    O.bodies[id].mat.joint=nj
    O.bodies[id].mat.jointNormal1=(j11,j12,j13)
    O.bodies[id].mat.jointNormal2=(j21,j22,j23)
    O.bodies[id].mat.jointNormal3=(j31,j32,j33)
inFile.close

#### Boundary conditions
e=2*Rmean
Xmax=0
Ymax=0
baseBodies=[]

for o in O.bodies:
   if isinstance(o.shape,Sphere):
      o.shape.color=(0.9,0.8,0.6)
      ## to fix boundary particles on ground
      if o.state.pos[1]<(yinf+2*e) :
	 o.state.blockedDOFs+='xyz'
	 baseBodies.append(o.id)
	 o.shape.color=(1,1,1)

      ## to identify indicator on top
      if o.state.pos[1]>(ysup-e) and o.state.pos[0]>(xsup-e) and o.state.pos[2]>(zinf+(Z-e)/2) and o.state.pos[2]<(zsup-(Z-e)/2) : 
	refPoint=o.id
	p0=o.state.pos[1]

baseBodies=tuple(baseBodies)
O.bodies[refPoint].shape.color=(1,0,0)

#### Engines definition
interactionRadius=1. # to set initial contacts to larger neighbours
O.engines=[

	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=interactionRadius,label='is2aabb'),]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=interactionRadius,label='ss2d3dg')],
		[Ip2_JCFpmMat_JCFpmMat_JCFpmPhys(cohesiveTresholdIteration=1,alpha=0.3,tensileStrength=1e6,cohesion=1e6,jointNormalStiffness=1e7,jointShearStiffness=1e7,jointCohesion=1e6,jointFrictionAngle=jointFrict,jointDilationAngle=jointDil,label='interactionPhys')],
		[Law2_ScGeom_JCFpmPhys_JointedCohesiveFrictionalPM(smoothJoint=smoothContact,label='interactionLaw')]
	),
	GlobalStiffnessTimeStepper(timestepSafetyCoefficient=0.8),
	PyRunner(iterPeriod=1000,initRun=False,command='jointStrengthDegradation()'),
	VTKRecorder(iterPeriod=5000,initRun=True,fileName=(output+'-'),recorders=['spheres','velocity','intr']),
	PyRunner(iterPeriod=10,initRun=True,command='dataCollector()'),
	NewtonIntegrator(damping=0.7,gravity=(0.,-9.82,0.)),

]

#### dataCollector
plot.plots={'iterations':('p',)}
def dataCollector():
	R=O.bodies[refPoint]
	plot.addData(v=R.state.vel[1],p=R.state.pos[1]-p0,iterations=O.iter,t=O.realtime)
	plot.saveDataTxt(output)

#### joint strength degradation
stableIter=2000
stableVel=0.001
degrade=True
def jointStrengthDegradation():
    global degrade
    if degrade and O.iter>=stableIter and abs(O.bodies[refPoint].state.vel[1])<stableVel :
	print '!joint cohesion total degradation!', ' | iteration=', O.iter
	degrade=False
	for i in O.interactions:
	    if i.phys.isOnJoint : 
		if i.phys.isCohesive:
		  i.phys.isCohesive=False
		  i.phys.FnMax=0.
		  i.phys.FsMax=0.
		
#### YADE windows
from yade import qt
v=qt.Controller()
v=qt.View()

#### time step definition (low here to create cohesive links without big changes in the assembly)
O.dt=0.1*utils.PWaveTimeStep()

#### set cohesive links with interaction radius>=1
O.step();
#### initializes now the interaction detection factor to strictly 1
ss2d3dg.interactionDetectionFactor=-1.
is2aabb.aabbEnlargeFactor=-1.
#### if you want to avoid contact detection (Lattice like)
#O.engines=O.engines[:1]+O.engines[3:]

#### RUN!!!
O.dt=-0.1*utils.PWaveTimeStep()

O.run(maxIter)
plot.plot()

