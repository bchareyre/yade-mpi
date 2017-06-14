# capillary bridge example between 2 spheres


r1,r2 = 1e-4,4e-4 # better take small particles
z1,z2=0,0.95*(r1+r2)

O.bodies.append(sphere(center=Vector3(0,0,z1),radius=r1,dynamic=0))
O.bodies.append(sphere(center=Vector3(0,0,z2),radius=r2,dynamic=0))

O.engines=[ForceResetter()
           ,InsertionSortCollider([Bo1_Sphere_Aabb()])
           ,InteractionLoop(
               [Ig2_Sphere_Sphere_ScGeom()],
               [Ip2_FrictMat_FrictMat_CapillaryPhys()],
               [Law2_ScGeom_FrictPhys_CundallStrack(neverErase=1)]
                           )
           ,Law2_ScGeom_CapillaryPhys_Capillarity(capillaryPressure=1e3)
           ,NewtonIntegrator()
           ,GlobalStiffnessTimeStepper()
           ,PyRunner(command='computeThings()',iterPeriod=1)
          ]

from yade import plot
def computeThings():
    if O.interactions.has(0,1) and O.interactions[0,1].isReal:
        i = O.interactions[0,1]
        un = i.geom.penetrationDepth
        vM = i.phys.vMeniscus
        delta1,delta2 = i.phys.Delta1,i.phys.Delta2
        fCap = i.phys.fCap.norm()
        nn11,nn33 = i.phys.nn11,i.phys.nn33
    else:
        un,vM,delta1,delta2,fCap,nn11,nn33 = float('nan'),float('nan'),float('nan'),float('nan'),float('nan'),float('nan'),float('nan')

    plot.addData(delta1 = delta1, delta2 = delta2, fCap = fCap
                 , it =O.iter, un = un ,vM = vM
                 , nn11 = nn11, nn33 = nn33)

plot.plots={'un':'fCap','un ':'vM',' un':('delta1','delta2'),' un ':('nn11','nn33')}
plot.plot()

# spheres get closer:
O.bodies[1].state.vel=Vector3(0,0,-0.002)
O.run(500,wait=1)

# spheres move apart:
O.bodies[1].state.vel=Vector3(0,0,0.02)
O.run(4000,wait=1)


