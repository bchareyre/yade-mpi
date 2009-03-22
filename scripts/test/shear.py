#
# script for testing SpheresContactGeometry shear computation
# Runs the same 2-sphere setup for useShear=False and for useShear=True
#

O.bodies.append([
	utils.sphere([0,0,0],.5000001,dynamic=False,color=(1,0,0)),
	utils.sphere([0,0,1],.5000001,dynamic=True,color=(0,0,1))
])
O.engines=[
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingFacet2AABB'),
	]),
	StandAloneEngine('PersistentSAPCollider',{'haveDistantTransient':True}),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[
		EngineUnit('SimpleElasticRelationships')
	]),
	DeusExMachina('RotationEngine',{'rotationAxis':[1,1,0],'angularVelocity':.001,'subscribedBodies':[1]}),
	StandAloneEngine('ElasticContactLaw',{'useShear':False}),
	StandAloneEngine('PeriodicPythonRunner',{'iterPeriod':10000,'command':'interInfo()'}),
	#DeusExMachina('NewtonsDampedLaw',{'damping':0})
]
O.miscParams=[
	Generic('GLDrawSphere',{'glutUse':True})
]

O.dt=1e-8
O.saveTmp('init')

def interInfo():
	i=O.interactions[0,1]
	if 1:
		print O.time,i.phys['shearForce']
	else:
		r1,r2=O.bodies[0].shape['radius'],O.bodies[1].shape['radius']
		theta=[e['angularVelocity'] for e in O.engines if e.name=='RotationEngine'][0]
		f=.5*(r1+r2)*theta*O.time*i.phys['ks']
		print O.time,i.phys['shearForce'],f,i.phys['shearForce'][0]/f


print '=========== no shear ============'
O.run(100000,True)
nIter=O.iter
print '============= shear ============='
O.loadTmp('init')
ecl=[e for e in O.engines if e.name=='ElasticContactLaw'][0]
ecl['useShear']=True
O.run(nIter,True)
quit()
