from yade import plot

# setup 2 interactions on 2 otherwise identical couples of spheres
# one is handled by Law2_Dem3DofGeom_CpmPhys_Cpm and the other by Law2_ScGeom_CpmPhys_Cpm

# move the second sphere tangentially or rotate it, pick [0] or [1]
mode=['mov','rot'][1]
# number of steps to do (some influence on the incremental computation)
nSteps=100


r1,r2=1e-3,1e-3
dist=r1+r2
offset=Vector3(0,0,2*dist)
p1,p2=Vector3(0,0,0),Vector3(dist,0,0)
O.materials.append(CpmMat(young=30e9,poisson=.2,frictionAngle=atan(.8),sigmaT=3e6,relDuctility=5,epsCrackOnset=1e-4,G_over_E=.2,neverDamage=False,plTau=-1,plRateExp=0,dmgTau=-1,dmgRateExp=0))
# first 2 spheres used for Dem3DofGeom
# the other 2 used for ScGeom (#3 is dynamic, since ScGeom needs that)
O.bodies.append([utils.sphere(p1,r1,dynamic=False),utils.sphere(p2,r2,dynamic=False)])
O.bodies.append([utils.sphere(p1+offset,r1,dynamic=False),utils.sphere(p2+offset,r2,dynamic=True)])


O.engines=[InteractionGeometryDispatcher([Ig2_Sphere_Sphere_Dem3DofGeom()]),InteractionPhysicsDispatcher([Ip2_CpmMat_CpmMat_CpmPhys()])]
i1=utils.createInteraction(0,1) # caches functors, no need to specify them in the main loop
O.engines=[InteractionGeometryDispatcher([Ig2_Sphere_Sphere_ScGeom()]),InteractionPhysicsDispatcher([Ip2_CpmMat_CpmMat_CpmPhys()])]
i2=utils.createInteraction(2,3)

O.engines=[
	InteractionDispatchers([],[],[Law2_ScGeom_CpmPhys_Cpm(),Law2_Dem3DofGeom_CpmPhys_Cpm(yieldSurfType=0)]),
	StepDisplacer(subscribedBodies=[1,3],setVelocities=True,label='jumper'), # displace non-dynamic #1, set velocity on #3
	NewtonIntegrator(damping=0),
	PyRunner(iterPeriod=1,initRun=True,command='plotData()'),
]

def plotData():
	allData={}
	# gather same data for both configurations, suffix their labels with -DD/-Sc
	for i,key,sphere in zip([i1,i2],['-DD','-Sc'],[1,3]):
		data=dict(
			zRot=O.bodies[sphere].state.ori.toAxisAngle()[1],
			zShift=O.bodies[sphere].state.pos[2],
			epsT=i.phys.epsT.norm() if key=='-Sc' else i.geom.strainT().norm(),
			Ft=i.phys.shearForce.norm(),
			epsN=i.phys.epsN,
			epsPlSum=i.phys.epsPlSum,
			relResStr=i.phys.relResidualStrength,
			dist=(O.bodies[i.id1].state.pos-O.bodies[i.id2].state.pos).norm(),
			sigmaT=i.phys.sigmaT.norm()
		)
		for k in data: allData[k+key]=data[k]
	plot.addData(allData)

if mode=='mov':
	jumper.mov=Vector3(0,0,1/(1e4*nSteps))
elif mode=='rot':
	jumper.rot=Quaternion(Vector3.UnitZ,1/(1e4*nSteps))

O.run(nSteps,True)
if mode=='mov':
	plot.plots={'zShift-DD':(
		#('epsT-DD','g-'),('epsT-Sc','r^'),
		('dist-DD','g-'),('dist-Sc','r^'),
		None,
		('sigmaT-DD','b-'),('sigmaT-Sc','m^')
		#('relResStr-DD','b-'),('relResStr-Sc','m^')
		#('epsN-DD','b-'),('epsN-Sc','m^')
	)}
elif mode=='rot':
	plot.plots={'zRot-DD':(
		('epsT-DD','g|'),('epsT-Sc','r-'),
		None,
		('sigmaT-DD','b-'),('sigmaT-Sc','mv')
	)}



if 1:
	f='/tmp/cpm-geom-'+mode+'.pdf'
	plot.plot(noShow=True).savefig(f)
	print 'Plot saved to '+f
	quit()
else:
	plot.plot()


