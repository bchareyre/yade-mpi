# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#
# I doubt there functions will be useful for anyone besides me.
#
from yade.wrapper import *
from math import *

def estimateStress(strain,cutoff=0.):
	"""Use summed stored energy in contacts to compute macroscopic stress over the same volume, provided known strain."""
	# E=(1/2)σεAl # global stored energy
	# σ=EE/(.5εAl)=EE/(.5εV)
	from yade import utils
	dim=utils.aabbDim(cutoff,centers=False)
	return utils.elasticEnergy(utils.aabbExtrema(cutoff))/(.5*strain*dim[0]*dim[1]*dim[2])

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
	if cutoff>0: cut=utils.fractionalBox(fraction=1-cutoff)
	for axis in [0,1,2]:
		if cutoff>0:
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


def oofemTextExport(fName):
	"""Export simulation data in text format 
	
	The format is line-oriented as follows:
		# 3 lines of material parameters:
		1. E G # elastic
		2. epsCrackOnset relDuctility xiShear transStrainCoeff #tension; epsFr=epsCrackOnset*relDuctility
		3. cohesionT tanPhi # shear
		4. [number of spheres] [number of links]
		5. id x y z r -1/0/1[on negative/no/positive boundary] # spheres
		…
		n. id1 id2 contact_point_x cp_y cp_z A # interactions """
	from yade.wrapper import Omega
	material,bodies,interactions=[],[],[]
	o=Omega()

	f=open(fName,'w') # fail early on I/O problem

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
	
	f.write('\n'.join(material+["%d %d"%(len(bodies),len(interactions))]+bodies+interactions))
	f.close()

def oofemDirectExport(fileBase,title=None,negIds=[],posIds=[]):
	from yade.wrapper import Omega
	material,bodies,interactions=[],[],[]
	o=Omega()
	strainers=[e for e in o.engines if e.name=='UniaxialStrainer']
	if len(strainers)>0:
		strainer=strainers[0]
		posIds,negIds=strainer['posIds'],strainer['negIds']
	else: strainer=None
	f=open(fileBase+'.in','w')
	# header
	f.write(fileBase+'.out\n')
	f.write((title if title else 'Yade simulation for '+fileBase)+'\n')
	f.write("NonLinearStatic nsteps 2 contextOutputStep 1 rtolv 1.e-2 stiffMode 2 maxiter 50 controllmode 1 nmodules 0\n")
	f.write("domain 3dShell\n")
	f.write("OutputManager tstep_all dofman_all element_all\n")
	inters=[i for i in o.interactions if (i.geom and i.phys)]
	f.write("ndofman %d nelem %d ncrosssect 1 nmat 1 nbc 2 nic 0 nltf 1 nbarrier 0\n"%(len(o.bodies)+2,len(inters)))
	# elements
	f.write("Node 1 coords 3 0.0 0.0 0.0 bc 6 1 1 1 1 1 1\n")
	f.write("Node 2 coords 3 0.0 0.0 0.0 bc 6 1 2 1 1 1 1\n")
	for b in o.bodies:
		f.write("Particle %d coords 3 %g %g %g rad %g"%(b.id+3,b.phys['se3'][0],b.phys['se3'][1],b.phys['se3'][2],b.shape['radius']))
		if b.id in negIds: f.write(" dofType 6 1 1 1 1 1 1 masterMask 6 0 1 0 0 0 0 ")
		elif b.id in posIds: f.write(" dofType 6 1 1 1 1 1 1 masterMask 6 0 2 0 0 0 0 0")
		f.write('\n')
	j=1
	for i in inters:
		f.write('CohSur3d %d nodes 2 %d %d mat 1 crossSect 1 area %g\n'%(j,i.id1+3,i.id2+3,i.phys['crossSection']))
		j+=1
	# crosssection
	f.write("SimpleCS 1 thick 1.0 width 1.0\n")
	# material
	ph=inters[0].phys
	f.write("CohInt 1 kn %g ks %g e0 %g ef %g c 0. ksi %g coh %g tanphi %g d 1.0\n"%(ph['E'],ph['G'],ph['epsCrackOnset'],ph['epsFracture'],ph['xiShear'],ph['undamagedCohesion'],ph['tanFrictionAngle']))
	# boundary conditions
	f.write('BoundaryCondition 1 loadTimeFunction 1 prescribedvalue 0.0\n')
	f.write('BoundaryCondition 2 loadTimeFunction 1 prescribedvalue 1.e-4\n')
	f.write('PiecewiseLinFunction 1 npoints 3 t 3 0. 10. 1000.  f(t) 3 0. 10. 1000.\n')


def displacementsInteractionsExport(fName):
	f=open(fName,'w')
	print "Writing body displacements and interaction strains."
	o=Omega()
	for b in o.bodies:
		x0,y0,z0=b.phys['refSe3'][0:3]; x,y,z=b.phys.pos
		rx,ry,rz,rr=b.phys['se3'][3:7]
		f.write('%d xyz [ %g %g %g ] dxyz [ %g %g %g ] rxyz [ %g %g %g ] \n'%(b.id,x0,y0,z0,x-x0,y-y0,z-z0,rr*rx,rr*ry,rr*rz))
	f.write('\n')
	for i in o.interactions:
		if not i['isReal']:continue
		epsTNorm=sqrt(sum([e**2 for e in i.phys['epsT']]))
		epsT='epsT [ %g %g %g ] %g'%(i.phys['epsT'][0],i.phys['epsT'][1],i.phys['epsT'][2],epsTNorm)
		en=i.phys['epsN']; n=i.geom['normal']
		epsN='epsN [ %g %g %g ] %g'%(en*n[0],en*n[1],en*n[2],en)
		Fn='Fn [ %g %g %g ] %g'%(i.phys['normalForce'][0],i.phys['normalForce'][1],i.phys['normalForce'][2],i.phys['Fn'])
		Fs='Fs [ %lf %lf %lf ] %lf'%(i.phys['shearForce'][0],i.phys['shearForce'][1],i.phys['shearForce'][2],sqrt(sum([fs**2 for fs in i.phys['shearForce']])))
		f.write('%d %d %s %s %s %s\n'%(i.id1,i.id2,epsN,epsT,Fn,Fs))
		# f.write('%d %d %g %g %g %g %g\n'%(i.id1,i.id2,i.phys['epsN'],i.phys['epsT'][0],i.phys['epsT'][1],i.phys['epsT'][2],epsTNorm))
	f.close()





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

