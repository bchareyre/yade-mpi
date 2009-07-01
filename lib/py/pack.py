# encoding: utf-8
#
import itertools,warnings
from numpy import arange
from math import sqrt
from yade import utils

# for now skip the import, but try in inGtsSurface constructor again, to give error if we really use it
try:
	import gts
except ImportError: pass

# make c++ predicates available in this module
from _packPredicates import *
# import SpherePack
from _packSpheres import *

class inGtsSurface_py(Predicate):
	"""This class was re-implemented in c++, but should stay here to serve as reference for implementing
	Predicates in pure python code. C++ allows us to play dirty tricks in GTS which are not accessible
	through pygts itself; the performance penalty of pygts comes from fact that if constructs and destructs
	bb tree for the surface at every invocation of gts.Point().is_inside(). That is cached in the c++ code,
	provided that the surface is not manipulated with during lifetime of the object (user's responsibility).

	---
	
	Predicate for GTS surfaces. Constructed using an already existing surfaces, which must be closed.

		import gts
		surf=gts.read(open('horse.gts'))
		inGtsSurface(surf)

	Note: padding is optionally supported by testing 6 points along the axes in the pad distance. This
	must be enabled in the ctor by saying doSlowPad=True. If it is not enabled and pad is not zero,
	warning is issued.
	"""
	def __init__(self,surf,noPad=False):
		# call base class ctor; necessary for virtual methods to work as expected.
		# see comments in _packPredicates.cpp for struct PredicateWrap.
		super(inGtsSurface,self).__init__()
		if not surf.is_closed(): raise RuntimeError("Surface for inGtsSurface predicate must be closed.")
		self.surf=surf
		self.noPad=noPad
		inf=float('inf')
		mn,mx=[inf,inf,inf],[-inf,-inf,-inf]
		for v in surf.vertices():
			c=v.coords()
			mn,mx=[min(mn[i],c[i]) for i in 0,1,2],[max(mx[i],c[i]) for i in 0,1,2]
		self.mn,self.mx=tuple(mn),tuple(mx)
		import gts
	def aabb(self): return self.mn,self.mx
	def __call__(self,_pt,pad=0.):
		p=gts.Point(*_pt)
		if self.noPad:
			if pad!=0: warnings.warn("Padding disabled in ctor, using 0 instead.")
			return p.is_inside(self.surf)
		pp=[gts.Point(_pt[0]-pad,_pt[1],_pt[2]),gts.Point(_pt[0]+pad,_pt[1],_pt[2]),gts.Point(_pt[0],_pt[1]-pad,_pt[2]),gts.Point(_pt[0],_pt[1]+pad,_pt[2]),gts.Point(_pt[0],_pt[1],_pt[2]-pad),gts.Point(_pt[0],_pt[1],_pt[2]+pad)]
		return p.is_inside(self.surf) and pp[0].is_inside(self.surf) and pp[1].is_inside(self.surf) and pp[2].is_inside(self.surf) and pp[3].is_inside(self.surf) and pp[4].is_inside(self.surf) and pp[5].is_inside(self.surf)

class inSpace(Predicate):
	"""Predicate returning True for any points, with infinite bounding box."""
	def aabb(self):
		inf=float('inf'); return [-inf,-inf,-inf],[inf,inf,inf]
	def __call__(self,pt): return True

#####
## surface construction and manipulation
#####

def gtsSurface2Facets(surf,**kw):
	"""Construct facets from given GTS surface. **kw is passed to utils.facet."""
	return [utils.facet([v.coords() for v in face.vertices()],**kw) for face in surf]

def sweptPolylines2gtsSurface(pts,threshold=0,capStart=False,capEnd=False):
	"""Create swept suface (as GTS triangulation) given same-length sequences of points (as 3-tuples).
	If threshold is given (>0), gts.Surface().cleanup(threshold) will be called before returning, which
	removes vertices mutually closer than threshold. Can be used to create closed swept surface (revolved), as
	we don't check for coincident vertices otherwise.
	"""
	if not len(set([len(pts1) for pts1 in pts]))==1: raise RuntimeError("Polylines must be all of the same length!")
	vtxs=[[gts.Vertex(x,y,z) for x,y,z in pts1] for pts1 in pts]
	sectEdges=[[gts.Edge(vtx[i],vtx[i+1]) for i in xrange(0,len(vtx)-1)] for vtx in vtxs]
	interSectEdges=[[] for i in range(0,len(vtxs)-1)]
	for i in range(0,len(vtxs)-1):
		for j in range(0,len(vtxs[i])):
			interSectEdges[i].append(gts.Edge(vtxs[i][j],vtxs[i+1][j]))
			if j<len(vtxs[i])-1: interSectEdges[i].append(gts.Edge(vtxs[i][j],vtxs[i+1][j+1]))
	surf=gts.Surface()
	for i in range(0,len(vtxs)-1):
		for j in range(0,len(vtxs[i])-1):
			surf.add(gts.Face(interSectEdges[i][2*j+1],sectEdges[i+1][j],interSectEdges[i][2*j]))
			surf.add(gts.Face(sectEdges[i][j],interSectEdges[i][2*j+2],interSectEdges[i][2*j+1]))
	def doCap(vtx,edg,start):
		ret=[]
		eFan=[edg[0]]+[gts.Edge(vtx[i],vtx[0]) for i in range(2,len(vtx))]
		for i in range(1,len(edg)):
			ret+=[gts.Face(eFan[i-1],eFan[i],edg[i]) if start else gts.Face(eFan[i-1],edg[i],eFan[i])]
		return ret
	caps=[]
	if capStart: caps+=doCap(vtxs[0],sectEdges[0],start=True)
	if capEnd: caps+=doCap(vtxs[-1],sectEdges[-1],start=False)
	for cap in caps: surf.add(cap)
	if threshold>0: surf.cleanup(threshold)
	return surf

