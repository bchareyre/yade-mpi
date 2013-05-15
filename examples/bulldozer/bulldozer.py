#!/usr/bin/python
# -*- coding: utf-8 -*-
### Simpificated buldozer simulation
from numpy import linspace
from numpy import arange
import gts
import itertools
from yade import geom,pack


###Initial Data
numKnifeParts = 10
radiusKnife=1
lengthKnife=2
buldozerHeight=1.2
radiusSph = 0.05
numBoxes = Vector3(15,5,2)
gapBetweenBoxes = 0.05
sizeBox = (lengthKnife-(numBoxes[1]-1)*gapBetweenBoxes)/numBoxes[1]


### Creating the Buldozer Knife
### from facets, using GTS
Knife=[]
for i in linspace(pi, pi*3/2, num=numKnifeParts, endpoint=True):
	Knife.append(Vector3(radiusKnife*cos(i),0,radiusKnife*sin(i)))

	
KnifeP=[Knife,[p+Vector3(0,lengthKnife,0) for p in Knife]]
KnifePoly=pack.sweptPolylines2gtsSurface(KnifeP,threshold=1e-4)
KnifeIDs=[]
KnifeIDs=O.bodies.append(pack.gtsSurface2Facets(KnifePoly,color=(1,0,0),wire=False))


KnifeIDs+=O.bodies.append(geom.facetBox((-lengthKnife/2-radiusKnife,lengthKnife/2,-radiusKnife+buldozerHeight/2),(lengthKnife/2,lengthKnife/2,buldozerHeight/2.),wallMask=47,color=(0,1,0),wire=False))

KnifeIDs+=O.bodies.append(geom.facetBox((-lengthKnife/2-radiusKnife-lengthKnife/4.,lengthKnife/2,-radiusKnife+buldozerHeight*3./2.-buldozerHeight/4.),(lengthKnife/4.,lengthKnife/3.,buldozerHeight/4.),wallMask=47,color=(0,0,1),wire=False))


O.bodies.append(geom.facetBox((0,lengthKnife/2,radiusKnife),(lengthKnife*4,lengthKnife*4,lengthKnife),wallMask=16,color=(1,1,1),wire=False))




### Creating the material for buldozer
colorsph1=Vector3(120,234,150);
colorsph2=Vector3(1,1,0);

colorsph1.normalize();
colorsph2.normalize();
colorSph=colorsph1
for xyz in itertools.product(arange(0,numBoxes[0]),arange(0,numBoxes[1]),arange(0,numBoxes[2])):
	ids_spheres=O.bodies.appendClumped(pack.regularHexa(pack.inEllipsoid((xyz[0]*(sizeBox+gapBetweenBoxes),xyz[1]*(sizeBox+gapBetweenBoxes)+sizeBox*0.5,xyz[2]*(sizeBox+gapBetweenBoxes)-radiusKnife+sizeBox*0.6),(sizeBox/2,sizeBox/2,sizeBox/2)),radius=radiusSph,gap=0,color=colorSph))
	if (colorSph==colorsph1):
		colorSph=colorsph2
	else:
		colorSph=colorsph1


from yade import qt

O.dt=2*PWaveTimeStep() # We do not need now a high accuracy
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb(),]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	TranslationEngine(translationAxis=[1,0,0],velocity=5,ids=KnifeIDs), # Buldozer motion
	NewtonIntegrator(damping=.3,gravity=(0,0,-9.8)),
	#qt.SnapshotEngine(iterPeriod=100,fileBase='/tmp/bulldozer-',label='snapshooter'),
]

O.saveTmp()
qt.Controller()
qt.View()
r=qt.Renderer()
r.lightPos=Vector3(0,0,50)
O.stopAtIter=2000
O.run()
#encodeVideoFromFrames(snapshooter.savedSnapshots,out='/tmp/bulldozer.ogg',fps=2)
