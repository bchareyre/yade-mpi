# encoding: utf-8
# © 2009 Vincent Richefeu <Vincent.Richefeu@hmg.inpg.fr>
"""Simple example to show the SpherePadder usage in the built-in python interpreter of yade.
The sample is show with qt.View() but no computation can be done."""

from yade import pack
from yade import config

padder=pack.SpherePadder('SpherePadder-test.msh')
padder.setRadiusRatio(4.0,0.125)
padder.maxOverlapRate=1.0e-4
padder.virtualRadiusFactor=100.0

padder.pad_5()
padder.place_virtual_spheres()
padder.maxNumberOfSpheres=8500

# would otherwise raise RuntimeError
if 'CGAL' in config.features:
	padder.densify()

#padder.save_mgpost("mgp.out.001")
sp=padder.asSpherePack()
O.bodies.append([utils.sphere(s[0],s[1]) for s in sp])
from yade import qt
qt.View()

