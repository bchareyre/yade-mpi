#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import division

from yade import utils,plot,pack
import time, sys, os, copy

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
#utils.readParamsFromTable(noTableOk=True, # unknownOk=True,
young=24e9
poisson=.2
G_over_E=.20
sigmaT=3.5e6
frictionAngle=atan(0.8)
epsCrackOnset=1e-4
crackOpening=1e-6

intRadius=1.5
dtSafety=.4
damping=0.2
strainRateTension=10
strainRateCompression=50
# 1=tension, 2=compression (ANDed; 3=both)
doModes=3
biaxial=True

# isotropic confinement (should be negative)
isoPrestress=0
#)

if 'description' in O.tags.keys(): O.tags['id']=O.tags['id']+O.tags['description']

packingFile='periCube.pickle'
# got periodic packing? Memoization not (yet) supported, so just generate it if there is not the right file
# Save and reuse next time.
if not os.path.exists(packingFile):
	sp=pack.randomPeriPack(radius=.05e-3,rRelFuzz=0.,initSize=Vector3(1.5e-3,1.5e-3,1.5e-3))
	dd=dict(cell=(sp.cellSize[0],sp.cellSize[1],sp.cellSize[2]),spheres=sp.toList())
	import cPickle as pickle
	pickle.dump(dd,open(packingFile,'w'))
#
# load the packing (again);
#
import cPickle as pickle
concreteId=O.materials.append(CpmMat(young=young, frictionAngle=frictionAngle, poisson=poisson, density=4800, sigmaT=sigmaT, crackOpening=crackOpening, epsCrackOnset=epsCrackOnset, G_over_E=G_over_E, isoPrestress=isoPrestress))
sphDict=pickle.load(open(packingFile))
from yade import pack
sp=pack.SpherePack()
sp.fromList(sphDict['spheres'])
sp.cellSize=sphDict['cell']

import numpy
avgRadius=numpy.average([r for c,r in sp])
O.bodies.append([utils.sphere(c,r,color=utils.randomColor()) for c,r in sp])
O.periodic=True
#O.cell.setBox=sp.cellSize	#doesnt work correctly, periodic cell is too big!!!!
O.cell.refSize=sp.cellSize
axis=2
ax1=(axis+1)%3
ax2=(axis+2)%3
O.dt=dtSafety*utils.PWaveTimeStep()

import yade.plot as yp

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(aabbEnlargeFactor=intRadius,label='is2aabb'),]),
	#,sweepLength=.05*avgRadius,nBins=5,binCoeff=5),
	InteractionLoop(
		[Ig2_Sphere_Sphere_Dem3DofGeom(distFactor=intRadius,label='ss2d3dg')],
		[Ip2_CpmMat_CpmMat_CpmPhys()],
		[Law2_Dem3DofGeom_CpmPhys_Cpm()],
	),
	NewtonIntegrator(damping=damping,label='newton'),
	CpmStateUpdater(realPeriod=1,label='updater'),
	#
	#UniaxialStrainer(strainRate=strainRateTension,axis=axis,asymmetry=0,posIds=posIds,negIds=negIds,crossSectionArea=crossSectionArea,blockDisplacements=False,blockRotations=False,setSpeeds=setSpeeds,label='strainer'),
	#
	PeriTriaxController(goal=[1,1,1],stressMask=( (7^(1<<axis | 1<<ax1)) if biaxial else (7^(1<<axis)) ),label='strainer',reversedForces=False,globUpdate=2),
	PyRunner(virtPeriod=1e-5/strainRateTension,command='addPlotData()',label='plotDataCollector'),
	PyRunner(realPeriod=4,command='stopIfDamaged()',label='damageChecker'),
]
#O.miscParams=[Gl1_CpmPhys(dmgLabel=False,colorStrain=False,epsNLabel=False,epsT=False,epsTAxes=False,normal=False,contactLine=True)]

