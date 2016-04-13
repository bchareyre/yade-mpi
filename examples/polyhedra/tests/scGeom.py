from yade import plot, polyhedra_utils
from yade import qt

scGeom = 1

m = FrictMat() if scGeom else PolyhedraMat()
O.materials.append(m)

t1,t2 = [polyhedron(((-1,-1,-1),(+1,-1,-1),(-1,+1,-1),(+1,+1,-1),(-1,-1,+1),(+1,-1,+1),(-1,+1,+1),(+1,+1,+1))) for i in (0,1)]
O.bodies.append((t1,t2))
t2.state.pos = (3,0,0)
t2.state.vel = (-1,0,0)

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb()]),
   InteractionLoop(
      [Ig2_Polyhedra_Polyhedra_ScGeom() if scGeom else Ig2_Polyhedra_Polyhedra_PolyhedraGeom()],
      [Ip2_FrictMat_FrictMat_FrictPhys() if scGeom else Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_ScGeom_FrictPhys_CundallStrack() if scGeom else Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
   ),
   NewtonIntegrator(),
]

O.dt=0.00001

try: qt.View()
except: pass