import euclid

def revolutionSurfaceMeridians(sects,angles,origin=euclid.Vector3(0,0,0),orientation=euclid.Quaternion()):
	"""Revolution surface given sequences of 2d points and sequence of corresponding angles,
	returning sequences of 3d points representing meridian sections of the revolution surface.
	The 2d sections are turned around z-axis, but they can be transformed
	using the origin and orientation arguments to give arbitrary orientation."""
	import math
	def toGlobal(x,y,z):
		return tuple(origin+orientation*(euclid.Vector3(x,y,z)))
	return [[toGlobal(x2d*math.cos(angles[i]),x2d*math.sin(angles[i]),y2d) for x2d,y2d in sects[i]] for i in range(0,len(sects))]

########
## packing generators
########


def regularOrtho(predicate,radius,gap,**kw):
	"""Return set of spheres in regular orthogonal grid, clipped inside solid given by predicate.
	Created spheres will have given radius and will be separated by gap space."""
	ret=[]
	mn,mx=predicate.aabb()
	if(max([mx[i]-mn[i] for i in 0,1,2])==float('inf')): raise ValueError("AABB of the predicate must not be infinite (didn't you use union | instead of intersection & for unbounded predicate such as notInNotch?");
	xx,yy,zz=[arange(mn[i]+radius,mx[i]-radius,2*radius+gap) for i in 0,1,2]
	for xyz in itertools.product(xx,yy,zz):
		if predicate(xyz,radius): ret+=[utils.sphere(xyz,radius=radius,**kw)]
	return ret

def regularHexa(predicate,radius,gap,**kw):
	"""Return set of spheres in regular hexagonal grid, clipped inside solid given by predicate.
	Created spheres will have given radius and will be separated by gap space."""
	ret=[]
	a=2*radius+gap
	h=a*sqrt(3)/2.
	mn,mx=predicate.aabb()
	dim=[mx[i]-mn[i] for i in 0,1,2]
	if(max(dim)==float('inf')): raise ValueError("AABB of the predicate must not be infinite (didn't you use union | instead of intersection & for unbounded predicate such as notInNotch?");
	ii,jj,kk=[range(0,int(dim[0]/a)+1),range(0,int(dim[1]/h)+1),range(0,int(dim[2]/h)+1)]
	for i,j,k in itertools.product(ii,jj,kk):
		x,y,z=mn[0]+radius+i*a,mn[1]+radius+j*h,mn[2]+radius+k*h
		if j%2==0: x+= a/2. if k%2==0 else -a/2.
		if k%2!=0: x+=a/2.; y+=h/2.
		if predicate((x,y,z),radius): ret+=[utils.sphere((x,y,z),radius=radius,**kw)]
	return ret

def filterSpherePack(predicate,spherePack,**kw):
	"""Using given SpherePack instance, return spheres the satisfy predicate.
	The packing will be recentered to match the predicate and warning is given if the predicate
	is larger than the packing."""
	mn,mx=predicate.aabb()
	dimP,centP=predicate.dim(),predicate.center()
	dimS,centS=spherePack.dim(),spherePack.center()
	if dimP[0]>dimS[0] or dimP[1]>dimS[1] or dimP[2]>dimS[2]: warnings.warn("Packing's dimension (%s) doesn't fully contain dimension of the predicate (%s)."%(dimS,dimP))
	spherePack.translate(tuple([centP[i]-centS[i] for i in 0,1,2]))
	ret=[]
	for s in spherePack:
		if predicate(s[0],s[1]): ret+=[utils.sphere(s[0],radius=s[1],**kw)]
	return ret

