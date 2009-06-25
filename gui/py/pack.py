
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


class inGtsSurface(Predicate):
	"""Predicate for GTS surfaces. Constructed using an already existing surfaces, which must be closed.

		import gts
		surf=gts.read(open('horse.gts'))
		inGtsSurface(surf)

	Note: padding is not supported, warning is given and zero used instead.
	"""
	def __init__(self,surf):
		import gts
		if not surf.is_closed(): raise RuntimeError("Surface for inGtsSurface predicate must be closed.")
		self.surf=surf
		inf=float('inf')
		mn,mx=[inf,inf,inf],[-inf,-inf,-inf]
		for v in surf.vertices():
			c=v.coords()
			mn,mx=[min(mn[i],c[i]) for i in 0,1,2],[max(mx[i],c[i]) for i in 0,1,2]
		self.mn,self.mx=tuple(mn),tuple(mx)
	def aabb(self): return self.mn,self.mx
	def __call__(self,_pt,pad=0.):
		p=gts.Point(*_pt)
		if(pad!=0): warnings.warn("Pad distance not supported for GTS surfaces, using 0 instead.")
		return p.is_inside(self.surf)

def gtsSurface2Facets(surf,**kw):
	"""Construct facets from given GTS surface. **kw is passed to utils.facet."""
	return [utils.facet([v.coords() for v in face.vertices()],**kw) for face in surf]



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

