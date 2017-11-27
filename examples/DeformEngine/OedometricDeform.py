#!/usr/bin/env python
#encoding: ascii

# Modefied Oedometric Test for particle deformation under const. volume
# The reference paper [Haustein2017]


from yade import utils, plot, timing
from yade import pack


o = Omega()

# Physical parameters
fr = 0.3
rho = 2000
tc = 0.002
en = 0.7
et = 0.7
Diameter = 16.5e-3       # R

#*************************************

o.dt = 1.0e-5       # Timestep

#*************************************

# Add material
mat1 = O.materials.append(ViscElMat(frictionAngle=fr, tc=tc, en=en, et=et))


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
    [Ip2_ViscElMat_ViscElMat_ViscElPhys()],
    [Law2_ScGeom_ViscElPhys_Basic()]
  ),
  NewtonIntegrator(damping=0.1, gravity=[0, 0, -9.81]),
  #VTKRecorder(fileName='vtk-',recorders=['all'],iterPeriod=20000),
  PyRunner(command='checkForce()', realPeriod=1, label="fCheck"),
  DeformControl(label="DefControl")
]


def checkForce():
    # at the very start, unbalanced force can be low as there is only few
    # contacts, but it does not mean the packing is stable
    if O.iter < 20000:
        return
    # the rest will be run only if unbalanced is < 1.0 (stabilized packing)
    timing.reset()
    if unbalancedForce() > 0.2:
        return

    highSphere=0.0
    for b in O.bodies:
        if highSphere < b.state.pos[2] and isinstance(b.shape, Sphere):
            highSphere = b.state.pos[2]
        else:
            pass


    # add plate at upper box side
    O.bodies.append(wall(highSphere+0.5*Diameter, axis=2, sense=-1, material=mat1))
    # without this line, the plate variable would only exist inside this
    # function
    global plate
    plate = O.bodies[-1]  # the last particles is the plate
    # Wall objects are "fixed" by default, i.e. not subject to forces
    # prescribing a velocity will therefore make it move at constant velocity
    # (downwards)
    plate.state.vel = (0, 0, -.01)
    # start plotting the data now, it was not interesting before
    O.engines = O.engines + [PyRunner(command='addPlotData()', iterPeriod=1000)]
    # next time, do not call this function anymore, but the next one
    # (unloadPlate) instead
    fCheck.command = 'unloadPlate()'


def unloadPlate():
    # if the force on plate exceeds maximum load, start unloading
    # if abs(O.forces.f(plate.id)[2]) > 5e-2:
    # if plate.state.pos[2] < (-4*Diameter+40.0e-3):
    if abs(O.forces.f(plate.id)[2]) > 5e2:
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
        w=plate.state.pos[2] - (-4*Diameter),    # hight of the box (Bottom to upper Wall)
        unbalanced=unbalancedForce(),
        i=O.iter
    )



O.timingEnabled=True
O.run(1, True)
plot.plots={'w':('Fz', None)}
plot.plot()
