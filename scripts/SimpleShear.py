# -*- coding: utf-8 -*-

# J. Duriez duriez@geo.hmg.inpg.fr
# "PreProcessor" script to define a numerical model for simple shear box
# In order to test various Kinem...Engine


from yade import plot
from yade.pack import *


O=Omega() 
O.initializers=[
		BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()])
]
O.engines=[
	ForceResetter(),
	BoundDispatcher([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InsertionSortCollider(),
	InteractionDispatchers(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_2xNormalInelasticMat_NormalInelasticityPhys()],
		[Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity()]
	),
	NewtonIntegrator(damping=.2)
	,PeriodicPythonRunner(iterPeriod=1,command='print O.interactions.countReal()')
	]


#Def of the material which will be used
O.materials.append(NormalInelasticMat(density=2600,young=4.0e9,poisson=.04,frictionAngle=.6,coeff_dech=3.0,label='Materiau1'))

#Def of dimensions of the box
#length=5
#height=1
#width=2
length=0.1
height=0.02
width=0.04
thickness=0.001

porosity = 0.7
Nspheres=800
Rmean = pow( ((1-porosity) * length * height * width) / (Nspheres * 4.0/3.0 * pi) , 1.0/3.0 )

#Definition of bodies constituing the numerical model : six boxes corresponding to sids of the simple shear box, containing a particle samples
pred=inAlignedBox((0,0,-0.02),(0.1,0.02,0.02))

LeftBox = utils.box( center=(-thickness/2.0,(height)/2.0,0), extents=(thickness/2.0,5*(height/2.0+thickness),width/2.0) ,dynamic=False,wire=True)

LowBox = utils.box( center=(length/2.0,-thickness/2.0,0), extents=(length/2.0,thickness/2.0,width/2.0) ,dynamic=False,wire=True)

RightBox = utils.box( center=(length+thickness/2.0,height/2.0,0), extents=(thickness/2.0,5*(height/2.0+thickness),width/2.0) ,dynamic=False,wire=True)

UpBox = utils.box( center=(length/2.0,height+thickness/2.0,0), extents=(length/2.0,thickness/2.0,width/2.0) ,dynamic=False,wire=True)

BehindBox = utils.box( center=(length/2.0,height/2.0,-width/2.0-thickness/2.0), extents=(2.5*length/2.0,height/2.0+thickness,thickness/2.0), dynamic=False,wire=True)

InFrontBox = utils.box( center=(length/2.0,height/2.0,width/2.0+thickness/2.0), extents=(2.5*length/2.0,height/2.0+thickness,thickness/2.0), dynamic=False,wire=True)

O.bodies.append([LeftBox,LowBox,RightBox,UpBox,BehindBox,InFrontBox])

memoizeDb='/tmp/simpleshear-triax-packings.sqlite'
#ListSph=randomDensePack(pred,radius=0.002,rRelFuzz=0.15,memoDbg=True,memoizeDb=memoizeDb)
#ListSph=randomDensePack(pred,radius=0.002,rRelFuzz=0.15,memoDbg=True,memoizeDb=memoizeDb,spheresInCell=100)
#O.bodies.append(ListSph)

sp=yade._packSpheres.SpherePack()
sp.makeCloud(Vector3(0,0.0,-width/2.0),Vector3(length,height,width/2.0),Rmean,.15)
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])

from yade import qt
qt.View()
#O.save("SimpleShearReady.xml")
O.saveTmp("InitialState")


#Compression
O.engines = O.engines+[KinemCTDEngine(compSpeed=0.5,sigma_save=(),temoin_save=(),targetSigma=2000.0,LOG=False)]
#from yade import log
#log.setLevel("KinemCTDEngine",log.TRACE)
#log.setLevel('',log.TRACE)
O.dt=.4*utils.PWaveTimeStep()
O.run()
