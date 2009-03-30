# -*- encoding=utf-8 -*-

from yade import utils

## Omega
o=Omega() 

## PhysicalParameters 
Young = 7e6
Poisson = 0.2
Density=2700

o.bodies.append([
        utils.sphere([0,0,0.6],0.25,young=Young,poisson=Poisson,density=Density),
        utils.facet([[-0.707,-0.707,0.1],[0,1.414,0],[1.414,0,0]],dynamic=False,color=[1,0,0],young=Young,poisson=Poisson),
        utils.facet([[0,1.414,0],[1.414,0,0],[0.707,0.707,-2.0]],dynamic=False,color=[1,0,0],young=Young,poisson=Poisson)])

## Initializers 
o.initializers=[
	MetaEngine('BoundingVolumeMetaEngine',[EngineUnit('InteractingSphere2AABB'),EngineUnit('InteractingFacet2AABB'),EngineUnit('MetaInteractingGeometry2AABB')])
	]

## Engines 
o.engines=[
	StandAloneEngine('PhysicalActionContainerReseter'),
	MetaEngine('BoundingVolumeMetaEngine',[
		EngineUnit('InteractingSphere2AABB'),
		EngineUnit('InteractingFacet2AABB'),
		EngineUnit('MetaInteractingGeometry2AABB')
	]),
	StandAloneEngine('PersistentSAPCollider'),
	MetaEngine('InteractionGeometryMetaEngine',[
		EngineUnit('InteractingSphere2InteractingSphere4SpheresContactGeometry'),
		EngineUnit('InteractingFacet2InteractingSphere4SpheresContactGeometry')
	]),
	MetaEngine('InteractionPhysicsMetaEngine',[EngineUnit('MacroMicroElasticRelationships')]),
	StandAloneEngine('ElasticContactLaw'),
    DeusExMachina('GravityEngine',{'gravity':[0,0,-10]}),
	DeusExMachina('NewtonsDampedLaw',{'damping':0.3}),
]

## Timestep 
o.dt=5e-6
o.saveTmp('init')

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Body_wire']=True
	renderer['Interaction_physics']=True
	qt.Controller()
except ImportError: pass

