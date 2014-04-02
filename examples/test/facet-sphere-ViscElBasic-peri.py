# -*- coding: utf-8

# Testing facet-sphere interaction in periodic case.
# Pass, if the sphere is rolling from left to right through the period.



sphereRadius=0.1
tc=0.001# collision time 
en=0.3  # normal restitution coefficient
es=0.3  # tangential restitution coefficient
density=2700
frictionAngle=radians(35)# 
facetMat=O.materials.append(ViscElMat(frictionAngle=frictionAngle,tc=tc,en=en,et=es)) 
sphereMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,tc=tc,en=en,et=es))

#floor
n=5
s=1./n
for i in range(0,n):
	for j in range(0,n):
		O.bodies.append([
			facet( [(i*s,j*s,0.1),(i*s,(j+1)*s,0.1),((i+1)*s,(j+1)*s,0.1)],material=facetMat),
			facet( [(i*s,j*s,0.1),((i+1)*s,j*s,0.1),((i+1)*s,(j+1)*s,0.1)],material=facetMat),
		])

# Spheres
sphId=O.bodies.append([sphere((0.5,0.5,0.2), 0.1, material=sphereMat)])
O.bodies[sphId[-1]].state.vel=(0.5,0,0)

## Engines 
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0,gravity=[0,0,-9.81]),
]

O.periodic=True
O.cell.refSize=(1,1,1)

O.dt=.01*tc

O.saveTmp()

