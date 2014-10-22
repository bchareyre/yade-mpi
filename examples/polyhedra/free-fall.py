from yade import plot, polyhedra_utils


gravel = PolyhedraMat()
gravel.density = 2600 #kg/m^3 
gravel.young = 1E7 #Pa
gravel.poisson = 20000/1E7
gravel.frictionAngle = 0.5 #rad

steel = PolyhedraMat()
steel.density = 7850 #kg/m^3 
steel.young = 10*gravel.young
steel.poisson = gravel.poisson
steel.frictionAngle = 0.4 #rad

rubber = PolyhedraMat()
rubber.density = 1000 #kg/m^3 
rubber.young = gravel.young/10
rubber.poisson = gravel.poisson
rubber.frictionAngle = 0.7 #rad

O.bodies.append(polyhedra_utils.polyhedra(gravel,v=((0,0,-0.05),(0.3,0,-0.05),(0.3,0.3,-0.05),(0,0.3,-0.05),(0,0,0),(0.3,0,0),(0.3,0.3,0),(0,0.3,0)),fixed=True, color=(0.35,0.35,0.35)))
#O.bodies.append(utils.wall(0,axis=1,sense=1, material = gravel))
#O.bodies.append(utils.wall(0,axis=0,sense=1, material = gravel))
#O.bodies.append(utils.wall(0.3,axis=1,sense=-1, material = gravel))
#O.bodies.append(utils.wall(0.3,axis=0,sense=-1, material = gravel))

polyhedra_utils.fillBox((0,0,0), (0.3,0.3,0.3),gravel,sizemin=[0.025,0.025,0.025],sizemax=[0.05,0.05,0.05],seed=4)

def checkUnbalancedI():   
    print "iter %d, time elapsed %f,  time step %.5e, unbalanced forces = %.5f"%(O.iter, O.realtime, O.dt, utils.unbalancedForce())		

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb(),Bo1_Facet_Aabb()]),
   InteractionLoop(
      [Ig2_Wall_Polyhedra_PolyhedraGeom(), Ig2_Polyhedra_Polyhedra_PolyhedraGeom(), Ig2_Facet_Polyhedra_PolyhedraGeom()], 
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()], # collision "physics"
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]   # contact law -- apply forces
   ),
   #GravityEngine(gravity=(0,0,-9.81)),
   NewtonIntegrator(damping=0.3,gravity=(0,0,-9.81)),
   PyRunner(command='checkUnbalancedI()',realPeriod=5,label='checker')
]


#O.dt=0.25*polyhedra_utils.PWaveTimeStep()
O.dt=0.0025*polyhedra_utils.PWaveTimeStep()


from yade import qt
qt.Controller()
V = qt.View()
V.screenSize = (550,450)
V.sceneRadius = 1
V.eyePosition = (0.7,0.5,0.1)
V.upVector = (0,0,1)
V.lookAt = (0.15,0.15,0.1)
