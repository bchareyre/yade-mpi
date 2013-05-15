# encoding: utf-8
# 2008 © Václav Šmilauer <eudoxos@arcig.cz>
#
# I doubt there functions will be useful for anyone besides me.
#
"""Miscillaneous functions that are not believed to be generally usable,
therefore kept in my "private" module here.

They comprise notably oofem export and various CPM-related functions.
"""

from yade.wrapper import *
from math import *
from yade._eudoxos import * ## c++ implementations


class IntrSmooth3d():
	r"""Return spatially weigted gaussian average of arbitrary quantity defined on interactions.

	At construction time, all real interactions are put inside spatial grid, permitting fast search for
	points in neighbourhood defined by distance.

	Parameters for the distribution are standard deviation :math:`\sigma` and relative cutoff distance
	*relThreshold* (3 by default) which will discard points farther than *relThreshold* :math:`\times \sigma`.

	Given central point :math:`p_0`, points are weighted by gaussian function

	.. math::

		\rho(p_0,p)=\frac{1}{\sigma\sqrt{2\pi}}\exp\left(\frac{-||p_0-p||^2}{2\sigma^2}\right)

	To get the averaged value, simply call the instance, passing central point and callable object which received interaction object and returns the desired quantity:

		>>> O.reset()
		>>> from yade import utils
		>>> O.bodies.append([utils.sphere((0,0,0),1),utils.sphere((0,0,1.9),1)])
		[0, 1]
		>>> O.engines=[InteractionLoop([Ig2_Sphere_Sphere_ScGeom(),],[Ip2_FrictMat_FrictMat_FrictPhys()],[])]
		>>> utils.createInteraction(0,1) #doctest: +ELLIPSIS
		<Interaction instance at 0x...>

		>> is3d=IntrSmooth3d(0.003)
		>> is3d((0,0,0),lambda i: i.phys.normalForce)
		Vector3(0,0,0)
	
	"""
	def __init__(self,stDev):
		self.locator=InteractionLocator()
		self.stDev=stDev
		self.relThreshold=3.
		self.sqrt2pi=sqrt(2*pi)
		import yade.config
		if not 'vtk' in yade.config.features: raise RuntimeError("IntrSmooth3d is only function with VTK-enabled builds.")
	def _ptpt2weight(self,pt0,pt1):
		distSq=(pt0-pt1).SquaredLength()
		return (1./(self.stDev*self.sqrt2pi))*exp(-distSq/(2*self.stDev*self.stDev))
	def bounds(self): return self.locator.bounds()
	def count(self): return self.locator.count()
	def __call__(self,pt,extr):
		intrs=self.locator.intrsAroundPt(pt,self.stDev*self.relThreshold)
		if len(intrs)==0: return float('nan')
		weights,val=0.,0.
		for i in intrs:
			weight=self._ptpt2weight(pt,i.geom.contactPoint)
			val+=weight*extr(i)
			weights+=weight
			#print i,weight,extr(i)
		return val/weights
		

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
	import pylab,numpy
	try:
		import stats
	except ImportError:
		raise ImportError("Unable to import stats; install the python-stats package.")
	from yade import utils
	if cutoff>0: cut=utils.fractionalBox(fraction=1-cutoff)
	for axis in [0,1,2]:
		if cutoff>0:
			w,dw=utils.coordsAndDisplacements(axis,Aabb=cut)
		else:
			w,dw=utils.coordsAndDisplacements(axis)
		l,ll=stats.linregress(w,dw)[0:2] # use only tangent and section
		dd.append((l,ll,min(w),max(w)))
		if plot: pylab.plot(w,dw,'.',label=r'$\Delta %s(%s)$'%('xyz'[axis],'xyz'[axis]))
	if plot:
		for axis in [0,1,2]:
			dist=dd[axis][-1]-dd[axis][-2]
			c=numpy.linspace(dd[axis][-2]-.2*dist,dd[axis][-1]+.2*dist)
			d=[dd[axis][0]*cc+dd[axis][1] for cc in c]
			pylab.plot(c,d,label=r'$\widehat{\Delta %s}(%s)$'%('xyz'[axis],'xyz'[axis]))
		pylab.legend(loc='upper left')
		pylab.xlabel(r'$x,\;y,\;z$')
		pylab.ylabel(r'$\Delta x,\;\Delta y,\; \Delta z$')
		pylab.show()
	otherAxes=(principalAxis+1)%3,(principalAxis+2)%3
	avgTransHomogenizedStrain=.5*(dd[otherAxes[0]][0]+dd[otherAxes[1]][0])
	principalHomogenizedStrain=dd[principalAxis][0]
	return -avgTransHomogenizedStrain/principalHomogenizedStrain,stress/principalHomogenizedStrain


