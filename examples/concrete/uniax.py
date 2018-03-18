#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import division

from yade import plot,pack,timing
import time, sys, os, copy

#import matplotlib
#matplotlib.rc('text',usetex=True)
#matplotlib.rc('text.latex',preamble=r'\usepackage{concrete}\usepackage{euler}')



"""
A fairly complex script performing uniaxial tension-compression test on hyperboloid-shaped specimen.

Most parameters of the model (and of the setup) can be read from table using yade-multi.

After the simulation setup, tension loading is run and stresses are periodically saved for plotting
as well as checked for getting below the maximum value so far. This indicates failure (see stopIfDamaged
function). After failure in tension, the original setup is loaded anew and the sense of loading reversed.
After failure in compression, strain-stress curves are saved via plot.saveGnuplot and we exit,
giving some useful information like peak stresses in tension/compression.

Running this script for the first time can take long time, as the specimen is prepared using triaxial
compression. Next time, however, an attempt is made to load previously-generated packing 
(from /tmp/triaxPackCache.sqlite) and this expensive procedure is avoided.

The specimen length can be specified, its diameter is half of the length and skirt of the hyperboloid is 
4/5 of the width.

The particle size is constant and can be specified using the sphereRadius parameter.

The 3d display has displacement scaling applied, so that the fracture looks more spectacular. The scale
is 1000 for tension and 100 for compression.

"""



# default parameters or from table
readParamsFromTable(noTableOk=True, # unknownOk=True,
	young=24e9,
	poisson=.2,

	sigmaT=3.5e6,
	frictionAngle=atan(0.8),
	epsCrackOnset=1e-4,
	relDuctility=30,

	intRadius=1.5,
	dtSafety=.8,
	damping=0.4,
	strainRateTension=.05,
	strainRateCompression=.5,
	setSpeeds=True,
	# 1=tension, 2=compression (ANDed; 3=both)
	doModes=3,

	specimenLength=.15,
	sphereRadius=3.5e-3,

	# isotropic confinement (should be negative)
	isoPrestress=0,
)

from yade.params.table import *

if 'description' in O.tags.keys(): O.tags['id']=O.tags['id']+O.tags['description']


# make geom; the dimensions are hard-coded here; could be in param table if desired
# z-oriented hyperboloid, length 20cm, diameter 10cm, skirt 8cm
# using spheres 7mm of diameter
concreteId=O.materials.append(CpmMat(young=young,frictionAngle=frictionAngle,poisson=poisson,density=4800,sigmaT=sigmaT,relDuctility=relDuctility,epsCrackOnset=epsCrackOnset,isoPrestress=isoPrestress))

sps=SpherePack()
sp=pack.randomDensePack(pack.inHyperboloid((0,0,-.5*specimenLength),(0,0,.5*specimenLength),.25*specimenLength,.17*specimenLength),spheresInCell=2000,radius=sphereRadius,memoizeDb='/tmp/triaxPackCache.sqlite',returnSpherePack=True)
#sp=pack.randomDensePack(pack.inAlignedBox((-.25*specimenLength,-.25*specimenLength,-.5*specimenLength),(.25*specimenLength,.25*specimenLength,.5*specimenLength)),spheresInCell=2000,radius=sphereRadius,memoizeDb='/tmp/triaxPackCache.sqlite',returnSpherePack=True)
sp.toSimulation(material=concreteId)
bb=uniaxialTestFeatures()
negIds,posIds,axis,crossSectionArea=bb['negIds'],bb['posIds'],bb['axis'],bb['area']
O.dt=dtSafety*PWaveTimeStep()
print 'Timestep',O.dt

