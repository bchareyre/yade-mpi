from yade import pack

O.materials.append(FrictMat(young=6.e8,poisson=.8,frictionAngle=.0))

sp = pack.SpherePack()
size = .24
sp.makeCloud(minCorner=(0,0,.05),maxCorner=(size,size,.05),rMean=.005,rRelFuzz=.4,num=400,periodic=True,seed=1)
sp.toSimulation()
O.cell.hSize = Matrix3(size,0,0, 0,size,0, 0,0,.1)
print len(O.bodies)
for p in O.bodies:
   p.state.blockedDOFs = 'zXY'
   p.state.mass = 2650 * 0.1 * pi * p.shape.radius**2 # 0.1 = thickness of cylindrical particle
   inertia = 0.5 * p.state.mass * p.shape.radius**2
   p.state.inertia = (.5*inertia,.5*inertia,inertia)

O.dt = utils.PWaveTimeStep()
print O.dt

O.engines = [
   ForceResetter(),
   InsertionSortCollider([Bo1_Sphere_Aabb()]),
   InteractionLoop(
      [Ig2_Sphere_Sphere_ScGeom()],
      [Ip2_FrictMat_FrictMat_FrictPhys()],
      [Law2_ScGeom_FrictPhys_CundallStrack()]
   ),
   PeriTriaxController(
      dynCell=True,
      goal=(-1.e5,-1.e5,0),
      stressMask=3,
      relStressTol=.001,
      maxUnbalanced=.001,
      maxStrainRate=(.5,.5,.0),
      doneHook='term()',
      label='biax'
   ),
   NewtonIntegrator(damping=.1)
]

def term():
   O.engines = O.engines[:3]+O.engines[4:]
   print getStress()
   print O.cell.hSize
   setContactFriction(0.5)
   O.cell.trsf=Matrix3.Identity
   O.cell.velGrad=Matrix3.Zero
   for p in O.bodies:
      p.state.vel = Vector3.Zero
      p.state.angVel = Vector3.Zero
      p.state.refPos = p.state.pos
      p.state.refOri = p.state.ori
   O.save('0.yade.gz')
   O.pause()

O.run();O.wait()

