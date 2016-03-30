from yade import plot, polyhedra_utils, ymport, export
from numpy import linspace
import itertools

frictionIn = 0.6
densityIn = 3000.0
youngIn = 1E10
poissonIn = 0.3
numP = 1
C = 0.2
Z = 1
sizeB = 0.020
odt = 0.00005
vtkP = 100
maxIter = 1e5
vel=0.01
tolerance = 0.05
startPos = 2*sizeB

def printWarning (f_awaited, f_real, n_awaited, n_real):
   print ("The awaited force is %.4f, but obtained force is %.4f; number of bodies: %d vs %d! Iteration %d"%(f_awaited, f_real, n_awaited, n_real, O.iter))

def printSuccess ():
   print ("Checkpoint: force values are OK! Iteration %d"%(O.iter))

mat1 = PolyhedraMat(density=densityIn, young=youngIn,poisson=poissonIn, frictionAngle=frictionIn,IsSplitable=True,strength=1)
O.bodies.append(utils.wall(0,axis=2,sense=1, material = mat1))

t = polyhedra_utils.polyhedralBall(sizeB, 50, mat1, (0,0,0))
t.state.pos = (0.,0.,0.020)
O.bodies.append(t)

topmesh=O.bodies.append(geom.facetBox((0.,0.,startPos),(sizeB,sizeB,0.), material=mat1))

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),
                          Bo1_Wall_Aabb(),
                          Bo1_Facet_Aabb()],
                          verletDist=.05*sizeB),
   InteractionLoop(
      [Ig2_Facet_Polyhedra_PolyhedraGeom(),
       Ig2_Wall_Polyhedra_PolyhedraGeom(),
       Ig2_Polyhedra_Polyhedra_PolyhedraGeom()], 
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
   ),
   NewtonIntegrator(damping=0.8,gravity=(0,0,-9.81)),
   PolyhedraSplitter(iterPeriod=1,label='Splitter'),
   TranslationEngine(translationAxis=[0,0,-1],velocity=vel,ids=topmesh, label='Tr'),
   PyRunner(command='addPlotData()',iterPeriod=10),
]

O.dt=odt

f=Vector3.Zero
  
def addPlotData():
  global f
  f=Vector3.Zero
  for i in topmesh:
    if (O.forces.f(i)):
      f+=O.forces.f(i)

O.run(4000, True)

if (abs(f[2] - 18.2849787)/f[2] >  tolerance or len(O.bodies) <> 4):
   printWarning (18.2849787, f[2], 4, len(O.bodies))
   resultStatus += 1
else:
   printSuccess()
   
O.run(150, True)
if (abs(f[2] - 23.68293)/f[2] >  tolerance or len(O.bodies) <> 4):
   printWarning (23.68293, f[2], 4, len(O.bodies))
   resultStatus += 1
else:
   printSuccess()
   
O.run(250, True)
if (abs(f[2] - 32.7028)/f[2] >  tolerance or len(O.bodies) <> 4):
   printWarning (32.7028, f[2], 4, len(O.bodies))
   resultStatus += 1
else:
   printSuccess()

O.run(130, True)
if (f.norm() == 0 >  tolerance or len(O.bodies) <> 7):
   printWarning (0.0, f[2], 7, len(O.bodies))
   resultStatus += 1
else:
   printSuccess()

