######################
Architecture overview
######################

In the following, a high-level overview of Yade architecture will be given.
As many of the features are directly represented in simulation scripts, which
are written in Python, being familiar with this language will help you follow
the examples. For the rest, this knowledge is not strictly necessary.

****************
Simulation loop
****************

Yade's simulation are explicit, proceeding in time by given timestep (:ref:`timestep`). At every step, a sequence of :ctype:`Engines` is run; these Engines operate on :ctype:`Body`'s (particles) and their :ctype:`Interaction`'s.

Bodies
======
Bodies in simulation are distinguished by their unique numerical id.

Each body has some :ctype:`Shape` in the geometrical sense: sphere, facet (triangle), infinite wall; this shape is constant throughout Body's life. Orthogonal to shape is :ctype:`Material` holding material properties, independently of the shape, such as Young's modulus; a material can be shared between multiple bodies. To account for various changes a Body might undergo, there is a :ctype:`State` associated with every Body − it holds spatial position and orientation, velocity and so on. Finally (but this is invisible to the user), there is :ctype:`Bound` approximating (bounding) the body in space, to speed up collision detection. 

All these four properties (shape, material, state and bound) can be of different types (derived from the parent classes InteractingGeometry, Material, State and Bound). Frequently, yade makes decisions about computation based on those types. Objects making those decisions are called :ctype:`Dispatcher`'s and are essential to understand Yade; they are discussed below. 

Utility functions are defined to create bodies with all their necessary ingredients. For example, we can create sphere body and look at all of them:


	>>> s=utils.sphere([0,0,0],1)
	>>> s.shape 
	<InteractingSphere instance at 0xa639d28>
	>>> s.shape, s.state, s.mat, s.bound
	(<InteractingSphere instance at 0xa639d28>,
	 <State instance at 0xa639d80>,
	 <GranularMat instance at 0xa634150>,
	 <AABB instance at 0x9fba6a0>)
	>>> s.state.pos
	Vector3(0,0,0)
	>>> s.shape['radius']
	1.0

We see that a sphere with material of type GranularMat (default, unless you provide another Material) and bounding volume of type AABB (axis-aligned bounding box) was created. Its position is at origin and its radius is 1.0. Finally, this object can be inserted into the simulation

	>>> O.bodies.append(s)
	0
	>>> O.bodies.append(utils.sphere([0,0,2],.5))
	1

The return value is id of the body inserted. Since till now the simulation was empty, its id is 0 for the first sphere and 1 for the second one. Saving the id value is not necessary, unless you want access this particular body later; it is remembered elsewhere internally. Adding the same body twice is not possible.

	>>> O.bodies[1]
	<Body instance at 0x92e8f60>
	>>> O.bodies[100]
	IndexError: Body id out of range.


Bodies can be iterated over using standard python syntax:

	>>> for b in O.bodies: print b.id,b.shape['radius']
	0 1.0
	1 0.5

Interactions
============
Interactions (used as uniting term for contacts and bonds, i.e. non-cohesive and cohesive interactions) are always between two bodies. In typical cases, they are created automatically based on spatial position of bodies. If two bodies are sufficiently close (in terms of their bounds), the collider creates interaction, which we call *potential*. Later in the loop, such interaction is checked for precise overlap based on shapes of the 2 bodies (sphere with triangle, for instance); if they do overlap, the interaction becomes *real*.

Each (real) :ctype:`Interaction` has again several components.

* :ctype:`InteractionGeometry`, geometrical configuration of the couple of bodies; it is updated at every step and can be queried for geometrical characteristics of the interaction such as strain.

* :ctype:`InteractionPhysics` which holds various physical parameters and variables of the interaction.

Suppose now interactions are already created. We can access them by the id pair:

	>>> O.interactions[0,1]
	<Interaction instance at 0x93f9528>
	>>> i=O.interactions[0,1]
	>>> i.id1,i.id2
	(0, 1)
	>>> i.geom
	<Dem3Dof_Sphere_Sphere instance at 0x9403838>
	>>> i.phys
	<ElasticContactInteraction instance at 0x94038d0>
	>>> O.interactions[100,10111]
	ValueError: No such interaction

Engines
=======

A typical simulation loop does the following:

* Reset forces on bodies.
* Detect collisions of bodies, create interactions as necessary.
* Solve interactions, applying forces on bodies.
* Apply other external conditions (gravity, for instance).
* Change position of bodies based on forces.

All these actions are represented by :ctype:`Engine`'s. Such loop can be described as follows in Python (don't be scared by details you don't know yet, they will be explained later); each of the list items is instance of a type deriving from Engine::
 
	O.engines=[
		# reset forces
		BexResetter(),
		# detect collisions
		BoundDispatcher([Bo1_Sphere_Aabb,Bo1_Facet_Aabb]),a
		InsertionSortCollider(),
		# handle interactions
		InteractionDispatchers(
			[Ig2_Sphere_Sphere_Dem3DofGeom(),Ig2_Facet_Sphere_Dem3DofGeom()],
			[Ip2_SimpleElasticRelationships()], #FIXME
			[Law2_Dem3Dof_Elastic_Elastic()],
		),
		# apply other conditions
		GravityEngine(gravity=(0,0,-9.81)),
		# update positions using Newton's equations
		NewtonsDampedLaw()
	]


