
#Simulates a 5mm Diameter bubble in water rising and colliding with another bubble of the same diameter

rad = 2.5e-3
#O.materials.append(FrictMat(young=1e3,density=1000))
O.materials.append(BubbleMat(density=1000,surfaceTension=71.97e-3))
O.bodies.append([
   utils.sphere(center=(0,0,0),radius=rad,fixed=True),
   utils.sphere((0,0,-2*rad*1.1),rad)
])
O.dt = 1e-7

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Sphere_Aabb()]),
   InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_BubbleMat_BubbleMat_BubblePhys()],[Law2_ScGeom_BubblePhys_Bubble()]),
#   InteractionLoop([Ig2_Sphere_Sphere_ScGeom()],[Ip2_FrictMat_FrictMat_FrictPhys()],[Law2_ScGeom_FrictPhys_CundallStrack()]),
   NewtonIntegrator(damping=0.1,gravity=(0,0,9.81))
]
O.saveTmp()
