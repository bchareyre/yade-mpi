#!/usr/bin/env python
# (c) 2007 Vaclav Smilauer <eudoxos@arcig.cz> 

from Scientific.Geometry import Vector
from Scientific.Geometry.Quaternion import Quaternion
Vector.__dict__['__str__']=lambda self: "%g %g %g"%(self[0],self[1],self[2])
Quaternion.__dict__['__str__']=lambda self: "%g %g %g %g"%(self[0],self[1],self[2],self[3])

try: 
	import psyco; psyco.full()
except ImportError: pass

if __name__=='__main__': import pyadeDummy as _pyade # testing, without yade running
else:	import _pyade
# convenience for users
from math import *

"""
In order to make vector arithmetics possible, this module imports
Scientific.Geometry.{Quaternion,Vector}. Debian package with these modules is
called `python-scientific'. We overrider Vector's and Quaternion's __str__
method so that they are printed as numbers with spaces (suitable for gnuplot,
for example).

Future things to do (maybe):
 * use serialization attributes to access values really; something like Body.attr(name), with  Body.__getitem__(name) as shorthand.
 * implement history:
 	attribute will have kw t (time) and i (iteration); for non-negative values, they are absolute; for negative, relative.
 	class History:
 * Find out how to make B[5].I equivalent to B[5].I(), so that we can also say B[5].I(t=-1)
		If not possible, use __call__ and mandate B[5].I(). Or use B[5].attr('I',dt=...) since it will be used quite rarely
"""


class Bodies:
	"""This class accesses individual bodies. User code uses its instance conveninently named "B".
	Iteration is supported, so you can say "for b in B: ...". """
	def __getitem__(self,id): return Bodies.Body(id)
	def __len__(self): return S.nBodies
	def __contains__(self,n): return n<self.__len__()
	def __iter__(self):
		return Bodies._iterator()
	class _iterator:
		def __init__(self): self.index=0
		def next(self):
			if self.index>=S.nBodies: raise StopIteration
			self.index+=1
			return B[self.index]

	class Body:
		"""Dummy class that returns a few body parameters as if they were its attributes.
		C++ code in pyade.cpp is called to get the actual values.
		Properties calculable from those already existing may be defined here - see Etrans, Erot, Epot, E for examples."""
		def __init__(self,id):
			self._id=id
		def __getattr__(self,name):
			if   name=='Etrans': return .5*self.m*sum([v**2 for v in self.v])
			elif name=='Erot': return sum([.5*self.I[i]*self.w[i]**2 for i in range(3)])
			#FIXME: gravity hardcoded...
			elif name=='Epot': return -sum([self.x[i]*(0,0,-10)[i] for i in range(3)])*self.m
			elif name=='E': return self.Etrans+self.Erot+self.Epot
			else: return _pyade.bodyProp(self._id,_pyade.bodyPropDict()[name])

class Simul:
	"""Class representing (current) simulation. User code uses its instance convenintly named "S". """
	def __getattr__(self,name):
		return _pyade.simulProp(_pyade.simulPropDict()[name])

#class History:
#	"""
#		example:
#			hash="B5I" for B[5].I
#			lambda to get value at current time: lambda: B[5].I
#		relWanted={hash:(delta_t>0,delta_i>0,lambda to get value),...}; highest delta_t/delta_i gets precedence
#		absWanted={hash:([t0,t1,t2,...],[i0,i1,...]),...}
#		data={hash:[(iter,time,value),...],hash2:[(,,),...]}
#	"""
#	def __init__:
#		relWanted={}; absWanted={};
#		# time and iteration when called for the last time - used to determine if we know how to calculate an abs value
#		prev=(0,0)
#		self.ITER,self.TIME=0,1
#	def addWanted(hash,coord,what,getFunc=None):
#		if coord<0: #relative
#			if relWanted.has_key(hash):
#				rec=list(relWanted[hash]) # to be mutable
#				if rec[what]>-coord: rec[what]=-coord
#					relWanted[hash]=tuple(rec)
#			else:
#				assert(getFunc)
#				rec=[0,0,getFunc]
#				rec[what]=-coord
#				relWanted[hash]=tuple(rec)
#		else: # absolute
#			if absWanted.has_key[hash]:
#				if not coord in absWanted[hash][what]: absWanted[hash][what].append(coord)
#			else:
#				rec=[][]
#				rec[what].append(coord)
#				absWanted[hash]=rec
#			
#	def get(hash,t=None,i=None):
#		assert((t or i) and not (t and i)); #args mutually exclusive
#		# self.what says whether we need time or iterations
#		if t:
#			d=t; what=ITER;
#		else
#			d=i; what=TIME
#		dd=d; wanted=absWanted #absolute, from origin
#		if d<0: # relative, towards past
#			dd=[S.i+d,S.t+d][what];
#		if data.has_key[hash]:
#			res=[ddh for ddh in data[hash] if ddh[what]==dd];
#			assert(len(res) in [0,1]);
#			addWanted(hash,dd,what)
#			if not d in absWanted[hash][what]: absWanted[hash][what].append(d)
#			if len(res)==1: return ret[0][2]
#		else: 
#			addWanted[hash][what].append(d)
#			return None




# instances
B=Bodies()
S=Simul()

# testing stuff
if __name__=='__main__':
		print B[0].m,B[0].I
		print B[2].x, B[2].r
		print S.nBodies
		for a in [(bb.m,bb.id) for bb in B if bb.id>50 and bb.id%2==0]: print a[0],a[1]
