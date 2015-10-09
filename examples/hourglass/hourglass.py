#!/usr/bin/env python
# encoding: utf-8
# Anton Gladky <gladk@debian.org>

# If you want to change the number of generated particles,
# change it in line 44. Do not forget yo increase the firstIterRun
# in line 62 not to revolute the hourglass before particles are
# settled down at the bottom.

from yade import ymport
import time

utils.readParamsFromTable(descriptionIn = 'noDescription', 
  frIn = 0.5, enIn=0.01, etIn=0.01, tcIn=0.0001,                 # Frictionangle (rad), strength-parameter
  rhoIn = 1500.0,                                                # Density
  dumpVTKIn = 4000                                               # Periods of dumps in iterations
)

from yade.params.table import *

import shutil

try:
  shutil.rmtree('cpt')
except OSError:
  pass
os.mkdir('cpt')

folderNameBase = 'cpt/' + str(descriptionIn)
folderName = folderNameBase
os.mkdir(folderNameBase)

o = Omega()
o.dt = 0.05*tcIn
rotPeriod = 0.01
rotPeriodIter = int(rotPeriod/o.dt)

mat1 = O.materials.append(ViscElMat(frictionAngle=frIn, density=rhoIn,tc=tcIn, en=enIn, et=etIn,))
id_HourGl = O.bodies.append(ymport.gmsh("hourglass.mesh",scale=1.0, material=mat1,color=(0,0,1),mask=5))

o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],verletDist=(2.0e-3),label='collider',ompThreads=1),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(),Ig2_Facet_Sphere_ScGeom()],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0,gravity=Vector3(0,0,-90.81), label='newt'),
  CircularFactory(maxParticles=1500,            #Number of generated particles
    center=(0.0e-3, 0.0e-3,20.0e-3),
    radius = (12.0e-3),
    PSDsizes = [0.5e-3],
    PSDcum   = [1.0],
    vMin=0.1,vMax=0.1,vAngle=0,massFlowRate=0.01,
    normal=(0.0,0.0,-1.0),label='factorySpheres',mask=3,materialId=mat1,
    stopIfFailed=False),
  RotationEngine(ids=id_HourGl, angularVelocity = math.pi/rotPeriod,
    rotateAroundZero=True, rotationAxis=Vector3(1,0,0),
    zeroPoint=Vector3(0.,0.,0.), label='rotEng', dead=True),
  VTKRecorder(iterPeriod=dumpVTKIn,fileName=folderName+'/spheres-',mask=2,
    recorders=['spheres','velocity','colors','intr','ids','mask','materialId','stress'], label='VTK_box1'),
  VTKRecorder(iterPeriod=dumpVTKIn,fileName=folderName+'/facetB-',mask=4,
    recorders=['facets'],label='VTK_box2'),
  PyRunner(iterPeriod=rotPeriodIter,initRun=False,firstIterRun=13000,command='startStepRotation()'),
]

def startStepRotation():
  if (rotEng.dead):
    rotEng.dead = False
  else:
    rotEng.dead = True

from yade import qt
qt.View()
r=qt.Renderer()