mm,mx=[pt[axis] for pt in aabbExtrema()]
coord_25,coord_50,coord_75=mm+.25*(mx-mm),mm+.5*(mx-mm),mm+.75*(mx-mm)
area_25,area_50,area_75=approxSectionArea(coord_25,axis),approxSectionArea(coord_50,axis),approxSectionArea(coord_75,axis)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=intRadius,label='is2aabb'),],verletDist=.05*sphereRadius),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(interactionDetectionFactor=intRadius,label='ss2sc')],
		[Ip2_CpmMat_CpmMat_CpmPhys()],
		[Law2_ScGeom_CpmPhys_Cpm()],
	),
	NewtonIntegrator(damping=damping,label='damper'),
	CpmStateUpdater(realPeriod=.5),
	UniaxialStrainer(strainRate=strainRateTension,axis=axis,asymmetry=0,posIds=posIds,negIds=negIds,crossSectionArea=crossSectionArea,blockDisplacements=False,blockRotations=False,setSpeeds=setSpeeds,label='strainer'),
	PyRunner(virtPeriod=1e-6/strainRateTension,realPeriod=1,command='addPlotData()',label='plotDataCollector',initRun=True),
	PyRunner(realPeriod=4,command='stopIfDamaged()',label='damageChecker'),
]
#O.miscParams=[Gl1_CpmPhys(dmgLabel=False,colorStrain=False,epsNLabel=False,epsT=False,epsTAxes=False,normal=False,contactLine=True)]

# plot stresses in ¼, ½ and ¾ if desired as well; too crowded in the graph that includes confinement, though
plot.plots={'eps':('sigma',)} #,'sigma.50')},'t':('eps')} #'sigma.25','sigma.50','sigma.75')}

O.saveTmp('initial');

O.timingEnabled=False

global mode
mode='tension' if doModes & 1 else 'compression'

def initTest():
	global mode
	print "init"
	if O.iter>0:
		O.wait();
		O.loadTmp('initial')
		print "Reversing plot data"; plot.reverseData()
	else: plot.plot(subPlots=False)
	strainer.strainRate=abs(strainRateTension) if mode=='tension' else -abs(strainRateCompression)
	try:
		from yade import qt
		renderer=qt.Renderer()
		renderer.dispScale=(1000,1000,1000) if mode=='tension' else (100,100,100)
	except ImportError: pass
	print "init done, will now run."
	O.step(); # to create initial contacts
	# now reset the interaction radius and go ahead
	ss2sc.interactionDetectionFactor=1.
	is2aabb.aabbEnlargeFactor=1.

	O.run()

def stopIfDamaged():
	global mode
	if O.iter<2 or not plot.data.has_key('sigma'): return # do nothing at the very beginning
	sigma,eps=plot.data['sigma'],plot.data['eps']
	extremum=max(sigma) if (strainer.strainRate>0) else min(sigma)
	minMaxRatio=0.5 if mode=='tension' else 0.5
	if extremum==0: return
	import sys;	sys.stdout.flush()
	if abs(sigma[-1]/extremum)<minMaxRatio or abs(strainer.strain)>(5e-3 if isoPrestress==0 else 5e-2):
		if mode=='tension' and doModes & 2: # only if compression is enabled
			mode='compression'
			O.save('/tmp/uniax-tension.yade.gz')
			print "Saved /tmp/uniax-tension.yade.gz (for use with interaction-histogram.py and uniax-post.py)"
			print "Damaged, switching to compression... "; O.pause()
			# important! initTest must be launched in a separate thread;
			# otherwise O.load would wait for the iteration to finish,
			# but it would wait for initTest to return and deadlock would result
			import thread; thread.start_new_thread(initTest,())
			return
		else:
			print "Damaged, stopping."
			ft,fc=max(sigma),min(sigma)
			if doModes==3:
				print 'Strengths fc=%g, ft=%g, |fc/ft|=%g'%(fc,ft,abs(fc/ft))
			if doModes==2:
				print 'Compressive strength fc=%g'%(abs(fc))
			if doModes==1:
				print 'Tensile strength ft=%g'%(abs(ft))
			title=O.tags['description'] if 'description' in O.tags.keys() else O.tags['params']
			print 'gnuplot',plot.saveGnuplot(O.tags['id'],title=title)
			print 'Bye.'
			O.pause()
			#sys.exit(0) # results in some threading exception
		
def addPlotData():
	yade.plot.addData({'t':O.time,'i':O.iter,'eps':strainer.strain,'sigma':strainer.avgStress+isoPrestress,
		'sigma.25':forcesOnCoordPlane(coord_25,axis)[axis]/area_25+isoPrestress,
		'sigma.50':forcesOnCoordPlane(coord_50,axis)[axis]/area_50+isoPrestress,
		'sigma.75':forcesOnCoordPlane(coord_75,axis)[axis]/area_75+isoPrestress,
		})
plot.plot(subPlots=False)
#O.run()
initTest()
waitIfBatch()

