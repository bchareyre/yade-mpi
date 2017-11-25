#!/usr/bin/env python
#encoding: ascii

# Testing of the Deformation Enginge with Luding Contact Law
# Modified Oedometric Test
# The reference paper [Haustein2017118]


from yade import utils, plot, timing
from yade import pack

o = Omega()

# Physical parameters
fr        = 0.3
rho       = 2000
Diameter  = 16.5e-3
r1        = Diameter
r2        = Diameter
k1        = 1005.0
kp        = 10.0*k1
kc        = k1 * 0.0
ks        = k1 * 0.1
DeltaPMax = Diameter/3.0
Chi1      = 0.34

o.dt = 1.0e-5

particleMass = 4.0/3.0*math.pi*r1*r1*r1*rho

Vi1 = math.sqrt(k1/particleMass)*DeltaPMax*Chi1

PhiF1 = DeltaPMax*(kp-k1)*(r1+r2)/(kp*2*r1*r2)


#*************************************

# Add material
mat1 = O.materials.append(LudingMat(frictionAngle=fr, density=rho, k1=k1, kp=kp, ks=ks, kc=kc, PhiF=PhiF1, G0 = 0.0))


# Spheres for compression

sp=pack.SpherePack()
sp.makeCloud((-4.0*Diameter,-4.0*Diameter,-2.5*Diameter),(3.0*Diameter,3.0*Diameter,15.0*Diameter), rMean=Diameter/2.0, num=300)
sp.toSimulation()


######################################################################
O.bodies.append(
    geom.facetBox((0,0,0), (4.0*Diameter,4.0*Diameter,4.0*Diameter), wallMask=63-32, material=mat1)
)

# Add engines
o.engines = [
  ForceResetter(),
  InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=1.05),
                         Bo1_Wall_Aabb(),
                         Bo1_Facet_Aabb()
                         ]),
  InteractionLoop(
    [Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=1.05),
     Ig2_Facet_Sphere_ScGeom(),
     Ig2_Wall_Sphere_ScGeom()],
    [Ip2_LudingMat_LudingMat_LudingPhys()],
    [Law2_ScGeom_LudingPhys_Basic()]
  ),
  NewtonIntegrator(damping=0.1, gravity=[0, 0, -9.81]),
  #VTKRecorder(fileName='vtk-',recorders=['all'],iterPeriod=10000),
  PyRunner(command='checkForce()', realPeriod=1, label="fCheck"),
  DeformControl(label="DefControl")
]


def checkForce():
    # at the very start, unbalanced force can be low as there is only few
    # contacts, but it does not mean the packing is stable
    if O.iter < 20000:
        return
    # the rest will be run only if unbalanced is < .1 (stabilized packing)
    timing.reset()
    if unbalancedForce() > 0.2:
        return
    # add plate at upper box side

    highSphere = 0.0
    for b in O.bodies:
        if highSphere < b.state.pos[2] and isinstance(b.shape, Sphere):
            highSphere = b.state.pos[2]
        else:
            pass

    O.bodies.append(wall(highSphere+0.5*Diameter, axis=2, sense=-1, material=mat1))
    # without this line, the plate variable would only exist inside this
    # function
    global plate
    plate = O.bodies[-1]  # the last particles is the plate
    # Wall objects are "fixed" by default, i.e. not subject to forces
    # prescribing a velocity will therefore make it move at constant velocity
    # (downwards)
    plate.state.vel = (0, 0, -.1)
    # start plotting the data now, it was not interesting before
    O.engines = O.engines + [PyRunner(command='addPlotData()', iterPeriod=1000)]
    # next time, do not call this function anymore, but the next one
    # (unloadPlate) instead
    fCheck.command = 'unloadPlate()'


def unloadPlate():
    # if the force on plate exceeds maximum load, start unloading
    # if abs(O.forces.f(plate.id)[2]) > 5e-2:
    if abs(O.forces.f(plate.id)[2]) > 5.0e2:
        plate.state.vel *= -1
        # next time, do not call this function anymore, but the next one
        # (stopUnloading) instead
        fCheck.command = 'stopUnloading()'


def stopUnloading():
    if abs(O.forces.f(plate.id)[2]) == 0:
        # O.tags can be used to retrieve unique identifiers of the simulation
        # if running in batch, subsequent simulation would overwrite each other's output files otherwise
        # d (or description) is simulation description (composed of parameter values)
        # while the id is composed of time and process number
        # plot.saveDataTxt(O.tags['d.id'] + '.txt')
        plot.saveDataTxt('data'+ O.tags['id'] +'.txt')
        print timing.stats()
        O.pause()


def addPlotData():
    if not isinstance(O.bodies[-1].shape, Wall):
        plot.addData()
        return
    Fz = O.forces.f(plate.id)[2]
    plot.addData(
        Fz=Fz,
        w=plate.state.pos[2] - (-4*Diameter),
        unbalanced=unbalancedForce(),
        i=O.iter
    )


def defVisualizer():
   with open("data.dat","a") as f:
       for b in O.bodies:
           if isinstance(b.shape, Sphere):
               rData = "{x},{y},{z},{r},{w}\t".format(x = b.state.pos[0],
                                                y = b.state.pos[1],
                                                z = b.state.pos[2],
                                                r = b.shape.radius + b.state.dR,
                                                w = plate.state.pos[2]
                                               )
               f.write(rData)
       f.write("\n")



O.timingEnabled=True
O.run(1, True)
plot.plots={'w':('Fz', None)}
plot.plot()

