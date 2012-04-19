#!/usr/bin/python
# -*- coding: utf-8 -*-
# © 2009 Václav Šmilauer <eudoxos@arcig.cz>

from yade import pack
import gts, os.path, locale

locale.setlocale(locale.LC_ALL, 'en_US.UTF-8')   #gts is locale-dependend.  If, for example, german locale is used, gts.read()-function does not import floats normally


# coarsen the original horse if we have it
# do nothing if we have the coarsened horse already
if not os.path.exists('horse.coarse.gts'):
	if os.path.exists('horse.gts'):
		surf=gts.read(open('horse.gts')); surf.coarsen(1000); surf.write(open('horse.coarse.gts','w'))
	else:
		print """horse.gts not found, you need to download input data:

		wget http://gts.sourceforge.net/samples/horse.gts.gz
		gunzip horse.gts.gz
		"""
		quit()

surf=gts.read(open('horse.coarse.gts'))

if surf.is_closed():
	pred=pack.inGtsSurface(surf)
	aabb=pred.aabb()
	dim0=aabb[1][0]-aabb[0][0]; radius=dim0/40. # get some characteristic dimension, use it for radius
	O.bodies.append(pack.regularHexa(pred,radius=radius,gap=radius/4.))
	surf.translate(0,0,-(aabb[1][2]-aabb[0][2])) # move surface down so that facets are underneath the falling spheres
O.bodies.append(pack.gtsSurface2Facets(surf,wire=True))

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Facet_Aabb()],label='collider'),
	InteractionLoop(
		[Ig2_Sphere_Sphere_L3Geom(),Ig2_Facet_Sphere_L3Geom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_L3Geom_FrictPhys_ElPerfPl()],
	),
	NewtonIntegrator(damping=.1,gravity=[0,0,-5000]),
	PyRunner(iterPeriod=1000,command='timing.stats(); O.pause();'),
	PyRunner(iterPeriod=10,command='addPlotData()')
]
O.dt=.7*utils.PWaveTimeStep()
O.saveTmp()
O.timingEnabled=True
O.trackEnergy=True
from yade import plot
plot.plots={'i':('total',O.energy.keys,)}
def addPlotData(): plot.addData(i=O.iter,total=O.energy.total(),**O.energy)
plot.plot(subPlots=False)

from yade import timing
from yade import qt
qt.View()
