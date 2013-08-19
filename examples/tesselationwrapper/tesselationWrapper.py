#!/usr/bin/python
# -*- coding: utf-8 -*-
#2012 Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>
"""Example usage of a TesselationWrapper object for getting microscale quantities."""
# See Catalano2013a for the definition of micro-strain
# (http://dx.doi.org/10.1002/nag.2198 or free-access at arxiv http://arxiv.org/pdf/1304.4895.pdf)

tt=TriaxialTest()
tt.generate("test.yade")
O.load("test.yade")
O.run(100,True)
TW=TesselationWrapper()
TW.triangulate()	#compute regular Delaunay triangulation, don’t construct tesselation
TW.computeVolumes()	#will silently tesselate the packing, then compute volume of each Voronoi cell
TW.volume(10)		#get volume associated to sphere of id 10
TW.setState(0)		#store current positions internaly for later use as the "0" state
O.run(100,True)		#make particles move a little (let's hope they will!)
TW.setState(1)		#store current positions internaly in the "1" (deformed) state

#Now we can define strain by comparing states 0 and 1, and average them at the particles scale
TW.defToVtk("strain.vtk")