# -*- coding: utf-8 -*-
# © Václav Šmilauer <eudoxos@arcig.cz>
#
# Test case for sphere-facet interaction preserving the original contact orientation.
# Z-gravity is being increased every 4000 iterations, the sphere dives more into the facet and stabilizes,
# etc. This process continues even if the sphere center passes on the other side of the facet and,
# (if distant transient interactions are allowed in the collider) if the sphere passes in its entirety to
# the other side of the facet. The interaction, however, still pushes in the same sense.
#
# After the gravity reaches some value, it is reset and the sphere should be pushed from the facet towards
# its original position. When the contact on the original side is lost, the interaction should be deleted.
#
#
# The only tunable sign places the sphere either on the top ot at the bottom of the facet
# and sets gravity accordingly. It can be +1 or -1
#
sign=-1
#

O.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingBox2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]
O.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingBox2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider',{'haveDistantTransient':True}),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry',{'hasShear':True,'interactionDetectionFactor':1.4}),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry',{'hasShear':True}),
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('SimpleElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
	DeusExMachina('GravityEngine',{'gravity':[0,0,-sign*500],'label':'gravitator'}),
	DeusExMachina("NewtonsDampedLaw",{'damping':0.8}),
	StandAloneEngine('PeriodicPythonRunner',{'iterPeriod':4000,'command':'setGravity()'}),
	]
O.bodies.append([
	utils.facet([[-1,-1,0],[1,-1,0],[0,1,0]],dynamic=False,color=[1,0,0],young=1e3),
	utils.sphere([0,0,sign*.49999],radius=.5,young=1e3,wire=True,density=1),
])
O.miscParams=[Generic('GLDrawSphere',{'glutUse':True})]
O.saveTmp('init')
O.dt=1e-4

def setGravity():
	gz=gravitator["gravity"][2]
	gravitator["gravity"]=[0,0,1.05*gz]
	if abs(gz)>=2500:
		print "Gravity reset & slow down"
		O.dt=1e-6;
		gravitator["gravity"]=[0,0,0]
	if abs(gz)>0: print gz

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Interaction_geometry']=True
	qt.Controller()
except ImportError: pass


