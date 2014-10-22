# gravity deposition, continuing with oedometric test after stabilization
# shows also how to run parametric studies with yade-batch

# The components of the batch are:
# 1. table with parameters, one set of parameters per line (ccc.table)
# 2. utils.readParamsFromTable which reads respective line from the parameter file
# 3. the simulation muse be run using yade-batch, not yade
#
# $ yade-batch --job-threads=1 03-oedometric-test.table 03-oedometric-test.py
#


# create box with free top, and ceate loose packing inside the box
from yade import plot, polyhedra_utils
from yade import qt

m = PolyhedraMat()
m.density = 2600 #kg/m^3 
m.young = 1E6 #Pa
m.poisson = 20000/1E6
m.frictionAngle = 0.6 #rad

O.bodies.append(utils.wall(0,axis=2,sense=1, material = m))

t = polyhedra_utils.polyhedra(m,size = (0.06,0.06,0.06),seed = 5)
t.state.pos = (0.,0.,0.5)
O.bodies.append(t)

def checkUnbalanced():   
   # at the very start, unbalanced force can be low as there is only few contacts, but it does not mean the packing is stable
   print "unbalanced forces = %.5f, position %f, %f, %f"%(utils.unbalancedForce(), t.state.pos[0], t.state.pos[1], t.state.pos[2])
    	
   

O.engines=[
   ForceResetter(),
   InsertionSortCollider([Bo1_Polyhedra_Aabb(),Bo1_Wall_Aabb(),Bo1_Facet_Aabb()]),
   InteractionLoop(
      [Ig2_Wall_Polyhedra_PolyhedraGeom(), Ig2_Polyhedra_Polyhedra_PolyhedraGeom(), Ig2_Facet_Polyhedra_PolyhedraGeom()], 
      [Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()], # collision "physics"
      [Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]   # contact law -- apply forces
   ),
   #GravityEngine(gravity=(0,0,-9.81)),
   NewtonIntegrator(damping=0.5,gravity=(0,0,-9.81)),
   PyRunner(command='checkUnbalanced()',realPeriod=3,label='checker')

]


#O.dt=0.025*polyhedra_utils.PWaveTimeStep()
O.dt=0.00025




qt.Controller()
V = qt.View()


O.saveTmp()
#O.run()
#O.save('./done')
utils.waitIfBatch()