There are 2 fundamental types of Engines:

:ctype:`GlobalEngine`
	operating on the whole simulation (e.g. GravityEngine looping over all bodies and applying force based on their mass)

:ctype:`Dispatcher`
	invoking various :ctype:`Functor`'s based on types they receive. For instance, ``BoundDispatcher([Bv1_Sphere_AABB])`` creates a ``BoundDispatcher``, which will, based on shape type, use one of its functors to create bound for each body. In this case, it has 2 functors, one for spheres, one for facets; the ``Bo`` functor creating Bound, which is called based on ``1`` type ``Sphere`` it receives, and creates bound of type ``Aabb`` (axis-aligned bounding box).

Dispatchers and functors
------------------------
As said, BoundDispatcher dispatches based on Shape type; for sphere, it will use ``Bo1_Sphere_Aabb``, for facets ``Bo1_Facet_Aabb``.

``InteractionDispatchers`` hides 3 dispatchers, which all operate on interactions.

``InteractionGeometryDispatcher``
	using the first set of functors (``Ig2``) is called based on combination of 2 Shapes and create an appropriate InteractionGeometry associated with the interactions. These functors may fail on potential interactions, indicating there is no contact between the bodies.

	#. The first functor, ``Ig2_Sphere_Sphere_Dem3DofGeom``, is called on interaction of 2 Spheres and creates ``Dem3DofGeom`` instance.

	#. The second functor, ``Ig2_Facet_Sphere_Dem3DofGeom``, is called for interaction of Facet with sphere and creates (again) a ``Dem3DofGeom``.

``InteractionPhysicsDispatcher``
	dispatching to the second set of functors (``Ip2``) based on combination of 2 Materials; they return InteractionPhysics instance. In our case, there is only 1 functor used, ``Ip2_Granular_Granular_Elastic``, which create ElastPhys from 2 GranularMat's.

``LawDispatcher``
	dispatching to the third set of functors (``Law2``), containing various (constitutive) laws. Based on data in InteractionGeometry and InteractionPhysics, these functors compute forces on bodies (repulsion, attraction) or otherwise update interaction variables.	


Controlling the loop
====================

As explained above, the loop consists in running defined sequence of engines. Step number can be queried by ``O.iter`` and advancing by one step is done by ``O.step()``. Every step advances *virtual time* by current timestep, ``O.dt``:

	>>> O.iter
	0
	>>> O.time
	0.0
	>>> O.dt=1e-4
	>>> O.step()
	>>> O.iter
	1
	>>> O.time
	1e-4

Normal simulations, however, are run continuously. Starting/stopping the loop is done by ``O.run()`` and ``O.pause()``; note that ``O.run()`` returns control to Python and the simulation runs in background; if you want to wait for it finish, use ``O.wait()``. Fixed number of steps can be run with ``O.run(1000)``, ``O.run(1000,True)`` will run and wait. To stop at absolute step number, ``O.stopAtIter`` can be set and ``O.run()`` called normally.

	>>> O.run()
	>>> O.pause()
	>>> O.iter
	104587
	>>> O.run(100000,True)
	>>> O.iter
	204587
	>>> O.stopAtIter=500000
	>>> O.wait()
	>>> O.iter
	500000
******************
Input and output
*****************
Yade provides functions for creating, saving and loading simulations.

Creating simulation
===================
To create simulation, one can either use a specialized class of type :ctype:`Generator` to create full scene, possibly receiving some parameters. Generators are written in c++ and their role is limited to well-defined scenarios. For instance, to create triaxial test scene:

	>>> TriaxialTest(numberOfGrains=1000).load()
	>>> len(O.bodies)
	1006

Generators are regular yade objects that support attribute access as explained in FIXME.

It is also possible to construct the scene by a python script; this gives much more flexibility
and speed of development and is the recommended way to create simulation. Yade provides modules
for streamlined body construction, import of geometries from files and reuse of common code.
Since this topic is more involved, it is explained elsewhere FIXME.

Saving and loading
------------------
Simulation can be saved at any point to (optionally compressed) XML file. With some limitations,
it is generally possible to load the XML later and resume the simulation as if it were not
interrupted. Note that since XML is merely readable dump of Yade's internal objects, it might not
(probably will not) open with different Yade version.

	>>> O.save('/tmp/a.xml.bz2')
	>>> O.reload() 
	>>> O.load('/tmp/another.xml.bz2')

The principal use of saving the simulation to XML is to use it as temporary in-memory storage
for checkpoints in simulation, e.g. for reloading the initial state and running again with
different parameters (think tension/compression test, where each begins from the same virgin
state). The functions ``O.saveTmp()`` and ``O.loadTmp()`` can be optionally given a slot name,
under which they will be found in memory:

	>>> O.saveTmp()
	>>> O.loadTmp()
	>>> O.saveTmp('init') ## named memory slot
	>>> O.loadTmp('init')

Simulation can be reset to empty state by ``O.reset()``.

It can be sometimes useful to run different simulation, while the original one is temporarily
suspended, e.g. when dynamically creating packing. ``O.switchWorld()``  toggles between the
primary and secondary simulation.

****************************
Creating simulation
****************************

Yade python introduction (class instantiation)
Mathematical formulation
	interaction geometry
	constitutive laws
	

