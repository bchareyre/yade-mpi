#!/usr/local/bin/yade-trunk -x
# -*- encoding=utf-8 -*-
# CWBoon 2015
# Some of the parameters are defunct..
# I use this for jointed rock mass, but you can use it to generate granular particles too

from yade import pack
import math


O.engines=[
	ForceResetter(),
	InsertionSortCollider([PotentialBlock2AABB()],verletDist=0),
	InteractionLoop(
		[Ig2_PB_PB_ScGeom()],
		[Ip2_FrictMat_FrictMat_KnKsPBPhys(Knormal = 1e8, Kshear = 1e8,useFaceProperties=False,calJointLength=False,twoDimension=True,unitWidth2D=1.0,viscousDamping=0.7)],
		[Law2_SCG_KnKsPBPhys_KnKsPBLaw(label='law',neverErase=False)]
		#[Ip2_FrictMat_FrictMat_FrictPhys()],
		#[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	#GravityEngine(gravity=[0,-10,0]),
	#GlobalStiffnessTimeStepper(),
	NewtonIntegrator(damping=0.0,exactAsphericalRot=False,gravity=[0,-10,0]),
	#PotentialBlockVTKRecorder(fileName='/home/chiab/yadeNew/mosek/8Nov/BranchA/scripts2/boon/ComputersGeotechnics/vtk/1000PP',iterPeriod=100,sampleX=50,sampleY=50,sampleZ=50)
	
]




powderDensity = 10000
distanceToCentre= 0.5
meanSize = 1.0
wallThickness = 0.5*meanSize
O.materials.append(FrictMat(young=150e6,poisson=.4,frictionAngle=radians(0.0),density=powderDensity,label='frictionless'))
lengthOfBase = 9.0*meanSize
heightOfBase = 14.0*meanSize
sp=pack.SpherePack()
mn,mx=Vector3(-0.5*(lengthOfBase-wallThickness),0.5*meanSize,-0.5*(lengthOfBase-wallThickness)),Vector3(0.5*(lengthOfBase-wallThickness),7.0*heightOfBase,0.5*(lengthOfBase-wallThickness))
sphereRad = sqrt(3.0)*0.5*meanSize
sp.makeCloud(mn,mx,sphereRad,0,100,False)


count= 0
rPP=0.05*meanSize
for s in sp:
	b=Body()
	radius=2.2
	dynamic=True
	wire=False
	color=[0,0,255.0]
	highlight=False
	b.shape=PotentialBlock(k=0.2, r=0.05*meanSize, R=1.02*sphereRad, a=[1.0,-1.0,0.0,0.0,0.0,0.0], b=[0.0,0.0,1.0,-1.0,0.0,0.0], c=[0.0,0.0,0.0,0.0,1.0,-1.0], d=[distanceToCentre-rPP,distanceToCentre-rPP,distanceToCentre-rPP,distanceToCentre-rPP,distanceToCentre-rPP,distanceToCentre-rPP],isBoundary=False,color=color,wire=wire,highlight=highlight,minAabb=Vector3(sphereRad,sphereRad,sphereRad),maxAabb=Vector3(sphereRad,sphereRad,sphereRad),maxAabbRotated=Vector3(sphereRad,sphereRad,sphereRad),minAabbRotated=Vector3(sphereRad,sphereRad,sphereRad),AabbMinMax=False)
	length=meanSize
  	V= 1.0
  	geomInert=(2./5.)*powderDensity*V*sphereRad**2
	utils._commonBodySetup(b,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=s[0], noBound=False, resetState=True, fixed=False)
	b.state.pos = s[0] #s[0] stores center
	b.state.ori = Quaternion((random.random(),random.random(),random.random()),random.random()) #s[2]
	O.bodies.append(b)
	b.dynamic = True
	count =count+1

#v1 = (0, 0, 0.2) (0, 0, 1) (0,0,2.498)
#v2 = (-0.0943, 0.1633, -0.0667) (-0.4714, 0.8165, -0.3334)
#v3 = (0.1886 0 -0.0667) (0.9428, 0, -0.3333)
#edge = 0.3266   1.6333 4.08
#volume = 0.0041 0.5132 8

r=0.1*wallThickness
bbb=Body()
wire=False
color=[0,255,0]
highlight=False
bbb.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(bbb,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True, fixed=True)
bbb.dynamic=False
bbb.state.pos = [0.0,0,0]
lidID = O.bodies.append(bbb)



b1=Body()
wire=False
color=[0,255,0]
highlight=False
b1.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b1,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b1.dynamic=False
b1.state.pos = [lengthOfBase/3.0,0,lengthOfBase/3.0]
O.bodies.append(b1)

b2=Body()
wire=False
color=[0,255,0]
highlight=False
b2.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b2,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b2.dynamic=False
b2.state.pos = [-lengthOfBase/3.0,0,lengthOfBase/3.0]
O.bodies.append(b2)

b3=Body()
wire=False
color=[0,255,0]
highlight=False
b3.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b3,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b3.dynamic=False
b3.state.pos = [0.0,0,lengthOfBase/3.0]
O.bodies.append(b3)

b4=Body()
wire=False
color=[0,255,0]
highlight=False
b4.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b4,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b4.dynamic=False
b4.state.pos = [lengthOfBase/3.0,0,-lengthOfBase/3.0]
O.bodies.append(b4)

b5=Body()
wire=False
color=[0,255,0]
highlight=False
b5.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b5,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b5.dynamic=False
b5.state.pos = [0.0,0,-lengthOfBase/3.0]
O.bodies.append(b5)


b6=Body()
wire=False
color=[0,255,0]
highlight=False
b6.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b6,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b6.dynamic=False
b6.state.pos = [-lengthOfBase/3.0,0.0,-lengthOfBase/3.0]
O.bodies.append(b6)

b7=Body()
wire=False
color=[0,255,0]
highlight=False
b7.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b7,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b7.dynamic=False
b7.state.pos = [-lengthOfBase/3.0,0.0,0.0]
O.bodies.append(b7)


b8=Body()
wire=False
color=[0,255,0]
highlight=False
b8.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.2*lengthOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[lengthOfBase/6.0-r,lengthOfBase/6.0-r,0.5*wallThickness-r,0.5*wallThickness-r,lengthOfBase/6.0-r,lengthOfBase/6.0-r], id=count,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabb=1.02*Vector3(lengthOfBase/6.0,0.4*wallThickness,lengthOfBase/6.0),maxAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0),minAabbRotated=1.02*Vector3(lengthOfBase/6.0,0.5*wallThickness,lengthOfBase/6.0))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(b8,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
b8.dynamic=False
b8.state.pos = [lengthOfBase/3.0,0.0,0.0]
O.bodies.append(b8)

bA=Body()
wire=False
color=[0,255,0]
highlight=False
bA.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.5*heightOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[0.5*wallThickness-r,0.5*wallThickness-r,0.5*heightOfBase-r,0.5*heightOfBase-r,0.5*lengthOfBase-r,0.5*lengthOfBase-r], id=count+1,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(0.3*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),maxAabb=1.02*Vector3(0.3*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),maxAabbRotated=1.02*Vector3(0.5*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),minAabbRotated=1.02*Vector3(0.5*wallThickness,0.5*heightOfBase,0.5*lengthOfBase))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(bA,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
bA.dynamic=False
bA.state.pos = [0.5*lengthOfBase,0.5*heightOfBase,0]
O.bodies.append(bA)

bB=Body()
wire=False
color=[0,255,0]
highlight=False
bB.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.5*heightOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[0.5*wallThickness-r,0.5*wallThickness-r,0.5*heightOfBase-r,0.5*heightOfBase-r,0.5*lengthOfBase-r,0.5*lengthOfBase-r], id=count+2,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(0.3*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),maxAabb=1.02*Vector3(0.3*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),maxAabbRotated=1.02*Vector3(0.5*wallThickness,0.5*heightOfBase,0.5*lengthOfBase),minAabbRotated=1.02*Vector3(0.5*wallThickness,0.5*heightOfBase,0.5*lengthOfBase))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(bB,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
bB.dynamic=False
bB.state.pos = [-0.5*lengthOfBase,0.5*heightOfBase,0]
O.bodies.append(bB)


bC=Body()
wire=False
color=[0,255,0]
highlight=False
bC.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.5*heightOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[0.5*lengthOfBase-r,0.5*lengthOfBase-r,0.5*heightOfBase-r,0.5*heightOfBase-r,0.5*wallThickness-r,0.5*wallThickness-r], id=count+3,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.3*wallThickness),maxAabb=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.3*wallThickness),maxAabbRotated=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.5*wallThickness),minAabbRotated=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.5*wallThickness))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(bC,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
bC.dynamic=False
bC.state.pos = [0,0.5*heightOfBase,0.5*lengthOfBase]
O.bodies.append(bC)

bD=Body()
wire=False
color=[0,255,0]
highlight=False
bD.shape=PotentialBlock(k=0.1, r=0.1*wallThickness, R=0.5*heightOfBase,a=[1,-1,0,0,0,0], b=[0,0,1,-1,0,0], c=[0,0,0,0,1,-1], d=[0.5*lengthOfBase-r,0.5*lengthOfBase-r,0.5*heightOfBase-r,0.5*heightOfBase-r,0.5*wallThickness-r,0.5*wallThickness-r], id=count+4,isBoundary=True,isBoundaryPlane=[True,True,True,True,True,True],color=color ,wire=wire,highlight=highlight,AabbMinMax=True, minAabb=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.3*wallThickness),maxAabb=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.3*wallThickness),maxAabbRotated=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.5*wallThickness),minAabbRotated=1.02*Vector3(0.5*lengthOfBase,0.5*heightOfBase,0.5*wallThickness))
length=lengthOfBase
V=lengthOfBase*lengthOfBase*wallThickness
geomInert=(1./6.)*V*length*wallThickness
utils._commonBodySetup(bD,V,Vector3(geomInert,geomInert,geomInert), material='frictionless',pos=[0.0,0,0], noBound=False, resetState=True,fixed=True)
bD.dynamic=False
bD.state.pos = [0.0,0.5*heightOfBase,-0.5*lengthOfBase]
O.bodies.append(bD)