def triaxialPack(predicate,radius,dim=None,cropLayers=1,radiusStDev=0.,assumedFinalDensity=.6,memoizeDb=None,**kw):
	"""Generator of triaxial packing, using TriaxialTest. Radius is radius of spheres, radiusStDev is its standard deviation.
	By default, all spheres are of the same radius. cropLayers is how many layer of spheres will be added to the computed
	dimension of the box so that there no (or not so much, at least) boundary effects at the boundaris of the predicate.
	assumedFinalDensity should be OK as it is, it is used to compute necessary number of spheres for the packing.

	The memoizeDb parameter can be passed a file (existent or nonexistent). If the file exists, it will be first looked
	for a suitable packing that was previously saved already (known as memoization). Saved packings will be scaled to
	requested sphere radius; those that are smaller are distcarded as well as those with different radiusStDev. From
	the remaining ones, the one with the least spheres will be loaded and returned. If no suitable packing is found, it
	is generated as usually, but saved into the database for later use.

	O.switchWorld() magic is used to have clean simulation for TriaxialTest without deleting the original simulation.
	This function therefore should never run in parallel with some code accessing your simulation.
	"""
	import sqlite3, os.path, cPickle, time
	from yade import log
	from math import pi
	if not dim: dim=predicate.dim()
	if max(dim)==float('inf'): raise RuntimeError("Infinite predicate and no dimension of packing requested.")
	fullDim=tuple([dim[i]+4*cropLayers*radius for i in 0,1,2])
	if(memoizeDb and os.path.exists(memoizeDb)):
		# find suitable packing and return it directly
		conn=sqlite3.connect(memoizeDb); c=conn.cursor();
		c.execute('select radius,radiusStDev,dimx,dimy,dimz,N,timestamp from packings order by N')
		for row in c:
			R,rDev,X,Y,Z,NN,timestamp=row[0:7]; scale=radius/R
			rDev*=scale; X*=scale; Y*=scale; Z*=scale
			if (radiusStDev==0 and rDev!=0) or (radiusStDev==0 and rDev!=0) or (radiusStDev!=0 and abs((rDev-radiusStDev)/radiusStDev)>1e-2): continue # not suitable, standard deviation differs too much
			if X<fullDim[0] or Y<fullDim[1] or Z<fullDim[2]: continue # not suitable, not large enough
			print "Found suitable packing in database (radius=%g±%g,N=%g,dim=%g×%g×%g,scale=%g), created %s"%(R,rDev,NN,X,Y,Z,scale,time.asctime(time.gmtime(timestamp)))
			c.execute('select pack from packings where timestamp=?',(timestamp,))
			sp=SpherePack(cPickle.loads(str(c.fetchone()[0])))
			sp.scale(scale)
			return filterSpherePack(predicate,sp,**kw)
		print "No suitable packing in database found, running triaxial"
	#if len(O.bodies)!=0 or len(O.engines)!=0: raise RuntimeError("triaxialPack needs empty simulation (no bodies, no engines) to run.")
	V=(4/3)*pi*radius**3; N=assumedFinalDensity*fullDim[0]*fullDim[1]*fullDim[2]/V;
	##
	O.switchWorld()
	##
	TriaxialTest(
		numberOfGrains=int(N),
		radiusMean=radius,
		# this is just size ratio if radiusMean is specified
		# if you comment out the line above, it will be the corner (before compaction) and radiusMean will be set accordingly
		upperCorner=fullDim,
		radiusStdDev=radiusStDev,
		## no need to touch any the following, I think
		noFiles=True,
		lowerCorner=[0,0,0],
		sigmaIsoCompaction=1e7,
		sigmaLateralConfinement=1e3,
		StabilityCriterion=.05,
		strainRate=.2,
		fast=True,
		thickness=-1, # will be set to sphere radius if negative
		maxWallVelocity=.1,
		wallOversizeFactor=1.5,
		autoUnload=True, # unload after isotropic compaction
		autoCompressionActivation=False # stop once unloaded
	).load()
	log.setLevel('TriaxialCompressionEngine',log.WARN)
	O.run(); O.wait()
	sp=SpherePack(); sp.fromSimulation()
	##
	O.switchWorld()
	##
	if(memoizeDb):
		if os.path.exists(memoizeDb):
			conn=sqlite3.connect(memoizeDb)
		else:
			conn=sqlite3.connect(memoizeDb)
			c=conn.cursor()
			c.execute('create table packings (radius real, radiusStDev real, dimx real, dimy real, dimz real, N integer, timestamp real, pack blob)')
		c=conn.cursor()
		packBlob=buffer(cPickle.dumps(sp.toList(),cPickle.HIGHEST_PROTOCOL))
		c.execute('insert into packings values (?,?,?,?,?,?,?,?)',(radius,radiusStDev,fullDim[0],fullDim[1],fullDim[2],len(sp),time.time(),packBlob,))
		c.close()
		conn.commit()
		print "Packing saved to the database",memoizeDb
	return filterSpherePack(predicate,sp,**kw)



