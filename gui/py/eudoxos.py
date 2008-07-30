# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#
# I doubt there functions will be useful for anyone besides me.
#


def oofemTextExport():
	"""Export simulation data in text format 
	
	The format is line-oriented as follows:
		# 3 lines of material parameters:
		1. E G # elastic
		2. epsCrackOnset epsFr expBending xiShear #tension
		3. cohesionT tanPhi # shear
		4. [number of spheres] [number of links]
		5. id x y z r -1/0/1[on negative/no/positive boundary] # spheres
		…
		n. id1 id2 contact_point_x cp_y cp_z A # interactions """
	from yade.wrapper import Omega
	material,bodies,interactions=[],[],[]
	o=Omega()

	ph=o.interactions.nth(0).phys # some params are the same everywhere
	material.append("%g %g"%(ph['E'],ph['G']))
	material.append("%g %g %g %g"%(ph['epsCrackOnset'],ph['epsFracture'],ph['expBending'],ph['xiShear']))
	material.append("%g %g"%(ph['undamagedCohesion'],ph['tanFrictionAngle']))

	# need strainer for getting bodies in positive/negative boundary
	strainers=[e for e in o.engines if e.name=='UniaxialStrainer']
	if len(strainers)>0: strainer=strainers[0]
	else: strainer=None

	for b in o.bodies:
		if strainer and b.id in strainer['negIds']: boundary=-1
		elif strainer and b.id in strainer['posIds']: boundary=1
		else: boundary=0
		bodies.append("%d %g %g %g %g %d"%(b.id,b.phys['se3'][0],b.phys['se3'][1],b.phys['se3'][2],b.shape['radius'],boundary))

	for i in o.interactions:
		if not i.geom or not i.phys: continue
		cp=i.geom['contactPoint']
		interactions.append("%d %d %g %g %g %g"%(i.id1,i.id2,cp[0],cp[1],cp[2],i.phys['crossSection']))

	return material+["%d %d"%(len(bodies),len(interactions))]+bodies+interactions


import numpy
from pylab import *
from matplotlib.mlab import *

def eliminateJumps(eps,sigma,numSteep=10,gapWidth=5,movWd=40):
	# get histogram of 'derivatives'
	ds=abs(diff(sigma))
	n,bins=numpy.histogram(ds)
	i=1; sum=0
	# numSteep steepest pieces will be discarded
	while sum<numSteep:
		#print n[-i],bins[-i]
		sum+=n[-i]; i+=1
	#print n[-i],bins[-i]
	threshold=bins[-i]
	# old algo: replace with nan's
	##rEps,rSigma=eps[:],sigma[:]; nan=float('nan')
	##indices=where(ds>threshold)[0]
	##for i in indices:
	##	for ii in range(max(0,i-gapWidth),min(len(rEps),i+gapWidth+1)): rEps[ii]=rSigma[ii]=nan

	## doesn't work with older numpy (?)
	# indices1=where(ds>threshold)[0]
	indices1=[]
	for i in range(len(ds)):
		if ds[i]>threshold: indices1.append(i)
	indices=[]
	for i in indices1:
		for ii in range(i-gapWidth,i+gapWidth+1): indices.append(ii)
	#print indices1, indices
	rEps=[eps[i] for i in range(0,len(eps)) if i not in indices]
	rSigma=[sigma[i] for i in range(0,len(sigma)) if i not in indices]
	# apply moving average to the result
	rSigma=movavg(rSigma,movWd)
	rEps=rEps[movWd/2:-movWd/2+1]
	return rEps,rSigma.flatten().tolist()

