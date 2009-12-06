Getting started
===============

.. ipython::
	:suppress:

	In [1]: from yade import *

	In [7]: O.reset()


Starting yade
-------------

TODO: command line / gui


Creating simulation
--------------------
To create simulation, one can either use a specialized class of type :ctype:`Generator` to create full scene, possibly receiving some parameters. Generators are written in c++ and their role is limited to well-defined scenarios. For instance, to create triaxial test scene:

.. ipython::

	In [1]: TriaxialTest(numberOfGrains=200).load()

	In [2]: len(O.bodies)
	1006

Generators are regular yade objects that support attribute access as explained in FIXME.

It is also possible to construct the scene by a python script; this gives much more flexibility
and speed of development and is the recommended way to create simulation. Yade provides modules
for streamlined body construction, import of geometries from files and reuse of common code.
Since this topic is more involved, it is explained elsewhere FIXME.

.. ipython::
	:suppress:

	In [7]: O.reset()


Running simulation
------------------

As explained above, the loop consists in running defined sequence of engines. Step number can be queried by ``O.iter`` and advancing by one step is done by ``O.step()``. Every step advances *virtual time* by current timestep, ``O.dt``:

.. ipython::

	In [1]: O.iter

	In [1]: O.time

	In [1]: O.dt=1e-4

	In [1]: O.step()

	In [1]: O.iter
	1

	In [1]: O.time
	1e-4

Normal simulations, however, are run continuously. Starting/stopping the loop is done by ``O.run()`` and ``O.pause()``; note that ``O.run()`` returns control to Python and the simulation runs in background; if you want to wait for it finish, use ``O.wait()``. Fixed number of steps can be run with ``O.run(1000)``, ``O.run(1000,True)`` will run and wait. To stop at absolute step number, ``O.stopAtIter`` can be set and ``O.run()`` called normally.

.. ipython::

	In [1]: O.run()

	In [1]: O.pause()

	In [1]: O.iter
	104587

	In [1]: O.run(100000,True)

	In [1]: O.iter
	204587

	In [1]: O.stopAtIter=500000

	In [1]: O.wait()

	In [1]: O.iter
	500000


Saving and loading
------------------
Simulation can be saved at any point to (optionally compressed) XML file. With some limitations,
it is generally possible to load the XML later and resume the simulation as if it were not
interrupted. Note that since XML is merely readable dump of Yade's internal objects, it might not
(probably will not) open with different Yade version.

.. ipython::

	In [1]: O.save('/tmp/a.xml.bz2')

	In [2]: O.reload() 

	@suppress
	In [4]: O.save('/tmp/another.xml.bz2')

	In [3]: O.load('/tmp/another.xml.bz2')

The principal use of saving the simulation to XML is to use it as temporary in-memory storage
for checkpoints in simulation, e.g. for reloading the initial state and running again with
different parameters (think tension/compression test, where each begins from the same virgin
state). The functions ``O.saveTmp()`` and ``O.loadTmp()`` can be optionally given a slot name,
under which they will be found in memory:

.. ipython::

	In [1]: O.saveTmp()

	In [1]: O.loadTmp()

	In [1]: O.saveTmp('init') ## named memory slot

	In [1]: O.loadTmp('init')

Simulation can be reset to empty state by ``O.reset()``.

It can be sometimes useful to run different simulation, while the original one is temporarily
suspended, e.g. when dynamically creating packing. ``O.switchWorld()``  toggles between the
primary and secondary simulation.


