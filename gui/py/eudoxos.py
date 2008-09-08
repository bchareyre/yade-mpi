# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#
# I doubt there functions will be useful for anyone besides me.
#


def plotDirections(mask=0,bins=20):
	"Plot 3 histograms for distribution of interaction directions, in yz,xz and xy planes."
	import pylab
	from yade import utils
	for axis in [0,1,2]:
		d=utils.interactionAnglesHistogram(axis,mask=mask,bins=bins)
		fc=[0,0,0]; fc[axis]=1.
		pylab.subplot(220+axis+1,polar=True);
		bar(d[0],d[1],width=math.pi/(1.2*bins),fc=fc,alpha=.7,label=['yz','xz','xy'][axis])
	pylab.show()

def estimatePoissonYoung(principalAxis,stress=0,plot=False,cutoff=0.):
	"""Estimate Poisson's ration given the "principal" axis of straining.
	For every base direction, homogenized strain is computed
	(slope in linear regression on discrete function particle coordinate →
	→ particle displacement	in the same direction as returned by
	utils.coordsAndDisplacements) and, (if axis '0' is the strained 
	axis) the poisson's ratio is given as -½(ε₁+ε₂)/ε₀.

	Young's modulus is computed as σ/ε₀; if stress σ is not given (default 0),
	the result is 0.

	cutoff, if > 0., will take only smaller part (centered) or the specimen into account
	"""
	dd=[] # storage for linear regression parameters
	import pylab,numpy,stats
	from yade import utils
	if cutoff>0:
		aabb=utils.aabbExtrema(); half=[.5*(aabb[1][i]-aabb[0][i]) for i in [0,1,2]]
		cut=(tuple([aabb[0][i]+cutoff*half[i] for i in [0,1,2]]),tuple([aabb[1][i]-cutoff*half[i] for i in [0,1,2]]))
	for axis in [0,1,2]:
		if cutoff>0:
			#print cutoff,utils.aabbExtrema(),cut
			w,dw=utils.coordsAndDisplacements(axis,AABB=cut)
		else:
			w,dw=utils.coordsAndDisplacements(axis)
		l,ll=stats.linregress(w,dw)[0:2] # use only tangent and section
		dd.append((l,ll,min(w),max(w)))
		if plot: pylab.plot(w,dw,'.',label='xyz'[axis])
	if plot:
		for axis in [0,1,2]:
			dist=dd[axis][-1]-dd[axis][-2]
			c=numpy.linspace(dd[axis][-2]-.2*dist,dd[axis][-1]+.2*dist)
			d=[dd[axis][0]*cc+dd[axis][1] for cc in c]
			pylab.plot(c,d,label='interp '+'xyz'[axis])
		pylab.legend()
		pylab.show()
	otherAxes=(principalAxis+1)%3,(principalAxis+2)%3
	avgTransHomogenizedStrain=.5*(dd[otherAxes[0]][0]+dd[otherAxes[1]][0])
	principalHomogenizedStrain=dd[principalAxis][0]
	return -avgTransHomogenizedStrain/principalHomogenizedStrain,stress/principalHomogenizedStrain


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
	material.append("%g %g %g %g"%(ph['epsCrackOnset'],ph['epsFracture'],1e50,ph['xiShear']))
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



def eliminateJumps(eps,sigma,numSteep=10,gapWidth=5,movWd=40):
	from matplotlib.mlab import movavg
	from numpy import diff,abs
	import numpy
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

