from yade import plot, polyhedra_utils, ymport, export

frictionIn = 0.0
densityIn = 3000.0
youngIn = 1E10
poissonIn = 0.3
sizeB = 0.020
odt = 0.00001
vel=0.01
tolerance = 0.05
startPos = sizeB

def printWarning (f_awaited, f_real, n_awaited, n_real):
   print ("The awaited force is %.4f, but obtained force is %.4f; number of bodies: %d vs %d! Iteration %d"%(f_awaited, f_real, n_awaited, n_real, O.iter))

def printSuccess ():
   print ("Checkpoint: force values and number of bodies are OK! Iteration %d"%(O.iter))

def checkForcesBodies(fR, bodNum):
   if (abs(f[2] - fR)/f[2] >  tolerance or len(O.bodies) <> bodNum):
      printWarning (fR, f[2], 4, len(O.bodies))
      resultStatus += 1
   else:
      printSuccess()

mat1 = PolyhedraMat(density=densityIn, young=youngIn,poisson=poissonIn, frictionAngle=frictionIn,IsSplitable=True,strength=1)
O.bodies.append(utils.wall(0,axis=2,sense=1, material = mat1))

vertices = [[0,0,0],[sizeB,0,0],[sizeB,sizeB,0],[sizeB,sizeB,sizeB],[0,sizeB,0],[0,sizeB,sizeB],[0,0,sizeB],[sizeB,0,sizeB]]
t = polyhedra_utils.polyhedra(mat1,v=vertices)
t.state.pos = (0,0,sizeB/2)
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

from yade import qt
qt.Controller()
V = qt.View()

O.run(250, True); checkForcesBodies(25.44893, 4)
O.run(50, True); checkForcesBodies(28.791353, 4)
O.run(10, True); checkForcesBodies(30.731547, 4)
O.run(20, True); checkForcesBodies(33.483438, 7)

