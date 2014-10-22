from yade import polyhedra_utils
import random

polyMat  = PolyhedraMat(young=1e10,poisson=.05)
frictMat = FrictMat(young=1e9,poisson=.2)

O.materials.append((polyMat,frictMat))

poly = polyhedra_utils.polyhedra(polyMat,(1,2,3)); poly.wire=False
sph1 = sphere((2,2,2),.5,material=frictMat)
sph2 = sphere((-2,0,0),.5,material=frictMat)
sph3 = sphere((0,-2,0),.5,material=frictMat)
O.bodies.append((
	poly,
	sph1,
	sph2,
	sph3
))
	
O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Sphere_Aabb()]),
   InteractionLoop(
      [Ig2_Sphere_Polyhedra_ScGeom()], 
      [Ip2_FrictMat_PolyhedraMat_FrictPhys()],
      [Law2_ScGeom_FrictPhys_CundallStrack()],
   ),
   NewtonIntegrator(),
]

O.dt = 1e-7
O.step()

poly.state.blockedDOFs = 'xyzXYZ'
for s in (sph1,sph2,sph3):
	r=random.random
	s.state.vel = -10*(s.state.pos+Vector3(r(),r(),r()))

from yade import qt
qt.View()
