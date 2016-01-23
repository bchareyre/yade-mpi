O.engines=[
	ForceResetter(),
	InsertionSortCollider([PotentialParticle2AABB()]),
	InteractionLoop(
		[Ig2_PP_PP_ScGeom()],
		[Ip2_FrictMat_FrictMat_KnKsPhys(Knormal = 1e14, Kshear = 1e8,useFaceProperties=False,calJointLength=False,twoDimension=True,unitWidth2D=1.0,viscousDamping=0.7)],
		[Law2_SCG_KnKsPhys_KnKsLaw(label='law',neverErase=False)]
	),
	NewtonIntegrator(),
]

radius = 1

pos = Vector3(0,0,0)
sphere=Body()
radius=1
dynamic=True
wire=False
color=[0,0,255.0]
highlight=False
d = 1*radius
aabb = 1.0*radius
sphere.shape=PotentialParticle(
	k=1,
	r=.1*radius,
	R=1.*radius,
	a=[1.0,-1.0,0.0,0.0,0.0,0.0],
	b=[0.0,0.0,1.0,-1.0,0.0,0.0],
	c=[0.0,0.0,0.0,0.0,1.0,-1.0],
	d=[d,d,d,d,d,d],
	minAabb=Vector3(aabb,aabb,aabb),
	maxAabb=Vector3(aabb,aabb,aabb),
	maxAabbRotated=Vector3(aabb,aabb,aabb),
	minAabbRotated=Vector3(aabb,aabb,aabb),
	id=0
)
volume=4/3.*pi*radius**3
geomInert=(2./5.)*volume*radius**2
utils._commonBodySetup(sphere,volume,Vector3(geomInert,geomInert,geomInert), material=-1,pos=pos, noBound=False, resetState=True, fixed=False, blockedDOFs="")
sphere.state.pos = pos
sphere.state.ori = Quaternion.Identity
O.bodies.append(sphere)

pos = Vector3(3,0,0)
box=Body()
radius=1
dynamic=True
wire=False
color=[0,0,255.0]
highlight=False
d = 1*radius
aabb = 1.0*radius
box.shape=PotentialParticle(
	k=.05,
	r=.05*radius,
	R=1.*radius,
	a=[1.0,-1.0,0.0,0.0,0.0,0.0],
	b=[0.0,0.0,1.0,-1.0,0.0,0.0],
	c=[0.0,0.0,0.0,0.0,1.0,-1.0],
	d=[d,d,d,d,d,d],
	minAabb=sqrt(3)*Vector3(aabb,aabb,aabb),
	maxAabb=sqrt(3)*Vector3(aabb,aabb,aabb),
	maxAabbRotated=sqrt(3)*Vector3(aabb,aabb,aabb),
	minAabbRotated=sqrt(3)*Vector3(aabb,aabb,aabb),
	id=1
)
volume=4/3.*pi*radius**3
geomInert=(2./5.)*volume*radius**2
utils._commonBodySetup(box,volume,Vector3(geomInert,geomInert,geomInert), material=-1,pos=pos, noBound=False, resetState=True, fixed=False, blockedDOFs="")
box.state.pos = pos
box.state.ori = Quaternion((1,2,3),1)
O.bodies.append(box)

O.step()

sphere.state.vel = (10,0,0)
O.dt = 1e-5

from yade import qt
qt.View()
s = 20
Gl1_PotentialParticle.sizeX = s
Gl1_PotentialParticle.sizeY = s
Gl1_PotentialParticle.sizeZ = s
Gl1_PotentialParticle.aabbEnlargeFactor = 3
