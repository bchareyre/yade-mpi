from yade import plot, polyhedra_utils
from yade import qt

m = PolyhedraMat()
O.materials.append(m)

t1,t2 = [polyhedra_utils.polyhedra(m,size=(1,1,1),seed=i) for i in (5,6)]
O.bodies.append((t1,t2))
t2.state.pos = (3,0,0)
t2.state.vel = (-1,0,0)

factor = 2
O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(aabbEnlargeFactor=factor)]),
   InteractionLoop(
      [Ig2_Polyhedra_Polyhedra_PolyhedraGeom(interactionDetectionFactor=factor)],
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
   ),
   NewtonIntegrator(),
]

O.dt=0.00001

try: qt.View()
except: pass