# plot stresses in ¼, ½ and ¾ if desired as well; too crowded in the graph that includes confinement, though
plot.plots={'eps':('sigma','sig1','sig2','|||','eps','eps1','eps2'),'eps_':('sigma','|||','relResid',),} #'sigma.25','sigma.50','sigma.75')}
plot.maxDataLen=4000

O.saveTmp('initial');

global mode
mode='tension' if doModes & 1 else 'compression'

def initTest():
	global mode
	print "init"
	if O.iter>0:
		O.wait();
		O.loadTmp('initial')
		print "Reversing plot data"; plot.reverseData()
	maxStrainRate=Vector3(1,1,1);
	goal=Vector3(1,1,1);
	if not biaxial: # uniaxial
		maxStrainRate[axis]=abs(strainRateTension) if mode=='tension' else abs(strainRateCompression)
		maxStrainRate[ax1]=maxStrainRate[ax2]=1000*maxStrainRate[axis]
		goal[axis]=1 if mode=='tension' else -1;
	else:
		maxStrainRate[axis]=abs(strainRateTension) if mode=='tension' else abs(strainRateCompression)
		maxStrainRate[ax1]=maxStrainRate[axis]
		maxStrainRate[ax2]=1000*maxStrainRate[axis]
		goal[axis]=1 if mode=='tension' else -1;
		goal[ax1]=goal[axis]
	strainer.maxStrainRate=maxStrainRate
	strainer.goal=goal
	try:
		from yade import qt
		renderer=qt.Renderer()
		renderer.scaleDisplacements=True
		renderer.displacementScale=(1000,1000,1000) if mode=='tension' else (100,100,100)
	except ImportError: pass
	print "init done, will now run."
	O.step(); O.step(); # to create initial contacts
	# now reset the interaction radius and go ahead
	ss2d3dg.distFactor=-1.
	is2aabb.aabbEnlargeFactor=-1.
	O.run()

def stopIfDamaged():
	global mode
	if O.iter<2 or not plot.data.has_key('sigma'): return # do nothing at the very beginning
	sigma,eps=plot.data['sigma'],plot.data['eps']
	extremum=max(sigma) if (strainer.maxStrainRate>0) else min(sigma)
	# FIXME: only temporary, should be .5
	minMaxRatio=0.5 if mode=='tension' else 0.5
	if extremum==0: return
	print O.tags['id'],mode,strainer.strain[axis],sigma[-1]
	#print 'strain',strainer['strain'],'stress',strainer['stress']
	import sys;	sys.stdout.flush()
	if abs(sigma[-1]/extremum)<minMaxRatio or abs(strainer.strain[axis])>6e-3:
		if mode=='tension' and doModes & 2: # only if compression is enabled
			mode='compression'
			#O.save('/tmp/uniax-tension.xml.bz2')
			print "Damaged, switching to compression... "; O.pause()
			# important! initTest must be launched in a separate thread;
			# otherwise O.load would wait for the iteration to finish,
			# but it would wait for initTest to return and deadlock would result
			import thread; thread.start_new_thread(initTest,())
			return
		else:
			print "Damaged, stopping."
			ft,fc=max(sigma),min(sigma)
			print 'Strengths fc=%g, ft=%g, |fc/ft|=%g'%(fc,ft,abs(fc/ft))
			title=O.tags['description'] if 'description' in O.tags.keys() else O.tags['params']
			print'gnuplot',plot.saveGnuplot(O.tags['id'],title=title)
			print 'Bye.'
			# O.pause()
			sys.exit(0)
		
def addPlotData():
	yade.plot.addData(t=O.time,i=O.iter,eps=strainer.strain[axis],eps_=strainer.strain[axis],sigma=strainer.stress[axis]+isoPrestress,eps1=strainer.strain[ax1],eps2=strainer.strain[ax2],sig1=strainer.stress[ax1],sig2=strainer.stress[ax2],relResid=updater.avgRelResidual)

initTest()
utils.waitIfBatch()