def oofemTextExport(fName):
	"""Export simulation data in text format 
	
	The format is line-oriented as follows::

		E G                                                 # elastic material parameters
		epsCrackOnset relDuctility xiShear transStrainCoeff # tensile parameters; epsFr=epsCrackOnset*relDuctility
		cohesionT tanPhi                                    # shear parameters
		number_of_spheres number_of_links
		id x y z r boundary                                 # spheres; boundary: -1 negative, 0 none, 1 positive
		…
		id1 id2 cp_x cp_y cp_z A                            # interactions; cp = contact point; A = cross-section
		
	"""
	material,bodies,interactions=[],[],[]

	f=open(fName,'w') # fail early on I/O problem

	ph=O.interactions.nth(0).phys # some params are the same everywhere
	material.append("%g %g"%(ph.E,ph.G))
	material.append("%g %g %g %g"%(ph.epsCrackOnset,ph.epsFracture,1e50,0.0))
	material.append("%g %g"%(ph.undamagedCohesion,ph.tanFrictionAngle))

	# need strainer for getting bodies in positive/negative boundary
	strainers=[e for e in O.engines if e.name=='UniaxialStrainer']
	if len(strainers)>0: strainer=strainers[0]
	else: strainer=None

	for b in O.bodies:
		if not b.shape or b.shape.name!='Sphere': continue
		if strainer and b.id in strainer.negIds: boundary=-1
		elif strainer and b.id in strainer.posIds: boundary=1
		else: boundary=0
		bodies.append("%d %g %g %g %g %d"%(b.id,b.state.pos[0],b.state.pos[1],b.state.pos[2],b.shape.radius,boundary))

	for i in O.interactions:
		cp=i.geom.contactPoint
		interactions.append("%d %d %g %g %g %g"%(i.id1,i.id2,cp[0],cp[1],cp[2],i.phys.crossSection))
	
	f.write('\n'.join(material+["%d %d"%(len(bodies),len(interactions))]+bodies+interactions))
	f.close()

