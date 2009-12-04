# -*- encoding=utf-8 -*-

from yade import utils

## PhysicalParameters 
Young = 7e6
Poisson = 0.2
Density=2700

O.bodies.append([
        utils.sphere([0,0,0.6],0.25,young=Young,poisson=Poisson,density=Density),
        utils.facet([[-0.707,-0.707,0.1],[0,1.414,0],[1.414,0,0]],dynamic=False,color=[1,0,0],young=Young,poisson=Poisson),
        utils.facet([[0,1.414,0],[1.414,0,0],[0.707,0.707,-2.0]],dynamic=False,color=[1,0,0],young=Young,poisson=Poisson)])

## Initializers 
O.initializers=[
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()])
	]

## Engines 
O.engines=[
	BexResetter(),
	BoundDispatcher([InteractingSphere2AABB(),InteractingFacet2AABB(),MetaInteractingGeometry2AABB()]),
	InsertionSortCollider(),
	InteractionGeometryDispatcher([InteractingSphere2InteractingSphere4SpheresContactGeometry(),InteractingFacet2InteractingSphere4SpheresContactGeometry()]),
	InteractionPhysicsDispatcher([MacroMicroElasticRelationships()]),
	ElasticContactLaw(),
	GravityEngine(gravity=(0,0,-10)),
	NewtonsDampedLaw(damping=.3)
]

## Timestep 
O.dt=5e-6
O.saveTmp()

try:
	from yade import qt
	renderer=qt.Renderer()
	renderer['Body_wire']=True
	renderer['Interaction_physics']=True
	qt.Controller()
except ImportError: pass

