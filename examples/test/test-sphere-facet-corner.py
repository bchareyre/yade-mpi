# -*- encoding=utf-8 -*-



## PhysicalParameters 
Young = 7e6
Poisson = 0.2
Density=2700

# Append a material
mat=O.materials.append(FrictMat(young=Young,poisson=Poisson,density=Density,frictionAngle=26))

O.bodies.append([
        sphere([0,0,0.6],0.25,material=mat),
        facet([[-0.707,-0.707,0.1],[0,1.414,0],[1.414,0,0]],dynamic=False,color=[1,0,0],material=mat),
        facet([[0,1.414,0],[1.414,0,0],[0.707,0.707,-2.0]],dynamic=False,color=[1,0,0],material=mat)])

## Engines 
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Facet_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()],
	),
	NewtonIntegrator(damping=.3,gravity=(0,0,-10))
]

## Timestep 
O.dt=5e-6
O.saveTmp()

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer.wire=True
	renderer.intrPhys=True
	qt.Controller()
except ImportError: pass