def oofemPrescribedDisplacementsExport(fileName):
	f=open(fileName,'w')
	f.write(fileName+'.out\n'+'''All Yade displacements prescribed as boundary conditions
NonLinearStatic nsteps 2 contextOutputStep 1 rtolv 1.e-2 stiffMode 2 maxiter 50 controllmode 1 nmodules 0
domain 3dshell
OutputManager tstep_all dofman_all element_all
''')
	inters=[i for i in O.interactions if (i.geom and i.phys)]
	f.write("ndofman %d nelem %d ncrosssect 1 nmat 1 nbc %d nic 0 nltf 1 nbarrier 0\n"%(len(O.bodies),len(inters),len(O.bodies)*6))
	bcMax=0; bcMap={}
	for b in O.bodies:
		mf=' '.join([str(a) for a in list(O.actions.f(b.id))+list(O.actions.m(b.id))])
		f.write("## #%d: forces %s\n"%(b.id+1,mf))
		f.write("Particle %d coords 3 %.15e %.15e %.15e rad %g"%(b.id+1,b.phys['se3'][0],b.phys['se3'][1],b.phys['se3'][2],b.mold['radius']))
		bcMap[b.id]=tuple([bcMax+i for i in [1,2,3,4,5,6]])
		bcMax+=6
		f.write(' bc '+' '.join([str(i) for i in bcMap[b.id]])+'\n')
	for j,i in enumerate(inters):
		epsTNorm=sqrt(sum([e**2 for e in i.phys['epsT']]))
		epsT='epsT [ %g %g %g ] %g'%(i.phys['epsT'][0],i.phys['epsT'][1],i.phys['epsT'][2],epsTNorm)
		en=i.phys['epsN']; n=i.geom['normal']
		epsN='epsN [ %g %g %g ] %g'%(en*n[0],en*n[1],en*n[2],en)
		Fn='Fn [ %g %g %g ] %g'%(i.phys['normalForce'][0],i.phys['normalForce'][1],i.phys['normalForce'][2],i.phys['Fn'])
		Fs='Fs [ %lf %lf %lf ] %lf'%(i.phys['shearForce'][0],i.phys['shearForce'][1],i.phys['shearForce'][2],sqrt(sum([fs**2 for fs in i.phys['shearForce']])))
		f.write('## #%d #%d: %s %s %s %s\n'%(i.id1+1,i.id2+1,epsN,epsT,Fn,Fs))
		f.write('CohSur3d %d nodes 2 %d %d mat 1 crossSect 1 area %g\n'%(j+1,i.id1+1,i.id2+1,i.phys['crossSection']))
	# crosssection
	f.write("SimpleCS 1 thick 1.0 width 1.0\n")
	# material
	ph=inters[0].phys
	f.write("CohInt 1 kn %g ks %g e0 %g ef %g c 0. ksi %g coh %g tanphi %g d 1.0 conf 0.0 maxdist 0.0\n"%(ph['E'],ph['G'],ph['epsCrackOnset'],ph['epsFracture'],ph['xiShear'],ph['undamagedCohesion'],ph['tanFrictionAngle']))
	# boundary conditions
	for b in O.bodies:
		displ=b.phys.displ; rot=b.phys.rot
		dofs=[displ[0],displ[1],displ[2],rot[0],rot[1],rot[2]]
		f.write('# particle %d\n'%b.id)
		for dof in range(6):
			f.write('BoundaryCondition %d loadTimeFunction 1 prescribedvalue %.15e\n'%(bcMap[b.id][dof],dofs[dof]))
	#f.write('PiecewiseLinFunction 1 npoints 3 t 3 0. 10. 1000.  f(t) 3 0. 10. 1000.\n')
	f.write('ConstantFunction 1 f(t) 1.0\n')




def oofemDirectExport(fileBase,title=None,negIds=[],posIds=[]):
	from yade.wrapper import Omega
	material,bodies,interactions=[],[],[]
	o=Omega()
	strainers=[e for e in o.engines if e.name=='UniaxialStrainer']
	if len(strainers)>0:
		strainer=strainers[0]
		posIds,negIds=strainer.posIds,strainer.negIds
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
		f.write("Particle %d coords 3 %g %g %g rad %g"%(b.id+3,b.state.refPos[0],b.state.refPos[1],b.state.refPos[2],b.shape.radius))
		if b.id in negIds: f.write(" dofType 6 1 1 1 1 1 1 masterMask 6 0 1 0 0 0 0 ")
		elif b.id in posIds: f.write(" dofType 6 1 1 1 1 1 1 masterMask 6 0 2 0 0 0 0 0")
		f.write('\n')
	j=1
	for i in inters:
		f.write('CohSur3d %d nodes 2 %d %d mat 1 crossSect 1 area %g\n'%(j,i.id1+3,i.id2+3,i.phys.crossSection))
		j+=1
	# crosssection
	f.write("SimpleCS 1 thick 1.0 width 1.0\n")
	# material
	ph=inters[0].phys
	f.write("CohInt 1 kn %g ks %g e0 %g ef %g c 0. ksi %g coh %g tanphi %g damchartime %g damrateexp %g plchartime %g plrateexp %g d 1.0\n"%(ph.E,ph.G,ph.epsCrackOnset,ph.epsFracture,0.0,ph.undamagedCohesion,ph.tanFrictionAngle,ph.dmgTau,ph.dmgRateExp,ph.plTau,ph.plRateExp))
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

