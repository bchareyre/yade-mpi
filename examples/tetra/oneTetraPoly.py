################################################################################
# 
# Script to test tetra gl functions and prescribed motion
#
################################################################################
v1 = (0,0,0)
v2 = (1,0,0)
v3 = (0,1,0)
v4 = (0,0,1)

t1 = tetraPoly((v1,v2,v3,v4),color=(0,1,0))
O.bodies.append((t1))

def changeVelocity():
	if O.iter == 50000:
		t1.state.vel = (-1,0,0)
	if O.iter == 100000:
		t1.state.vel = (0,1,-1)
	if O.iter == 150000:
		t1.state.vel = (0,0,0)
		t1.state.angMom = (0,0,10)
	if O.iter == 200000:
		O.pause()

O.engines = [
	ForceResetter(),
	InsertionSortCollider([Bo1_Polyhedra_Aabb()]),
	InteractionLoop([],[],[]),
	NewtonIntegrator(),
	PyRunner(iterPeriod=1,command="changeVelocity()"),
]
O.step()


try:
	from yade import qt
	qt.View()
except:
	pass

O.dt = 1e-5
t1.state.vel = (1,0,0)
O.run()