escapeNo=0
def myAddPlotData():
	global escapeNo
	global wallThickness
	global meanSize
	uf=utils.unbalancedForce()
	if isnan(uf):
		uf = 1.0
	KE = utils.kineticEnergy()

	for b in O.bodies:
		if b.state.pos[1] < -5.0*meanSize and b.dynamic==True:
			escapeNo = escapeNo+1
			O.bodies.erase(b.id)
	if O.iter>12000:
		removeLid()
	plot.addData(timeStep1=O.iter,timeStep2=O.iter,timeStep3=O.iter,timeStep4=O.iter,time=O.time,unbalancedForce=uf,kineticEn=KE,outsideNo=escapeNo)


from yade import plot
plot.plots={'timeStep1':('unbalancedForce'),'timeStep2':('kineticEn'),'time':('outsideNo')}
O.engines=O.engines+[PyRunner(iterPeriod=10,command='myAddPlotData()')]

def removeLid():
	global lidID	
	if (O.bodies[lidID]):
		O.bodies.erase(lidID)	



O.engines=O.engines+[PotentialBlockVTKRecorder(fileName='/home/boon/yadeRev/trunk/examples/PotentialParticles/vtk/cubeScaled',iterPeriod=1000,sampleX=50,sampleY=50,sampleZ=50)]

#for b in O.bodies:
#	b.state.blockedDOFs=['rx','ry','rz','x','z']

#O.bodies[0].state.pos = [0,meanSize*10.0,0]
#O.bodies[0].state.vel =[0,0.0,0]
O.dt = 0.2*sqrt(O.bodies[0].state.mass*0.33333333/1.0e8) 
#from yade import qt
#qt.Controller()
#qt.View()

#Gl1_PotentialBlock.sizeX = 30
#Gl1_PotentialBlock.sizeY = 30
#Gl1_PotentialBlock.sizeZ = 30
#from yade import qt
#qt.View()

import yade.timing
O.timingEnabled = True
yade.timing.reset()
#O.engines[2].geomDispatcher.functors[0].timingDeltas.data
#yade.timing.stats()
