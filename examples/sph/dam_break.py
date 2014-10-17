#!/usr/bin/env python
# encoding: utf-8

from yade import utils, plot, qt
o = Omega()

# Physical parameters
fr = 0.5;
rho= 1000.0

k = 1000.0
mu = 10.0
tc = 0.01; en = 0.7; et = 0.7;
vel = 0.0

#Rad = 0.006
#h = 0.011

Rad = 0.015
h = 0.03

#Rad = 0.02
#h = 0.04

o.dt = 0.0005

X = 4.0
Z = 3.0
yCoeff = 4.0

SpheresX = 1.0
SpheresZ = 2.0

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr,density=rho, SPHmode=True, h=h, mu=mu,tc=tc, en=en, et=et, KernFunctionPressure = 1, KernFunctionVisco = 1))
idBox = pack.regularHexa(pack.inAlignedBox((0.0,-Rad*yCoeff,0.0),(X,Rad*yCoeff,Z)),radius=Rad,gap=0.0,color=(0,1,1), material=mat1, mask = 1, fixed=True)

idBoxAdd = []
idSpheresAdd = []

for i in range(len(idBox)):
  if (((idBox[i].state.pos[0])<yCoeff*Rad) or
      ((idBox[i].state.pos[0])>(X-yCoeff*Rad - 2.0*Rad)) or
      ((idBox[i].state.pos[2])<(yCoeff*Rad)) or
      ((idBox[i].state.pos[2])>(Z - 2.0*Rad))):
    idBoxAdd.append(idBox[i])
  elif (((idBox[i].state.pos[0])<SpheresX) and
        ((idBox[i].state.pos[2])<SpheresZ) and
        ((idBox[i].state.pos[1])>-Rad) and
        ((idBox[i].state.pos[1])<2.0*Rad)):
          idBox[i].shape.color = Vector3(1,0,0)
          idBox[i].state.fixed = False
          idBox[i].state.blockedDOFs = 'y'
          idSpheresAdd.append(idBox[i])
    


O.bodies.append(idBoxAdd)
idSpheres = O.bodies.append(idSpheresAdd)

    
# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(label='is2aabb')],ompThreads=1),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(label='ss2sc')],
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()],
  ),
  NewtonIntegrator(damping=0.01,gravity=[0,0,-9.81]),
  SPHEngine(mask=1, k=k, rho0 = rho, h=h, KernFunctionDensity= 1),
  PyRunner(command='addPlotData()',iterPeriod=10,initRun=True,dead=False),
]

enlargeF = h/Rad*1.1
print "enlargeF = %g"%enlargeF
is2aabb.aabbEnlargeFactor = enlargeF
ss2sc.interactionDetectionFactor = enlargeF


# Function to add data to plot
def addPlotData(): 
  plot.addData(t = O.time, eKin = utils.kineticEnergy())
  
plot.plots={'t':('eKin')};
plot.plot()

qt.View()
