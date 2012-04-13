# -*- coding: utf-8

# Several spheres falling down into the box.
# Their weight is measured and compares with real mass particles

from yade import utils,pack,export,geom

tc=0.001
en=.003
es=.003
frictionAngle=radians(35)
density=2300

params=utils.getViscoelasticFromSpheresInteraction(tc,en,es)
defMat=O.materials.append(ViscElMat(density=density,frictionAngle=frictionAngle,**params)) # **params sets kn, cn, ks, cs


O.dt=.1*tc # time step
rad=0.2 # particle radius
tolerance = 0.0001


SpheresID=[]
SpheresID+=O.bodies.append(pack.regularHexa(pack.inSphere((Vector3(0.0,0.0,0.0)),0.5),radius=rad,gap=rad*0.5,material=defMat))

floorId=[]
floorId+=O.bodies.append(geom.facetBox((0,0,0),(0.6,0.6,0.6),material=defMat)) #Floor

#Calculate the weight of spheres
sphMass = utils.getSpheresVolume()*density*9.81


# Create engines
O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(), Ig2_Facet_Sphere_ScGeom()],
		[Ip2_ViscElMat_ViscElMat_ViscElPhys()],
		[Law2_ScGeom_ViscElPhys_Basic()],
	),
	NewtonIntegrator(damping=0.0,gravity=[0.0,0.0,-9.81])
]


O.run(10000)
O.wait()
curForce = utils.sumForces(ids=floorId,direction=Vector3(0,0,1))*(-1)
print ("Precalculated weight %f" % sphMass)
print ("Obtained weight %f" % curForce)

if (((sphMass-curForce)/curForce)>tolerance):
	resultStatus += 1

