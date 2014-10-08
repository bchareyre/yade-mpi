################################################################################
#
# Python script to test tetra-tetra contact detection for different possible
# contact modes. Some tests are run twice to test the symmetry of the law.
#
# It runs several tests, making pause before each one. If run with GUI, you can
# adjust the viewer
#
# During the test, momentum, angular momentum and kinetic energy is tracked in
# plot.data. You can use plot.plot() to see the results (in sime modes the
# energy is not conserved for instace).
#
################################################################################
from yade import qt,plot

mat = PolyhedraMat()
mat.density = 2600 #kg/m^3 
mat.Ks = 2000000
mat.Kn = 1E9 #Pa
mat.frictionAngle = 0.5 #rad
O.materials.append(mat)

O.dt = 4e-5
O.engines = [
	ForceResetter(),
	InsertionSortCollider([Bo1_Polyhedra_Aabb()]),
	InteractionLoop(
		[Ig2_Polyhedra_Polyhedra_PolyhedraGeom()],
		[Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys()],
		[Law2_PolyhedraGeom_PolyhedraPhys_Volumetric()]
	),
	NewtonIntegrator(damping=0),
	PyRunner(iterPeriod=500,command="addPlotData()"),
	PyRunner(iterPeriod=1,command="runExamples()"),
]

def addPlotData():
	p = utils.momentum()
	l = utils.angularMomentum()
	plot.addData(
		i = O.iter,
		e = utils.kineticEnergy(),
		px = p[0],
		py = p[1],
		pz = p[2],
		lx = l[0],
		ly = l[1],
		lz = l[2],
	)

def prepareExample(vertices1,vertices2,vel1=(0,0,0),vel2=(0,0,0),amom1=(0,0,0),amom2=(0,0,0),label=''):
	O.interactions.clear()
	O.bodies.clear()
	t1 = tetraPoly(vertices1,color=(0,1,0),wire=False)
	t2 = tetraPoly(vertices2,color=(0,1,1),wire=False)
	O.bodies.append((t1,t2))
	t1.state.vel = vel1
	t2.state.vel = vel2
	t1.state.angMom = amom1
	t2.state.angMom = amom2
	if label: print label
	O.pause()
	O.step()

def runExamples():
	dt = 20000
	if O.iter == 1:
		vertices1 = ((0,0,0),(2,0,0),(0,2,0),(0,0,2))
		vertices2 = ((1,1,1),(3,1,1),(1,3,1),(1,1,3))
		prepareExample(vertices1,vertices2,vel2=(-1,-1,-1),label='\ntesting vertex-triangle contact...\n')
	if O.iter == 1*dt:
		plot.data = {}
		vertices1 = ((1,1,1),(3,1,1),(1,3,1),(1,1,3))
		vertices2 = ((0,0,0),(2,0,0),(0,2,0),(0,0,2))
		prepareExample(vertices1,vertices2,vel1=(-1,-1,-1),label='\ntesting vertex-triangle contact 2...\n')
	elif O.iter == 2*dt:
		vertices1 = ((0,0,0),(0,0,1.1),(1,0,1),(0,1,.9))
		vertices2 = ((0,.5,1.4),(-.5,.5,.6),(-1.6,0,1),(-1.6,1.1,1))
		prepareExample(vertices1,vertices2,vel2=(1,0,0),amom2=(0,10,0),label='\ntesting edge-edge contact\n')
	elif O.iter == 3*dt:
		vertices1 = ((0,0,0),(0,0,1.1),(1,0,1),(0,1,.9))
		vertices2 = ((-.5,.5,.6),(0,.5,1.4),(-1.6,1.1,1),(-1.6,0,1))
		prepareExample(vertices1,vertices2,vel2=(1,0,0),amom2=(0,10,0),label='\ntesting edge-edge contact\n')
	elif O.iter == 4*dt:
		vertices1 = ((.1,-.4,-.3),(-.3,-.4,2),(3,-.2,2),(-.1,3,2))
		vertices2 = ((.5,1.5,2.3),(1.5,.5,2.3),(2,2,3),(0,0,3))
		prepareExample(vertices1,vertices2,vel2=(0,0,-1),amom2=(0,0,0),label='\ntesting edge-triangle contact\n')
	elif O.iter == 5*dt:
		vertices1 = ((.1,-.4,-.3),(-.3,-.4,2),(3,-.2,2),(-.1,3,2))
		vertices2 = ((-.5,2.5,2.3),(2.5,-.5,2.3),(2,2,3),(0,0,3))
		prepareExample(vertices1,vertices2,vel2=(0,0,-1),amom2=(0,0,0),label='\ntesting edge-triangle contact 2\n')
	elif O.iter == 6*dt:
		vertices1 = ((1,0,0),(0,1,0),(0,0,1),(1,1,1))
		vertices2 = ((.5,.5,1.2),(0,.1,2),(2,0,2),(0,1,2))
		prepareExample(vertices1,vertices2,vel2=(0,0,-1),label='\ntesting vertex-edge contact\n')
	elif O.iter == 7*dt:
		vertices1 = ((.5,.5,1.2),(0,.1,2),(2,0,2),(0,1,2))
		vertices2 = ((1,0,0),(0,1,0),(0,0,1),(1,1,1))
		prepareExample(vertices1,vertices2,vel1=(0,0,-1),label='\ntesting vertex-edge contact 2\n')
	elif O.iter == 8*dt:
		vertices1 = ((0,0,0),(1,0,0),(0,1,0),(0,0,1))
		vertices2 = ((0,0,1.2),(.9,.8,2),(-.7,.61,2.3),(0,-.7,2.1))
		prepareExample(vertices1,vertices2,vel2=(0,0,-1),label='\ntesting vertex-edge contact\n')
	elif O.iter == 9*dt:
		vertices1 = ((0,0,1.2),(.9,.8,2),(-.7,.61,2.3),(0,-.7,2.1))
		vertices2 = ((0,0,0),(1,0,0),(0,1,0),(0,0,1))
		prepareExample(vertices1,vertices2,vel1=(0,0,-1),label='\ntesting vertex-edge contact 2\n')
	elif O.iter == 10*dt:
		O.pause()

viewer = qt.View()
plot.plots = {'i':'e','i ':('px','py','pz'),'i  ':('lx','ly','lz')}

O.step()
O.step()
O.step()
