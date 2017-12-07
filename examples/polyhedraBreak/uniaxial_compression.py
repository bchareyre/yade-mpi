from yade import plot, polyhedra_utils, ymport, export
import os.path

utils.readParamsFromTable(descriptionIn = 'noDescription', 
   frictionIn = 0.6,
   factorR = 1e4,
   densityIn = 2500.0,
   youngIn = 1e14,
   poissonIn = 0.3,
   numP = 2,
   C = 0.06,
   Z = 1,
   sizeB = 0.035,
   highB = 0.0315,
   odt = 1e-5,
   vtkP = 1000,
   maxIter = 200002,
   velT = 10e-4,
   velL =  2e-4,
   velB =  2e-4,
   tolerance = 0.05,
   startPos = 0.035,
   strength = 1e-2,
   strengthTau = 5e-3,
   Wei_m = 3,
   Wei_S0 = 8e5,
   Wei_V0 = 1e-9,
   Wei_P = 0.6,
   sigmaCZ =   5e9,
   sigmaCD =   1e10,
)

from yade.params.table import *

youngIn = youngIn/factorR
sigmaCZ = sigmaCZ/factorR
sigmaCD = sigmaCD/factorR
#odt     = odt*factorR

mat1 = PolyhedraMat(density=densityIn, young=youngIn,poisson=poissonIn, frictionAngle=frictionIn,IsSplitable=True,
   strength=1e-2, strengthTau=3e-3,
   sigmaCZ = sigmaCZ, sigmaCD = sigmaCD,
   Wei_m=Wei_m, Wei_S0 = Wei_S0, Wei_V0=Wei_V0, Wei_P=Wei_P)

#O.bodies.append(utils.wall(0,axis=2,sense=1,        color=[0,1,1], material = mat1))
#O.bodies.append(utils.wall(-sizeB/2,axis=1,sense=1, color=[0,1,1], material = mat1))
#O.bodies.append(utils.wall(-sizeB/2,axis=0,sense=1, color=[0,1,1], material = mat1))

#t = polyhedra_utils.polyhedralBall(sizeB, 50, mat1, (0,0,0))
#t.state.pos = (0.,0.,0.020)
#O.bodies.append(t)

topmesh  = O.bodies.append(geom.facetBox((0.,0.,startPos), (sizeB/2.0,sizeB/2.0,0.),material=mat1))
leftmesh = O.bodies.append(geom.facetBox((0.,sizeB/2.0,sizeB/2.0),(sizeB/2.0,0.,sizeB/2.0),material=mat1))
backmesh = O.bodies.append(geom.facetBox((sizeB/2.0,0.,sizeB/2.0),(0.,sizeB/2.0,sizeB/2.0),material=mat1))

vertices = [[0,0,0],[sizeB,0,0],[sizeB,sizeB,0],[sizeB,sizeB,sizeB],[0,sizeB,0],[0,sizeB,sizeB],[0,0,sizeB],[sizeB,0,sizeB]]
t = polyhedra_utils.polyhedra(mat1,v=vertices, fixed=True)
t.state.pos = (0,0,sizeB/2)
O.bodies.append(t)

"""
i1 = ymport.textPolyhedra('new2.poly',shift=[0.012, 0.018, 0.0],material=mat1)
importedStones = O.bodies.append(i1[1])
"""

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
   #PolyhedraSplitter(iterPeriod=1,label='Splitter'),
   SplitPolyMohrCoulomb(iterPeriod=10,dead=False,label='Splitter', fileName = "./StressData"),
   TranslationEngine(translationAxis=[0,0,-1],velocity=velT,ids=topmesh, label='TT'),
   TranslationEngine(translationAxis=[0,-1,0],velocity=velL,ids=leftmesh,label='TL'),
   TranslationEngine(translationAxis=[-1,0,0],velocity=velB,ids=backmesh,label='TB'),
   PyRunner(command='addPlotData()',iterPeriod=10),
   PyRunner(command='breakControl()',iterPeriod=10, dead=False),
]

O.dt=odt

fT =Vector3.Zero
fL =Vector3.Zero
fB =Vector3.Zero
maxFT = Vector3.Zero
maxFL = Vector3.Zero
maxFB = Vector3.Zero

def addPlotData():
  global fT, maxFT
  global fL, maxFL
  global fB, maxFB
  fT=Vector3.Zero
  fL=Vector3.Zero
  fB=Vector3.Zero
  for i in topmesh:
    if (O.forces.f(i)):
      fT+=O.forces.f(i)
      
  for i in leftmesh:
    if (O.forces.f(i)):
      fL+=O.forces.f(i)
      
  for i in backmesh:
    if (O.forces.f(i)):
      fB+=O.forces.f(i)
      
  if (fT.norm() > maxFT.norm()):
    maxFT = fT
      
  if (fL.norm() > maxFL.norm()):
    maxFL = fL
  
  if (fB.norm() > maxFB.norm()):
    maxFB = fB
  
  SigmaT = fT[2]/(sizeB*sizeB)*factorR; uT = startPos - O.bodies[topmesh[0]].state.pos[2]
  SigmaL = fL[1]/(sizeB*sizeB)*factorR; uL = sizeB - O.bodies[leftmesh[0]].state.pos[1]
  SigmaB = fB[0]/(sizeB*sizeB)*factorR; uB = sizeB - O.bodies[backmesh[0]].state.pos[0]
  SigmaLT = 0.
  if (SigmaT):
    SigmaLT = SigmaL / SigmaT
  plot.addData(time=O.time, time2=O.time, 
    Energy=utils.kineticEnergy(),
    SigmaT = SigmaT, uT = uT,
    SigmaL = SigmaL, uL = uL,
    SigmaB = SigmaB, uB = uB,
    SigmaLT = SigmaLT)
    
  for i in O.interactions:
    if (isinstance(O.bodies[i.id1].shape,Polyhedra)):
      print("%d\t%g\t%g\n" % (i.id1, i.geom.penetrationVolume, O.bodies[i.id1].shape.GetVolume()))
    if (isinstance(O.bodies[i.id2].shape,Polyhedra)):
      print("%d\t%g\t%g\n" % (i.id2, i.geom.penetrationVolume, O.bodies[i.id2].shape.GetVolume()))

def breakControl():
  if (len(O.bodies) > 7):
    sigmaResT = maxFT[2]/(sizeB*sizeB)*factorR
    uT = startPos - O.bodies[topmesh[0]].state.pos[2]
    print ("STOP!!!!!!!!!!!!!!!!")
    print ('maxFT = %g;'%(sigmaResT))
    plot.saveGnuplot(descriptionIn + 'plot')
    if (not(os.path.isfile('./resFile'))):
      fWT = open('./resFile','w')
      fWT.write('m\tV0\tP\tSCz\tSCd\todt\tSRES\tfactorR\tU\n')
      fWT.close()
    
    fW = open('./resFile','a');
    fW.write(('%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\n' % (Wei_m, Wei_V0, Wei_P, sigmaCZ, sigmaCD, odt, sigmaResT, factorR, uT)))
    fW.close()
    O.pause()
  
plot.plots={'time':('Energy'),'uT':('SigmaT'),'uL':('SigmaL'),'uB':('SigmaB'),'time2':('SigmaLT')};

  
plot.plot()



from yade import qt
qt.Controller()
V = qt.View()

R=yade.qt.Renderer()
R.bgColor=(1.,1.,1.) 

"""
O.run(100000, True)
"""
