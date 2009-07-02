# encoding: utf-8
# © 2009 Vincent Richefeu <Vincent.Richefeu@hmg.inpg.fr>
"""Simple example to show the SpherePadder usage in the built-in python interpreter of yade.
The sample is show with qt.View() but no computation can be done."""

# For the moment, the module packing (ie. file packing.so) 
# must copyed manually in the folder gui (of the dynamic libs tree)
import packing

mesh = packing.TetraMesh()
padder = packing.SpherePadder()

mesh.read_gmsh('bench_1000.msh')
padder.plugTetraMesh(mesh)

padder.setRadiusRatio(4.0,0.125)
padder.setMaxOverlapRate(1.0e-4)
padder.setVirtualRadiusFactor(100.0)

padder.pad_5()
padder.place_virtual_spheres()
padder.setMaxNumberOfSpheres(8500)
padder.densify()
padder.save_mgpost("mgp.out.001")

from yade import utils
o=Omega()

lst = packing.getSphereList(padder)
for Sph in lst:
  o.bodies.append(utils.sphere(center=[Sph[0],Sph[1],Sph[2]],radius=Sph[3]))

o.bodies.append(utils.box(center=[0.0,-0.1,0.0],extents=[3.0,0.05,3.0]))

from yade import qt
qt.View()

